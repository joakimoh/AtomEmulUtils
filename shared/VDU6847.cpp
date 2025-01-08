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

bool VDU6847::advance(uint64_t stopCycle)
{
	VideoTiming timing;

	while (mCycleCount < stopCycle) {

		int field_cycle = mCycleCount % mN60HzCycles;

		int line = (int)round(timing.linesPerField * (mCycleCount % mN60HzCycles) / mN60HzCycles);

		if (line < timing.vBlankinglines) {
			mRowPixel = 0;
			mCycleCount += 2;
		}
		else { 

			// draw one row of a character (8 pixels)
			

			int pixel_line = (int)round((line - timing.vBlankinglines));
			int char_symbol_line = pixel_line % 12;
			int char_row = (int)round(pixel_line / timing.visibleLines * 16);
			int char_col = mRowPixel / 8;

			ALLEGRO_COLOR green = al_map_rgb(0, 0xff, 0);
			ALLEGRO_COLOR black = al_map_rgb(0, 0, 0);
			uint8_t data;
			mVideoMem->read(mVideoMemAdr + char_row * 32 + char_col, data);
			CharDef char_symbol = mCharRom[data & 0x3f];


			
			ALLEGRO_BITMAP* display = al_get_target_bitmap();

			// Create bitmap for one row of the character to be displayed
			ALLEGRO_BITMAP* char_row_bitmap = al_create_bitmap(8, 1);
			al_set_target_bitmap(char_row_bitmap);

			uint8_t char_bitmap = 0x0;
			if (char_symbol_line >= 3 && char_symbol_line <= 9)
				char_bitmap = char_symbol.rows[char_symbol_line - 3];

			cout << "DRAW ROW " << dec << (int)char_symbol_line << hex << " (0x" << (int) char_bitmap << ") of symbol '" << dec <<
				(char)char_symbol.asc <<
				"' at [" << char_row << ", " << char_col << "] and specified by memory 0x" <<
				hex << mVideoMemAdr + char_row * 32 + char_col << "\n";

			for (int x = 0; x < 8; x++) {
				int pixel = 0;
				if (x >= 2 && x <= 7)
					pixel = char_bitmap & (1 << x);
				if (pixel)
					al_put_pixel(x, 0, green);
				else
					al_put_pixel(x, 0, black);
			}

			al_set_target_bitmap(display);
			
			// Draw one line of the symbol on the display
			al_draw_bitmap_region(char_row_bitmap, 0, 0, 8, 12, mRowPixel, pixel_line, 0);
			

			// Dispose of the temporary char row bitmpap
			al_destroy_bitmap(char_row_bitmap);

			mRowPixel = (mRowPixel + 8) % 256;

			// Advance time taken to draw eight horizontal pixels
			mCycleCount += timing.hzPixelduration * 8;

		}

		//cout << "CycleCount for VDU: " << dec << mCycleCount << "\n";

	}

	//cout << "CycleCount for VDU: " << dec << mCycleCount << "\n";
	//cout << timing.toString() << "\n";

	// Update display (i.e make changes visible) with 60 Hz frequency
	if (mCycleCount % (int)round(timing.linesPerField) == 0) {
		cout << "Update display!\n";
		al_flip_display();
	}

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
// 0		0		Internal alphanumerics (standard 5 x 7 dot matrix characters)
// 0		1		Internal alphanumerics inverted (standard 5 x 7 dot matrix characters - inverted)
// 1		0		External alphanumerics (external 8 x 12 dot matrix characters)
// 1		1		External alphanumerics inverted (external 8 x 12 dot matrix characters - inverted)
// 
// For the Acorn Atom, INV is connected to b7 of read graphics memory data, INT/EXT to b6. But Acorn Atom
// doesn't have external aplhanumerics.
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

VDU6847::VDU6847(uint16_t adr, int n60HzCycles, uint16_t videoMemAdr, DebugInfo debugInfo) :
	Device(VDU6847_DEV, adr, 0x100, debugInfo), mVideoMemAdr(videoMemAdr), mN60HzCycles(n60HzCycles)
{
	// Set the size of the VDU register vector
	mMem.resize((size_t) mDevSz);

	// Initialise the VDU registers with zeros
	mMem.assign(mDevSz, 0);

	if (mDebugInfo.verbose)
		cout << "VDU 6847 at address 0x" << hex << setfill('0') << setw(4) << mDevAdr <<
		" to 0x" << mDevAdr + mDevSz - 1 << " (" << dec << mDevSz << " bytes)\n";
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
