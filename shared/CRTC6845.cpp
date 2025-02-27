#include "CRTC6845.h"
#include <iomanip>

//
// Note:
// The video memory address provided will not be used as the start address of the memory (and size) is programmed (R12 & R13 for the start address)
// 
//
CRTC6845::CRTC6845(
	string name, uint16_t adr, double clockSpeed, ALLEGRO_BITMAP* disp, int dispW, int dispH, DebugInfo  *debugInfo, ConnectionManager* connectionManager
) : VideoDisplayUnit(name, CRTC6845_DEV, adr, 0x2, disp, dispW, dispH, 0x0 /* dummy adr as not used by the 6845 */, debugInfo, connectionManager)
{

	registerPort("CLK",			IN_PORT,  0x1,	CLK,		&mCLK);
	registerPort("RESET",		IN_PORT,  0x1,	RESET,		&mRESET);
	registerPort("DISPTMG",		OUT_PORT, 0x1,	DISPTMG,	&mDISPTMG);
	registerPort("RA",			OUT_PORT, 0x1f, RA,			&mRA);
	registerPort("CUDISP",		OUT_PORT, 0x1,	CUDISP,		&mCUDISP);
	registerPort("HS",			OUT_PORT, 0x1,	HS,			&mHS);
	registerPort("VS",			OUT_PORT, 0x1,	VS,			&mVS);

}

