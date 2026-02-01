#include "TT5050.h"
#include <iostream>
#include <iomanip>
#include <cmath>
#include "Utility.h"
#include "Display.h"

using namespace std;

TT5050::TT5050(
	string name, uint16_t adr, double tickRate, uint16_t videoMemAdr, DebugTracing  *debugTracing, ConnectionManager* connectionManager
) : Device(name, TT_5050_DEV, VDU_DEVICE, debugTracing, connectionManager), TimedDevice(tickRate)
{
	if (VERBOSE_EXT_OUTPUT)
		cout << "Teletext Character Generator SA5050 '" << name << "' added\n";

	registerPort("LOSE",	IN_PORT, 0x1, LOSE,		&mLOSE);
	registerPort("CRS",		IN_PORT, 0x1, CRS,		&mCRS);
	registerPort("DEW",		IN_PORT, 0x1, DEW,		&mDEW); // Data Entry Window - resets the row adr counter prior to the display period
	registerPort("GLR",		IN_PORT, 0x1, GLR,		&mGLR); // General Line Reset

	createInterpolatedSymbols();
	stretchTo16Pixels();

}

TT5050::~TT5050()
{
	if (pSymbolRasterBits != nullptr)
		delete pSymbolRasterBits;

	if (pInterpolatedSymbolRasterBits != nullptr)
		delete pInterpolatedSymbolRasterBits;

	if (pStretchedSymbolRasterBits != nullptr)
		delete pStretchedSymbolRasterBits;
}

