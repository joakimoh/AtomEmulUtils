#include "TT5050.h"
#include <iostream>
#include <iomanip>
#include <cmath>

using namespace std;

TT5050::TT5050(
	string name, uint16_t adr, double cpuClock, ALLEGRO_BITMAP* disp, uint16_t videoMemAdr, DebugManager  *debugManager, ConnectionManager* connectionManager
) : Device(name, TT_5050_DEV, OTHER_DEVICE, cpuClock, debugManager, connectionManager)
{
	if (mDM->debug(DBG_VERBOSE))
		cout << "Teletext Character Generator SA5050 '" << name << "' added\n";

	registerPort("LOSE",	IN_PORT, 0x1, LOSE,		&mLOSE);
	registerPort("RESET",	IN_PORT, 0x1, RESET,	&mRESET);
	registerPort("CRS",		IN_PORT, 0x1, CRS,		&mCRS);
	registerPort("DEW",		IN_PORT, 0x1, DEW,		&mDEW); // Data Entry Window - resets the row adr counter prior to the display period
	registerPort("GLR",		IN_PORT, 0x1, GLR,		&mGLR); // General Line Reset



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

	// Generate table to stretch 12-pixel-wide raster line into 16 pixels
	for (int i = 0; i < 16; i++) {
		int left_pixel_12 = i * 3 / 4;
		int right_pixel_12 = (i + 1) * 3 / 4;
		double left_pixel_12_fraction = (1.0 - ((double)(i * 3.0 / 4.0) - (int)(i * 3 / 4))) * 4 / 3;
		double right_pixel_12_fraction = ((double)((i + 1) * 3.0 / 4.0) - (int)((i + 1) * 3 / 4)) * 4 / 3;
		if (left_pixel_12_fraction > 1.0)
			left_pixel_12_fraction = 1.0;
		if (right_pixel_12_fraction > 1.0)
			right_pixel_12_fraction = 1.0;
		if (right_pixel_12 == left_pixel_12)
			right_pixel_12_fraction = 0.0;
		uint8_t leftFactor = (uint8_t)round(255 * left_pixel_12_fraction);
		uint8_t rightFactor = (uint8_t)round(255 * right_pixel_12_fraction);
		mStretchMatrix[i] = { left_pixel_12, right_pixel_12, leftFactor, rightFactor };

	}

	if (true && mDM->debug(DBG_VERBOSE)) {
		cout << "Teletext stretch Matrix:\n\n";
		for (int i = 0; i < 16; i++)
			cout << i << ", " << mStretchMatrix[i].srcLeftPixel << ", " << (int)mStretchMatrix[i].leftFactor << ", " <<
			i << ", " << mStretchMatrix[i].srcRightPixel << ", " << (int)mStretchMatrix[i].rightFactor << "\n";
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
// Returns true if data was enabled (LOSE active) and the TCG was properly initialised; otherwise false is returned	
//
bool TT5050::getScreenData(uint8_t pageData, vector <TTColour>& screenData)
{
	vector <uint8_t> screenData12(12);

	if (!initialised()) {
		mCycleCount += max(1, (int)round(mCPUClock / 1.0));
		return true;
	}

	// Advance time 1 us
	mCycleCount += max(1, (int) round(mCPUClock / 1.0));

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
		case TT_CONCEAL:			/*			// Subsequent text in the row will be hidden (displayed as spaces in the current background colour) until			// the next text colour or graphics colour control code is encountered.			*/			mHiddenText = true;			break; 		case TT_CONTIGUOUS_GRAPHICS:			mSeparatedGraphics = false;			break; 		case TT_SEPARATED_GRAPHICS:			mSeparatedGraphics = true;			break;
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

		TTColour background_colour = mBackgroundColour;
		TTColour foreground_colour = mColours[(int)TT_BLACK];

		if (char_data < 0x20) { // invisible control char - decide how to show it
			if (!mHeldGraphics) { // draw hidden text and control char as space as default
				foreground_colour = mAlpaNumericColour;
				symbol_index = 0; // SPACE symbol
			}
			else { // draw control char as last graphics symbol
				foreground_colour = mGraphicsColour;
				symbol_index = mLastGraphicsSymbolIndex;
				mGraphicSymbols = true;
			}
		} 
			
		// Character or graphical symbol to be displayed
		else {

			if (draw_sixels)
				foreground_colour = mGraphicsColour;
			else {
				if (mHiddenText)
					symbol_index = 0;
				else
					symbol_index = char_data - 0x20; // should give an index in the range [0,95]
				if (mGraphicSymbols)
					foreground_colour = mGraphicsColour;
				else
					foreground_colour = mAlpaNumericColour;

				double hz_0_75 = mCPUClock * 1e6 * 0.75;
				int flash_75 = (int)round(hz_0_75 * 0.75);
				int flash_100 = (int)round(hz_0_75);
				if (mFlash && mCycleCount % flash_100 >= flash_75) {
					background_colour = mBackgroundColour;
					foreground_colour = mBackgroundColour;
				}
			}
		}



		if (draw_sixels) {

			// Create one scan line of two "big" pixels (sixels) occupying 2 x 6 actual pixels
			uint8_t left_sixel, right_sixel;

			if (mCharRasterLine < 6) { // 6 raster lines for top sixels
				if (mSeparatedGraphics && (mCharRasterLine == 0 || mCharRasterLine == 5)) {
					left_sixel = right_sixel = 0;
				}
				else {
					left_sixel = (char_data >> 0) & 0x1;
					right_sixel = (char_data >> 1) & 0x1;
				}
			}
			else if (mCharRasterLine < 14) { // 8 raster lines for middle sixels
				if (mSeparatedGraphics && (mCharRasterLine == 6 || mCharRasterLine == 13)) {
					left_sixel = right_sixel = 0;
				}
				else { 
					left_sixel = (char_data >> 2) & 0x1;
					right_sixel = (char_data >> 3) & 0x1;
				}
			}
			else { // 6 raster lines for bottom sixels
				if (mSeparatedGraphics && (mCharRasterLine == 14 || mCharRasterLine == 19)) {
					left_sixel = right_sixel = 0;
				}
				else {
					left_sixel = (char_data >> 4) & 0x1;
					right_sixel = (char_data >> 6) & 0x1;
				}
			}
			if (mSeparatedGraphics) {
				screenData12[0] = 0;
				for (int p = 1; p < 5; p++)
					screenData12[p]= left_sixel;
				screenData12[6] = screenData12[7] = 0;
				for (int p = 1; p < 5; p++)
					screenData12[p+6]= right_sixel;
				screenData12[11] = 0;
			}
			else {
				for (int p = 0; p < 6; p++)
					screenData12[p] = left_sixel;
				for (int p = 0; p < 6; p++)
					screenData12[p+6] = right_sixel;
			}
		}

		else {
			
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
					screenData12[p] = 1;
				}
				else {
					screenData12[p] = 0;
				}
			}
		}


		// Stretch 12-pixel wide sixel/character raster line into 16 pixels
		for (int i = 0; i < 16; i++) {
			uint8_t val = screenData12[mStretchMatrix[i].srcLeftPixel] * mStretchMatrix[i].leftFactor +
				screenData12[mStretchMatrix[i].srcRightPixel] * mStretchMatrix[i].rightFactor;
			TTColour pixel_colour = background_colour;
			if (val > 0) {
				pixel_colour.B = val * foreground_colour.B;
				pixel_colour.G = val * foreground_colour.G;
				pixel_colour.R = val * foreground_colour.R;
			}
			else {
				pixel_colour.B = 255 * background_colour.B;
				pixel_colour.G = 255 * background_colour.G;
				pixel_colour.R = 255 * background_colour.R;
			}
			screenData[i] = pixel_colour;
		}
	}

	return mLOSE;
}

