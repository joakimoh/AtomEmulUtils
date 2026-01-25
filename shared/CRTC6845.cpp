#include "CRTC6845.h"
#include <iomanip>
#include "Utility.h"


//
// Emulation of the Hitachi 6845 Video Display Unit
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
	string name, uint16_t adr, double cpuClock, uint8_t waitStates, DebugTracing  *debugTracing,
	ConnectionManager* connectionManager, DeviceManager *deviceManager
//) : VideoDisplayUnit(name, CRTC6845_DEV, display, cpuClock, waitStates, adr, 0x2, 0x0 /* dummy adr as not used by the 6845 */, debugTracing,
//	connectionManager, deviceManager)
): MemoryMappedDevice(name, CRTC6845_DEV, VDU_DEVICE, cpuClock, waitStates, adr, 0x2, debugTracing, connectionManager, deviceManager)
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
	
	updateSettings(0);
	

	return true;
}

// Called by other device to get next memory address to fetch char/graphics data from
bool CRTC6845::getMemFetchAdr(uint16_t &adr)
{
	adr = 0x0;

	//if (!mInitialised)
	//	return true;

	updateOutputs();

	// If in the active display area (excluding skew!), update the fetch address and cursor position
	if (mCharCol >= mCharSkew_R8 && mCharCol < mActiveRowChars_R1+ mCharSkew_R8 && mCharRow < mActiveRows_R6) {
		adr = mStartAdr_R12_R13 + mCharRow * mActiveRowChars_R1 + mCharCol - mCharSkew_R8;
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

	if (DBG_LEVEL_DEV(this, DBG_GRAPHICS) && mCLK != pCLK)
		printSettings();
	pCLK = mCLK;

	// Increase the character column one step.
	// The 6845 linear address generator repeats the same sequence of addresses for each scan line of a character row
	mCharCol = (mCharCol + 1) % mCharCols_R0;

	// Check for a new character raster line, a new character rowm and for a new field
	if (mCharCol == 0) {

		// A new character row

		DBG_LOG(
			this, DBG_GRAPHICS, "CRTC SCAN LINE = "  + to_string(mScanLine) +
			", RASTER LINE = " + to_string(mRA) + ", CHAR ROW = "  + to_string(mCharRow) +
			" (" + to_string(mActiveRows_R6) + ")"
		);
		
		// Step to next scan line.
		// Only every second scan line is however used per field.
		// An even field uses even scan lines (0,2,4,...) whereas an odd field use odd scan lines (1,3,5,...).
		mScanLine = (mScanLine + 2) % mScreenScanLines;

		if ((mField % 2 == 0 && mScanLine == 0) || (mField % 2 == 1 && mScanLine == 1)) {
			
			// Start of new field
				
			mField++;
			mCharRow = 0;
			mCharCol = 0;
			if (!interlaced_video(mInterlaceMode_R8)) {

				// For non-interlace and interlace mode, either even or odd scan lines are used per field.
				// The raster lines are however always in sequence, i.e., 0,1,2,3,... as the same character 
				// rasters will be repeated for even and even fields.

				updatePort(RA, 0);
				if (mField % 2 == 0)
					mScanLine = 0; // Even field => start at scan line 0
				else
					mScanLine = 1; // Odd field => start at scan line 1
			}
			else {

				// For interlaced video, either even or odd scan lines are used per field.
				// The raster lines follow the same pattern, i.e., 0,2,4,8... or 1,3,5,7,... as different character
				// rasters will be provided for even and odd fields.

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

			//  A new line of the current field

			if (!interlaced_video(mInterlaceMode_R8)) {
				
				// for non-interlace and interlace mode, the same content is used for odd & even fields

				updatePort(RA, (mRA + 1) % mCharLines_R9);
				if (mRA == 0)
					mCharRow++; // Row no will be reset when a new field starts => no need to limit here
			}
			else {
				
				// for interlaced video, different content is used for odd & even fields

				updatePort(RA, (mRA + 2) % mCharLines_R9);
				if ((mField % 2 == 0 && mRA == 0) || (mField % 2 == 1 && mRA == 1)) { // new character row
					mCharRow++; // Row no will be reset when a new field starts => no need to limit here
					if (mField % 2 == 0)
						updatePort(RA, 0); // Even field and new character row => start raster line at 0
					else
						updatePort(RA, 1); // Odd field and new character row => start raster line at 1
				}
			}
			}
	
	}
	

	return true;
}

bool CRTC6845::updateOutputs()
{

	// Check for Vertical Sync Output
	if (mCharRow >= mVSyncRow_R7 && mCharRow < mVSyncRow_R7 + mVSyncPulseH_R3) {
		if (mVS == 0)
			updatePort(VS, 0x1);
	}
	else {
		if (mVS == 1)
			updatePort(VS, 0x0);
	}

	// Check for Horizontal Sync Output
	if (mCharCol >= mHzSyncPos_R2 && mCharCol < mHzSyncPos_R2 + mHzSyncPulseW_R3) {
		if (mHS == 0)
			updatePort(HS, 0x1);
	}	
	else {
		if (mHS == 1)
			updatePort(HS, 0x0);
	}

	//
	// Check for Visible active part of scan line
	// 
	// Display skew (mCharSkew_R8) is also considered by moving the left border - see updateSettings()
	//
	if (mCharCol >= mCharSkew_R8 && mCharCol  < mActiveRowChars_R1 + mCharSkew_R8 && mCharRow < mActiveRows_R6)
		updatePort(DISPTMG, 0x1);
	else
		updatePort(DISPTMG, 0x0);

	// Check for cursor being selected
	// Cursor skew is considered when setting CUDISP
	//
	int cursor_first_line = mReg[R10_CursorStart] & 0x1f;
	int cursor_last_line = mReg[R11_CursorEnd] & 0x1f;
	int cursor_disp_mode = (mReg[R10_CursorStart] >> 5) & 0x3; // 00: Non-blink,  01: non-display, 10: blink 16-field, 11: blink 32-field
	mCursorLocation_R14_R15 = ((mReg[R14_CursorH] & 0x3f) << 8) | mReg[R15_CursorL];// + mCursSkew_R8;
	bool cursor_on = (
		cursor_disp_mode == 0x0 ||
		(cursor_disp_mode == 0x2 && mField % 16 < 8) ||
		(cursor_disp_mode == 0x3 && mField % 32 < 16)
		) && mRA >= cursor_first_line && mRA <= cursor_last_line && mDISPTMG;
	if (cursor_on && mStartAdr_R12_R13 + mCharRow * mActiveRowChars_R1 + mCharCol + mCharSkew_R8 == mCursorLocation_R14_R15 + mCursSkew_R8) {
		DBG_LOG(this, DBG_GRAPHICS,
			"Cursor active for CRTC scan line " + to_string(mScanLine) + ", raster line " +
			to_string(mRA) + ", char row " + to_string(mCharRow) + ", char col " + to_string(mCharCol) +
			", start address 0x" + Utility::int2HexStr(mStartAdr_R12_R13, 2) +
			", cursor location 0x" + Utility::int2HexStr(mCursorLocation_R14_R15, 2) + ", active row chars " + to_string(mActiveRowChars_R1)
		);
		updatePort(CUDISP, 0x1);
	}
	else
		updatePort(CUDISP, 0x0);

	return true;
}

bool CRTC6845::advanceUntil(uint64_t stopCycle)
{
	bool reset_transition = mRESET != pRESET;
	pRESET = mRESET;

	DBG_LOG_COND(reset_transition, this, DBG_RESET, "RESET => " + to_string(mRESET) + "\n");

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
	if (!MemoryMappedDevice::triggerBeforeRead(adr, data))
		return false;

	int16_t a = adr - mStartOfSpace;

	if (mAddressRegister > 0 && mAddressRegister < 18 && mRegInfo[mAddressRegister].readable)
		data = mReg[mAddressRegister];
	else
		data = 0xff;
	return true;
}

bool CRTC6845::dump(uint16_t adr, uint8_t& data)
{
	if (selected(adr)) {
		int16_t a = adr - mStartOfSpace;
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

	int16_t a = adr - mStartOfSpace;
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


	// Call parent c]lass to trigger scheduling of other devices when applicable
	return MemoryMappedDevice::triggerAfterWrite(adr, data);
}

//
// Update CRTC parameters based on the register settings.
// 
// 
void CRTC6845::updateSettings(uint8_t reg)
{

	//
	// Parameters specified by CRTC registers
	//

	// Total no of character columns
	mCharCols_R0 = mReg[R0_HorizontalTotal] + 1;
	// Ensure at least a value of 2 as the mCharCol updating will fail otherwise
	if (mCharCols_R0 < 2)
		mCharCols_R0 = 2;

	// No of active (displayed) character columns
	mActiveRowChars_R1 = mReg[R1_HorizontalDisplayed];

	// No of raster lines per character row
	int pCharLines = mCharLines_R9;
	if ((mReg[R8_InterlaceMode] & 0x3) == 0x3) 
		mCharLines_R9 = mReg[R9_MaxScanLineAddress] + 2; // interlace video mode
	else
		mCharLines_R9 = mReg[R9_MaxScanLineAddress] + 1; // non-interlace and interlace modes

	// Horizontal Sync
	mHzSyncPos_R2 = mReg[R2_HSYncPosition] + 1;
	mHzSyncPulseW_R3 = mReg[R3_SyncWidth] & 0xf;

	// Total no of active (displatyed) character rows with unique content over a frame (i.e. a pair of fields)
	mCharRows_R4 = mReg[R4_VerticalTotal] + 1;

	// No of active (i.e. displayed) character rows with unique content over a frame (i.e. a pair of fields)
	mActiveRows_R6 = mReg[R6_VerticalDisplayed];
	

	// Vertical Sync for a unique content over a frame (i.e. a pair of fields)
	mVSyncRow_R7 = mReg[R7_VSyncPosition];
	mVSyncPulseH_R3 = (mReg[R3_SyncWidth] >> 4) & 0xf;
	if (mVSyncPulseH_R3 == 0)
		mVSyncPulseH_R3 = 16;
	mVSyncLine_R7xR9 = mVSyncRow_R7 * mCharLines_R9;

	// Interlace mode
	mInterlaceMode_R8 = mReg[R8_InterlaceMode] & 0x3;

	// Character skew
	mCharSkew_R8 = (mReg[R8_InterlaceMode] >> 4) & 0x3;
	if (mCharSkew_R8 == 3)
		mCharSkew_R8 = 0;

	// Cursor skew
	mCursSkew_R8 = (mReg[R8_InterlaceMode] >> 6) & 0x3;
	if (mCursSkew_R8 == 3)
		mCursSkew_R8 = 0;

	// Video memory start address
	mStartAdr_R12_R13 = (mReg[R12_StartAddressH] << 8) | mReg[R13_StartAddressL];

	// Current cursor address
	mCursorLocation_R14_R15 = ((mReg[R14_CursorH] & 0x3f) << 8) | mReg[R15_CursorL];


	// No of active and no of total scan lines
	int scan_lines_R4xR9_R5 = mCharRows_R4 * mCharLines_R9 + mReg[R5_VerticalTotalAdjust]; // adjust is per field and rows per field => don't multiply R5 value by 2
	mActiveLines_R6xR9 = mActiveRows_R6 * mCharLines_R9;
	//R4_R5_R9

	// Determine actual screen scan lines based on the raster scan mode (non-interlace, interlace, interlace video)
	if (non_interlaced(mInterlaceMode_R8)) {

		// In the non-interlace mode, the rasters of even and odd fields are scanned duplicately.
		// There would then in theory be a thin blank 1/2 scan line between each scan line compared to
		// the interlace mode, but here we igore this and fill that 1/2 scan line as well (duplicately as for the interlace mode).
		// Therefore each screen scan line parameter will be twice that as specified by the CRTC registers.

		mScreenRasterLines = mCharLines_R9 * 2;
		mScreenActiveLines = mActiveLines_R6xR9 * 2;
		mScreenVSyncLine = mVSyncLine_R7xR9 * 2;
		mScreenVSyncPulseH = mVSyncPulseH_R3 * 2;
		mScreenScanLines = 2 * scan_lines_R4xR9_R5; // screen has twice as many visible scan lines as the specifed no of scan lines

	}
	else if (interlaced(mInterlaceMode_R8)) {

		// In the interlace mode,,the rasters of the odd fields are scanned in the middle of the even fields.
		// Therefore each screen scan line parameter will be twice that as specified by the CRTC registers.
		// There will be an extra 1/2 can line added to each field (and therefore one extra to the complete screen) as well,
		// For PAL, there will e.g., be 312x2=624 screen scan lines based on configuration but with this extra 1/2 scan line per field,
		// the total no of screen scan lines will become 625.

		mScreenRasterLines = mCharLines_R9 * 2;
		mScreenActiveLines = mActiveLines_R6xR9 * 2;
		mScreenVSyncLine = mVSyncLine_R7xR9 * 2;
		mScreenVSyncPulseH = mVSyncPulseH_R3 * 2;
		mScreenScanLines = 2 * scan_lines_R4xR9_R5; // screen has twice as many visible scan lines as the specifed no of unique content scan lines

	}
	else { // if (interlaced_video(mInterlaceMode_R8)) {

		// For the video interlace mode, the configured parameters were already per frame except for the vertical adjust

		mScreenRasterLines = mCharLines_R9;
		mScreenActiveLines = mActiveLines_R6xR9;
		mScreenVSyncLine = mVSyncLine_R7xR9;
		mScreenVSyncPulseH = mVSyncPulseH_R3;
		scan_lines_R4xR9_R5 = mCharRows_R4 * mCharLines_R9 + 2 * mReg[R5_VerticalTotalAdjust]; // adjust is per field but rows per frame => multiply R5 value by 2
		mScreenScanLines = scan_lines_R4xR9_R5;

	}
	// Ensure at least a value of 2 as the mScreenScan updating will fail otherwise
	if (mScreenScanLines < 2)
		mScreenScanLines = 2;

	// After power-on each writable register should have been updated to consider the
	// CRTC as ready for operation
	if (reg < 16) {
		int n_updated_regs;
		for (n_updated_regs = 0; n_updated_regs < 16 && mRegWriteCnt[n_updated_regs] > mRegUpdates; n_updated_regs++);
		if (n_updated_regs == 16) {
			mInitialised = true;
			mRegUpdates++;
			if (DBG_LEVEL_DEV(this, DBG_GRAPHICS))
				printSettings();
		}
	}

	// Reset the internal state if significant changes have been made to timing
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
		if (DBG_LEVEL_DEV(this, DBG_GRAPHICS)) {
			stringstream sout;
			outputState(sout);
			DBG_LOG(this, DBG_GRAPHICS, sout.str());
		}

	}


}

void CRTC6845::printSettings()
{
	outputState(cout);
}

bool CRTC6845::outputState(ostream& sout)
{
	// Update parameters based on the register update														
	double Tc = 1.0 / mCLK;

	sout << dec << "\n";
	for (int i = 0; i < 18; i++)
		sout << mRegInfo[i].descShort << hex << " = 0x" << setw(2) << setfill('0') << (int)mReg[i] << dec << " " << mRegInfo[i].descLong << "\n";

	// Field settings (default for non-interlaced mode)
	int scan_lines_per_field = mScreenScanLines / 2.0;
	double field_rate = mCLK * 1e6 / (mScreenScanLines / 2.0 * mCharCols_R0);
	if ((mInterlaceMode_R8 & 0x1) != 0) {
		// Settings for interlaced modes
		scan_lines_per_field = mScreenScanLines / 2.0 + 0.5;	
		field_rate = mCLK * 1e6 / ((mScreenScanLines / 2.0 + 0.5) * mCharCols_R0);
	}

	sout << "CLK:                                                " << (int)mCLK << "\tMhz\n";
	sout << "Scan Line duration: [R0,CLK]                        " << mCharCols_R0 * Tc << "\tus\n";
	sout << "Total no of characters per line [R0]:               " << mCharCols_R0 << "\tchars\n";
	sout << "Active characters per line [R1]:                    " << mActiveRowChars_R1 << "\tchars\n";
	sout << "Horizontal sync position [R2]:                      " << mHzSyncPos_R2 << " chars (" << round(mHzSyncPos_R2 * Tc) << " us)\n";
	sout << "Horizontal sync pulse width [R3]:                   " << mHzSyncPulseW_R3 << " chars (" << round(mHzSyncPulseW_R3 * Tc) << " us)\n";
	sout << "Vertical sync position [R7];                        " << mVSyncRow_R7 << " Rows\n";
	sout << "Vertical sync pulse width [R3]:                     " << mScreenVSyncPulseH << " lines (" << round(mScreenVSyncPulseH * mCharCols_R0 * Tc) << " us)\n";
	sout << "No of character rows per field [R4]:                " << mCharRows_R4 << "\tRows\n";
	sout << "No of scan lines per field [R4,R9,R5]:              " << scan_lines_per_field << "\tlines\n";
	sout << "No of unique scan lines per frame [R4,R9,R5]:       " << mScreenScanLines << "\tlines\n";
	sout << "No of active character rows per field [R6]:         " << mActiveRows_R6 << "\tRows\n";
	sout << "Active scan lines per screen [R6,R9]:               " << mScreenActiveLines << "\tlines\n";
	sout << "Vertical sync scan line [R7]:                       " << mScreenVSyncLine << " lines (" << round(mScreenVSyncLine * mCharCols_R0 * Tc / 1000) << " ms)\n";
	sout << "Interlace mode[R8]:                                 " << _INTERLACE_MODE(mInterlaceMode_R8) << "\n";
	sout << "Character skew [R8]:                                " << mCharSkew_R8 << "\tchars\n";
	sout << "Cursor skew [R8]:                                   " << mCursSkew_R8 << "\tchars\n";
	sout << "Scan Lines per Character [R9]:                      " << mCharLines_R9 << "\tlines\n";
	sout << "Cursor raster lines [R10:R11]                       [" << dec << (int)(mReg[R10_CursorStart] & 0x1f) << ":" << (int)mReg[R11_CursorEnd] << "]\n";
	sout << "Start address [R12:R13]:                            0x" << hex << mStartAdr_R12_R13 << dec << "\n";
	sout << "Cursor position [R14:R15]:                          0x" << hex << ((mReg[R14_CursorH] << 8) | mReg[R15_CursorL]) << dec << "\n";
	sout << "Field Rate [CLK,R0,R4,R9,R5]:                       " << field_rate << "\tHz\n";
	int cursor_disp_mode = (mReg[R10_CursorStart] >> 5) & 0x3; // 00: Non-blink,  01: non-display, 10: blink 16-field, 11: blink 32-field
	sout << "Cursor mode:                                        " << (cursor_disp_mode == 0 ? "Fixed" : (cursor_disp_mode == 1 ? "None" : (cursor_disp_mode == 2 ? "Blink-16" : "Blink-32"))) <<
		"\n";
	sout << "Scan Line:                                           " << mScanLine << "\n";
	sout << "Char Row:                                            " << mCharRow << "\n";
	sout << "Char Col:                                            " << mCharCol << "\n";
	sout << "DSIPTMG:                                             " << (int)mDISPTMG << "\n";
	sout << "CUDISP:                                              " << (int)mCUDISP << "\n";

	return true;
}