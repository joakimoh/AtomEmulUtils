#include "TT5050.h"
#include <iostream>

using namespace std;

TT5050::TT5050(
	string name, uint16_t adr, double clockSpeed, ALLEGRO_BITMAP* disp, uint16_t videoMemAdr, DebugInfo debugInfo, ConnectionManager* connectionManager
) : Device(name, TT_5050_DEV, OTHER_DEVICE, debugInfo, connectionManager), mSystemClock(clockSpeed)
{
	if (mDebugInfo.dbgLevel & DBG_VERBOSE)
		cout << "Teletext Character Generator SA5050 '" << name << "' added\n";

	registerPort("DEW", IN_PORT, 0x1, DEW, &mDEW);
	registerPort("CLR", IN_PORT, 0x1, CLR, &mCLR);
	registerPort("LOSE", IN_PORT, 0x1, LOSE, &mLOSE);


}

// Reset device
bool TT5050::reset()
{
	return true;
}

// Advance until clock cycle stopcycle has been reached
bool TT5050::advance(uint64_t stopCycle)
{
	return true;
}

//
// Generate RGB data from page memory data
// 
// Called by the display unit fetching the data from the page memory on 1 us character basis
// 
// pageData:	Data read from the page memory
// charPixels:	12 × 20 pixels, hardware defined (interpolated from 6 × 10 matrix)
//				and the output is based on the page data, the current scan line & scan line character pos
// 
//				
//
bool TT5050::updateDataOutput(uint8_t pageData, vector <uint32_t> &charPixels)
{
	if (!initialised()) {
		mCycleCount += max(1, (int)round(mSystemClock / 1.0));
		return true;
	}

	// Advance time 1 us
	mCycleCount += max(1, (int) round(mSystemClock / 1.0));

	// Check for start of frame
	if (mDEW && mDEW != pDEW) {
		mScanLine = 0;
		mCharRowPos = 0;
		mCharRasterLine = 0;
	}
	pDEW = mDEW;

	// Check for start of line
	if (mCLR && mCLR != pCLR) { // 
		mCharRowPos = 0;
	}
	pCLR = mCLR;

	uint8_t char_data = (pageData >> 1) & 0x7f;

	if (char_data < 0x20) {
		switch (char_data) {
		case TT_NULL:
			break;
		case TT_ALPHA_RED:			mAlpaNumericColour = mColours[TT_RED];			mHiddenText = false;			break; 		case TT_ALPHA_GREEN:			mAlpaNumericColour = mColours[TT_GREEN];			mHiddenText = false;			break; 		case TT_ALPHA_YELLOW:			mAlpaNumericColour = mColours[TT_YELLOW];			mHiddenText = false;			break; 		case TT_ALPHA_BLUE:
			mAlpaNumericColour = mColours[TT_BLUE];
			mHiddenText = false;
			break;
		case TT_ALPHA_MAGENTA:			mAlpaNumericColour = mColours[TT_MAGENTA];			mHiddenText = false;			break; 		case TT_ALPHA_CYAN:			mAlpaNumericColour = mColours[TT_CYAN];			mHiddenText = false;			break; 		case TT_ALPHA_WHITE:
			mAlpaNumericColour = mColours[TT_WHITE];
			mHiddenText = false;
			break;
		case TT_FLASH:			mFlash = true;			break; 		case TT_STEADY:
			mFlash = false;
			break;
			// These codes are not used
		case TT_END_BOX: 		case TT_START_BOX:			break;
		case TT_NORMAL_HEIGHT:			mDoubleHeight = false;			break; 		case TT_DOUBLE_HEIGHT:			mDoubleHeight = true;			break;
			// These codes are not used
		case TT_S0: 		case TT_S1:	
		case TT_DLE:			break;
		case TT_GRAPHICS_RED:			mGraphicsColour = mColours[TT_RED];			mHiddenText = false;			break; 		case TT_GRAPHICS_GREEN:			mGraphicsColour = mColours[TT_GREEN];			mHiddenText = false;			break; 		case TT_GRAPHICS_YELLOW:			mGraphicsColour = mColours[TT_YELLOW];			mHiddenText = false;			break; 		case TT_GRAPHICS_BLUE:			mGraphicsColour = mColours[TT_BLUE];			mHiddenText = false;			break;
		case TT_GRAPHICS_MAGENTA:			mGraphicsColour = mColours[TT_MAGENTA];			mHiddenText = false;			break; 		case TT_GRAPHICS_CYAN:			mGraphicsColour = mColours[TT_CYAN];			mHiddenText = false;			break; 		case TT_GRAPHICS_WHITE:			mGraphicsColour = mColours[TT_WHITE];			mHiddenText = false;			break;
		case TT_CONCEAL:			// Subsequent text in the row will be hidden (displayed as spaces in the current background colour) until			// the next text colour or graphics colour control code is encountered.			mHiddenText = true;			break; 		case TT_CONTIGUOUS_GRAPHICS:			mSeparatedGraphics = false;			break; 		case TT_SEPARATED_GRAPHICS:			mSeparatedGraphics = true;			break;
		case TT_ESC:			break; 		case TT_BLACK_BACKGROUND:			mBackgroundColour = mColours[TT_BLACK];			break; 		case TT_NEW_BACKGROUND:			break;
		case TT_HOLD_GRAPHICS:			// In the held graphics mode, control codes are displayed as a copy of the most recently displayed graphics symbol.			mHeldGraphics = true;			break;		case TT_RELEASE_GRAPHICS:
			mHeldGraphics = false;
			break;
		default:
			break;
		}
	}

	// Generate output
	if (mLOSE)  {
		uint8_t char_raster_data;
		uint8_t symbol_index = char_data - 0x20; // should give an index in the range [0,95]
		uint32_t colour = mColours[TT_BLACK];
		if (!mHiddenText || char_data < 0x20) { // draw hidden text and control char as space as default
			colour = mColours[mAlpaNumericColour];
			char_raster_data = mSymbols[0x20].rows[mCharRasterLine];
		}
		else if (mHiddenText && char_data >= 0x20) { // draw control char as last graphics symbol
			colour = mColours[mGraphicsColour];
			char_raster_data = mSymbols[mLastGraphicsSymbolIndex].rows[mCharRasterLine];
		}
		// Character or graphical symbol to be displayed
		else if (!mGraphics) {
			colour = mColours[mAlpaNumericColour];
			char_raster_data = mSymbols[symbol_index].rows[mCharRasterLine];
		}
		else {
			colour = mColours[mGraphicsColour];
			uint8_t b1b0;
			if (mCharRasterLine <= 2)
				b1b0 = symbol_index & 0x3;
			else if (mCharRasterLine <= 5)
				b1b0 = (symbol_index >> 2) & 0x3;
			else
				b1b0 = ((symbol_index >> 3) & 0x2) | ((symbol_index >> 6) & 0x1);
			char_raster_data = ((b1b0 & 0x2) ? 0x38 : 0x00) | ((b1b0 & 0x1) ? 0x7 : 0x0);
		}
		// Create 6 pixels of the correct colour
		for (int p = 0; p < 6; p++)
			charPixels.push_back(colour);
	}

	mCharRowPos = (mCharRowPos + 1) % 64;
	if (mCharRowPos == 0) {
		mScanLine = (mScanLine + 1) % 312;
		mCharRasterLine = (mCharRasterLine + 1) % 10;
	}

	return true;
}