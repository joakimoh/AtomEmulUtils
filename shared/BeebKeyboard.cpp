
#include <map>
#include "Device.h"
#include <allegro5/allegro5.h>
#include "DebugInfo.h"
#include "BeebKeyboard.h"

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
	case 3:mKB_ENA = ctrl_val; break;
	case 6:mLED2 = ctrl_val; break;
	case 7:mLED1 = ctrl_val; break;
	default: break;
	}

	// Stop keyboard scanning (i.e. keep last value of ROW output) if KB_ENA is High
	//if (mKB_ENA)
	//	return true;


	//cout << "ADVANCE KB, mCOL_SEL = " << dec << (int)mCOL_SEL << ", mROW_SEL=" << (int)mROW_SEL << "...\n";

	al_get_keyboard_state(&mKeyboardState);

	uint8_t oROW = mROW;

	// Toggle ROW state from active (HIGH) to inactive (LOW) to secure that update is propagated.
	// The keyboard ROW output is connected to the System VIA PA7 input.
	// If the PA7 was previously configured as an output (with a HIGH value) and the keyboard's
	// ROW output was LOW, then the VIA will not get an update of the ROW unless a port update is made here.
	// Therefore we start by setting ROW output to '0' (inactive)
	updatePort(ROW, 0x0);
	updatePort(BREAK, 0x1);
	updatePort(PRESSED, 0x0);

	if (mDebugInfo->dbgLevel & DBG_6502)
		cout << "COL_SEL = " << dec << (int)mCOL_SEL << ", ROW_SEL = " << (int)mROW_SEL << "\n";


	if (mCOL_SEL <= 9 && mROW_SEL <= 7) {

		// Check for key at COL_SEL,ROW_SEL being pressed as well as DIP switches being ON <=> LOW
		vector<Key> &key_vec = mKeyboardMatrix[mROW_SEL];
		Key& key = key_vec[mCOL_SEL];
		if (key.keyCode != -1 && al_key_down(&mKeyboardState, key.keyCode) || (mROW_SEL == 0 && (mSW & (1 << (mCOL_SEL-2))) == 0x1))
			updatePort(ROW, 0x1);

		// Check for any key in selected column being pressed (row 0 with SHT, CTRL & DIP switches is excluded)
		bool key_pressed = false;
		for (int row = 1; row < 8; row++) {
			Key& key = mKeyboardMatrix[row][mCOL_SEL];
			if (key.keyCode != -1 && al_key_down(&mKeyboardState, key.keyCode)) {
				key_pressed = true;
				//cout << "Key " << key.keyName << " pressed!\n";
				break;
			}
				
		}
		if (key_pressed)
			updatePort(PRESSED, 0x1);


		// Get BREAK key
		if (al_key_down(&mKeyboardState, mBreakKey.keyCode))
			updatePort(BREAK, 0x0);

		if (mDebugInfo->dbgLevel & DBG_6502 || mCOL_SEL != pCOL_SEL || mROW_SEL != pROW_SEL) {
			cout << "COL_SEL = " << dec << (int)mCOL_SEL << ", ROW_SEL = " << (int)mROW_SEL << ", PRESSED = " << (int)mPRESSED << ", ROW =" << (int) mROW << "\n";
		}
		pCOL_SEL = mCOL_SEL;
		pROW_SEL = mROW_SEL;

	}

	oROW = mROW;

	return true;
}