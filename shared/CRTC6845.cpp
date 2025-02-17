#include "CRTC6845.h"
#include <iomanip>

//
// Note:
// The video memory address provided will not be used as the start address of the memory (and size) is programmed (R12 & R13 for the start address)
// 
//
CRTC6845::CRTC6845(
	string name, uint16_t adr, double clockSpeed, ALLEGRO_BITMAP* disp, int dispW, int dispH, DebugInfo debugInfo, ConnectionManager* connectionManager
) : VideoDisplayUnit(name, CRTC6845_DEV, adr, 0x2, disp, dispW, dispH, 0x0 /* dummy adr as not used by the 6845 */, debugInfo, connectionManager)
{

	registerPort("CLK",			IN_PORT,  0x1,	CLK,		&mCLK);
	registerPort("RESET",		IN_PORT,  0x1,	RESET,		&mRESET);
	registerPort("DISPTMG",			OUT_PORT, 0x1,	DISPTMG,		&mDISPTMG);
	registerPort("RA",			OUT_PORT, 0x1f, RA,			&mRA);
	registerPort("CUDISP",		OUT_PORT, 0x1,	CUDISP,		&mCUDISP);
	registerPort("HS",			OUT_PORT, 0x1,	HS,			&mHS);
	registerPort("VS",			OUT_PORT, 0x1,	VS,			&mVS);

	reset();
}

bool CRTC6845::reset()
{
	mCycleCount = 0;
	mCharRow = 0;
	mScanLine = 0;
	mCharCol = 0;
	mInitialised = false;
	mRegWrtCnt = 0;

	for (int i = 0; i < 18; mReg[i++] = 0);

	// Example: BBC Micro Mode 7 Settings... (see https://beebwiki.mdfs.net/MODE_7) and actual from execution
	// mReg[R0_HorizontalTotal] =		0x3f;	0x3f
	// mReg[R1_HorizontalDisplayed] =	0x28;	0x28
	// mReg[R2_HSYncPosition] =			0x33;	0x31
	// mReg[R3_SyncWidth] =			0x24;	0x4 (0x24 written but truncated to 4 bits)
	// mReg[R4_VerticalTotal] =			0x1e;	0x26
	// mReg[R5_VerticalTotalAdjust] =	0x02;	0x00
	// mReg[R6_VerticalDisplayed] =		0x19;	0x20
	// mReg[R7_VSyncPosition] =			0x1b;	0x23
	// mReg[R8_InterlaceMode] =			0x93;	0x01
	// mReg[R9_MaxScanLineAddress] =	0x12;	0x07
	// mReg[R10_CursorStart] =			0x72;	0x67
	// mReg[R11_CursorEnd] =			0x13;	0x08
	// mReg[R12_StartAddressH] =		0;		0x0b	0x0b00
	// mReg[R13_StartAddressL] =		0;		0x00
	// mReg[R14_CursorH] =				0;		0x0b
	// mReg[R15_CursorL] =				0;		0x00
	// mReg[R16_LightPenL] =			0;
	// mReg[R17_LightPenH] =			0;
	
	updateSettings();

	return true;
}

// Called by other device to get next memory address to fetch char/graphics data from
bool CRTC6845::getMemFetchAdr(uint16_t &adr, uint16_t &cursor, bool &activeArea)
{

	// Advance time corresponding to one character and check for HS, VS & DISPTMG
	int nextCycleCount = (int) round( mCycleCount + mCPUClock / mCLK);
	if (nextCycleCount == mCycleCount)
		nextCycleCount++;
	advance(nextCycleCount);

	if (!mInitialised)
		return true;

	// If in the active display area, update the fetch address abd cursor position
	adr = 0x0;
	activeArea = false;
	if (mDISPTMG) {
		//cout << "\nstart adr: 0x" << hex << mStartAdr << ", char row:" << dec << mCharRow << ", char col " << mCharCol << 
			//", start visible col " << mStartVisibleCharCol  << "\n";
		adr = mStartAdr + (mCharRow - mStartVisibleCharRow) * mActiveRowChars + mCharCol - mStartVisibleCharCol;
		cursor = mCursorAdr;
		activeArea = true;
	}

	// Increase char column
	// The M6845 linear address generator repeats the same sequence of addresses for each scan line of a character row
	mCharCol = (mCharCol + 1) % mCharCols;
	if (mCharCol == 0) {
		mScanLine = (mScanLine + 1) % (int) round(mScanLines);
		updatePort(RA, (mRA + 1) % mCharLines);
		if (mRA == 0)
			mCharRow = (mCharRow + 1) % mCharRows;
		if (mScanLine == 0) {
			mCharRow = 0;
			mCharCol = 0;
			updatePort(RA, 0);
		}
	}

	return true;

}

