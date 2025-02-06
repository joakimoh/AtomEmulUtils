#include "CRTC6845.h"

CRTC6845::CRTC6845(
	string name, uint16_t adr, double clockSpeed, ALLEGRO_BITMAP* disp, uint16_t videoMemAdr, DebugInfo debugInfo, ConnectionManager* connectionManager
) : VideoDisplayUnit(name, CRTC6845_DEV, OTHER_DEVICE, debugInfo, connectionManager), mCPUClock(clockSpeed)
{

	registerPort("CLK",			IN_PORT,  0x1,	CLK,		&mCLK);
	registerPort("RESET",		IN_PORT,  0x1,	RESET,		&mRESET);
	registerPort("READ_REQ",	IN_PORT,  0x1,	READ_REQ,	&mREAD_REQ);
	registerPort("DEN",			OUT_PORT, 0x1,	DEN,		&mDEN);
	registerPort("RA",			OUT_PORT, 0x1f, RA,			&mRA);
	registerPort("CURS",		OUT_PORT, 0x1,	CURS,		&mCURS);
	registerPort("HS",			OUT_PORT, 0x1,	HS,			&mHS);
	registerPort("VS",			OUT_PORT, 0x1,	VS,			&mVS);
	registerPort("D_OUT",		OUT_PORT, 0xff, D_OUT,		&mD_OUT);

	reset();
}

bool CRTC6845::reset()
{
	mCycleCount = 0;
	mCharRow = 0;
	mScanLine = 0;
	mCharRowPos = 0;

	mNLines = mReg[R6_VerticalDisplayed] * (mReg[R9_MaxScanLineAddress] + 1);
}

// Executed on input port update (when configured)
bool CRTC6845::trigger(int port)
{
	if (port == READ_REQ) {
		
		// Advance time corresponding to one character and check for HS, VS & DEN
		advance(mCycleCount + (int)round((mCPUClock / mCLK)));

		// Advance to next raster memory location
		a = a % (mReg[R1_HorizontalDisplayed] * mNLines);
		if (a == 0) {
			mCharRow = 0;
			mScanLine = 0;
			mCharRowPos = 0;
		}
		
		// If in the active display area, read the memory location and output the address on port RA and the read data on port D_OUT
		if (mDEN) {
			uint8_t a = mDevAdr + mRA;
			uint8_t data;
			return mVideoMem->read(a, data) && updatePort(RA, a) && updatePort(mD_OUT, data);
		}
	}

}

bool CRTC6845::advance(uint64_t stopCycle)
{
	while (mCycleCount < stopCycle) {

		// Advance time corresponding to one character
		mCycleCount += (int)round((mCPUClock / mCLK));

		// Check for Vertical Sync Output
		if (mCharRow == mReg[mR7_VSyncPosition])
			updatePort(VS, 0x1);
		else
			updatePort(VS, 0x10);

		// Check for Horizontal Sync Output
		if (mCharRowPos >= mReg[R2_HSYncPosition] && mCharRowPos <= mReg[R2_HSYncPosition] + mReg[R3_HSyncWidth])
			updatePort(HS, 0x1);
		else
			updatePort(HS, 0x0);

		// Check for Visible part of scan line
		if (mCharRowPos < mReg[R1_HorizontalDisplayed])
			updatePort(DEN, 0x1);
		else
			updatePort(DEN, 0x0);

	}

	
	return true;


}

bool CRTC6845::read(uint16_t adr, uint8_t& data)
{
	int16_t a = adr - mDevAdr;

	if (mAddressRegister > 0 && mAddressRegister < 18 && mRegInfo[mAddressRegister].readable)
		data = mReg[mAddressRegister];
	else
		data = 0xff;
	return true;
}

