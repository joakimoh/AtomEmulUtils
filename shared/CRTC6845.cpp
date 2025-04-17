#include "CRTC6845.h"
#include <iomanip>

//
// Note:
// The video memory address provided will not be used as the start address of the memory (and size) is programmed (R12 & R13 for the start address)
// 
//
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
// R4        Vertical total                    38    38    38    30    38    38    30    30 => (18+2) * (30+1) + 2*2 = 624 scan lines for mode 7 and (7+1)*(38+1) = 312 scan lines for mode 0-2,4-5
// R5        Vertical total adjust              0     0     0     2     0     0     2     2
// R6        Vertical displayed characters     32    32    32    25    32    32    25    25 => (18+2) * 25 = 500 visible scan lines for mode 7
// R7        Vertical sync position            34    34    34    27    34    34    27    28 
// R8        Interlace mode(bits 0, 1)          1     1     1     1     1     1     1     3
//           +Display delay(bits 4, 5)          0     0     0     0     0     0     0     1
//           +Cursor delay(bits 6, 7)           0     0     0     0     0     0     0     2
// R9        Scan lines per character           7     7     7     9     7     7     9    18 => (18+2)  = 20 raster lines for mode 7 and (7+1) = 8 raster lines for modes 0-2,4-5
// R10       Cursor start(bits 0 - 4)           7     7     7     7     7     7     7    18
//           Cursor type(bit 5)                 1     1     1     1     1     1     1     1
//           Cursor blink(bit 6)                1     1     1     1     1     1     1     1
// R11       Cursor end                         8     8     8     9     8     8     9    19
// R12, R13   Screen start address / 8			-     -     -     -     -     -     -     -
// R14, R15   Cursor position					-     -     -     -     -     -     -     -
// R16, R17   Light pen position				-     -     -     -     -     -     -     -
//
CRTC6845::CRTC6845(
	string name, uint16_t adr, double cpuclock, uint8_t waitStates, ALLEGRO_BITMAP* disp, int dispW, int dispH, DebugManager  *debugManager, ConnectionManager* connectionManager
) : VideoDisplayUnit(name, CRTC6845_DEV, cpuclock, waitStates, adr, 0x2, disp, dispW, dispH, 0x0 /* dummy adr as not used by the 6845 */, debugManager, connectionManager)
{

	registerPort("CLK",			IN_PORT,  0x3,	CLK,		&mCLK);
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

	mField = 0;

	mCycleCount = 0;
	mCharRow = 0;
	mScanLine = 0;
	mCharCol = 0;
	mInitialised = false;

	// Reset ports
	mDISPTMG = 0x0;
	mRA = 0x0;
	mCUDISP = 0x1;
	mHS = 0x0;
	mVS = 0x0;
	updatePort(DISPTMG, mDISPTMG);
	updatePort(RA, mRA);
	updatePort(CUDISP, mCUDISP);
	updatePort(HS, mHS);
	updatePort(VS, mVS);

	// Internal registers are not affected by the RESET - only the internal logic is...
	//for (int i = 0; i < 18; mReg[i++] = 0);
	
	updateSettings(0);
	

	return true;
}

