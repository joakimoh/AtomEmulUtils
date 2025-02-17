#include "VDU6847.h"
#include <filesystem>
#include <iostream>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <cmath>

using namespace std;

//
// 
// Can be in either Alphanumeric/Semigraphics (major mode 1, A/G=0) or Graphics mode (major mode 2, A/G=1)
// 
// 
// Supported Alphanumeric modes (major mode 1, A/S=0) :
// 
// A/G (PA4)	A/S (b6)	INT/EXT (b6)	INV (b7)	Description																#Colours	Memory usage	Used by Atom
// 0			0			0				0			Internal alphanumerics (standard 5 x 7 dot matrix characters)			2			512 bytes		Yes
// 0			0			0				1			Internal alphanumerics inverted (standard 5 x 7 dot matrix characters)	2			512 bytes		Yes
// 0			0			1				0			External alphanumerics (external 8 x 12 dot matrix characters)			2			512 bytes		No
// 0			0			1				1			External alphanumerics inverted (external 8 x 12 dot matrix characters)	2			512 bytes		No
// 
// For the Acorn Atom, INV is connected to b7 of read graphics memory data, A/S and INT/EXT to b6. But Acorn Atom
// doesn't have external alphanumerics so b6 needs always to be '0'. The CSS imput is connected to the PIA output PC4 and selects the colour palette for the
// semigraphics modes.
// 
// 
// Graphics memory Byte content: c1 c0 l5 l4 l3 l2 l1 l0
// 
// Pixel structure (2 scan lines per pixel row and 4 'character' pixels per graphics pixel (two 3.58 Mhz half clocks)
// l5 l4
// l3 l2
// l1 l0
// 
// // Pixel value '0' => Black
// CSS	Pixel bit	Colour bits		Colour
// -	0			-				Black
// 0	1			00				Green
// 0	1			01				Yellow
// 0	1			10				Blue
// 0	1			11				Red
// 1	1			00				-
// 1	1			01				Cyan
// 1	1			10				Magenta
// 1	1			11				Orange
// 			
// 
// The graphics mode inputs A/G:GM01:GM1:GM2 are connected to the 8255 PIA's PA4-7 (this is the mode input below).
// 
// Supported Semigraphics modes (major mode 1, A/S=1):
// 
// A/G (PA4)	A/S (b6)		INT/EXT (b6)	Description								Resolution	#Colours	Memory usage	Used by Atom
// 0			1				0				Supported Semigraphics mode 4 (SG4)		64 x 32		8			512 bytes		No
// 0			1				1				Supported Semigraphics mode 4 (SG6)		64 x 48		8			512 bytes		Yes - Graphics mode 0
// 
// Semigraphics 4
// 
// Supported Graphic (Major Mode 2) modes: 
//
// A/G	GM2	GM1	GM3	Mode	resolution	#colours	Memory usage		Description							Used by Atom
// 1	0	0	0	0		64 x 64		4			1kB					colour graphics (CG1)				Yes - Graphics mode 1a
// 1	0	0	1	1		128 x 64	2			1kB					resolution graphics one (RG1)		Yes - Graphics mode 1
// 1	0	1	0	2		128 x 64	4			2kB					colour graphics two (CG2)			Yes - Graphics mode 2a
// 1	0	1	1	3		128 x 96	2			1.5kB				resolution graphics two (RG2)		Yes - Graphics mode 2
// 1	1	0	0	4		128 x 96	4			3kB					colour graphics three (CG3)			Yes - Graphics mode 3a
// 1	1	0	1	5		128 x 192	2			3kB					resolution graphics three (RG3)		Yes - Graphics mode 3
// 1	1	1	0	6		128 x 192	4			6kB					colour graphics six (CG6)			Yes - Graphics mode 4a
// 1	1	1	1	7		256 x 192	2			6kB					resolution graphics six (RG6)		Yes - Graphics mode 4
//