bool CRTC6845::advance(uint64_t stopCycle)
{
	if (!mInitialised)
		mCycleCount = stopCycle;

	while (mCycleCount < stopCycle) {

		// Advance time corresponding to one character
		int step = (int)round((mCPUClock / mCLK));
		if (step == 0)
			step = 1;
		mCycleCount += step;

		// Check for Vertical Sync Output
		if (mCharRow >= mVSyncRow && mCharRow < mVSyncRow + mVSyncPulseW)
			updatePort(VS, 0x1);
		else
			updatePort(VS, 0x0);

		// Check for Horizontal Sync Output
		if (mCharCol >= mHzSyncPos  && mCharCol < mHzSyncPos + mHzSyncPulseW) {
			updatePort(HS, 0x1);
		}
		else {
			updatePort(HS, 0x0);
		}
		
		//
		// Check for Visible part of scan line
		// 
		// Determined by the hz sync pulse location and the no of chars/row
		// and the row no.
		// 
		// Visible part of a row:
		// 
		// <left border> <active area> <hsync pulse> <right border>
		//    (R2-R1)        (R1)         (R2,R3)        (R0-R2-R3)
		// <------------------------------------------------------>
		//                  (R0)
		//
		// Scan lines:
		// 
		// <     top border   > <active area> <    vsync pulse      > <     bottom border             >
		//   (R7-R6)*(R9+1/2)    R6*(R9+1/2)    (R7:R7+R3)*(R9+1/2)   R4*(R9+1/2)+R5-(R7+R3)*(R9+1/2)
		// <------------------------------------------------------------------------------------------>
		//                          R4*(R9+1/2)+R5
		//
		if (mCharCol >= mStartVisibleCharCol && mCharCol < mHzSyncPos && mCharRow < mReg[R6_VerticalDisplayed])		
			updatePort(DISPTMG, 0x1);
		else
			updatePort(DISPTMG, 0x0);

		// Check for cursor being selected
		int cursor_first_line = mReg[R10_CursorStart];
		int curstor_last_line = mReg[R11_CursorEnd];
		int cursor_char_pos = (mReg[R14_CursorH] << 8) + mReg[R15_CursorL];
		if (mCursorAdr + mCharRow * mReg[R1_HorizontalDisplayed] + mCharCol == cursor_char_pos)
			updatePort(CUDISP, 0x1);
		else
			updatePort(CUDISP, 0x0);
	}

	
	return true;


}

bool CRTC6845::read(uint16_t adr, uint8_t& data)
{
	// Call parent class to trigger scheduling of other devices when applicable
	if (!VideoDisplayUnit::read(adr, data))
		return false;

	int16_t a = adr - mMemorySpace.adr;

	if (mAddressRegister > 0 && mAddressRegister < 18 && mRegInfo[mAddressRegister].readable)
		data = mReg[mAddressRegister];
	else
		data = 0xff;
	return true;
}