//
// Register settings for the BBC Micro Model B (from https://beebwiki.mdfs.net/CRTC):
// 
//							MODE
//			Register		|	0	|	1	|	2	|	3	|	4	|	5	|	6	|	7	|
//			----------------+-------+-------+-------+-------+-------+-------+-------+-------+
//			R0	HTC			|	127	|	127	|	127	|	127	|	127	|	127	|	127	|	127	|
//			R1	HDC			|	80	|	80	|	80	|	80	|	40	|	40	|	40	|	40	|
//			R2	HSP			|	98	|	98	|	98	|	98	|	49	|	49	|	49	|	51	|
//			R3	HSW	b3-b0	|	8	|	8	|	8	|	8	|	4	|	4	|	4	|	4	|
//			VSW	b7-b4		|	2	|	2	|	2	|	2	|	2	|	2	|	2	|	2	|
//			R4	VTC			|	38	|	38	|	38	|	30	|	38	|	38	|	30	|	30	|
//			R5	VTA			|	0	|	0	|	0	|	2	|	0	|	0	|	2	|	2	|	plus	*	TV	setting
//			R6	VDC			|	32	|	32	|	32	|	25	|	32	|	32	|	25	|	25	|
//			R7	VSP			|	34	|	34	|	34	|	27	|	34	|	34	|	27	|	27	|
//			R8	INT	b1-b0	|	1	|	1	|	1	|	1	|	1	|	1	|	1	|	1	|	plus	*	TV	setting
//				UND	b3-b2	|		|		|		|		|		|		|		|		|
//				DIS	b5-b4	|	0	|	0	|	0	|	0	|	0	|	0	|	0	|	1	|
//				CUR	b7-b6	|	0	|	0	|	0	|	0	|	0	|	0	|	0	|	2	|
//			R9	NSL			|	7	|	7	|	7	|	9	|	7	|	7	|	9	|	18	|
//			R10	b4-b0		|	7	|	7	|	7	|	7	|	7	|	7	|	7	|	18	|
//				b5			|	1	|	1	|	1	|	1	|	1	|	1	|	1	|	1	|	changed	for	editing	cursor
//				b6			|	1	|	1	|	1	|	1	|	1	|	1	|	1	|	1	|	changed	by	VDU	23,	1
//			R11				|	8	|	8	|	8	|	9	|	8	|	9	|	9	|	19	|
//
bool CRTC6845::write(uint16_t adr, uint8_t data)
{
	int16_t a = adr - mDevAdr;
	if (adr == 0 && (data & 0x1f) < 18)
		mAddressRegister = data & 0x1f;
	else if (mAddressRegister > 0 && mAddressRegister < 18 && mRegInfo[mAddressRegister].writable)
		mReg[mAddressRegister] = data & mRegInfo[mAddressRegister].mask;

	// Update parameters based on the register update															Beeb Mode 0  (80 x 32 chars, 640 x 256 pixels)
	double Tc = 1 / mCLK;										// [us]		Clock period
	double Nsl = mReg[R9_MaxScanLineAddress];					// [lines]	Character Scan Lines (up to 32)						  7 lines
	double Tsl = (mReg[R0_HorizontalTotal] + 1) * Tc;			// [us]		Scan Line duration					(127+1) * 0.5 =	 64 us
	double Tcr = Nsl * Tsl;										// [us]		Character Row Period				7 * 64 =		448 us
	double Nhd = mReg[R1_HorizontalDisplayed] * Tc;				// [us]		Visible horizontal area				80 * 0.5 =		 40 us
	double Nhsp = mReg[R2_HSYncPosition] * Tc;					// [us]		Horizontal sync pos					98 * 0.5 =		49 us
	double Nhsw = mReg[R3_HSyncWidth] * Tc;						// [us]		Horizontal sync pulse width			8 * 0.5 =		 4 us
	double Nvt = (mReg[R4_VerticalTotal] + 1) * Tcr / 1000;		// [ms]		Vertical total (integer part)		(38+1) * 448 = 17.5 ms (57 Hz)
	double Nadj = mReg[R5_VerticalTotalAdjust] * Tsl / 1000;	// [ms]		Vertical total (fraction)							  0 ms
	double Nvd = mReg[R6_VerticalDisplayed] * Tcr;				// [ms]		Visible vertical duration			32 * 448 =	   14.3 ms
	double Nvsp = mReg[mR7_VSyncPosition] * Tcr;				// [ms]		Vertical sync position				34 * 448 =	   15.2 ms	

	mNLines = mReg[R6_VerticalDisplayed] * (mReg[R9_MaxScanLineAddress] + 1);

	return true;
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

inline int CRTC6845::getScanLinesPerFrame()
{
	return (int) round(mReg[R4_VerticalTotal] + 1 + mReg[R5_VerticalTotalAdjust]);
}

inline int CRTC6845::getFrameRate()
{
	//																											Beeb Mode 0 (80 x 32 chars, 640 x 256 pixels)
	double Tc = 1.0 / mCLK;										// [us]		Clock period
	double Nsl = mReg[R9_MaxScanLineAddress];					// [lines]	Character Scan Lines (up to 32)						  7 lines
	double Tsl = (mReg[R0_HorizontalTotal] + 1) * Tc;			// [us]		Scan Line duration					(127+1) * 0.5 =	 64 us
	double Tcr = Nsl * Tsl;										// [us]		Character Row Period				7 * 64 =		448 us
	double Nvt = (mReg[R4_VerticalTotal] + 1) * Tcr / 1000;		// [ms]		Vertical total (integer part)		(38+1) * 448 = 17.5 ms (57 Hz) 

	return 1000/Nvt;
}