VDU6847::VDU6847(string name, uint16_t adr, double clockSpeed, ALLEGRO_BITMAP* disp, int dispW, int dispH, uint16_t videoMemAdr, DebugInfo debugInfo, ConnectionManager* connectionManager) :
	VideoDisplayUnit(name, VDU6847_DEV, adr, 0x100, disp, dispW, dispH, videoMemAdr, debugInfo, connectionManager), mN60HzCycles((int)round(clockSpeed * 1e6 / 60))
{
	// Specify ports that can be connectde to other devices
	registerPort("RESET", IN_PORT, 0x01, RESET, &mRESET);
	registerPort("A/S", IN_PORT, 0x01, VDU_PORT_AS, &mAS);
	registerPort("A/G", IN_PORT, 0x01, VDU_PORT_AG, &mAG);
	registerPort("GM", IN_PORT, 0x07, VDU_PORT_GM, &mGM);
	registerPort("CSS", IN_PORT, 0x01, VDU_PORT_CSS, &mCSS);
	registerPort("INT/EXT", IN_PORT, 0x01, VDU_PORT_INT_EXT, &mIntExt);
	registerPort("INV", IN_PORT, 0x01, VDU_PORT_INV, &mInv);
	registerPort("FS", OUT_PORT, 0x01, VDU_PORT_FS, &mFS);
	registerPort("Din", OUT_PORT, 0xff, VDU_PORT_DIN, &mDin);


	// Set the size of the VDU register vector
	mMem.resize((size_t)mMemorySpace.sz);

	// Initialise the VDU registers with zeros
	mMem.assign(mMemorySpace.sz, 0);

	if (mDebugInfo.dbgLevel & DBG_VERBOSE)
		cout << "VDU 6847 at address 0x" << hex << setfill('0') << setw(4) << mMemorySpace.adr <<
		" to 0x" << mMemorySpace.adr + mMemorySpace.sz - 1 << " (" << dec << mMemorySpace.sz << " bytes)\n";

	// Create 256 x 192 display bitmap and clear it
	mDisplayBitmap = al_create_bitmap(mVisW, mVisH);

	green = al_map_rgb(0, 0xff, 0);
	black = al_map_rgb(0, 0, 0);

	lockDisplay();

	if (mDebugInfo.dbgLevel & DBG_VERBOSE) {
		cout << dec << "\n\nM6847 Parameters:\n\n";
		cout << "Frame rate: " << mFrameFreq << " [Hz]\n";
		cout << "Scane lines per field frame: " << mScanLines << " lines\n";
		cout << "Line duration: " << mlineDur << " [us] (" << mLineW << " pixels)\n";
		cout << "Duration of horizontal borders: " << mBrdH << " [us] (" << mLBrdW + mRBrdW << " pixels)\n";
		cout << "Vertical borders: " << mTBrdH + mBBrdH << " lines\n";
		cout << "Vertical blanking: " << mTVBlkH + mBVBlkH << " lines\n";
		cout << "Horizontal blanking: " << mHBlkDur << " [us] (" << mLBlkW + mRBlkW << " pixels)\n";
		cout << "Visible Active Display Area: " << mActAreaW << " x " << mActAreaH << " pixels\n";
		cout << "Total Visible Display Area: " << mVisW << " x " << mVisH << " pixels\n";
		cout << "Total Display Area (including invisible parts): " << mTotalW << " x " << mTotalH << " pixels\n";
		cout << "Scaled Visible Display Area: " << mScaledW << " x " << mScaledH << "\n";
		cout << "\n\n";
	}


}

VDU6847::~VDU6847()
{
	unlockDisplay();
	al_destroy_bitmap(mDisplayBitmap);
}

