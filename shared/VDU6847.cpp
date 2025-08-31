#include "VDU6847.h"
#include <filesystem>
#include <iostream>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <cmath>
#include "Utility.h"

using namespace std;

//
// 
// Emulation of the M6847 (interlaced version).
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

VDU6847::VDU6847(string name, uint16_t adr, VideoSettings videoSettings, double cpuClock, uint8_t waitStates, ALLEGRO_DISPLAY *disp,
	ALLEGRO_BITMAP* dispBitMap, uint16_t videoMemAdr, DebugManager  *debugManager, ConnectionManager* connectionManager, DeviceManager *deviceManager) :
	VideoDisplayUnit(name, VDU6847_DEV, videoSettings, cpuClock, waitStates, adr, 0x100, dispBitMap, videoMemAdr, debugManager, connectionManager,
		deviceManager), mN60HzCycles((int)round(cpuClock * 1e6 / 60))
{
	// Specify ports that can be connectde to other devices
	registerPort("A/S", IN_PORT, 0x01, VDU_PORT_AS, &mAS);
	registerPort("A/G", IN_PORT, 0x01, VDU_PORT_AG, &mAG);
	registerPort("GM", IN_PORT, 0x07, VDU_PORT_GM, &mGM);
	registerPort("CSS", IN_PORT, 0x01, VDU_PORT_CSS, &mCSS);
	registerPort("INT/EXT", IN_PORT, 0x01, VDU_PORT_INT_EXT, &mIntExt);
	registerPort("INV", IN_PORT, 0x01, VDU_PORT_INV, &mInv);
	registerPort("FS", OUT_PORT, 0x01, VDU_PORT_FS, &mFS);
	registerPort("Din", OUT_PORT, 0xff, VDU_PORT_DIN, &mDin);


	// Set the size of the VDU register vector
	mMem.resize((size_t)mAddressSpace.getSizeOfSpace());

	// Initialise the VDU registers with zeros
	mMem.assign(mAddressSpace.getSizeOfSpace(), 0);

	DBG_LOG(this,DBG_VERBOSE, "VDU 6847 at address 0x" + Utility::int2HexStr(mAddressSpace.getStartOfSpace(),4) +
		" to 0x" + Utility::int2HexStr(mAddressSpace.getStartOfSpace() + mAddressSpace.getEndOfSpace(),4) + " (" + to_string(mAddressSpace.getSizeOfSpace()) + " bytes)"
	);

	// Initialise line counters
	calcLineSettings();

	// Create display bitmap and clear it
	mDisplayBitmap = al_create_bitmap(mScreenW, mScreenH);

	al_resize_display(disp, mScreenW, mScreenH);

	green = al_map_rgb(0, 0xff, 0);
	black = al_map_rgb(0, 0, 0);

	lockDisplay();

	if (DBG_LEVEL_DEV(this,DBG_VERBOSE)) {
		int mLineW = 2;
		int mlineDur = 1;
		DBG_LOG(this, DBG_VERBOSE,  "\n\nM6847 Parameters:");
		DBG_LOG(this, DBG_VERBOSE,  "Field rate: " + to_string(mFieldFreq) + " [Hz]");
		DBG_LOG(this, DBG_VERBOSE,  "Scan lines per field: " + to_string(mFieldScanLines) + " lines");
		DBG_LOG(this, DBG_VERBOSE,  "Scan lines per frame: " + to_string(mScreenScanLines) + " lines");
		DBG_LOG(this, DBG_VERBOSE,  "Line duration: " + to_string(mlineDur) + " [us] (" + to_string(mLineW) + " pixels)");
		DBG_LOG(this, DBG_VERBOSE,  "Duration of horizontal borders: " + to_string(mBrdH) + " [us] (" + to_string(mHzBorderPixels) + " pixels)");
		DBG_LOG(this, DBG_VERBOSE,  "Vertical borders: " + to_string(mTopBorderLines + mBottomBorderLines) + " lines");
		DBG_LOG(this, DBG_VERBOSE,  "Vertical blanking: " + to_string(mTVBlkH + mBVBlkH) + " lines");
		DBG_LOG(this, DBG_VERBOSE,  "Horizontal blanking: " + to_string(mHBlkDur) + " [us] (" + to_string(mHzBlankingPixels) + " pixels)");
		DBG_LOG(this, DBG_VERBOSE,  "Visible Active Display Area: " + to_string(mHzDisplayedPixels) + " x " + to_string(mDisplayedLines) + " pixels");
		DBG_LOG(this, DBG_VERBOSE,  "Total Visible Display Area: " + to_string(mScreenW) + " x " + to_string(mScreenH) + " pixels");
		DBG_LOG(this, DBG_VERBOSE,  "Total Display Area (including invisible parts): " + to_string(mScreenScanLines) + " x " + to_string(mHzPixels) + " pixels");
		DBG_LOG(this, DBG_VERBOSE,  "Visible Display Area: " + to_string(mScreenW) + " x " + to_string(mScreenH) + "");
		DBG_LOG(this, DBG_VERBOSE,  "\n");
	}

	mHzHalfLinePixels = (int)round(mHzPixels / 2);
}