bool CRTC6845::reset()
{
	Device::reset();

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
bool CRTC6845::getMemFetchAdr(uint16_t &adr)
{

	if (!mInitialised)
		return true;

	updateOutputs();

	// If in the active display area, update the fetch address abd cursor position
	adr = 0x0;
	if (mDISPTMG) {;
		adr = mStartAdr + mCharRow * mActiveRowChars + mCharCol;
		//cout << dec << "#" << mCharCol << "#";
	}
	
	// Advance time corresponding to one character and check for HS, VS & DISPTMG
	advanceChar();

	return true;

}

bool CRTC6845::advanceChar()
{
	int nextCycleCount = (int)round(mCycleCount + mCPUClock / mCLK);
	if (nextCycleCount == mCycleCount)
		nextCycleCount++;

	mCycleCount = nextCycleCount;

	updateOutputs();

	// Increase char column
	// The M6845 linear address generator repeats the same sequence of addresses for each scan line of a character row
	mCharCol = (mCharCol + 1) % mCharCols;
	if (mCharCol == 0) {
		mScanLine = (mScanLine + 1) % (int)round(mScanLines);
		if (mScanLine == 0)
			mFrame++;
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

bool CRTC6845::updateOutputs()
{

	// Check for Vertical Sync Output
	if (mCharRow >= mVSyncRow && mCharRow < mVSyncRow + mVSyncPulseH)
		updatePort(VS, 0x1);
	else
		updatePort(VS, 0x0);

	// Check for Horizontal Sync Output
	if (mCharCol >= mHzSyncPos && mCharCol < mHzSyncPos + mHzSyncPulseW) {
		updatePort(HS, 0x1);
	}
	else {
		updatePort(HS, 0x0);
	}

	//
	// Check for Visible active part of scan line
	// 

	if (mCharCol < mActiveRowChars && mCharRow < mActiveRows)
		updatePort(DISPTMG, 0x1);
	else
		updatePort(DISPTMG, 0x0);

	// Check for cursor being selected
	int cursor_first_line = mReg[R10_CursorStart] & 0x1f;
	int cursor_last_line = mReg[R11_CursorEnd] & 0x1f;
	int cursor_disp_mode = (mReg[R10_CursorStart] >> 5) & 0x3; // 00: Non-blink,  01: non-display, 10: blink 16-field, 11: blink 32-field
	mCursorLocation = ((mReg[R14_CursorH] & 0x3f) << 8) | mReg[R15_CursorL] + mCursSkew - mCharSkew;
	bool cursor_on = (
		cursor_disp_mode == 0x0 ||
		cursor_disp_mode == 0x2 && mFrame % 16 < 8 ||
		cursor_disp_mode == 0x3 && mFrame % 32 < 16
		) && mRA >= cursor_first_line && mRA <= cursor_last_line;
	if (cursor_on && mStartAdr + mCharRow * mActiveRowChars + mCharCol == mCursorLocation)
		updatePort(CUDISP, 0x1);
	else
		updatePort(CUDISP, 0x0);

	return true;
}

bool CRTC6845::advance(uint64_t stopCycle)
{
	if (!mInitialised)
		mCycleCount = stopCycle;

	while (mCycleCount < stopCycle)
		advanceChar();
	
	return true;

}

bool CRTC6845::read(uint16_t adr, uint8_t& data)
{
	// Call parent class to trigger scheduling of other devices when applicable
	if (!VideoDisplayUnit::triggerBeforeRead(adr, data))
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
// R12, R13   Screen start address / 8			-     -     -     -     -     -     -     -
// R14, R15   Cursor position					-     -     -     -     -     -     -     -
// R16, R17   Light pen position				-     -     -     -     -     -     -     -
//
bool CRTC6845::write(uint16_t adr, uint8_t data)
{

	if (!selected(adr))
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
		if ((mDebugInfo->dbgLevel & DBG_VERBOSE))
			printSettings();
		mInitialised = true;
	}


	// Call parent class to trigger scheduling of other devices when applicable
	return VideoDisplayUnit::triggerAfterWrite(adr, data);
}

//
// Update CRTC parametrs based on the register settings.
// 
// 
// The visible part of a scan line is determined by the hz sync pulse location,
// the no of chars/row and the retrace time (that time is not configured but 
// rather a property of the connected monitor - we will assume 10% of the line
// duration for this). The hz sync pulse starts a new line but time for
// retracing needs to be considered before content can be displayed.
// 
// A border is displayed when DISPTMG is low
// 
// Visible part of a row:
//			              ,____________.								   ,__
// DISPTMG  ______________|            |___________________________________|
//			,_________.							           ,___________.
// HSYNC    |         |____________________________________|           |____________
// 
//			<left border*><active area ><  right border    >< left border* >
//			                 (R1)                             
//			<------------------------------------------------------------->
//                                       (R0)
// 
// * Including sync pulse & retrace
//
// The visible part of a frame  is determined by the vertical sync pulse location,
// the no of scan lines and the retrace time (that time is not configured but 
// rather a property of the connected monitor - we will assume 5% of the frame
// duration for this). The vertical sync pulse starts a new frame but time for
// retracing needs to be considered before content can be displayed.
// 
// 			               ,________________________.					               ,__
// DISPTMG  _______________|XXXXXXXXXXXXXXXXXXXXXXXX|__________________________________|XX
//			,_____.								                       ,_____.
// VSYNC    |     |____________________________________________________|     |________________________
// 
//          <top border** ><      active area       >< bottom border  ><top border**  ><active area
//									R6*(R9+1/2)	
//          <--------------------------------------------------------->
//                          R4*(R9+1/2)+R5
// 
// ** Including sync pulse & retrace
// 
void CRTC6845::updateSettings()
{

	if ((mReg[R8_InterlaceMode] & 0x3) == 0x3)
		mCharLines = mReg[R9_MaxScanLineAddress] + 2;
	else
		mCharLines = mReg[R9_MaxScanLineAddress] + 1;

	// Vertical scan lines: top border, active lines, sync pulse, bottom border
	mCharRows = mReg[R4_VerticalTotal] + 1;
	mScanLines = mCharRows * mCharLines + mReg[R5_VerticalTotalAdjust];
	mRetraceRows = (int) round(mCharRows * 0.05);
	mRetraceLines = mRetraceRows * mCharLines;
	mVSyncRow = mReg[R7_VSyncPosition];
	mVSyncLine = mVSyncRow * mCharLines;
	mVSyncPulseH = (mReg[R3_SyncWidth] >> 4) & 0xf;
	if (mVSyncPulseH == 0)
		mVSyncPulseH = 16;
	mActiveRows = mReg[R6_VerticalDisplayed];
	mActiveLines = mActiveRows * mCharLines;
	mCharSkew = (mReg[R8_InterlaceMode] >> 4) & 0x3;
	if (mCharSkew == 3)
		mCharSkew = 0;
	mCursSkew = (mReg[R8_InterlaceMode] >> 6) & 0x3;
	if (mCursSkew == 3)
		mCursSkew = 0;
	mBottomBorderLines = mVSyncLine - mActiveLines;
	mTopBorderLines = mScanLines - mBottomBorderLines - mActiveLines - mRetraceLines;
	mVisibleScanLines = mTopBorderLines + mActiveLines + mBottomBorderLines;

	// Horizontal line: left border, active chars, sync pulse, right border (all in unit 'char')
	mCharCols = mReg[R0_HorizontalTotal] + 1;
	mHzSyncPos = mReg[R2_HSYncPosition] + 1;
	mActiveRowChars = mReg[R1_HorizontalDisplayed];
	mRetraceChars = mActiveRowChars * 0.1;
	mRightBorderChars = mHzSyncPos - mActiveRowChars - mCharSkew; // Gap between active area and sync pulse
	mHzSyncPulseW = mReg[R3_SyncWidth] & 0xf;
	mLeftBorderChars = mCharCols - mRightBorderChars - mActiveRowChars - mRetraceChars;
	mVisibleChars = mLeftBorderChars + mActiveRowChars + mRightBorderChars;

	// Video memory start address
	mStartAdr = (mReg[R12_StartAddressH] << 8) | mReg[R13_StartAddressL];

	// Current cursor address
	mCursorLocation = ((mReg[R14_CursorH] & 0x3f)<< 8) | mReg[R15_CursorL];

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
	cout << "Retrace lines:                     " << mRetraceLines << "\tlines\n";
	cout << "Top border lines:                  " << mTopBorderLines << "\tlines\n";
	cout << "Active scan lines:                 " << mActiveLines << "\tlines\n";
	cout << "Bottom border lines:               " << mBottomBorderLines << "\tlines\n";
	cout << "Scan Line duration:                " << mCharCols * Tc << "\tus\n";
	cout << "Scan Lines per Character:          " << mCharLines << "\tlines\n";
	cout << "Total no of characters per line:   " << mCharCols << "\tchars\n";
	cout << "Retrace characters per line:       " << mRetraceChars << "\tchars\n";
	cout << "Left border chars:                 " << mLeftBorderChars << "\tchars\n";
	cout << "Active characters per line:        " << mActiveRowChars << "\tchars\n";
	cout << "Right border chars:                " << mRightBorderChars << "\tchars\n";
	cout << "Vertical sync position:            " << mVSyncLine << " lines (" << round(mVSyncLine * mCharCols * Tc / 1000) << " ms)\n";
	cout << "Horizontal sync position:          " << mHzSyncPos << " chars (" << round(mHzSyncPos*Tc) << " us)\n";
	cout << "Horizontal sync pulse width:       " << mHzSyncPulseW << " chars (" << round(mHzSyncPulseW*Tc) << " us)\n";
	cout << "Vertical sync pulse width:         " << mVSyncPulseH << " lines (" << round(mVSyncPulseH*mCharCols*Tc) << " us)\n";
	cout << "Start address:                     0x" << hex << mStartAdr << "\n";
	cout << "Cursor raster lines:               [" << dec << (int)(mReg[R10_CursorStart] & 0x1f) << ":" << (int)mReg[R11_CursorEnd] << "]\n";
	cout << "Cursor position:                   0x" << hex << ((mReg[R14_CursorH] << 8) | mReg[R15_CursorL]) << "\n";
	cout << "Interlace mode:                    " << ((mReg[R8_InterlaceMode]&1)==0?"Non-interlaced":mReg[R8_InterlaceMode]&2?"Interlaced & video":"Interlaced") << "\n";
	cout << "Character skew:                    " << mCharSkew << "\tchars\n";
	cout << "Cursor skew:                       " << mCursSkew << "\tchars\n";
	int cursor_disp_mode = (mReg[R10_CursorStart] >> 5) & 0x3; // 00: Non-blink,  01: non-display, 10: blink 16-field, 11: blink 32-field
	cout << "Cursor mode:                       " << (cursor_disp_mode==0?"Fixed":(cursor_disp_mode==1?"None":(cursor_disp_mode==2?"Blink-16":"Blink-32"))) <<
		"\n";
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
	return mCharCols;
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

inline int CRTC6845::getRightBorderChars()
{
	return mRightBorderChars;
}

inline int CRTC6845::getBottomBorderLines()
{
	return mBottomBorderLines;
}

inline int CRTC6845::getRetraceLines()
{
	return mRetraceLines;
}

inline int CRTC6845::getRetraceChars()
{
	return mRetraceChars;
}