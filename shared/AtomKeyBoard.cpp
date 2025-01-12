#include "AtomKeyboard.h"
#include <iostream>
#include "DebugInfo.h"

using namespace std;

AtomKeyboard::AtomKeyboard(DebugInfo debugInfo): Keyboard(debugInfo)
{
	// Create 10 rows by 6 columns vector with key data
	// Also get keycodes for SHIFT, CTRL & REPEAT keys
	map<int, AtomKey>::iterator keys_it;
	for (keys_it = mKeycodes.begin(); keys_it != mKeycodes.end(); keys_it++) {
		AtomKey *key = &(keys_it->second);
		if (key->row >= 0 && key->row <= 9 && key->col >= 0 && key->col <= 5)
			mKeyboardMatrix[key->row][key->col] = key;
		else if (key->atomKeyName == "SHIFT") {
			if (mShiftKeyCodes[0] == -1)
				mShiftKeyCodes[0] = key->keyCode;
			else
				mShiftKeyCodes[1] = key->keyCode;
		}
		else if (key->atomKeyName == "CTRL") {
			mCtrlKeyCode = key->keyCode;
		}
		else if (key->atomKeyName == "REPEAT")
			mRepeatKeyCode = key->keyCode;
	}

	if (mDebugInfo.dbgLevel & DBG_VERBOSE) {
		cout << "KeyBoard initialised\n";
		cout << "SHIFT keys have keycodes 0x" << hex << mShiftKeyCodes[0] << " and " << mShiftKeyCodes[1] << dec << "\n";
		cout << "CTRL key has keycodes 0x" << hex << mCtrlKeyCode << dec << "\n";
		cout << "REPEAT key has keycode 0x" << hex << mRepeatKeyCode << dec << "\n";
		cout << "Keyboard matrix is:\n";
		for (int r = 0; r < 10; r++) {
			for (int c = 0; c < 6; c++) {
				AtomKey* key = mKeyboardMatrix[r][c];
				if (key != NULL && key->row != -1 && key->col != -1)
					cout << key->atomKeyName << "\t(0x" << hex << key->keyCode << dec << ")\t";
				else
					cout << "-\t\t";
			}
			cout << "\n";
		}
	}

}


uint8_t AtomKeyboard::readColumn()
{
	uint8_t column = 0xff;
	for (uint8_t c = 0; c < 6; c++) {
		AtomKey* key = mKeyboardMatrix[mSelectedRow][c];
		if (key != NULL && keyDown(key->keyCode))
				column &= ~(0x1 << c);
	}

	return column;
}

void AtomKeyboard::selectRow(uint8_t row)
{
	mSelectedRow = row % 10;
}

bool AtomKeyboard::ctrlPressed()
{
	return keyDown(mCtrlKeyCode);
}

bool AtomKeyboard::shiftPressed()
{
	return keyDown(mShiftKeyCodes[0]) || keyDown(mShiftKeyCodes[1]);
}

bool AtomKeyboard::repeatPressed()
{
	return keyDown(mRepeatKeyCode);
}