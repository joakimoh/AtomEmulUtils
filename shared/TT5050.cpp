#include "TT5050.h"
#include <iostream>

using namespace std;

TT5050::TT5050(
	string name, uint16_t adr, double cpuClock, ALLEGRO_BITMAP* disp, uint16_t videoMemAdr, DebugManager  *debugManager, ConnectionManager* connectionManager
) : Device(name, TT_5050_DEV, OTHER_DEVICE, cpuClock, debugManager, connectionManager)
{
	if (mDM->debug(DBG_VERBOSE))
		cout << "Teletext Character Generator SA5050 '" << name << "' added\n";

	registerPort("LOSE",	IN_PORT, 0x1, LOSE, &mLOSE);
	registerPort("RESET",	IN_PORT, 0x1, RESET, &mRESET);
	registerPort("CRS",		IN_PORT, 0x1, CRS, &mCRS);

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


	if (false && mDM->debug(DBG_VERBOSE)) {

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
	Device::reset();

	mScanLine = 0;
	mCharRowPos = 0;
	mCharRasterLine = 0;

	return true;
}

// Advance until clock cycle stopcycle has been reached
bool TT5050::advance(uint64_t stopCycle)
{
	bool reset_transition = (mRESET == 0 && mRESET != pRESET);
	pRESET = mRESET;

	if (reset_transition)
		reset();


	return true;
}

//
// Generate RGB data from page memory data
// 
// Called by the display unit fetching the data from the page memory on 1 us character basis
// 
// pageData:			Data read from the page memory
// screenData:			One scan line of one of the following:
//						- 12 x 20 pixel symbol interpolated from an 6 x 10 symbolc, or
//						- 2 x 3 graphics encoded as 12 x 20 pixels
// 
//				
//
bool TT5050::getScreenData(bool HS, bool VS, uint8_t pageData, vector <TTColour> &screenData)
{
	const int n_raster_lines = 20;

	if (!initialised()) {
		mCycleCount += max(1, (int)round(mCPUClock / 1.0));
		return true;
	}

	// Advance time 1 us
	mCycleCount += max(1, (int) round(mCPUClock / 1.0));

	bool even_field = (mCRS == 1);
	
	// start of field => reset scan line, raster line & char row counters
	if (VS) { 
		if (even_field) {
			mScanLine = 0;
			mCharRasterLine = 0;
		}
		else { // odd field
			mScanLine = 1;
			mCharRasterLine = 1;
		}
		mCharRowPos = 0;	
		mDoubleHeightHalf = -1;
	}
	
	// start of line => reset char pos
	if (HS) {
		mFlash = false;
		mDoubleHeight = false;
		mGraphicSymbols = false;
		mGraphicsColour = mColours[TT_WHITE];
		mAlpaNumericColour = mColours[TT_WHITE];
		mBackgroundColour = mColours[TT_BLACK];
		mCharRowPos = 0;
		mHiddenText = false;
		mSeparatedGraphics = false;
		mHeldGraphics = false;
		if (!VS) {
			mCharRasterLine = (mCharRasterLine + 2) % n_raster_lines;
			mScanLine += 2;
		}
		// start of double-height character half?
		if (even_field && mCharRasterLine == 0 || !even_field && mCharRasterLine == 1) {
			if (mDoubleHeightHalf == 0) // lower half starts
				mDoubleHeightHalf = 1;
			else if (mDoubleHeightHalf == 1) // double-height halfs completed => reset
				mDoubleHeightHalf = -1;
		}
	}



	// Only the lower 7 bits are connected to the TCG
	uint8_t char_data = pageData & 0x7f;
	bool draw_sixels = mGraphicSymbols && (char_data & 0x20) != 0;
	// Check for teletext control codes	
	if (char_data < 0x20) {
		switch (char_data) {
		case TT_NULL:
			break;
		case TT_ALPHA_RED:			mAlpaNumericColour = mColours[TT_RED];			mHiddenText = false;			mGraphicSymbols = false;			break; 		case TT_ALPHA_GREEN:			mAlpaNumericColour = mColours[TT_GREEN];			mHiddenText = false;			mGraphicSymbols = false;			break; 		case TT_ALPHA_YELLOW:			mAlpaNumericColour = mColours[TT_YELLOW];			mHiddenText = false;			mGraphicSymbols = false;			break; 		case TT_ALPHA_BLUE:
			mAlpaNumericColour = mColours[TT_BLUE];
			mHiddenText = false;
			mGraphicSymbols = false;
			break;
		case TT_ALPHA_MAGENTA:			mAlpaNumericColour = mColours[TT_MAGENTA];			mHiddenText = false;			mGraphicSymbols = false;			break; 		case TT_ALPHA_CYAN:			mAlpaNumericColour = mColours[TT_CYAN];			mHiddenText = false;			mGraphicSymbols = false;			break; 		case TT_ALPHA_WHITE:
			mAlpaNumericColour = mColours[TT_WHITE];
			mHiddenText = false;
			mGraphicSymbols = false;
			break;
		case TT_FLASH:			/*			// Flash characters with 0.75 Hz (3:1 on/off ratio).			// The foreground and background colours are interchanged when flashing.			*/			mFlash = true;			break; 		case TT_STEADY:
			mFlash = false;
			break;
			// These codes are not used
		case TT_END_BOX: 		case TT_START_BOX:			break;
		case TT_NORMAL_HEIGHT:			mDoubleHeight = false;			break; 		case TT_DOUBLE_HEIGHT:			/*			// Double-height characters are split onto rows where the first row			// contains the upper half of the double-height character and the second			// row contains the lower half ot the same double-height character.			// The first row of a pair of double-height characters can mix single			// and double-height characters whereas the second row cannt (if mixed			// the single-height ones will become invisible).			*/			mDoubleHeight = true;			if (mDoubleHeightHalf == -1)				mDoubleHeightHalf = 0;			break;
			// These codes are not used
		case TT_S0: 		case TT_S1:	
		case TT_DLE:			break;
		case TT_GRAPHICS_RED:			mGraphicsColour = mColours[TT_RED];			mHiddenText = false;			mGraphicSymbols = true;			break; 		case TT_GRAPHICS_GREEN:			mGraphicsColour = mColours[TT_GREEN];			mHiddenText = false;			mGraphicSymbols = true;			break; 		case TT_GRAPHICS_YELLOW:			mGraphicsColour = mColours[TT_YELLOW];			mHiddenText = false;			mGraphicSymbols = true;			break; 		case TT_GRAPHICS_BLUE:			mGraphicsColour = mColours[TT_BLUE];			mHiddenText = false;			mGraphicSymbols = true;			break;
		case TT_GRAPHICS_MAGENTA:			mGraphicsColour = mColours[TT_MAGENTA];			mHiddenText = false;			mGraphicSymbols = true;			break; 		case TT_GRAPHICS_CYAN:			mGraphicsColour = mColours[TT_CYAN];			mHiddenText = false;			mGraphicSymbols = true;			break; 		case TT_GRAPHICS_WHITE:			mGraphicsColour = mColours[TT_WHITE];			mHiddenText = false;			mGraphicSymbols = true;			break;
		case TT_CONCEAL:			// Subsequent text in the row will be hidden (displayed as spaces in the current background colour) until			// the next text colour or graphics colour control code is encountered.			mHiddenText = true;			break; 		case TT_CONTIGUOUS_GRAPHICS:			mSeparatedGraphics = false;			break; 		case TT_SEPARATED_GRAPHICS:			mSeparatedGraphics = true;			break;
		case TT_ESC:			break; 		case TT_BLACK_BACKGROUND:			mBackgroundColour = mColours[TT_BLACK];			break; 		case TT_NEW_BACKGROUND:			mBackgroundColour = mAlpaNumericColour;			break;
		case TT_HOLD_GRAPHICS:			// In the held graphics mode, control codes are displayed as a copy of the most recently displayed graphics symbol.			mHeldGraphics = true;			break;		case TT_RELEASE_GRAPHICS:
			mHeldGraphics = false;
			break;
		default:
			break;
		}
	}

	// Generate output
	if (mLOSE) {
		uint8_t symbol_index = 0x0;
		TT5050::TTColour colour = mColours[(int)TT_BLACK];
		if (char_data < 0x20) { // invisible control char - decide how to show it
			if (!mHeldGraphics) { // draw hidden text and control char as space as default
				colour = mAlpaNumericColour;
				symbol_index = 0; // SPACE symbol
			}
			else { // draw control char as last graphics symbol
				colour = mGraphicsColour;
				symbol_index = mLastGraphicsSymbolIndex;
				mGraphicSymbols = true;
			}
		} 
			
		// Character or graphical symbol to be displayed
		else {
			
			if (draw_sixels)
				colour = mGraphicsColour;
			else {
				if (mHiddenText)
					symbol_index = 0;
				else
					symbol_index = char_data - 0x20; // should give an index in the range [0,95]
				if (mGraphicSymbols)
					colour = mGraphicsColour;
				else
					colour = mAlpaNumericColour;
			}
		}

		if (draw_sixels) {
			// Create one scan line of two "big" pixels (sixels) occupying 2 x 6 actual pixels
			uint8_t left_sixel, right_sixel;

			if (mCharRasterLine <= 4) {
				left_sixel = (char_data >> 0) & 0x1;
				right_sixel = (char_data >> 1) & 0x1;
			}
			else if (mCharRasterLine <= 12) {
				left_sixel = (char_data >> 2) & 0x1;
				right_sixel = (char_data >> 3) & 0x1;
			}
			else {
				left_sixel = (char_data >> 4) & 0x1;
				right_sixel = (char_data >> 6) & 0x1;
			}
			TT5050::TTColour left_sixel_colour = (left_sixel == 1) ? colour : mBackgroundColour;
			TT5050::TTColour right_sixel_colour = (right_sixel == 1)? colour : mBackgroundColour;
			for (int p = 0; p < 6; p++)
				screenData.push_back(left_sixel_colour);
			for (int p = 0; p < 6; p++)
				screenData.push_back(right_sixel_colour);
		}

		else {

			TTColour background_colour = mBackgroundColour;
			TTColour foreground_colour = colour;// mAlpaNumericColour;
			double hz_0_75 = mCPUClock * 1e6 * 0.75;
			int flash_75 = (int)round(hz_0_75 * 0.75);
			int flash_100 = (int)round(hz_0_75);
			if (mFlash && mCycleCount % flash_100 >= flash_75) {
				background_colour = mBackgroundColour;
				foreground_colour = mBackgroundColour;
			}
			// Create 12 pixels of the correct colour
			int raster_line = mCharRasterLine;
			if (mDoubleHeight) {
				if (mDoubleHeightHalf == 0)
					raster_line = mCharRasterLine / 2;
				else // mDoubleHeightHalf == 1
					raster_line = 10 + mCharRasterLine / 2;
			}

			// Normal-height characters on the second half of a double-height pair of rows are invisible
			if (mDoubleHeightHalf == 1 && !mDoubleHeight)
				symbol_index = 0; // space <=> invisible character

			for (int p = 0; p < 12; p++) {
				if (mInterpolatedSymbolRasterBits[symbol_index][raster_line][p]) {
					screenData.push_back(foreground_colour);
				}
				else {
					screenData.push_back(background_colour);
				}
			}
		}

	}

	mCharRowPos++; // if it was the last char pos, this will be corrected at the next call of getScreenData() based on the HS input

	return true;
}