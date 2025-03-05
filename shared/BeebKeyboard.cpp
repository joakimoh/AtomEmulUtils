
#include <map>
#include "Device.h"
#include <allegro5/allegro5.h>
#include "DebugInfo.h"
#include "BeebKeyboard.h"

//
// This is an emulation of a BBC Micro Keyboard
// 
// The emulation uses the interface tke Keyboard exposes against the VIA 6522 circuit (and the RESET line).
// 
// The Keyboard has two modes of operation:
// 
// 1) No scan - Key at a certain row and column is explictly selected and the ROW output will give its value (LOW if pressed)
// 2) Auto scan - the columns are scanned repeatedly and the PRESSED output will go high if any key is pressed (except for the row ones)
// 
// 
// Connector pins	Direction		Name	Connected to					Active level				Description
// PL4				IN				KB_ENA	PB2:0 as BDC 3;PB3 has val		Low (load), High (count)	Enables column scanning counter
//																										Counts upwards from COL_SEL
// PL5:7			IN				ROW_SEL	PA4:6							BCD							Selected keyboard row (0-7)
// PL8:11			IN				COL_SEL	PA0:3							BCD							Selected keyboard column (0-9)
// PL2				OUT				BREAK	RESET							Low							Break key connected to RESET line
// PL12				OUT				ROW		PA7								Low							State of selected key at row,column
// PL14				OUT				PRESSED	CA2								High						Indicates if any key is pressed (auto scanning mode)
// PL16				IN				LED1	PB2:0 as BDC 7;PB3 has val		Low							SHIFT Lock LED
// PL17				IN				LED2	PB2:0 as BDC 6;PB3 has val		Low							CAPS Lock LED
//
BeebKeyboard::BeebKeyboard(string name, uint8_t startupOptions, DebugInfo  *debugInfo, ConnectionManager* connectionManager):
	Device(name, BEEB_KEYBOARD_DEV, KEYBOARD_DEVICE, debugInfo, connectionManager)
{
	// Specify ports that can be connected to other devices	
	registerPort("CTRL",		IN_PORT,	0xf, CTRL,		&mCTRL);
	registerPort("COL_SEL",		IN_PORT,	0xf, COL_SEL,	&mCOL_SEL);
	registerPort("ROW_SEL",		IN_PORT,	0x7, ROW_SEL,	&mROW_SEL);
	registerPort("ROW",			OUT_PORT,	0x1, ROW,		&mROW);
	registerPort("BREAK",		OUT_PORT,	0x1, BREAK,		&mBREAK);
	registerPort("PRESSED",		OUT_PORT,	0x1, PRESSED,	&mPRESSED);
}

// Reset device
bool BeebKeyboard::reset()
{
	Device::reset();

	mCycleCount = 0;

	return true;
}

//  Advance until clock cycle stopcycle has been reached
bool BeebKeyboard::advance(uint64_t stopCycle)
{
	mCycleCount = stopCycle;

	uint8_t ctrl_sel = mCTRL & 0x7;
	uint8_t ctrl_val = (mCTRL >> 3) & 0x1;
	switch (ctrl_sel) {
	case 3:	// Disable Keyboard auto scanning
		mKB_ENA = ctrl_val; break;
	case 6:	// Turn on CAPS LOCK LED
		mLED2_CAPS_LOCK = ctrl_val; break;
	case 7:	// Turn on SHIFT LOCK LED
		mLED1_SHIFT_LOCK = ctrl_val; break;
	case 0:	// Enable sound chip
	case 1:	// Enable Read Speech
	case 2:	// Enable Write Speech
	case 4:	// Hardware scrolling - set C0 = 0 (See below)
	case 5:	// Hardware scrolling - set C1 = 0 (See below)
	default:
		break;
	}

	// Stop keyboard scanning (i.e. keep last value of ROW output) if KB_ENA is High
	//if (mKB_ENA)
	//	return true;

	al_get_keyboard_state(&mKeyboardState);

	uint8_t oROW = mROW;

	// Toggle ROW state from active (HIGH) to inactive (LOW) to secure that update is propagated.
	// The keyboard ROW output is connected to the System VIA PA7 input.
	// If the PA7 was previously configured as an output (with a HIGH value) and the keyboard's
	// ROW output was LOW, then the VIA will not get an update of the ROW unless a port update is made here.
	// Therefore we start by setting ROW output to '0' (inactive)
	updatePort(ROW, 0x0);
	//updatePort(PRESSED, 0x0);

	bool selected_key_pressed = false;
	bool column_key_pressed = false;

	// Check for keys only if row and column are valid
	if (mCOL_SEL <= 9 && mROW_SEL <= 7) {

		// Check for key at the selected row and column being pressed as well as DIP switches being ON <=> LOW
		vector<Key>& key_vec = mKeyboardMatrix[mROW_SEL];
		Key& key = key_vec[mCOL_SEL];

		if (key.keyCode != -1 && al_key_down(&mKeyboardState, key.keyCode) || (mROW_SEL == 0 && (mSW & (1 << (mCOL_SEL - 2))) == 0x1)) {
			//cout << "Key '" << key.keyName << "' detected at ROW " << (int)mROW_SEL << ", " << (int)mCOL_SEL << "\n";
			selected_key_pressed = true;
		}
	}
	
	// Check for any key in a selected column (or all columns if KB_ENA is High) being pressed (row 0 with SHIFT, CTRL & DIP switches is excluded)
	int start_col, end_col;
	if (mKB_ENA) { // Auto scanning of keyboard - keys in all columns are checked
		start_col = 0;
		end_col = 9;
	}
	else // No auto scanning - only keys in the currently selected column are checked
		start_col = end_col = mCOL_SEL;
	for (int row = 1; row <= 7; row++) {
		for (int col = start_col; col <= end_col; col++) {
			Key& key = mKeyboardMatrix[row][col];
			if (key.keyCode != -1 && al_key_down(&mKeyboardState, key.keyCode)) {
				column_key_pressed = true;
				//cout << "Key " << key.keyName << " pressed!\n";
				break;
			}
		}
	}


	// Any key pressed in a selected column
	if (column_key_pressed)
		updatePort(PRESSED, 0x1);
	else
		updatePort(PRESSED, 0x0);

	// Only the key in a selected row and column pressed
	if (selected_key_pressed)
		updatePort(ROW, 0x1);
	else
		updatePort(ROW, 0x0);

	// Get BREAK key
	if (al_key_down(&mKeyboardState, mBreakKey.keyCode))
		updatePort(BREAK, 0x0);
	else
		updatePort(BREAK, 0x1);

	if (((mDebugInfo->dbgLevel & DBG_KEYBOARD)) && (mCOL_SEL != pCOL_SEL || mROW_SEL != pROW_SEL || mKB_ENA != pKB_ENA)) {
		cout << "KB_ENA = " << (int) mKB_ENA << ",COL_SEL = " << dec << (int)mCOL_SEL << ", ROW_SEL = " << (int)mROW_SEL << ", PRESSED = " << (int)mPRESSED << ", ROW = " << (int)mROW << "\n";
	}

	oROW = mROW;
	pCOL_SEL = mCOL_SEL;
	pROW_SEL = mROW_SEL;
	pKB_ENA = mKB_ENA;

	return true;
}