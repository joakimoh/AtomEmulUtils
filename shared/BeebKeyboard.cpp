
#include <map>
#include "Device.h"
#include <allegro5/allegro5.h>
#include "DebugInfo.h"
#include "BeebKeyboard.h"

BeebKeyboard::BeebKeyboard(string name, DebugInfo debugInfo, ConnectionManager* connectionManager):
	Device(name, BEEB_KEYBOARD_DEV, KEYBOARD_DEVICE, debugInfo, connectionManager)
{
	// Specify ports that can be connected to other devices	
	registerPort("COL_SEL", IN_PORT, 0x0, COL_SEL, &mCOL_SEL);
	registerPort("ROW_SEL", IN_PORT, 0x0, ROW_SEL, &mROW_SEL);
	registerPort("ROW", OUT_PORT, 0x0, ROW, &mROW);
	registerPort("BREAK", OUT_PORT, 0x0, BREAK, &mBREAK);
	registerPort("PRESSED", OUT_PORT, 0x0, PRESSED, &mPRESSED);
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

	if (mCOL_SEL <= 9 && ROW_SEL <= 7) {

		// Check for key a COL_SEL,ROW_SEL being pressed
		vector<Key> &key_vec = mKeyboardMatrix[mROW_SEL];
		Key& key = key_vec[mCOL_SEL];
		if (al_key_down(&mKeyboardState, key.keyCode))
			updatePort(ROW, 0x0);
		else
			updatePort(ROW, 0x1);

		// Check for any key in selected column being presseed
		bool key_pressed = false;
		for (int row = 0; row < 8; row++) {
			Key& key = mKeyboardMatrix[row][mCOL_SEL];
			if (al_key_down(&mKeyboardState, key.keyCode)) {
				key_pressed = true;
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