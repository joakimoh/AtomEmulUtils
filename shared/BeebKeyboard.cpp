
#include <map>
#include "Device.h"
#include <allegro5/allegro5.h>
#include "DebugManager.h"
#include "BeebKeyboard.h"

//
// This is an emulation of a BBC Micro Keyboard
// 
// CIRCUIT REFERENCE (BBC Micro)
// 
// Connector pins	Direction		Port	VIA Connections					Active level				Description
// PL4				IN				ENA		PB2:0 as BCD 3;PB3 has val		Low (load), High (count)	Enables column scanning counter
//																										Counts upwards from COL_SEL
// PL5:7			IN				ROW_SEL	PA4:6							BCD							Selected keyboard row (0-7)
// PL8:11			IN				COL_SEL	PA0:3							BCD							Selected keyboard column (0-9)
// PL2				OUT				BREAK	RESET							Low							Break key connected to RESET line
// PL12				OUT				ROW		PA7								Low							State of selected key at row,column
// PL14				OUT				PRESSED	CA2								High						Indicates if any key is pressed (auto scanning mode)
// PL16				IN				LED1	PB2:0 as BDC 7;PB3 has val		Low							SHIFT Lock LED
// PL17				IN				LED2	PB2:0 as BDC 6;PB3 has val		Low							CAPS Lock LED
// 
BeebKeyboard::BeebKeyboard(string name, double cpuClock, double deviceClock, uint8_t startupOptions, DebugManager  *debugManager, ConnectionManager* connectionManager):
	KeyboardDevice(name, BEEB_KEYBOARD_DEV, cpuClock, debugManager, connectionManager), ClockedDevice(cpuClock, deviceClock)
{
	// Specify ports that can be connected to other devices	
	registerPort("SW",			IN_PORT,	0xf, SW,		&mSW);
	registerPort("ENA",			IN_PORT,	0xf, ENA,		&mENA);
	registerPort("COL_SEL",		IN_PORT,	0xf, COL_SEL,	&mCOL_SEL);
	registerPort("ROW_SEL",		IN_PORT,	0x7, ROW_SEL,	&mROW_SEL);
	registerPort("ROW",			OUT_PORT,	0x1, ROW,		&mROW);
	registerPort("BREAK",		OUT_PORT,	0x1, BREAK,		&mBREAK);
	registerPort("PRESSED",		OUT_PORT,	0x1, PRESSED,	&mPRESSED);

	// Make sure Keyboard refresh rate always is 50 Hz (or less)
	mKeyboardRefreshCycles = max(1, (int) round(cpuClock * 1e6 * mEmulationSpeed/ 50));

}


void BeebKeyboard::processPortUpdate(int index)
{

	if (index == COL_SEL || index == ROW_SEL) {

		al_get_keyboard_state(&mKeyboardState);

		//
		// Need to process changes to column/row directly so that the PRESSED and ROW outputs (connected to the System VIA)
		// will make the System VIA update its flag and A registers before the processor checks it immedialtely with a BIT instruction
		// 
		// There are two sequences in the MOS related to this as part of a keyboard scan routine (at f0d1) that is executed after a
		// keyboard interrupt and with interrupts disabled. The keyboard routine scans column by column until a key is pressed and then
		// loops through all rows until the pressed key is identified. The sequences are:
		// 1) Checking for any key in a column being pressed (PRESSED output fed to VIA CA2 input when a column has been
		//    selected using the COL_SEL input controlled by the VIA PA0:3 output). The column is written into the VIA register(STX ...) and then
		//    the interrupt status flag related to the CA2 input is checked (BIT...)
		//		Instruction "STX .systemVIARegisterANoHandshake " at address f0fa followed by "BIT .systemVIAInterruptFlagRegister"
		// 2) Checking for a specific key for a selected row and column (ROW output fed to the VIA PA7 input when a certain row has been
		//    selected using the ROW_SEL input controlled by the VIA PA4:6 output). The row is written into the VIA register (STA ...) and then
		//    the row status is checked (BIT ...)
		//		Instruction "STA .systemVIARegisterANoHandshake" at address f108 followed by " BIT .systemVIARegisterANoHandshake"
		//

		bool column_key_pressed = false;
		bool selected_key_pressed = false;

		// Check for any key being pressed for a selected column
		if (mCOL_SEL <= 9)
		{
			for (int row = 1; row <= 7; row++) {

				Key& key = mKeyboardMatrix[row][mCOL_SEL];
				if (key.keyCode != -1 && al_key_down(&mKeyboardState, key.keyCode)) {
					column_key_pressed = true;
					//cout << "Key " << key.keyName << " pressed - any row for column " << (int) mCOL_SEL << "!\n";
					DBG_LOG(this, DBG_KEYBOARD, "Key " + key.keyName + " pressed!");
					break;
				}
			}
		}

		// Check for key at the selected row and column being pressed as well as DIP switches being ON <=> LOW
		if (mCOL_SEL <= 9 && mROW_SEL <= 7) {
			vector<Key>& key_vec = mKeyboardMatrix[mROW_SEL];
			Key& key = key_vec[mCOL_SEL];
			if (key.keyCode != -1 && al_key_down(&mKeyboardState, key.keyCode) || (mROW_SEL == 0 && linkSet(mCOL_SEL))) {
				//cout << "Key " << key.keyName << " pressed - for selected row " << (int)mROW_SEL << " and column " << (int)mCOL_SEL << "!\n";
				selected_key_pressed = true;
			}
		}

		if (column_key_pressed)
			updatePort(PRESSED, 0x1);
		else
			updatePort(PRESSED, 0x0);

		// Only the key in a selected row and column pressed
		if (selected_key_pressed)
			updatePort(ROW, 0x1);
		else
			updatePort(ROW, 0x0);
	}

}