void TT5050::createInterpolatedSymbols()
{
	pSymbolRasterBits = new SymbolRasterBits();
	auto& mSymbolRasterBits = pSymbolRasterBits->data;

	pInterpolatedSymbolRasterBits = new InterpolatedSymbolRasterBits();
	auto& mInterpolatedSymbolRasterBits = pInterpolatedSymbolRasterBits->data;

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

	if (VERBOSE_EXT_OUTPUT) {
		cout << "Teletext stretch Matrix:\n\n";
		for (int i = 0; i < 16; i++)
			cout << i << ", " << mStretchMatrix[i].srcLeftPixel << ", " << (int)mStretchMatrix[i].leftFactor << ", " <<
			i << ", " << mStretchMatrix[i].srcRightPixel << ", " << (int)mStretchMatrix[i].rightFactor << "\n";
	}

	if (VERBOSE_EXT_OUTPUT) {

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
void TT5050::createSixels12(uint8_t separated_graphics, uint8_t char_data, uint8_t raster_line, vector <uint8_t>& screenData12)
{
	//
	// Create one raster line made up of a graphical symbol that is made up of two "big" pixels (sixels) occupying 2 x 6 actual pixels
	//
	uint8_t left_sixel, right_sixel;

	if (raster_line < 6) { // 6 raster lines for top sixels
		if (separated_graphics && (raster_line == 0 || raster_line == 5)) {
			left_sixel = right_sixel = 0;
		}
		else {
			left_sixel = (char_data >> 0) & 0x1;
			right_sixel = (char_data >> 1) & 0x1;
		}
	}
	else if (raster_line < 14) { // 8 raster lines for middle sixels
		if (separated_graphics && (raster_line == 6 || raster_line == 13)) {
			left_sixel = right_sixel = 0;
		}
		else {
			left_sixel = (char_data >> 2) & 0x1;
			right_sixel = (char_data >> 3) & 0x1;
		}
	}
	else { // 6 raster lines for bottom sixels
		if (separated_graphics && (raster_line == 14 || raster_line == 19)) {
			left_sixel = right_sixel = 0;
		}
		else {
			left_sixel = (char_data >> 4) & 0x1;
			right_sixel = (char_data >> 6) & 0x1;
		}
	}
	if (separated_graphics) {
		screenData12[0] = 0;
		for (int p = 1; p < 5; p++)
			screenData12[p] = left_sixel;
		screenData12[6] = screenData12[7] = 0;
		for (int p = 1; p < 5; p++)
			screenData12[p + 6] = right_sixel;
		screenData12[11] = 0;
	}
	else {
		for (int p = 0; p < 6; p++)
			screenData12[p] = left_sixel;
		for (int p = 0; p < 6; p++)
			screenData12[p + 6] = right_sixel;
	}

}

void TT5050::stretchTo16Pixels()
{
	vector <uint8_t> screenData12(12);
	pStretchedSymbolRasterBits = new StretchedSymbolRasterBits();
	auto& mStretchedSymbolRasterBits = pStretchedSymbolRasterBits->data;
	auto& mInterpolatedSymbolRasterBits = pInterpolatedSymbolRasterBits->data;
	//
	// Stretch 12-pixel wide sixel/character raster line into 16 pixels
	// Also scale the pixel values from the initial range 0:1 to the range 0:255
	//
	for (int symbol_index = 0; symbol_index < 224; symbol_index++) {
		for (int raster_line = 0; raster_line < 20; raster_line++) {
			vector <uint8_t> screenData12(12, 0);
			if (symbol_index >= 96) {
				int sixel_symbol_index = (symbol_index - 96) % 64;
				uint8_t separated_graphics = (symbol_index - 96) / 64;
				uint8_t char_data = (sixel_symbol_index < 0x20 ? sixel_symbol_index + 0x20 : sixel_symbol_index + 0x40);
				createSixels12(separated_graphics, char_data, raster_line, screenData12);
			}
			for (int p = 0; p < 12; p++) {
				for (int i = 0; i < 16; i++) {
					uint8_t v1, v2;
					if (symbol_index < 96) {
						v1 = mInterpolatedSymbolRasterBits[symbol_index][raster_line][mStretchMatrix[i].srcLeftPixel];
						v2 = mInterpolatedSymbolRasterBits[symbol_index][raster_line][mStretchMatrix[i].srcRightPixel];
					}
					else {
						v1 = screenData12[mStretchMatrix[i].srcLeftPixel];
						v2 = screenData12[mStretchMatrix[i].srcRightPixel];
					}
					int val = v1 * mStretchMatrix[i].leftFactor + v2 * mStretchMatrix[i].rightFactor;
					if (val > 0)
						mStretchedSymbolRasterBits[symbol_index][raster_line][i] = val;
					else
						mStretchedSymbolRasterBits[symbol_index][raster_line][i] = 0;
				}
			}
		}
	}
}

// Advance until time tickTime
bool TT5050::advanceUntil(uint64_t tickTime)
{
	// Currently not intended to be used as getScreenData() should instead be called periodically

	return true;
}

//
// Generate RGB data from page memory data
// 
// Called by the display unit fetching the data from the page memory on 1 us character basis
// 
// pageData:			Data read from the page memory
// screenData:			One character column of a scan line made up of one of the following:
//						- 16 x 20 pixel symbol interpolated from an 6 x 10 symbolic, or
//						- 2 x 3 sixels symbol encoded as 16 x 20 pixels
// 
// Returns true if data was enabled (LOSE active) and the TCG was properly initialised; otherwise false is returned	
//
bool TT5050::getScreenData(uint8_t pageData, ScreenDataType* &screenData, TTColour& bgColour, TTColour& fgColour)
{

	// Advance time 1 us
	mTicks += max(1, (int) round(mTickRate / 1.0));

	if (mLOSE) {

		// Only the lower 7 bits are connected to the TCG
		uint8_t char_data = pageData & 0x7f;
		bool draw_sixels = mGraphicSymbols && (char_data & 0x20) != 0;
		// Check for teletext control codes	
		if (char_data < 0x20) {
			switch (char_data) {
			case TT_NULL:
				break;
			case TT_ALPHA_RED:
				mAlpaNumericColour = mColours[TT_RED];
				mHiddenText = false;
				mGraphicSymbols = false;
				break;
			case TT_ALPHA_GREEN:
				mAlpaNumericColour = mColours[TT_GREEN];
				mHiddenText = false;
				mGraphicSymbols = false;
				break;
			case TT_ALPHA_YELLOW:
				mAlpaNumericColour = mColours[TT_YELLOW];
				mHiddenText = false;
				mGraphicSymbols = false;
				break;
			case TT_ALPHA_BLUE:
				mAlpaNumericColour = mColours[TT_BLUE];
				mHiddenText = false;
				mGraphicSymbols = false;
				break;
			case TT_ALPHA_MAGENTA:
				mAlpaNumericColour = mColours[TT_MAGENTA];
				mHiddenText = false;
				mGraphicSymbols = false;
				break;
			case TT_ALPHA_CYAN:
				mAlpaNumericColour = mColours[TT_CYAN];
				mHiddenText = false;
				mGraphicSymbols = false;
				break;
			case TT_ALPHA_WHITE:
				mAlpaNumericColour = mColours[TT_WHITE];
				mHiddenText = false;
				mGraphicSymbols = false;
				break;
			case TT_FLASH:
				/*
				// Flash characters with 0.75 Hz (3:1 on/off ratio).
				// The foreground and background colours are interchanged when flashing.
				*/
				mFlash = true;
				break;
			case TT_STEADY:
				mFlash = false;
				break;
				// These codes are not used
			case TT_END_BOX:
			case TT_START_BOX:
				break;
			case TT_NORMAL_HEIGHT:
				mDoubleHeight = false;
				break;
			case TT_DOUBLE_HEIGHT:
				/*
				// Double-height characters are split onto rows where the first row
				// contains the upper half of the double-height character and the second
				// row contains the lower half ot the same double-height character.
				// The first row of a pair of double-height characters can mix single
				// and double-height characters whereas the second row cannt (if mixed
				// the single-height ones will become invisible).
				*/
				mDoubleHeight = true;
				mDoubleHeightLine = true;
				break;
				// These codes are not used
			case TT_S0:
			case TT_S1:
			case TT_DLE:
				break;
			case TT_GRAPHICS_RED:
				mGraphicsColour = mColours[TT_RED];
				mHiddenText = false;
				mGraphicSymbols = true;
				break;
			case TT_GRAPHICS_GREEN:
				mGraphicsColour = mColours[TT_GREEN];
				mHiddenText = false;
				mGraphicSymbols = true;
				break;
			case TT_GRAPHICS_YELLOW:
				mGraphicsColour = mColours[TT_YELLOW];
				mHiddenText = false;
				mGraphicSymbols = true;
				break;
			case TT_GRAPHICS_BLUE:
				mGraphicsColour = mColours[TT_BLUE];
				mHiddenText = false;
				mGraphicSymbols = true;
				break;
			case TT_GRAPHICS_MAGENTA:
				mGraphicsColour = mColours[TT_MAGENTA];
				mHiddenText = false;
				mGraphicSymbols = true;
				break;
			case TT_GRAPHICS_CYAN:
				mGraphicsColour = mColours[TT_CYAN];
				mHiddenText = false;
				mGraphicSymbols = true;
				break;
			case TT_GRAPHICS_WHITE:
				mGraphicsColour = mColours[TT_WHITE];
				mHiddenText = false;
				mGraphicSymbols = true;
				break;
			case TT_CONCEAL:
				/*
				// Subsequent text in the row will be hidden (displayed as spaces in the current background colour) until
				// the next text colour or graphics colour control code is encountered.
				*/
				mHiddenText = true;
				break;
			case TT_CONTIGUOUS_GRAPHICS:
				mSeparatedGraphics = false;
				break;
			case TT_SEPARATED_GRAPHICS:
				mSeparatedGraphics = true;
				break;
			case TT_ESC:
				break;
			case TT_BLACK_BACKGROUND:
				mBackgroundColour = mColours[TT_BLACK];
				break;
			case TT_NEW_BACKGROUND:
				mBackgroundColour = mAlpaNumericColour;
				break;
			case TT_HOLD_GRAPHICS:
				// In the held graphics mode, control codes are displayed as a copy of the most recently displayed graphics symbol.
				mHeldGraphics = true;
				break;
			case TT_RELEASE_GRAPHICS:
				mHeldGraphics = false;
				break;
			default:
				break;
			}
		}

		// Generate output

		uint8_t symbol_index = 0x0;

		bgColour = mBackgroundColour;
		fgColour = mColours[(int)TT_BLACK];

		if (char_data < 0x20) { // invisible control char - decide how to show it
			if (!mHeldGraphics) { // draw hidden text and control char as space as default
				fgColour = mAlpaNumericColour;
				symbol_index = 0; // SPACE symbol
			}
			else { // draw control char as last graphics symbol
				fgColour = mGraphicsColour;
				symbol_index = mLastGraphicsSymbolIndex;
				mGraphicSymbols = true;
			}
		}

		// Character or graphical symbol to be displayed
		else {

			if (draw_sixels)
				fgColour = mGraphicsColour;
			else {
				if (mHiddenText)
					symbol_index = 0;
				else
					symbol_index = char_data - 0x20; // should give an index in the range [0,95]
				if (mGraphicSymbols)
					fgColour = mGraphicsColour;
				else
					fgColour = mAlpaNumericColour;

				double hz_0_75 = mTickRate * 1e6 * 0.75;
				int flash_75 = (int)round(hz_0_75 * 0.75);
				int flash_100 = (int)round(hz_0_75);
				if (mFlash && mTicks % flash_100 >= flash_75) {
					bgColour = mBackgroundColour;
					fgColour = mBackgroundColour;
				}
			}
		}


		int raster_line = mCharRasterLine;
		if (draw_sixels) {
			
			// Sixels symbol

			// Calculate symbol index
			int sixels_symbol_index = (char_data < 0x40 ? char_data - 0x20: char_data - 0x40); // should give an index in the range [0,63]
			symbol_index = 96+(mSeparatedGraphics?sixels_symbol_index+64: sixels_symbol_index); // should give an index in the range [96,223]
		}

		else {
			
			// Character symbol

			//
			// Check for double height
			//

			if (mDoubleHeight) {
				if (!mSecondDoubleHeightRow )
					raster_line = mCharRasterLine / 2;
				else // mSecondDoubleHeightRow
					raster_line = 10 + mCharRasterLine / 2;
			}

			// Normal-height characters on the second half of a double-height pair of rows are invisible
			if (mSecondDoubleHeightRow && !mDoubleHeight)
				symbol_index = 0; // space <=> invisible character

		}
	
		// Generate one 16-pixels wide colour raster line for the selected symbol
		auto& mStretchedSymbolRasterBits = pStretchedSymbolRasterBits->data;
		screenData = &mStretchedSymbolRasterBits[symbol_index][raster_line];

	}

	return mLOSE;
}



// Process a port update directly (and not just next time the advanceUntil() method is called)
void  TT5050::processPortUpdate(int index)
{
	if (index == GLR) {
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
				if (mDoubleHeightLine && !mSecondDoubleHeightRow) // upper half completed => switch to lower half
					mSecondDoubleHeightRow = true; 
				else {// potential start of upper half
					mSecondDoubleHeightRow = false;
				}
			}
			mDoubleHeightLine = false;
		}
		pGLR = mGLR;
	}

	else if (index == LOSE) {
		if (mLOSE == 1)
			mNewField = false;
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
		}
		pDEW = mDEW;
	}

}

