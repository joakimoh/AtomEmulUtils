#include "CRTC6845.h"

//
// Note:
// The video memory address provided will not be used as the start address of the memory (and size) is programmed (R12 & R13 for the start address)
// 
//
CRTC6845::CRTC6845(
	string name, uint16_t adr, double clockSpeed, ALLEGRO_BITMAP* disp, uint16_t videoMemAdr, DebugInfo debugInfo, ConnectionManager* connectionManager
) : VideoDisplayUnit(name, CRTC6845_DEV, adr, 0x2, disp, videoMemAdr, debugInfo, connectionManager)
{

	registerPort("CLK",			IN_PORT,  0x1,	CLK,		&mCLK);
	registerPort("RESET",		IN_PORT,  0x1,	RESET,		&mRESET);
	registerPort("DEN",			OUT_PORT, 0x1,	DEN,		&mDEN);
	registerPort("RA",			OUT_PORT, 0x1f, RA,			&mRA);
	registerPort("CURS",		OUT_PORT, 0x1,	CURS,		&mCURS);
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
	mInitialisedCount = 0;
	mRegWrtCnt = 0;

	for (int i = 0; i < 18; mReg[i++] = 0);

	// Example: BBC Micro Mode 7 Settings... (see https://beebwiki.mdfs.net/MODE_7) and actual from execution
	// mReg[R0_HorizontalTotal] =		0x3f;	0x3f
	// mReg[R1_HorizontalDisplayed] =	0x28;	0x28
	// mReg[R2_HSYncPosition] =			0x33;	0x31
	// mReg[R3_HSyncWidth] =			0x24;	0x4 (0x24 written but truncated to 4 bits)
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

// Called by other device to trigger the output of new data
bool CRTC6845::updateDataOutput(uint8_t &data)
{
	data = 0xff;

	// Advance time corresponding to one character and check for HS, VS & DEN
	int step = max(1, (int)round((mCPUClock / mCLK)));
	advance(mCycleCount + step);

	if (mInitialisedCount < 2)
		return true;

	// If in the active display area, read the memory location and output the address on port RA and the read data on port D_OUT
	if (mDEN) {
		uint16_t video_mem_adr = mVideoMemAdr + mCharRow * mReg[R1_HorizontalDisplayed] + mCharCol + mRA;
		if (!mVideoMem->read(video_mem_adr, data)) 
			return false;	
	}

	// Increase character position (includes the invisible non-displayed chars)
	mCharCol = (mCharCol + 1) % (mReg[R0_HorizontalTotal] + 1);
	if (mCharCol == 0) {
		mScanLine = (mScanLine + 1) % (mReg[R4_VerticalTotal] + 1);
		mCharRow = mScanLine % (mReg[R9_MaxScanLineAddress] + 1);
		uint8_t raster_row = mScanLine % (mReg[R9_MaxScanLineAddress] + 1);
		updatePort(RA, raster_row);
	}

	return true;

}

bool CRTC6845::advance(uint64_t stopCycle)
{
	if (mInitialisedCount < 2)
		mCycleCount = stopCycle;

	while (mCycleCount < stopCycle) {

		// Advance time corresponding to one character
		int step = (int)round((mCPUClock / mCLK));
		if (step == 0)
			step = 1;
		mCycleCount += step;

		// Check for Vertical Sync Output
		if (mCharRow == mReg[R7_VSyncPosition])
			updatePort(VS, 0x1);
		else
			updatePort(VS, 0x10);

		// Check for Horizontal Sync Output
		if (mCharCol >= mReg[R2_HSYncPosition] && mCharCol <= mReg[R2_HSYncPosition] + mReg[R3_HSyncWidth])
			updatePort(HS, 0x1);
		else
			updatePort(HS, 0x0);

		// Check for Visible part of scan line
		if (mCharRow < mReg[R6_VerticalDisplayed] && mCharCol < mReg[R1_HorizontalDisplayed])
			updatePort(DEN, 0x1);
		else
			updatePort(DEN, 0x0);

		// Check for cursor being selected
		int cursor_first_line = mReg[R10_CursorStart];
		int curstor_last_line = mReg[R11_CursorEnd];
		int cursor_char_pos = (mReg[R14_CursorH] << 8) + mReg[R15_CursorL];
		if (mCharRow * mReg[R1_HorizontalDisplayed] + mCharCol == cursor_char_pos)
			updatePort(CURS, 0x1);
		else
			updatePort(CURS, 0x0);
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
// R4        Vertical total                    38    38    38    30    38    38    30    30
// R5        Vertical total adjust              0     0     0     2     0     0     2     2
// R6        Vertical displayed characters     32    32    32    25    32    32    25    25
// R7        Vertical sync position            34    34    34    27    34    34    27    27
// R8        Interlace mode(bits 0, 1)          1     1     1     1     1     1     1     3
//           +Display delay(bits 4, 5)          0     0     0     0     0     0     0     1
//           +Cursor delay(bits 6, 7)           0     0     0     0     0     0     0     2
// R9        Scan lines per character           7     7     7     9     7     7     9    18
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
	
	if (mRegWrtCnt >= 18 && (mDebugInfo.dbgLevel & DBG_VERBOSE))
		printSettings();

	if (a == R12_StartAddressH || a == R13_StartAddressL)
		mInitialisedCount++;

	return true;
}

void CRTC6845::updateSettings()
{
	mVisibleLines = mReg[R6_VerticalDisplayed] * (mReg[R9_MaxScanLineAddress] + 1);
	mScanLines = (mReg[R4_VerticalTotal] + 1 + mReg[R5_VerticalTotalAdjust] / 32.0) * (mReg[R9_MaxScanLineAddress] + 1);

}

void CRTC6845::printSettings()
{

	// Update parameters based on the register update															Beeb Mode 0  (80 x 32 chars, 640 x 256 pixels)
	double Tc = 1.0 / mCLK;														// [us]		Clock period
	double Nsl = mReg[R9_MaxScanLineAddress] + 1;								// [lines]	Character Scan Lines (up to 32)						  8 lines
	double Tsl = (mReg[R0_HorizontalTotal] + 1) * Tc;							// [us]		Scan Line duration					(127+1) * 0.5 =	 64 us
	double Tcr = Nsl * Tsl;														// [us]		Character Row Period				7 * 64 =		448 us
	double Nhd = mReg[R1_HorizontalDisplayed] * Tc;								// [us]		Visible horizontal area				80 * 0.5 =		 40 us
	double Nhsp = mReg[R2_HSYncPosition] * Tc;									// [us]		Horizontal sync pos					98 * 0.5 =		49 us
	double Nhsw = mReg[R3_HSyncWidth] * Tc;										// [us]		Horizontal sync pulse width			8 * 0.5 =		 4 us
	double Nvt = (mReg[R4_VerticalTotal] + 1) * Tcr / 1000;						// [ms]		Vertical total (integer part)		(38+1) * 448 = 17.5 ms (57 Hz)
	double Nadj = mReg[R5_VerticalTotalAdjust] * Tsl / 1000;					// [ms]		Vertical total (fraction)							  0 ms
	double Nvd = mReg[R6_VerticalDisplayed] * Tcr;								// [ms]		Visible vertical duration			32 * 448 =	   14.3 ms
	double Nvsp = mReg[R7_VSyncPosition] * Tcr;									// [ms]		Vertical sync position				34 * 448 =	   15.2 ms
	int VS_pos = mReg[R7_VSyncPosition] * (mReg[R9_MaxScanLineAddress] + 1);	// Vertical sync position in scan lines			8 * 34 =	    272 lines
	mVisibleLines = mReg[R6_VerticalDisplayed] * (mReg[R9_MaxScanLineAddress] + 1);
	mScanLines = (mReg[R4_VerticalTotal] + 1) * (mReg[R9_MaxScanLineAddress] + 1) + mReg[R5_VerticalTotalAdjust];			// 39 * 8 + 0 =			312 lines

	cout << dec;
	for (int i = 0; i < 18; i++)
		cout << "R" << i << ": " << hex << (int) mReg[i] << dec << "\n";
 	cout << "CLK:                               " << (int) mCLK << "\tMhz\n";
	cout << "Frame Rate:                        " << round(1000 / Nvt) << "\tHz\n";
	cout << "No of scan lines per frame:        " << round(mScanLines) << "\n";
	cout << "No of visible scan lines:          " << mVisibleLines << "\n";
	cout << "Scan Line duration:                " << Tsl << "\tus\n";
	cout << "Scan Lines per Character:          " << Nsl << "\tlines\n";
	cout << "Total no of characters per line:   " << (int) mReg[R0_HorizontalTotal] + 1 << "\tchars\n";
	cout << "No of visible characters per line: " << (int)mReg[R1_HorizontalDisplayed] << "\tchars\n";
	cout << "Vertical sync position:            " << VS_pos << " lines (" << round(Nvsp / 1000) << " ms)\n";
	cout << "Horizontal sync position:          " << (int) mReg[R2_HSYncPosition] << " chars (" << round(Nhsp) << " us)\n";
	cout << "Start address:                     0x" << hex << (mReg[R12_StartAddressH] << 8) + mReg[R13_StartAddressL] << "\n";
	cout << "Interlace mode:                    " << (mReg[R8_InterlaceMode]&1?"Non-interlaced":mReg[R8_InterlaceMode]&2?"Interlaced & video":"Interlaced") << "\n";
	
}

bool CRTC6845::getVisibleCharArea(int& w, int& h)
{
	w = mReg[R1_HorizontalDisplayed];
	h = mReg[R6_VerticalDisplayed];
	return true;
}

inline double CRTC6845::getScanLineDuration()
{
	return (mReg[R0_HorizontalTotal] + 1) / mCLK;
}

inline double CRTC6845::getScanLinesPerFrame()
{
	return mScanLines;
}

inline double CRTC6845::getFrameRate()
{
	//																											Beeb Mode 0 (80 x 32 chars, 640 x 256 pixels)
	double Tc = 1.0 / mCLK;										// [us]		Clock period
	double Nsl = mReg[R9_MaxScanLineAddress] +1;				// [lines]	Character Scan Lines (up to 32)						  8 lines
	double Tsl = (mReg[R0_HorizontalTotal] + 1) * Tc;			// [us]		Scan Line duration					(127+1) * 0.5 =	 64 us
	double Tcr = Nsl * Tsl;										// [us]		Character Row Period				7 * 64 =		448 us
	double Nvt = (mReg[R4_VerticalTotal] + 1) * Tcr / 1000;		// [ms]		Vertical total (integer part)		(38+1) * 448 = 17.5 ms (57 Hz) 

	return 1000/Nvt;
}

inline int CRTC6845::getCharScanLines()
{
	return mReg[R9_MaxScanLineAddress] + 1;
}

inline int CRTC6845::getVerticalSyncPos()
{
	return mReg[R2_HSYncPosition] * (mReg[R9_MaxScanLineAddress] + 1);
}