bool VDU6847::reset()
{
	Device::reset();
	mScanLine = 0;
	mFieldCount = 0;

	if (((mDebugInfo.dbgLevel & DBG_VERBOSE) != 0) && mRESET != pRESET) {
		cout << "VDU 6847 RESET\n";
		pRESET = mRESET;
	}

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

	float proc_clk_rate_Mhz = mN60HzCycles * 60 / 1e6;


	int pixel_line = mScanLine - (mTVBlkH + mTBrdH);
	int visible_line = mScanLine - mTVBlkH;

	if (!mRESET) {
		reset();
		mCycleCount += (int)round(63.5 / proc_clk_rate_Mhz);
		endCycle = mCycleCount;
		return true;
	}

	
	if (pixel_line == mActAreaH) {

		// The Field Sync (FS) signal goes High to Low at the end of the active display area
		updatePort(VDU_PORT_FS, 0);  // for Acorn atom this will set PIA port C:b7 to '0'

		unlockDisplay();

		// Scale the display bitmap including borders to match the size of the display
		al_draw_scaled_bitmap(mDisplayBitmap, 0, 0, mVisW, mVisH, 0, 0, mScaledW, mScaledH, 0);

		// Make the updates visible on the display
		al_flip_display();


		// Clear the display
		al_clear_to_color(black);

		mFieldCount++;

		lockDisplay();

	}

	if (pixel_line >= 0 && pixel_line < mActAreaH)
		// Draw a visible active line
	{		

		// Get pointer to bitmap data for the concerned scan line.
		// pitch <=> bytes/line; data <=> pixel bytes with left-most pixel first
		unsigned int* bitmap_data_p = (unsigned int*)((char*)mLockedDisplayBitMap->data + mLockedDisplayBitMap->pitch * visible_line);
			
		// Draw left border
		for (int p = 0; p < mLBrdW; p++) {
			*bitmap_data_p++ = 0xff00ff00; // opaque green ARGB 8888
		}

		// Iterate over all horizontal bytes

		uint8_t mem_data;
		if (mAG)
			// Graphic mode
		{		
			switch (mGM) {
			case 0: // CG1:  64 x  64, 4 colours, 1   kB, 16 bytes/line <=> Acorn Atom mode 1a
			case 2: // CG2: 128 x  64, 4 colours, 2   kB, 32 bytes/line <=> Acorn Atom mode 2a
			case 4: // CG4: 128 x  96, 4 colours, 3   kB, 32 bytes/line <=> Acorn Atom mode 3a
			case 6: // CG6: 128 x 192, 4 colours, 6   kB, 32 bytes/line <=> Acorn Atom mode 4a
			{
				int bytes_per_line = 32;
				int pixel_width = 2;
				int pixel_height = 4;
				if (mGM == 0) {
					bytes_per_line = 16;
					pixel_width = 4;
				}
				else if (mGM == 4)
					pixel_height = 2;
				else if (mGM == 6)
					pixel_height = 1;
				int big_pixel_line = pixel_line / pixel_height;
				int base_mem_adr = mVideoMemAdr + big_pixel_line * bytes_per_line;
				// byte c1 c0 c1 c0 c1 c0 c1 c0 c1 c0 => pixels e3 e2 e1 e0
				for (int pixel_byte = 0; pixel_byte < bytes_per_line; pixel_byte++) {
					int mem_adr = base_mem_adr + pixel_byte;
					if (!readGraphicsMem(mem_adr, mem_data))
						return false;
					for (int p = 0; p < 4; p++) {
						uint8_t colour = (mem_data >> 6) & 0x3; 
						for (int pw = 0; pw < pixel_width; pw++)
							*bitmap_data_p++ = mColours[mCSS][colour];
						mem_data = mem_data << 2;
					}
				}
				break;
			}
			case 1: // CG1: 128 x  64, 2 colours, 1   kB, 16 bytes/line <=> Acorn Atom mode 1
			case 3: // CG3: 128 x  96, 2 colours, 1.5 kB, 16 bytes/line <=> Acorn Atom mode 2
			case 5: // CG5: 128 x 192, 2 colours, 3   kB, 16 bytes/line <=> Acorn Atom mode 3
			case 7: // CG7: 256 x 192, 2 colours, 6   kB, 32 bytes/line <=> Acorn Atom mode 4
			{
				int bytes_per_line = 16;
				int pixel_width = 2;
				int pixel_height = 1;			
				if (mGM == 7) {
					bytes_per_line = 32;
					pixel_width = 1;
				}
				else if (mGM == 1)
					pixel_height = 3;
				else if (mGM == 3)
					pixel_height = 2;
				int big_pixel_line = pixel_line / pixel_height;
				int base_mem_adr = mVideoMemAdr + big_pixel_line * bytes_per_line;
				for (int pixel_byte = 0; pixel_byte < bytes_per_line; pixel_byte++) {					
					int mem_adr = base_mem_adr + pixel_byte;
					if (!readGraphicsMem(mem_adr, mem_data))
						return false;
					for (int p = 0; p < 8; p++) {
						for (int pw = 0; pw < pixel_width; pw++) {
							if (mem_data & 0x80)
								*bitmap_data_p++ = 0xff00ff00; // opaque green ARGB 8888
							else
								*bitmap_data_p++ = 0xff000000; // opaque black ARGB 8888
						}
						mem_data = mem_data << 1;
					}
				}
				break;
			}

			
			default: // ERROR - should never happen
				break;
			}
		}
		else 
				// Alphanumeric or semigraphic mode
			{
				int pixel_row = (pixel_line % 12) / 4;
				int mem_row = pixel_line / 12;
				int base_mem_adr = mVideoMemAdr + mem_row * 32;
				int y = pixel_line % 12;
				for (int char_col = 0; char_col < 32; char_col++) {
					int mem_adr = base_mem_adr + char_col;
					if (!readGraphicsMem(mem_adr, mem_data))
						return false;
					if (mAS)
						// Semigraphic mode 6 with 2 x 3 graphic symbols (A/S HIGH)
					{
						// Pixel value '0' => Black
						// CSS '0' => 00 = Green, 01 = yellow, 10 = Blue, 11 = Red
						// CSS '1' => 00 = -, 01 = Green, 01 = Cyan, 10 = Magenta, 11 = Orange
						uint8_t colour = (mem_data >> 6) & 0x3;					
						uint8_t pixel_data = ((mem_data & 0x3f) >> 2 * (2 - pixel_row)) & 0x3;
						for (int pixel = 0; pixel < 2; pixel++) {
							if (pixel_data & (1 << (1 - pixel))) {
								*bitmap_data_p = *(bitmap_data_p + 1) = *(bitmap_data_p + 2) = *(bitmap_data_p + 3) = mColours[mCSS][colour];
							}
							else {
								*bitmap_data_p = *(bitmap_data_p + 1) = *(bitmap_data_p + 2) = *(bitmap_data_p + 3) = 0xff000000; // opaque black ARGB 8888;
							}
							bitmap_data_p += 4;
						}
					}
					else
						// Alpanumeric mode (A/S LOW)
					{
						uint8_t symbol = (mem_data & 0x3f);
						CharDef symbol_def;
						if (!mIntExt) // INT/EXT LOW => internal char ROM
							symbol_def = mCharRom[symbol];
						else
							// INT/EXT HIGH => external char ROM
							// Not yet supported!!!
						{
							symbol_def = mCharRom[symbol];
						}
						uint8_t symbol_mask;

						symbol_mask = symbol_def.rows[y];
						if (mInv) {
							symbol_mask = ~symbol_mask; // inverted character
						}

						// Update display bitmap with character (starting with the left-most pixel)
						// Windows seems to prefer to use ALLEGRO_PIXEL_FORMAT_ARGB_8888 (0x9)
						// The bitmap pointer has been advanced 8 pixels (one character) when completed.
						for (int x = 0; x < 8; x++) {
							if (symbol_mask & 0x80)
								*bitmap_data_p++ = 0xff00ff00; // opaque green ARGB 8888
							else
								*bitmap_data_p++ = 0xff000000; // opaque black ARGB 8888
							symbol_mask = symbol_mask << 1;
						}
					}
				}
			}

		
		

		// Draw right border
		for (int p = 0; p < mRBrdW; p++) {
			*bitmap_data_p++ = 0xff00ff00; // opaque green ARGB 8888
		}

	}
	

	else if (mScanLine >= mTVBlkH && mScanLine < mScanLines - mBVBlkH)
		// Draw top or bottom border
	{	
		
		unsigned int* bitmap_data_p = (unsigned int*)((char*)mLockedDisplayBitMap->data + mLockedDisplayBitMap->pitch * visible_line);
		for (int p = 0; p < mVisW; p++) {
			*bitmap_data_p++ = 0xff00ff00; // opaque green ARGB 8888
		}
		
	}

	// Advance time taken to process one scan line
	mCycleCount += (int) round(63.5/ proc_clk_rate_Mhz);

	// Next scan line if a complete line has been scanned
	mScanLine = (mScanLine + 1) % 262;

	if (mScanLine == 0) {
		// The Field Sync (FS) signal goes High at the end of the vertical synchronisation pulse
		updatePort(VDU_PORT_FS, 1); // For Acorn Atom this will set PIA port C:b7 to '1'
	}

	endCycle = mCycleCount;

	return true;
}