// Outputs the internal state of the device
bool TT5050::outputState(ostream& sout)
{
	sout << "LOSE [Start of visible line] = " << (int)mLOSE << " <=> " << (mLOSE ? "Active" : "Inactive") << "\n";
	sout << "CRS  [Field polarity       ] = " << (int)mCRS << " <=> " << (mCRS ? "Even field":"Odd field") << "\n";
	sout << "DEW  [Start of line        ] = " << (int)mDEW << " <=> " << (mDEW ? "Active" : "Inactive") << "\n";
	sout << "GLR  [Start of field       ] = " << (int)mGLR << " <=> " << (mGLR ? "Inactive" : "Active") << "\n";

	sout << "Graphic symbols              = " << (mGraphicSymbols ? "True" : "False") << "\n";
	sout << "Flash                        = " << (mFlash ? "True" : "False") << "\n";
	sout << "Double height                = " << (mDoubleHeight ? "True" : "False") << "\n";
	sout << "Separated graphics           = " << (mSeparatedGraphics ? "True" : "False") << "\n";
	sout << "Hidden text                  = " << (mHiddenText ? "True" : "False") << "\n";
	sout << "Held graphics                = " << (mHeldGraphics ? "True" : "False") << "\n";
	sout << "Raster line                  = " << dec << (int)mCharRasterLine << "\n";

	return true;
}