//
// Register settings for the BBC Micro Model B (from https://beebwiki.mdfs.net/CRTC):
// Register  Description                             Default value for MODE
//                                              0     1     2     3     4     5     6     7
// ----------------------------------------------------------------------------------------
// R0        Horizontal total                 127   127   127   127    63    63    63    63
// R1        Characters per line               80    80    80    80    40    40    40    40
// R2        Horizontal sync position          98    98    98    98    49    49    49    51
// R3        Horizontal sync width(bits 0 - 3)  8     8     8     8     4     4     4     4
//           +Vertical sync width(bits 4 - 7)   2     2     2     2     2     2     2     2
// R4        Vertical total                    38    38    38    30    38    38    30    30 => (18+2) * (30+1) + 2 = 622 scan lines for mode 7
// R5        Vertical total adjust              0     0     0     2     0     0     2     2
// R6        Vertical displayed characters     32    32    32    25    32    32    25    25 => (18+2) * 25 = 500 visible scan lines for mode 7
// R7        Vertical sync position            34    34    34    27    34    34    27    28 
// R8        Interlace mode(bits 0, 1)          1     1     1     1     1     1     1     3
//           +Display delay(bits 4, 5)          0     0     0     0     0     0     0     1
//           +Cursor delay(bits 6, 7)           0     0     0     0     0     0     0     2
// R9        Scan lines per character           7     7     7     9     7     7     9    18 => (18+2)  = 20 raster lines for mode 7
// R10       Cursor start(bits 0 - 4)           7     7     7     7     7     7     7    18
//           Cursor type(bit 5)                 1     1     1     1     1     1     1     1
//           Cursor blink(bit 6)                1     1     1     1     1     1     1     1
// R11       Cursor end                         8     8     8     9     8     8     9    19
// R12, R13   Screen start address / 8 - -------
// R14, R15   Cursor position - -------
// R16, R17   Light pen position - -------
//
bool CRTC6845::write(uint16_t adr, uint8_t data)
{

	// Call parent class to trigger scheduling of other devices when applicable
	if (!VideoDisplayUnit::write(adr, data))
		return false;

	

	int16_t a = adr - mMemorySpace.adr;
	if (a == 0 && (data & 0x1f) < 18)
		mAddressRegister = data & 0x1f;
	else if (mAddressRegister >= 0 && mAddressRegister < 18 && mRegInfo[mAddressRegister].writable) {
		mReg[mAddressRegister] = data & mRegInfo[mAddressRegister].mask;
		mRegWrtCnt++;
		//cout << "R" << dec << (int)mAddressRegister << " = 0x" << hex << (int)mReg[mAddressRegister] << " for data 0x" << (int) data << "\n";
	}

	updateSettings();
	
	if (mRegWrtCnt >= 18) {
		if ((mDebugInfo.dbgLevel & DBG_VERBOSE))
			printSettings();
		mInitialised = true;
	}


	return true;
}

void CRTC6845::updateSettings()
{

	if ((mReg[R8_InterlaceMode] & 0x3) == 0x3)
		mCharLines = mReg[R9_MaxScanLineAddress] + 2;
	else
		mCharLines = mReg[R9_MaxScanLineAddress] + 1;

	// Vertical scan lines: top border, active lines, sync pulse, bottom border
	mCharRows = mReg[R4_VerticalTotal] + 1;
	mScanLines = mCharRows * mCharLines + mReg[R5_VerticalTotalAdjust];
	mVSyncRow = mReg[R7_VSyncPosition];
	mVSyncLine = mVSyncRow * mCharLines;
	mActiveRows = mReg[R6_VerticalDisplayed];
	mActiveLines = mActiveRows * mCharLines;
	mTopBorderLines = mVSyncRow * mCharLines - mActiveLines;
	mVSyncPulseW = (mReg[R3_SyncWidth] >> 4) & 0xf;
	if (mVSyncPulseW == 0)
		mVSyncPulseW = 16;
	mBottomBorderLines = mScanLines - mTopBorderLines - mActiveLines - mVSyncPulseW;
	mVisibleScanLines = mTopBorderLines + mActiveLines + mBottomBorderLines;

	// Horizontal line: left border, active chars, sync pulse, right border (all in unit 'char')
	mCharCols = mReg[R0_HorizontalTotal] + 1;
	mHzSyncPos = mReg[R2_HSYncPosition] + 1;
	mActiveRowChars = mReg[R1_HorizontalDisplayed];
	mLeftBorderChars = mHzSyncPos - mActiveRowChars;		
	mHzSyncPulseW = mReg[R3_SyncWidth] & 0xf;
	mRightBorderChars = mCharCols - mLeftBorderChars - mActiveRowChars - mHzSyncPulseW;
	mVisibleChars = mLeftBorderChars + mActiveRowChars + mRightBorderChars;

	// Video memory start address
	mStartAdr = (mReg[R12_StartAddressH] << 8) | mReg[R13_StartAddressL];

	// Current cursor address
	mCursorAdr = (mReg[R14_CursorH] << 8) | mReg[R15_CursorL];

}