// Called by other device to get next memory address to fetch char/graphics data from
bool CRTC6845::getMemFetchAdr(uint16_t &adr)
{
	adr = 0x0;

	if (!mInitialised)
		return true;

	updateOutputs();

	// If in the active display area, update the fetch address and cursor position
	
	if (mDISPTMG) {;
		adr = mStartAdr + mCharRow * mActiveRowChars + mCharCol;
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

	if (mDM->debug(DBG_VERBOSE) && mCLK != pCLK)
		printSettings();
	pCLK = mCLK;

	// Increase char column
	// The 6845 linear address generator repeats the same sequence of addresses for each scan line of a character row
	mCharCol = (mCharCol + 1) % mCharCols;

	// Check for new character row
	if (mCharCol == 0) {

		if (mDM->debug(DBG_VDU))
			cout << "CRTC SCAN LINE = " << dec << mScanLine << ", RASTER LINE = " << (int) mRA << ", CHAR ROW = " << mCharRow << 
			" (" << mActiveRows << ")\n";
		
		if (interlaceOn()) {
			mScanLine = (mScanLine + 2) % mScreenScanLines;
			if ((mField % 2 == 0 && mScanLine == 0) || (mField % 2 == 1 && mScanLine == 1)) { // start of new field
				
				mField++;
				mCharRow = 0;
				mCharCol = 0;
				if (interlaced(mInterlaceMode)) {
					updatePort(RA, 0);
					if (mField % 2 == 0)
						mScanLine = 0; // Even field => start at scan line 0
					else
						mScanLine = 1; // Odd field => start at scan line 1
				}
				else { // interlaced_video(mInterlaceMode)
					if (mField % 2 == 0) {
						updatePort(RA, 0); // Even field => start at raster line 0
						mScanLine = 0; // next field is even => first scan line is 0
					}
					else {
						updatePort(RA, 1); // Odd field => start at raster line 1
						mScanLine = 1; // next field is odd => first scan line is 1
					}
				}
			}
			else {
				if (interlaced(mInterlaceMode)) {
					updatePort(RA, (mRA + 1) % mCharLines);
					if (mRA == 0)
						mCharRow = (mCharRow + 1) % mCharRows;
				}
				else { // interlaced_video(mInterlaceMode)
					updatePort(RA, (mRA + 2) % mCharLines);
					if ((mField % 2 == 0 && mRA == 0) || (mField % 2 == 1 && mRA == 1)) { // new character row
						mCharRow = (mCharRow + 1) % mCharRows;
						if (mField % 2 == 0)
							updatePort(RA, 0); // Even field and new character row => start raster line at 0
						else
							updatePort(RA, 1); // Odd field and new character row => start raster line at 1
					}
				}
			}
		}

		else {
			mScanLine = (mScanLine + 1) % mScreenScanLines;
			if (mScanLine == 0) {
				mField++;
				mCharRow = 0;
				mCharCol = 0;
				updatePort(RA, 0);
			}
			else {
				updatePort(RA, (mRA + 1) % mCharLines);
				if (mRA == 0)
					mCharRow = (mCharRow + 1) % mCharRows;
			}
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
	// Display skew not considered (yet)!!!
	//

	if (mCharCol  < mActiveRowChars && mCharRow < mActiveRows)
		updatePort(DISPTMG, 0x1);
	else
		updatePort(DISPTMG, 0x0);

	// Check for cursor being selected
	// Cursor skew not considered (yet)!!!
	// Cursor blink not implemented (yet)!!!
	//
	int cursor_first_line = mReg[R10_CursorStart] & 0x1f;
	int cursor_last_line = mReg[R11_CursorEnd] & 0x1f;
	int cursor_disp_mode = (mReg[R10_CursorStart] >> 5) & 0x3; // 00: Non-blink,  01: non-display, 10: blink 16-field, 11: blink 32-field
	mCursorLocation = ((mReg[R14_CursorH] & 0x3f) << 8) | mReg[R15_CursorL];// +mCursSkew - mCharSkew;
	bool cursor_on = (
		cursor_disp_mode == 0x0 ||
		(cursor_disp_mode == 0x2 /* && mField % 16 < 8*/) ||
		(cursor_disp_mode == 0x3 /* && mField % 32 < 16*/)
		) && mRA >= cursor_first_line && mRA <= cursor_last_line;
	if (cursor_on && mStartAdr + mCharRow * mActiveRowChars + mCharCol == mCursorLocation)
		updatePort(CUDISP, 0x1);
	else
		updatePort(CUDISP, 0x0);

	return true;
}

bool CRTC6845::advance(uint64_t stopCycle)
{
	bool reset_transition = (mRESET == 0 && mRESET != pRESET);
	pRESET = mRESET;

	if (reset_transition)
		reset();

	if (!mInitialised) {
		mCycleCount = stopCycle;
		return true;
	}

	while (mCycleCount < stopCycle) {
		updateOutputs();
		advanceChar();
	}
	
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

bool CRTC6845::dump(uint16_t adr, uint8_t& data)
{
	if (selected(adr)) {
		int16_t a = adr - mMemorySpace.adr;
		if (mAddressRegister > 0 && mAddressRegister < 18 && mRegInfo[mAddressRegister].readable)
			data = mReg[mAddressRegister];
		else
			data = 0xff;
		return true;
	}
	return false;
}


//
// Only the start address (R12/13) and cursor registers (R14/R15) should normally be
// changed during display operation as an unpredictable behaviour
// would otherwise be the result.
// 
// Writing into the cursor registers should be performed during the horizontal and vertical
// retrace period.
// 
// Writing into the start address registers for scrolling and paging should be
// performed during the horizontal and vertical display period.
//
bool CRTC6845::write(uint16_t adr, uint8_t data)
{

	if (!selected(adr))
		return false;

	int written_reg = 999;

	int16_t a = adr - mMemorySpace.adr;
	if (a == 0 && (data & 0x1f) < 18)
		mAddressRegister = data & 0x1f;
	else if (mAddressRegister >= 0 && mAddressRegister < 18 && mRegInfo[mAddressRegister].writable) {
		written_reg = mAddressRegister;
		mReg[written_reg] = data & mRegInfo[written_reg].mask;
		if (written_reg < 16)
			mRegWriteCnt[written_reg]++;
	}

	// Update internal state based on the register update
	updateSettings(written_reg);


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
// The visible part of a field  is determined by the vertical sync pulse location,
// the no of scan lines and the retrace time (that time is not configured but 
// rather a property of the connected monitor - we will assume 5% of the field
// duration for this). The vertical sync pulse starts a new field but time for
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
void CRTC6845::updateSettings(uint8_t reg)
{

	//
	// Parameters specified by CRTC registers
	//

	// Total no of character columns
	mCharCols = mReg[R0_HorizontalTotal] + 1;

	// No of active (displayed) character columns
	mActiveRowChars = mReg[R1_HorizontalDisplayed];

	// No of raster lines per character row
	int pCharLines = mCharLines;
	if ((mReg[R8_InterlaceMode] & 0x3) == 0x3) 
		mCharLines = mReg[R9_MaxScanLineAddress] + 2; // interlace video mode
	else
		mCharLines = mReg[R9_MaxScanLineAddress] + 1; // non-interlace and interlace modes

	// Horizontal Sync
	mHzSyncPos = mReg[R2_HSYncPosition] + 1;
	mHzSyncPulseW = mReg[R3_SyncWidth] & 0xf;

	// Total no of active (displatyed) character rows with unique content over a frame (i.e. a pair of fields)
	mCharRows = mReg[R4_VerticalTotal] + 1;

	// No of active (i.e. displayed) character rows with unique content over a frame (i.e. a pair of fields)
	mActiveRows = mReg[R6_VerticalDisplayed];
	

	// Vertical Sync for a unique content over a frame (i.e. a pair of fields)
	mVSyncRow = mReg[R7_VSyncPosition];
	mVSyncPulseH = (mReg[R3_SyncWidth] >> 4) & 0xf;
	if (mVSyncPulseH == 0)
		mVSyncPulseH = 16;
	mVSyncLine = mVSyncRow * mCharLines;

	// Interlace mode
	mInterlaceMode = mReg[R8_InterlaceMode] & 0x3;

	// Character skew
	mCharSkew = (mReg[R8_InterlaceMode] >> 4) & 0x3;
	if (mCharSkew == 3)
		mCharSkew = 0;

	// Cursor skew
	mCursSkew = (mReg[R8_InterlaceMode] >> 6) & 0x3;
	if (mCursSkew == 3)
		mCursSkew = 0;

	// Video memory start address
	mStartAdr = (mReg[R12_StartAddressH] << 8) | mReg[R13_StartAddressL];

	// Current cursor address
	mCursorLocation = ((mReg[R14_CursorH] & 0x3f) << 8) | mReg[R15_CursorL];


	// Determine field parameters based on the register (frame) parameters
	mFieldActiveRows = mActiveRows;
	mVFieldSyncPulseH = mVSyncPulseH;
	mScanLines = mCharRows * mCharLines + mReg[R5_VerticalTotalAdjust]; // adjust is per field and rows per field => don't multiply R5 value by 2
	mFieldScanLines = mScanLines;
	mActiveLines = mActiveRows * mCharLines;
	mFieldActiveLines = mActiveLines;
	mFieldVSyncRow = mVSyncRow;
	mFieldVSyncLine = mVSyncLine;
	mFieldCharRows = mCharRows;
	if (interlaced(mInterlaceMode)) {
		mScreenActiveLines = mActiveLines * 2;
		mScreenVSyncLine = mVSyncLine * 2;
		mScreenVSyncPulseH = mVSyncPulseH * 2;
	}
	else {
		mScreenActiveLines = mActiveLines;
		mScreenVSyncLine = mVSyncLine;
		mScreenVSyncPulseH = mVSyncPulseH;
	}
	if (interlaced_video(mInterlaceMode)) { // for video interlace mode, the parameters were per frame => divide by 2 to get field parameter
		mFieldCharRows = mCharRows / 2;
		mFieldActiveRows = mActiveRows / 2;
		mVFieldSyncPulseH = mVSyncPulseH / 2;
		mScanLines = mCharRows * mCharLines + 2 * mReg[R5_VerticalTotalAdjust]; // adjust is per field but rows per frame => multiply R5 value by 2
		mFieldScanLines = mScanLines / 2;
		mFieldActiveLines = mActiveLines / 2;
		mFieldVSyncRow = mVSyncRow / 2;
		mFieldVSyncLine = mVSyncLine / 2;	
	}


	//
	// The scan lines and rasters per character row configured by R9 is to be interpreted as below:
	// 
	// Mode						Configured	Unique	Configured		unique rasters			active rasters							active rasters
	//							scan lines	scan	rasters			per pair				in even field						in odd field
	//										lines					of fields = one frame
	//																						row #0				row #1				row #0			row #1
	// Non-interlaced			l=n(R4+1)		l		n=R9+1			n					0,1,2,...,n-1		same as row #0		0,1,2,...,n-1	same as v #0
	// Interlaced				l=n(R4+1)		2l		n=R9+1			2n					0,2,4,...,2n-2		same as row #0		1,3,5,...,2n-1	same as row #0			
	// Interlace & video		l=n(R4+1)		l		n=R9+2			n					0,2,4,...,n-2		same as row #0		1,3,5,...,n-1	same as row #0		n is even
	//																						0,2,4,...,n-1		1,3,5,...,n-2		1,3,5,...,n-2	0,2,4,...,n-1		n is odd
	//	
	if (non_interlaced(mInterlaceMode) || interlaced_video(mInterlaceMode)) {
		mUniqueCharLines = mCharLines; // the specified no of unique content character raster lines is the same as the no of character raster lines visible on the screen
		mScreenScanLines = (int)round(mScanLines);
	}
	else {// interlaced(mInterlaceMode) // the raster and scan lines have the same content for odd and even fields and are just duplicated to make a complete frame
		mUniqueCharLines = 2 * mCharLines; // screen has twice as many visible character raster lines as the specifed no of unique content character raster lines
		mScreenScanLines = (int)round(2 * mScanLines); // screen has twice as many visible scan lines as the specifed no of unique content scan lines
	}

	//
	// Borders and active area for the scan lines visible on a screen
	// 

	// Make assumptions about retracing (10% vertical and 30% horizontal)
	mRetraceChars = mActiveRowChars * 0.3;
	if (mRetraceChars % 2 == 1)
		mRetraceChars++;
	mRetraceLines = (int)round(mScreenScanLines * 0.1);
	if (mRetraceLines % 2 == 1) // Make sure no of retrace lines is even
		mRetraceLines++;
	
	// Vertical scan lines: top border, active lines, sync pulse, bottom border
	mVisibleScanLines = mScreenScanLines - mRetraceLines;
	int vt_borders = mVisibleScanLines - mScreenActiveLines;
	vt_borders -= vt_borders % 4; // make sure vt_borders / 2 becomes even
	mTopBorderLines = mBottomBorderLines = vt_borders / 2;
	mVisibleScanLines = mTopBorderLines + mScreenActiveLines + mBottomBorderLines;

	// Horizontal line: left border, active chars, sync pulse, right border (all in unit 'char')
	mVisibleChars = mCharCols - mRetraceChars;
	int hz_borders = mVisibleChars - mActiveRowChars;
	if (hz_borders % 2 == 1)
		hz_borders++;
	mLeftBorderChars = mRightBorderChars = hz_borders / 2;
	mLeftBorderChars += mCharSkew; // adjust for character skewing
	mRightBorderChars = hz_borders - mLeftBorderChars;
	mVisibleChars = mLeftBorderChars + mActiveRowChars + mRightBorderChars;


	// After power-on each writable register should have been updated to consider the
	// CRTC as ready for operation
	if (reg < 16) {
		int n_updated_regs;
		for (n_updated_regs = 0; n_updated_regs < 16 && mRegWriteCnt[n_updated_regs] > mRegUpdates; n_updated_regs++);
		if (n_updated_regs == 16) {
			mInitialised = true;
			mRegUpdates++;
			if (mDM->debug(DBG_VERBOSE))
				printSettings();
		}
	}

	// Reset the internal state if signficant changes have been made to timing
	if (reg == R0_HorizontalTotal || reg == R4_VerticalTotal || reg < 10) {
		mCharRow = 0;
		mScanLine = 0;
		mCharCol = 0;
		mDISPTMG = 0x0;
		mRA = 0x0;
		mCUDISP = 0x1;
		mHS = 0x0;
		mVS = 0x0;
		updatePort(DISPTMG, mDISPTMG);
		updatePort(RA, mRA);
		updatePort(CUDISP, mCUDISP);
		updatePort(HS, mHS);
		updatePort(VS, mVS);
		mField = 0;
	}


}


int CRTC6845::getUniqueCharScanLines()
{
	return mUniqueCharLines;
}

void CRTC6845::printSettings()
{

	// Update parameters based on the register update														
	double Tc = 1.0 / mCLK;	

	cout << dec;
	for (int i = 0; i < 18; i++)
		cout << "R" << setw(2) <<  setfill('0') << i << ": 0x" << hex << setfill('0') << setw(2) << (int)mReg[i] << dec << "\n";

 	cout << "CLK:                                                " << (int) mCLK << "\tMhz\n";
	cout << "Field Rate:                                         " << getFieldRate() << "\tHz\n";
	cout << "No of character rows per field:                     " << mFieldCharRows << "\tRows\n";
	cout << "No of character rows per frame:                     " << mCharRows << "\tRows\n";
	cout << "Scan Lines per Character:                           " << mCharLines << "\tlines\n";
	cout << "Unique Content Scan Lines per Character:            " << mUniqueCharLines << "\tlines\n";
	cout << "No of scan lines per field:                         " << round(mFieldScanLines) << "\tlines\n";
	cout << "No of unique scan lines per frame:                  " << mScreenScanLines << "\tlines\n";
	cout << "Retrace lines:                                      " << mRetraceLines << "\tlines\n";
	cout << "Top border lines:                                   " << mTopBorderLines << "\tlines\n";
	cout << "Active scan lines per field:                        " << mFieldActiveLines << "\tlines\n";
	cout << "Active scan lines per screen:                       " << mScreenActiveLines << "\tlines\n";
	cout << "Bottom border lines:                                " << mBottomBorderLines << "\tlines\n";
	cout << "Scan Line duration:                                 " << mCharCols * Tc << "\tus\n";
	cout << "Total no of characters per line:                    " << mCharCols << "\tchars\n";
	cout << "Retrace characters per line:                        " << mRetraceChars << "\tchars\n";
	cout << "Left border chars:                                  " << mLeftBorderChars << "\tchars\n";
	cout << "Active characters per line:                         " << mActiveRowChars << "\tchars\n";
	cout << "Right border chars:                                 " << mRightBorderChars << "\tchars\n";
	cout << "Vertical sync position:                             " << mScreenVSyncLine << " lines (" << round(mScreenVSyncLine * mCharCols * Tc / 1000) << " ms)\n";
	cout << "Horizontal sync position:                           " << mHzSyncPos << " chars (" << round(mHzSyncPos*Tc) << " us)\n";
	cout << "Horizontal sync pulse width:                        " << mHzSyncPulseW << " chars (" << round(mHzSyncPulseW*Tc) << " us)\n";
	cout << "Vertical sync pulse width:                          " << mScreenVSyncPulseH << " lines (" << round(mScreenVSyncPulseH *mCharCols*Tc) << " us)\n";
	cout << "Start address:                                      0x" << hex << mStartAdr << "\n";
	cout << "Cursor raster lines:                                [" << dec << (int)(mReg[R10_CursorStart] & 0x1f) << ":" << (int)mReg[R11_CursorEnd] << "]\n";
	cout << "Cursor position:                                    0x" << hex << ((mReg[R14_CursorH] << 8) | mReg[R15_CursorL]) << "\n";
	cout << "Interlace mode:                                     " << _INTERLACE_MODE(mInterlaceMode) << "\n";
	cout << "Character skew:                                     " << mCharSkew << "\tchars\n";
	cout << "Cursor skew:                                        " << mCursSkew << "\tchars\n";
	int cursor_disp_mode = (mReg[R10_CursorStart] >> 5) & 0x3; // 00: Non-blink,  01: non-display, 10: blink 16-field, 11: blink 32-field
	cout << "Cursor mode:                                        " << (cursor_disp_mode==0?"Fixed":(cursor_disp_mode==1?"None":(cursor_disp_mode==2?"Blink-16":"Blink-32"))) <<
		"\n";
}

bool CRTC6845::getVisibleCharArea(int &charsPerLine, int &lines)
{
	charsPerLine = mLeftBorderChars + mActiveRowChars + mRightBorderChars;
	lines = mVisibleScanLines;
	return true;
}

inline double CRTC6845::getScanLineDuration()
{
	return mCharCols / mCLK;
}

inline double CRTC6845::getScanLinesPerField()
{
	return mFieldScanLines;

}

inline double CRTC6845::getScreenScanLines()
{
	return mScreenScanLines;
}

inline double CRTC6845::getFieldRate()
{
	if (mScanLines * mCharCols > 0) {
		if (interlaceOn())
			return mCLK * 1e6 / ((mFieldScanLines + 0.5) * mCharCols);
		else
			return mCLK * 1e6 / (mFieldScanLines * mCharCols);
	}
	else
		return 50;
}

inline int CRTC6845::getCharScanLines()
{
	return mCharLines;
}

inline int CRTC6845::getVerticalSyncLine()
{
	return mScreenVSyncLine;
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
	return mScreenActiveLines;
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

// Check if interlace is enabled (On)
inline bool CRTC6845::interlaceOn()
{
	return (!non_interlaced(mInterlaceMode));
}

// Get scan line offset (0 for even field or non-interlaced mode, 1 for odd field)
int CRTC6845::fieldScanLineOffset()
{
	return (mField % 2);
}