void VDU6847::lockDisplay()
{
	// Save allegro state and lock display bitmap for update
	al_store_state(&mAllegroState, ALLEGRO_STATE_TARGET_BITMAP);
	al_set_target_bitmap(mDisplayBitmap);
	mLockedDisplayBitMap = al_lock_bitmap(mDisplayBitmap, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_WRITEONLY);
}

void VDU6847::unlockDisplay()
{
	// Restore allegro state and unlock display for update
	al_unlock_bitmap(mDisplayBitmap);
	al_restore_state(&mAllegroState);
}

bool VDU6847::read(uint16_t adr, uint8_t& data)
{

	// Call parent class to trigger scheduling of other devices when applicable
	if (!MemoryMappedDevice::read(adr, data))
		return false;

	data = mMem[adr - mMemorySpace.adr];

	return true;

}
bool VDU6847::write(uint16_t adr, uint8_t data)
{
	// Call parent class to trigger scheduling of other devices when applicable
	if (!MemoryMappedDevice::write(adr, data))
		return false;

	mMem[adr - mMemorySpace.adr] = data;

	return true;
}

inline bool VDU6847::readGraphicsMem(uint16_t adr, uint8_t& data)
{
	return mVideoMem->read(adr, data) && updatePort(VDU_PORT_DIN, data);
}