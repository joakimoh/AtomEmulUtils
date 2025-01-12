#include "VDU6847.h"
#include <filesystem>
#include <iostream>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include "Video.h"
using namespace std;


bool VDU6847::reset()
{
	Device::reset();
	return true;
}


//
// M6847 displays 256 x 192 pixels in 60 Hz non-interlaced mode
//
// The 262 1/2 scan lines of an NTSC field (525 / 2) is split into
// 13 (vertical blanking) + top border (25) + pixels (192) + bottom border (26) + vertical retrace (6)
//
// Each scan line lasts 63.5 us. The visible part is 128/3.58 = 35.8 us, visible part + left & right borders = 185.5/3.58 = 51.8 us.
// The horizontal blanking then becomes 63.5 - 51.8 = 11.7 us. The border than takes 51.8 - 35.8 = 16.0 us.
// The left part of the border and blanking becomes (11.7 + 16.0) / 2 = 13.8 us
// 
// 12.3 us (left & right hz blanking) +  15.9 us (left & right border) + 35.8 us (256 pixels) = 28.2 us + 35.8 us = 64 us
//
// The pixel part of the scan line (35.8 us). A character (8 pixels) takes 35.8 / 32 = 1.12 us. If we measure the scan line in
// this unit we will have 56.7 such units per scan line and the first visible one starts after 13.8 / 1.12 = 12.4 units,
// lasts 32 units (as their are 32 visible characters per scan line) and is followed by 12.4 non-visible units.
//
// advance() advances one scan line (but only if the stop cycle hasn't already been reached)
//
bool VDU6847::advance(uint64_t stopCycle)
{
	uint64_t dummy;
	while (mCycleCount < stopCycle)
		advanceLine(dummy);

	return true;
}


bool VDU6847::advanceLine(uint64_t& endCycle)
{
	VideoTiming timing;
	float proc_clk_rate_Mhz = mN60HzCycles * 60 / 1e6;
	int first_visible_scan_line = 13 + 25;
	int last_visible_scan_line = 262 - (26 + 6);
	int first_visible_scan_pos = 14;

	

	if (mScanLine == 0) {

		// Refresh the display at the start of a new field

		// Direct drawing to the display
		al_set_target_bitmap(mDisplay);

		// Clear the display
		al_clear_to_color(al_map_rgb(0, 0, 0));

		// Draw the 256 x 192 display bitmap while scaling it to 648 x 486
		al_draw_scaled_bitmap(mDisplayBitmap, 0, 0, 256, 192, 0, 0, 648, 486, 0);

		// Make the updates visible on the display
		al_flip_display();

	}

	else if (mScanLine >= first_visible_scan_line && mScanLine <= last_visible_scan_line ) {
			
		// Draw a visible line
			
		for (int char_col = 0; char_col < 32; char_col++) {
			ALLEGRO_COLOR green = al_map_rgb(0, 0xff, 0);
			ALLEGRO_COLOR black = al_map_rgb(0, 0, 0);

			// draw one character of a scan line
			int pixel_line = mScanLine - first_visible_scan_line;
			int y = pixel_line % 12;

			al_set_target_bitmap(mCharBitmap);
			al_clear_to_color(black);

			uint8_t mem_data;
			int mem_row = pixel_line / 12;
			int mem_adr = mVideoMemAdr + mem_row * 32 + char_col;
			if (!mVideoMem->read(mem_adr, mem_data))
				return false;
			uint8_t symbol = (mem_data & 0x3f);
			CharDef symbol_def = mCharRom[symbol];
			uint8_t symbol_mask = symbol_def.rows[y];
			for (int x = 0; x < 8; x++) {
				if (symbol_mask & 0x80)
					al_put_pixel(x, 0, green);
				symbol_mask = symbol_mask << 1;
			}

			al_set_target_bitmap(mDisplayBitmap);
			al_draw_bitmap(mCharBitmap, char_col * 8, pixel_line, 0);

			/*
			if (mem_adr < 0x8080 && symbol_def.asc == 'A')
				cout << "Line " << dec << y << " of symbol: '" << symbol_def.asc << "' with " <<
				" bitmask 0x" << hex << (int)symbol_mask << " at 0x" << hex << mem_adr <<
				" drawn at position " << dec << char_col * 8 << "," << pixel_line << "\n";
			*/
		}

	}
		
	// Advance time taken to process one scan line
	mCycleCount += (int) round(63.5/ proc_clk_rate_Mhz);

	// Next scan line if a complete line has been scanned
	mScanLine = (mScanLine + 1) % 262;

	endCycle = mCycleCount;

	return true;
}