// Process a port update directly (and not just next time the advance() method is called)
void  TT5050::processPortUpdate(int index)
{
	if (index == RESET && mRESET == 0) {
		cout << "TCG reset!\n";
		reset();
	}

	else if (index == GLR) {
		if (mGLR == 1 && pGLR == 0 && mLOSE == 0) {
			// General Line Reset - for a negative GLR pulse when LOSE is low
			mFlash = false;
			mDoubleHeight = false;
			mGraphicSymbols = false;
			mGraphicsColour = mColours[TT_WHITE];
			mAlpaNumericColour = mColours[TT_WHITE];
			mBackgroundColour = mColours[TT_BLACK];
			mHiddenText = false;
			mSeparatedGraphics = false;
			mHeldGraphics = false;
			if (!mNewField)
				mCharRasterLine = (mCharRasterLine + 2) % RASTER_LINES;

			// Start of double-height character half?
			if (mCRS == 1 && mCharRasterLine == 0 || mCRS == 0 && mCharRasterLine == 1) {
				if (mDoubleHeightHalf == 0) // lower half starts
					mDoubleHeightHalf = 1;
				else if (mDoubleHeightHalf == 1) // double-height halfs completed => reset
					mDoubleHeightHalf = -1;
			}
			mNewField = false;
		}
		pGLR = mGLR;
	}

	else if (index == DEW) {
		if (mDEW == 0 && pDEW == 1) {
			mNewField = true;
			// Data Entry Window - high level
			// Resets the row adr counter prior to the display period when inactive (high)
			if (mCRS == 0) // even field
				mCharRasterLine = 0;
			else // odd field
				mCharRasterLine = 1;
			mDoubleHeightHalf = -1;
		}
		pDEW = mDEW;
	}

	else if (index == LOSE && mLOSE == 1) {	
	}
}