//  Advance until clock cycle stopcycle has been reached
bool BeebKeyboard::advanceUntil(uint64_t stopCycle)
{

	// Don't update keyboard state too often as it creates a lot of load...
	int next_refresh_cycle = mCycleCount + mKeyboardRefreshCycles;
	mCycleCount = stopCycle;
	//if (stopCycle > next_refresh_cycle)
	//	return true;

	al_get_keyboard_state(&mKeyboardState);

	// Check BREAK key
	if (al_key_down(&mKeyboardState, mBreakKey.keyCode)) {
		DBG_LOG_COND(mBREAK == 1, this, DBG_RESET, "BREAK key pressed\n");
		updatePort(BREAK, 0x0);
	}
	else {
		DBG_LOG_COND(mBREAK == 0, this, DBG_RESET, "BREAK key released\n");
		updatePort(BREAK, 0x1);
	}
	

	autoScan();
	
	return true;
}



// Scans the next column
bool BeebKeyboard::scanNextColumn()
{
	if (mENA && mCycleCount % mCpuCyclesPerDeviceCycle == 0) {
		mCOL_SEL = (mCOL_SEL + 1) % 10;
		return scanColumn(mCOL_SEL);
	}
	return false;
}

// Scan a whole column and set the PRESSED output if any key was pressed in the column
bool BeebKeyboard::scanColumn(int col)
{
	bool column_key_pressed = false;

	for (int row = 1; row <= 7; row++) {
		Key& key = mKeyboardMatrix[row][col];
		if (key.keyCode != -1 && al_key_down(&mKeyboardState, key.keyCode)) {
			column_key_pressed = true;
			DBG_LOG(this, DBG_KEYBOARD, "Key " + key.keyName + " pressed!");
			break;
		}
	}

	return column_key_pressed;
}

// Outputs the internal state of the device
bool BeebKeyboard::outputState(ostream& sout)
{
	sout << "PRESSED =    " << (int) mPRESSED << "\n";
	sout << "BREAK =      " << (int) mBREAK << "\n";
	return true;
}

//
// Auto scan keyboard
// 
// Iterates over all columns and sets the PRESSED output if any key was pressed.
//
bool BeebKeyboard::autoScan()
{
	bool column_key_pressed = false;

	// Auto scanning of keyboard - keys in all columns are checked
	if (mENA) {
		for (int col = 0; col <= 9; col++) {
			if (scanColumn(col)) {
				column_key_pressed = true;
				break;
			}
		}
	}

	// Any key pressed in a selected column
	uint8_t old_pressed = mPRESSED;
	if (column_key_pressed)
		updatePort(PRESSED, 0x1);
	else
		updatePort(PRESSED, 0x0);


	if (DBG_LEVEL_DEV(this, DBG_KEYBOARD) && (mCOL_SEL != pCOL_SEL || mROW_SEL != pROW_SEL || mENA != pENA || mPRESSED != old_pressed)) {
		DBG_LOG(this, DBG_KEYBOARD, "ENA = " + to_string(mENA) + ",COL_SEL = " + to_string(mCOL_SEL) +
			", ROW_SEL = " + to_string(mROW_SEL) + ", PRESSED = " + to_string(mPRESSED) + ", ROW = " + to_string(mROW));
	}

	pCOL_SEL = mCOL_SEL;
	pROW_SEL = mROW_SEL;
	pENA = mENA;

	return column_key_pressed;
}

// Set emulation speed
void BeebKeyboard::setEmulationSpeed(double speed)
{
	KeyboardDevice::setEmulationSpeed(speed);

	// Make sure Keyboard refresh rate always is 50 Hz (or less)
	mKeyboardRefreshCycles = max(1, (int)round(mCPUClock * 1e6 * mEmulationSpeed / 50));
}
