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
	registerPort("CRS", IN_PORT, 0x1, CRS, &mCRS);

	createInterpolatedSymbols();

}

void TT5050::createInterpolatedSymbols()
{

	// Make temp symbol table with bit resolution
	for (int symbol = 0; symbol < mSymbols.size(); symbol++) {
		for (int row = 0; row < 10; row++) {
			uint8_t raster = mSymbols[symbol].rows[row];
			for (int col = 0; col < 6; col++) {
				if (raster & 0x20)
					mSymbolRasterBits[symbol][row][col] = 1;
				else
					mSymbolRasterBits[symbol][row][col] = 0;
				raster = raster << 1;
			}
		}
	}

	// Create new interpolated symbol table
	for (int symbol = 0; symbol < mSymbols.size(); symbol++) {

		auto& bits = mSymbolRasterBits[symbol];
		auto& i_bits = mInterpolatedSymbolRasterBits[symbol];

		// Create the "big" pixels
		for (int row = 0; row < 10; row++) {

			for (int col = 0; col < 6; col++) {

				uint8_t b = bits[row][col];

				// set "big" pixel based on "small" pixel			
				i_bits[row * 2][col * 2] = i_bits[row * 2][col * 2 + 1] = i_bits[row * 2 + 1][col * 2] = i_bits[row * 2 + 1][col * 2 + 1] = b;

				


			}
		}

		// Round the corners
		for (int row = 0; row < 10; row++) {

			for (int col = 0; col < 6; col++) {

				uint8_t b = bits[row][col];

				// add interpolated pixel for north-west to south-east dialogonal
				if (row >= 1 && row <= 9 && col >= 1) {
					uint8_t b_NW = bits[row - 1][col - 1];
					uint8_t b_N = bits[row - 1][col];
					uint8_t b_W = bits[row][col - 1];
					if (b_NW && b && !b_W && !b_N)
						i_bits[row * 2 - 1][col * 2] = i_bits[row * 2][col * 2 - 1] = 0x1;
				}
				// add interpolated pixel for north-east to south-west dialogonal
				if (row >= 1 && row <= 9 && col <= 5) {
					uint8_t b_NE = bits[row - 1][col + 1];
					uint8_t b_E = bits[row][col + 1];
					uint8_t b_N = bits[row - 1][col];
					if (b_NE && b && !b_E && !b_N)
						i_bits[row * 2 - 1][col * 2 + 1] = i_bits[row * 2][col * 2 + 2] = 0x1;
				}
			}
		}

	}


	if (mDebugInfo.dbgLevel & DBG_VERBOSE) {

		cout << "Non-interpolated and interpolated symbols:\n\n";

		for (int s = 0; s < 96; s++) {

			cout << mSymbols[s].asc << " (0x" << (int) mSymbols[s].asc << ")\n";

			for (int r = 0; r < 10; r++) {
				for (int c = 0; c < 6; c++) {
					cout << (mSymbolRasterBits[s][r][c] ? "x" : " ");
				}
				cout << "\n";
			}
			cout << "\n";

			for (int r = 0; r < 20; r++) {
				for (int c = 0; c < 12; c++) {
					cout << (mInterpolatedSymbolRasterBits[s][r][c] ? "x" : " ");
				}
				cout << "\n";
			}

			cout << "\n";
		}
	}
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
// pageData:			Data read from the page memory
// interpolatedChar:	true if pixel symbol is interpolated
// screenData:			One scan line of one of the following:
//						- 6 x 10 pixel symbol
//						- 12 x 20 pixel symbol interpolated from an 6 x 10 symbolc, or
//						- 2 x 3 graphics encoded as 6 x 10 pixels
// 
//				
//
bool TT5050::getScreenData(uint8_t pageData, bool &interpolatedChar, vector <TTColour> &screenData)
{
	if (!initialised()) {
		mCycleCount += max(1, (int)round(mSystemClock / 1.0));
		return true;
	}

	// Advance time 1 us
	mCycleCount += max(1, (int) round(mSystemClock / 1.0));

	if (mCRS)
		interpolatedChar = true;
	else
		interpolatedChar = false;

	// Check for start of frame
	if (mDEW && mDEW != pDEW) { // start of frame => reset scan line, raster line & char row counters
		mScanLine = 0;
		mCharRowPos = 0;
		mCharRasterLine = 0;
	}
	pDEW = mDEW;

	// Check for start of line
	if (mCLR && mCLR != pCLR) { // 
		mCharRowPos = 0;
		int n_raster_lines = (interpolatedChar ? 20 : 10);
		if (mScanLine > 0) { // start of new line (and not of new frame) => increase scan line counter
			mScanLine++;
			mCharRasterLine = (mCharRasterLine + 1) % n_raster_lines;
		}

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
	if (mLOSE) {
		uint16_t char_raster_data;
		uint8_t symbol_index = char_data - 0x20; // should give an index in the range [0,95]
		TT5050::TTColour colour = mColours[(int)TT_BLACK];
		if (!mHiddenText || char_data < 0x20) { // draw hidden text and control char as space as default
			colour =mAlpaNumericColour;
			char_raster_data = mSymbols[0x20].rows[mCharRasterLine];
		}
		else if (mHiddenText && char_data >= 0x20) { // draw control char as last graphics symbol
			colour = mGraphicsColour;
			char_raster_data = mSymbols[mLastGraphicsSymbolIndex].rows[mCharRasterLine];
		}
		// Character or graphical symbol to be displayed
		else if (!mGraphics) {
			colour = mAlpaNumericColour;
		}
		else {
			colour = mGraphicsColour;
			// Create one scan line of two "big" pixels occupying 6 actual pixels
			uint8_t b1b0;
			if (mCharRasterLine <= 2)
				b1b0 = symbol_index & 0x3;
			else if (mCharRasterLine <= 5)
				b1b0 = (symbol_index >> 2) & 0x3;
			else
				b1b0 = ((symbol_index >> 3) & 0x2) | ((symbol_index >> 6) & 0x1);
			TT5050::TTColour b1_colour = (b1b0 & 0x2) ? colour : mBackgroundColour;
			TT5050::TTColour b0_colour = (b1b0 & 0x1) ? colour : mBackgroundColour;
			for (int p = 0; p < 3; p++)
				screenData.push_back(b1_colour);
			for (int p = 0; p < 3; p++)
				screenData.push_back(b0_colour);
		}

		if (interpolatedChar) {
			// Create 12 pixels of the correct colour
			for (int p = 0; p < 12; p++) {
				if (mInterpolatedSymbolRasterBits[symbol_index][mCharRasterLine][p])
					screenData.push_back(colour);
				else
					screenData.push_back(mBackgroundColour);
			}
		}
		else {
			// Create 6 pixels of the correct colour
			for (int p = 0; p < 6; p++) {
				if (mSymbolRasterBits[symbol_index][mCharRasterLine][p])
					screenData.push_back(colour);
				else
					screenData.push_back(mBackgroundColour);
			}
		}
	}

	mCharRowPos++; // if it was the last char pos, this will be corrected at the next call of getScreenData() based on the CLR input

	return true;
}