void CRTC6845::printSettings()
{

	// Update parameters based on the register update														
	double Tc = 1.0 / mCLK;	

	cout << dec;
	for (int i = 0; i < 18; i++)
		cout << "R" << setw(2) <<  setfill('0') << i << ": 0x" << hex << setfill('0') << setw(2) << (int)mReg[i] << dec << "\n";

 	cout << "CLK:                               " << (int) mCLK << "\tMhz\n";
	cout << "Frame Rate:                        " << round(mCLK * 1e6 / (mScanLines * mCharCols)) << "\tHz\n";
	cout << "No of scan lines per frame:        " << round(mScanLines) << "\tlines\n";
	cout << "Top border lines:                  " << mTopBorderLines << "\tlines\n";
	cout << "Active scan lines:                 " << mActiveLines << "\tlines\n";
	cout << "Bottom border lines:               " << mBottomBorderLines << "\tlines\n";
	cout << "Scan Line duration:                " << mCharCols * Tc << "\tus\n";
	cout << "Scan Lines per Character:          " << mCharLines << "\tlines\n";
	cout << "Total no of characters per line:   " << (int) mCharCols << "\tchars\n";
	cout << "Left border chars:                 " << mLeftBorderChars << "\tchars\n";
	cout << "Active characters per line:        " << mActiveRowChars << "\tchars\n";
	cout << "Right border chars:                " << mRightBorderChars << "\tchars\n";
	cout << "Vertical sync position:            " << mVSyncLine << " lines (" << round(mVSyncLine * mCharCols * Tc / 1000) << " ms)\n";
	cout << "Horizontal sync position:          " << (int)mHzSyncPos << " chars (" << round(mHzSyncPos*Tc) << " us)\n";
	cout << "Horizontal sync pulse width:       " << (int)mHzSyncPulseW << " chars (" << round(mHzSyncPulseW*Tc) << " us)\n";
	cout << "Vertical sync pulse width:         " << (int)mVSyncPulseW << " lines (" << round(mVSyncPulseW*mCharCols*Tc) << " us)\n";
	cout << "Start address:                     0x" << hex << mStartAdr << "\n";
	cout << "Cursor raster lines:               [" << dec << (int)(mReg[R10_CursorStart] & 0x1f) << ":" << (int)mReg[R11_CursorEnd] << "]\n";
	cout << "Cursor position:                   0x" << hex << ((mReg[R14_CursorH] << 8) | mReg[R15_CursorL]) << "\n";
	cout << "Interlace mode:                    " << ((mReg[R8_InterlaceMode]&1)==0?"Non-interlaced":mReg[R8_InterlaceMode]&2?"Interlaced & video":"Interlaced") << "\n";
	
}

bool CRTC6845::getVisibleCharArea(int &charsPerLine, int &lines)
{
	charsPerLine = mLeftBorderChars + mActiveRowChars + mRightBorderChars;
	lines = mTopBorderLines + mActiveLines + mBottomBorderLines;
	return true;
}

inline double CRTC6845::getScanLineDuration()
{
	return mCharCols / mCLK;
}

inline double CRTC6845::getScanLinesPerFrame()
{
	return mScanLines;
}

inline double CRTC6845::getFrameRate()
{

	return mCLK / (mScanLines * mCharCols);
}

inline int CRTC6845::getCharScanLines()
{
	return mCharLines;
}

inline int CRTC6845::getVerticalSyncLine()
{
	return mVSyncLine;
}

inline int CRTC6845::getHorizontalSyncPos()
{
	return mVSyncRow;
}

inline int CRTC6845::getCharsPerLine()
{
	return mCharLines;
}

inline int CRTC6845::getVisibleCharsPerLine()
{
	return mActiveRowChars;
}

inline int CRTC6845::getLeftBorderChars()
{
	return mLeftBorderChars;
}
inline int CRTC6845::getTopBorderLines()
{
	return mTopBorderLines;
}
inline int CRTC6845::getActiveChars()
{
	return mActiveRowChars;
}
inline int CRTC6845::getActiveLines()
{
	return mActiveLines;
}