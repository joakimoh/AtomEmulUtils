
#include <map>
#include "Device.h"
#include <allegro5/allegro5.h>
#include "DebugInfo.h"
#include "BeebKeyboard.h"

BeebKeyboard::BeebKeyboard(string name, uint8_t startupOptions, DebugInfo debugInfo, ConnectionManager* connectionManager):
	Device(name, BEEB_KEYBOARD_DEV, KEYBOARD_DEVICE, debugInfo, connectionManager)
{
	// Specify ports that can be connected to other devices	
	registerPort("COL_SEL",		IN_PORT,	0xf, COL_SEL,	&mCOL_SEL);
	registerPort("ROW_SEL",		IN_PORT,	0x7, ROW_SEL,	&mROW_SEL);
	registerPort("ROW",			OUT_PORT,	0x1, ROW,		&mROW);
	registerPort("BREAK",		OUT_PORT,	0x1, BREAK,		&mBREAK);
	registerPort("PRESSED",		OUT_PORT,	0x1, PRESSED,	&mPRESSED);
}

// Reset device
bool BeebKeyboard::reset()
{
	mCycleCount = 0;

	return true;
}

//  Advance until clock cycle stopcycle has been reached
bool BeebKeyboard::advance(uint64_t stopCycle)
{
	mCycleCount = stopCycle;

	//cout << "ADVANCE KB, mCOL_SEL = " << dec << (int)mCOL_SEL << ", mROW_SEL=" << (int)mROW_SEL << "...\n";

	if (mCOL_SEL <= 9 && mROW_SEL <= 7) {

		// Check for key a COL_SEL,ROW_SEL being pressed as well as DIP switches being ON <=> Low
		vector<Key> &key_vec = mKeyboardMatrix[mROW_SEL];
		Key& key = key_vec[mCOL_SEL];
		if (al_key_down(&mKeyboardState, key.keyCode) || (mROW_SEL == 0 && (mSW & (1 << (mCOL_SEL-2))) == 0x0))
			updatePort(ROW, 0x0);
		else
			updatePort(ROW, 0x1);

		// Check for any key in selected column being pressed (row 0 with SHT, CTRL & DIP switches is excluded)
		bool key_pressed = false;
		for (int row = 1; row < 8; row++) {
			Key& key = mKeyboardMatrix[row][mCOL_SEL];
			if (al_key_down(&mKeyboardState, key.keyCode)) {
				key_pressed = true;
				cout << "Key " << key.keyName << " pressed!\n";
				break;
			}
				
		}
		if (key_pressed)
			updatePort(PRESSED, 0x1);
		else
			updatePort(PRESSED, 0x0);

		// Get BREAK key
		if (al_key_down(&mKeyboardState, mBreakKey.keyCode))
			updatePort(BREAK, 0x0);
		else
			updatePort(BREAK, 0x1);

	}

	return true;
}