//
// 
// Can be in either Alphanumeric (major mode 1) or Graphic mode (major mode 2)
// 
// 
// Supported Alphanumeric (major mode 1) modes:
// 
// INT/EXT	INV
// 0		0		Internal alphanumerics (standard 5 x 7 dot matrix characters) - Semigraphic 4
// 0		1		Internal alphanumerics inverted (standard 5 x 7 dot matrix characters - inverted)
// 1		0		External alphanumerics (external 8 x 12 dot matrix characters) - Semigraphic 6
// 1		1		External alphanumerics inverted (external 8 x 12 dot matrix characters - inverted)
// 
// For the Acorn Atom, INV is connected to b7 of read graphics memory data, INT/EXT to b6. But Acorn Atom
// doesn't have external alphanumerics so b6 needs always to be '0'.
// 
// Supported Graphic (Major Mode 2) modes: 
//
// Mode	resolution	#colours	memory usage		description
// 0	64 x 64		4			1kB					colour graphics (CG1)
// 1	128 x 64	2			1kB					resolution graphics one (RG1)
// 2	128 x 64	4			2kB					colour graphics two (CG2)
// 3	128 x 96	2			1.5kB				resolution graphics two (RG2)
// 4	128 x 96	4			3kB					colour graphics three (CG3)
// 5	128 x 192	2			3kB					resolution graphics three (RG3)
// 6	128 x 192	4			6kB					colour graphics six (CG6)
// 7	256 x 192	2			6kB					resolution graphics six (RG6)
//
bool VDU6847::setGraphicMode(uint8_t mode)
{
	mMajorMode = (mode >> 3) & 0x1;
	mGraphicMode = mode & 0x7;
	return true;
}

bool VDU6847::setVideoRam(RAM* ram)
{
	mVideoMem = ram;
	return true;
}

VDU6847::VDU6847(uint16_t adr, int n60HzCycles, ALLEGRO_BITMAP* disp, uint16_t videoMemAdr, DebugInfo debugInfo) :
	Device(VDU6847_DEV, adr, 0x100, debugInfo), mVideoMemAdr(videoMemAdr), mN60HzCycles(n60HzCycles), mDisplay(disp)
{

	// Set the size of the VDU register vector
	mMem.resize((size_t) mDevSz);

	// Initialise the VDU registers with zeros
	mMem.assign(mDevSz, 0);

	if (mDebugInfo.verbose)
		cout << "VDU 6847 at address 0x" << hex << setfill('0') << setw(4) << mDevAdr <<
		" to 0x" << mDevAdr + mDevSz - 1 << " (" << dec << mDevSz << " bytes)\n";

	// Create 256 x 192 display bitmap and clear it
	mDisplayBitmap = al_create_bitmap(256, 192);
	al_set_target_bitmap(mDisplayBitmap);
	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_set_target_bitmap(mDisplay);

	mCharBitmap = al_create_bitmap(8, 1);
}

VDU6847::~VDU6847()
{
	al_destroy_bitmap(mCharBitmap);
	al_destroy_bitmap(mDisplayBitmap);
}



bool VDU6847::read(uint16_t adr, uint8_t& data)
{

	if (!validAdr(adr))
		return false;

	data = mMem[adr - mDevAdr];

	return true;

}
bool VDU6847::write(uint16_t adr, uint8_t data)
{

	if (!validAdr(adr))
		return false;

	mMem[adr - mDevAdr] = data;

	return true;
}