VDU6847::~VDU6847()
{
	unlockDisplay();
	al_destroy_bitmap(mDisplayBitmap);
}

bool VDU6847::power()
{
	Device::reset();

	mScanLine = 0;
	mField = 0;


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
// lasts 32 units (as there are 32 visible characters per scan line) and is followed by 12.4 non-visible units.
//
// advanceUntil() advances one character of a scan line (but only if the stop cycle hasn't already been reached)
//
bool VDU6847::advanceUntil(uint64_t stopCycle)
{
	uint64_t dummy;
	while (mCycleCount < stopCycle)
		advanceChar(dummy);

	return true;
}

bool VDU6847::addHalfLine(uint64_t& endCycle)
{
	// Check for 1/2 line
	// 'Render' pixels for the duration of one 1/2 line, but only one 'character' at a time
	if (mAddHalfLine) {
		mRenderedPixels = 8;
		if (mLineRenderedPixels > mVisiblePixelsEnd && mHzHalfLinePixels - mLineRenderedPixels < 8)
			mRenderedPixels = mHzHalfLinePixels - mLineRenderedPixels;
		mLineRenderedPixels += mRenderedPixels;
		mCycleCount = mCycleCountLineRef + (int)round(mLineRenderedPixels / 4.0 * mCPUClock / mClockFreq);
		if (mLineRenderedPixels == mHzHalfLinePixels) {
			mLineRenderedPixels = 0;
			mCycleCountLineRef = mCycleCount;
			mAddHalfLine = false;
		}

		endCycle = mCycleCount;
		return true;
	}

	return false;
}

bool VDU6847::advanceChar(uint64_t& endCycle)
{

	// Update visible and displayed line counters for each new scan line
	if (mScanLine != pScanLine)
		calcLineSettings();
	pScanLine = mScanLine;
	pField = mField;

	// Increase time by a 1/2 scan line and return if at the end of the field
	if (addHalfLine(endCycle))
		return true;

	if (mLineRenderedPixels == 0 && mAdjustedDisplayedLine == mDisplayedLines && mField % mFieldsPerRefreshPeriod == 0) {

		// The Field Sync (FS) signal goes High to Low at the end of the active display area
		updatePort(VDU_PORT_FS, 0);  // For the Acorn atom this will set PIA port C:b7 to '0'

		unlockDisplay();

		// Draw the bitmap on the display
		al_draw_bitmap(mDisplayBitmap, 0, 0, 0);

		// Make the updates visible on the display
		al_flip_display();

		// Clear the display
		al_clear_to_color(black);

		lockDisplay();

	}

	// Initialise  pixel counter that is used to limit the no of pixels rendered at a time
	mRenderedPixels = 0;

	// Only do rendering if in the visible region
	if (
		mLineRenderedPixels >= mVisiblePixelsStart && mLineRenderedPixels < mVisiblePixelsEnd &&
		mAdjustedVisibleLine >= 0 && mAdjustedVisibleLine < mScreenH
		)
	{
		// Get pointer to bitmap data for the concerned scan line.
		// pitch <=> bytes/line; data <=> pixel bytes with left-most pixel first
		if (mLineRenderedPixels == mVisiblePixelsStart)
			mBitmapDataP = (unsigned int*)((char*)mLockedDisplayBitMap->data + mLockedDisplayBitMap->pitch * mVisibleLine);

		// Draw up to eight bits at a time of the top border
		if (mAdjustedVisibleLine >= 0 && mAdjustedVisibleLine < mTopBorderLines) {
			drawPartialBorder(mVisiblePixelsStart, mScreenW);
		}

		// Draw up to eight bits at a time of the bottom border
		else if (mAdjustedVisibleLine >= mScreenH - mBottomBorderLines && mAdjustedVisibleLine < mScreenH)
			drawPartialBorder(mVisiblePixelsStart, mScreenW);

		// Draw up to eight bits at a time of the left border of a displayed line
		// (mAdjustedVisibleLine >= 0 && mAdjustedVisibleLine << mDisplayedLines)
		else if (mLineRenderedPixels >= mVisiblePixelsStart && mLineRenderedPixels < mDisplayedPixelsStart)
			drawPartialBorder(mVisiblePixelsStart, mHzLeftBorderPixels);

		// Draw up to eight bits at a time of the right border of a displayed line
		// (mAdjustedVisibleLine >= 0 && mAdjustedVisibleLine << mDisplayedLines)
		else if (mLineRenderedPixels >= mDisplayedPixelsEnd && mLineRenderedPixels < mVisiblePixelsEnd)
			drawPartialBorder(mDisplayedPixelsEnd, mHzRightBorderPixels);

		// Draw up to eight bits at a time of the displayed part of a visible line
		else
			// mLineRenderedPixels >= mDisplayedPixelsStart && mLineRenderedPixels < mDisplayedPixelsEnd &&
			// mAdjustedVisibleLine >= 0 && mAdjustedVisibleLine < mHzDisplayedPixels		
		{
			int p = mLineRenderedPixels - mDisplayedPixelsStart;

			uint8_t mem_data;
			if (mAG)
				// Graphic mode
			{
				mBigPixelLine = mAdjustedDisplayedLine / mPixelHeight / 2;
				int base_mem_adr = mVideoMemAdr + mBigPixelLine * mBytesPerLine;
				mPixelByte = (mLineRenderedPixels - mDisplayedPixelsStart) / mRenderedPixelsPerByte;
				if (mPixelByte >= 0 && mPixelByte < mBytesPerLine) {
					// Only render pixels if the device is properly intialised so that the memory address is valid
					int mem_adr = base_mem_adr + mPixelByte;
					if (!readGraphicsMem(mem_adr, mem_data))
						return false;
					for (int p = 0; p < mPixelsPerByte; p++) {
						uint32_t pixel_colour = calcGraphicModePixelColour(mem_data);
						for (int pw = 0; pw < mPixelWidth; pw++)
							*mBitmapDataP++ = pixel_colour;
						mem_data <<= mShiftsPerByte;
					}
					mRenderedPixels = mPixelsPerByte * mPixelWidth;
				}
				else
					mRenderedPixels = 8;
				
			}
			else
				// Alphanumeric or semigraphic mode
			{
				int field_pixel_line = mAdjustedDisplayedLine / 2;
				int pixel_row = (field_pixel_line % 12) / 4;
				int mem_row = field_pixel_line / 12;
				int base_mem_adr = mVideoMemAdr + mem_row * mBytesPerLine;
				int y = field_pixel_line % 12;

				mPixelByte = (mLineRenderedPixels - mDisplayedPixelsStart) / mRenderedPixelsPerByte;
				if (mPixelByte >= 0 && mPixelByte < mBytesPerLine) {
					// Only render pixels if the device is properly intialised so that the memory address is valid
					int mem_adr = base_mem_adr + mPixelByte;
					if (!readGraphicsMem(mem_adr, mem_data))
						return false;
					if (mAS)
						// Semigraphic mode 6 with 2 x 3 graphic symbols (A/S HIGH)
					{
						// Pixel value '0' => Black
						uint8_t colour_selector = (mem_data >> 6) & 0x3;
						uint8_t pixel_data = ((mem_data & 0x3f) >> 2 * (2 - pixel_row)) & 0x3;
						for (int pixel = 0; pixel < 2; pixel++) { // <=> mPixelsPerByte = 2
							uint32_t pixel_colour = calcSemiGraphicModePixelColour(colour_selector, pixel_data);
							for (int w = 0; w < mBasePixelW; w++) {
								*mBitmapDataP = *(mBitmapDataP + 1) = *(mBitmapDataP + 2) = *(mBitmapDataP + 3) = pixel_colour;
								mBitmapDataP += 4; // <=> mPixelWidth = 4 * mBasePixelW
							}
							pixel_data <<= 1;
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
						for (int x = 0; x < 8; x++) { // <=> mPixelsPerByte = 8
							for (int w = 0; w < mBasePixelW; w++) {
								if (symbol_mask & 0x80)
									*mBitmapDataP++ = 0xff00ff00; // opaque green ARGB 8888
								else
									*mBitmapDataP++ = 0xff000000; // opaque black ARGB 8888								
							}
							symbol_mask = symbol_mask << 1;
						}
					}

					mRenderedPixels = mPixelsPerByte * mPixelWidth;
				}
				else
					mRenderedPixels = 8;
						
			}

		}	
		
	}
	else {
		// no pixels rendered here but we need still to advance the pixel count
		mRenderedPixels = 8;

		// Make sure mVisiblePixelsStart is not passed when increasing mLineRenderedPixels  
		if (mLineRenderedPixels < mVisiblePixelsStart && mVisiblePixelsStart - mLineRenderedPixels < 8)
			mRenderedPixels = mVisiblePixelsStart - mLineRenderedPixels;

		// make sure mHzPixels is not passed when  increasing mLineRenderedPixels
		else if (mLineRenderedPixels > mVisiblePixelsEnd && mHzPixels - mLineRenderedPixels < 8)
			mRenderedPixels = mHzPixels - mLineRenderedPixels;

	}

	// Update time (in the unit CPU clock cycles) based on the no of rendered pixels
	// One M6847 horizontal pixel lasts 1/2 mClockFreq - '1 / (2*mClockFreq)' but as these pixels are over-sampled (doubled)
	// when rendering, we need also to divide by 2 - 'mRenderedPixels / 2'
	// As the M6847 is usually higher (3.58 MHz) then the CPU clock (normally 1 MHz), the cycle count delta will be small
	// and fractions will be lost unless compensation is made. That's why the cycle count update is made based on the start of
	// the line rather than the previous character.
	mLineRenderedPixels += mRenderedPixels;
	mCycleCount = mCycleCountLineRef + (int)round(mLineRenderedPixels / 4.0 * mCPUClock / mClockFreq);

	// Next scan line?
	assert(mLineRenderedPixels < mHzPixels);
	if (mLineRenderedPixels == mHzPixels) {
		mLineRenderedPixels = 0;
		mCycleCountLineRef = mCycleCount;
		mScanLine = (mScanLine + 2) % mScreenScanLines;
		if ((mField % 2 == 0 && mScanLine == 0) || (mField % 2 == 1 && mScanLine == 1)) { // start of new field
			mField++;
			if (mField % 2 == 0)
				mScanLine = 0; // Even field => start at scan line 0
			else
				mScanLine = 1; // Odd field => start at scan line 1

			// The Field Sync (FS) signal goes High at the end of the vertical synchronisation pulse
			updatePort(VDU_PORT_FS, 1); // For the Acorn Atom this will set PIA port C:b7 to '1'

			// Change state to add 1/2 line to ensure that the new field doesn't start until a 1/2 line has passed
			mAddHalfLine = true;

		}
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
	if (!MemoryMappedDevice::triggerBeforeRead(adr, data))
		return false;

	data = mMem[adr - mAddressSpace.getStartOfSpace()];

	return true;

}

bool VDU6847::dump(uint16_t adr, uint8_t& data)
{
	if (selected(adr)) {
		data = mMem[adr - mAddressSpace.getStartOfSpace()];
		return true;
	}
	return false;
}

bool VDU6847::write(uint16_t adr, uint8_t data)
{
	if (!selected(adr))
		return false;

	mMem[adr - mAddressSpace.getStartOfSpace()] = data;

	// Call parent class to trigger scheduling of other devices when applicable
	return MemoryMappedDevice::triggerAfterWrite(adr, data);
}

inline bool VDU6847::readGraphicsMem(uint16_t adr, uint8_t& data)
{
	return mVideoMem->read(adr, data) && updatePort(VDU_PORT_DIN, data);
}


// Outputs the internal state of the device
bool VDU6847::outputState(ostream& sout)
{
	sout << hex;
	sout << "AS      = " << (int)mAS     << " <=> " << (mAG? "N/A - No meaning when in graphic mode":(mAS ? "semigraphics" : "alphanumeric")) << "\n";
	sout << "AG      = " << (int)mAG     << " <=> " << (mAG ? "graphics" : "alphanumeric/semigraphics") << "\n";
	sout << "CSS     = " << (int)mCSS    << " <=> " << (mCSS ? "Cya, Magenta, Orange" : "Black, Green, Yellow, Blue & Red") << "\n";
	switch (mGM & 0x7) {
	case 0:
		sout << "GM      = " << (int)mGM << " <=>  64 x 64 pixels, 4 colours, 1kB graphics memory\n"; break;
	case 1:
		sout << "GM      = " << (int)mGM << " <=> 128 x 64 pixels, 2 colours, 1kB graphics memory\n"; break;
	case 2:
		sout << "GM      = " << (int)mGM << " <=> 128 x 64 pixels, 4 colours, 2kB graphics memory\n"; break;
	case 3:
		sout << "GM      = " << (int)mGM << " <=> 128 x 96 pixels, 2 colours, 1.5kB graphics memory\n"; break;
	case 4:
		sout << "GM      = " << (int)mGM << " <=> 128 x 96 pixels, 4 colours, 3kB graphics memory\n"; break;
	case 5:
		sout << "GM      = " << (int)mGM << " <=> 128 x 192 pixels, 2 colours, 3kB graphics memory\n"; break;
	case 6:
		sout << "GM      = " << (int)mGM << " <=> 128 x 192 pixels, 4 colours, 6kB graphics memory\n"; break;
	default: // 7
		sout << "GM      = " << (int)mGM << " <=> 256 x 192 pixels, 2 colours, 6kB graphics memory\n"; break;
	}
	sout << "INV     = " << (int)mInv    << " <=> " << (mInv ? "Inverted Characters" : "Non-inverted Characters") << "\n";
	sout << "INT/EXT = " << (int)mIntExt << " <=> " << (mIntExt ? "External Character ROM" : "Internal Character ROM") << "\n";
	sout << "FS      = " << (int)mFS << " <=> " << (mFS ? "Sync active" : "Sync inactive") << "\n";

	return true;
}

void VDU6847::processPortUpdate(int index)
{
	if (index == VDU_PORT_GM || index == VDU_PORT_AG || index == VDU_PORT_AS)
		calcGraphicModeSettings();
}


uint32_t VDU6847::calcGraphicModePixelColour(uint8_t pixelData)
{
	if (mGM % 2 == 0)
		// 0 <=> CG0:  64 x  64, 4 colours, 1   kB, 16 bytes/line <=> Acorn Atom mode 1a
		// 2 <=> CG2: 128 x  64, 4 colours, 2   kB, 32 bytes/line <=> Acorn Atom mode 2a
		// 4 <=> CG4: 128 x  96, 4 colours, 3   kB, 32 bytes/line <=> Acorn Atom mode 3a
		// 6 <=> CG6: 128 x 192, 4 colours, 6   kB, 32 bytes/line <=> Acorn Atom mode 4a
		return mColours[mCSS][(pixelData >> 6) & 0x3];
	else
		// 1 <=> CG1: 128 x  64, 2 colours, 1   kB, 16 bytes/line <=> Acorn Atom mode 1
		// 3 <=> CG3: 128 x  96, 2 colours, 1.5 kB, 16 bytes/line <=> Acorn Atom mode 2
		// 5 <=> CG5: 128 x 192, 2 colours, 3   kB, 16 bytes/line <=> Acorn Atom mode 3
		// 7 <=> CG7: 256 x 192, 2 colours, 6   kB, 32 bytes/line <=> Acorn Atom mode 4
		return ((pixelData & 0x80) == 0x80 ? 0xff00ff00 : 0xff000000); //  opaque green ARGB 8888 and black ARGB 8888
}

uint32_t VDU6847::calcSemiGraphicModePixelColour(uint8_t colourSelector, uint8_t pixelData)
{
	if (pixelData & 0x2)
		// CSS '0' => 00 = Green, 01 = yellow, 10 = Blue, 11 = Red
		// CSS '1' => 00 = -, 01 = Green, 01 = Cyan, 10 = Magenta, 11 = Orange
		return mColours[mCSS][colourSelector];
	else
		return 0xff000000; // opaque black ARGB 8888;
}

void VDU6847::calcGraphicModeSettings()
{
	if (mAG == 0) {
		mBytesPerLine = 32;
		if (mAS) {
			mPixelsPerByte = 2;
			mPixelWidth = 4 * mBasePixelW;
		}
		else {
			mPixelsPerByte = 8;
			mPixelWidth = 1 * mBasePixelW;
		}
		mRenderedPixelsPerByte = 512 / mBytesPerLine;
	}
	else {
		if (mGM % 2 == 0)
			// 0 <=> CG0:  64 x  64, 4 colours, 1   kB, 16 bytes/line <=> Acorn Atom mode 1a
			// 2 <=> CG2: 128 x  64, 4 colours, 2   kB, 32 bytes/line <=> Acorn Atom mode 2a
			// 4 <=> CG4: 128 x  96, 4 colours, 3   kB, 32 bytes/line <=> Acorn Atom mode 3a
			// 6 <=> CG6: 128 x 192, 4 colours, 6   kB, 32 bytes/line <=> Acorn Atom mode 4a
		{
			// byte c1 c0 c1 c0 c1 c0 c1 c0 c1 c0 => pixels e3 e2 e1 e0
			mBytesPerLine = 32;
			mPixelsPerByte = 4;
			mPixelWidth = 2 * mBasePixelW;
			mPixelHeight = 4;
			if (mGM == 0) {
				mBytesPerLine = 16;
				mPixelWidth = 4 * mBasePixelW;
			}
			else if (mGM == 4)
				mPixelHeight = 2;
			else if (mGM == 6)
				mPixelHeight = 1;
		}
		else
			// 1 <=> CG1: 128 x  64, 2 colours, 1   kB, 16 bytes/line <=> Acorn Atom mode 1
			// 3 <=> CG3: 128 x  96, 2 colours, 1.5 kB, 16 bytes/line <=> Acorn Atom mode 2
			// 5 <=> CG5: 128 x 192, 2 colours, 3   kB, 16 bytes/line <=> Acorn Atom mode 3
			// 7 <=> CG7: 256 x 192, 2 colours, 6   kB, 32 bytes/line <=> Acorn Atom mode 4
		{
			mBytesPerLine = 16;
			mPixelsPerByte = 8;
			mPixelWidth = 2 * mBasePixelW;
			mPixelHeight = 1;
			if (mGM == 7) {
				mBytesPerLine = 32;
				mPixelWidth = 1 * mBasePixelW;
			}
			else if (mGM == 1)
				mPixelHeight = 3;
			else if (mGM == 3)
				mPixelHeight = 2;

		}
		mShiftsPerByte = 8 / mPixelsPerByte;
		mRenderedPixelsPerByte = 512 / mBytesPerLine;
	}
	/*
	cout << "GM:                " << dec << (int)mGM << "\n";
	cout << "Bytes/Line:        " << mBytesPerLine << "\n";
	cout << "Pixels/Byte:       " << mPixelsPerByte << "\n";
	cout << "Pixel Width:       " << mPixelWidth << "\n";
	cout << "Base Pixel Width:  " << mBasePixelW << "\n";
	cout << "Pixel Height:      " << mPixelHeight << "\n";
	cout << "Shifts/Byte:       " << mShiftsPerByte << "\n";
	cout << "\n";
	*/

}

void VDU6847::calcLineSettings()
{
	int field_offset = (mField % 2);
	mAdjustedScanLine = mScanLine - field_offset;
	mVisibleLine = mScanLine - mTopBlankingLines;
	mAdjustedVisibleLine = mVisibleLine - field_offset;
	mDisplayedLine = mVisibleLine - mTopBorderLines;
	mAdjustedDisplayedLine = mDisplayedLine - field_offset;

	//cout << "F " << mField << ", SL " << mScanLine << " (" << mAdjustedScanLine << "), VL " << mVisibleLine << " (" << mAdjustedVisibleLine << "), DL " <<
	//	mDisplayedLine << " (" << mAdjustedDisplayedLine << ")\n";
}

void VDU6847::drawPartialBorder(int borderStartPixel, int borderWidth)
{
	int p = mLineRenderedPixels - borderStartPixel;
	for (; p >= 0 && p + mRenderedPixels < borderWidth && mRenderedPixels < 8; mRenderedPixels++) {
		*mBitmapDataP++ = 0xff00ff00; // opaque green ARGB 8888
	}
}
