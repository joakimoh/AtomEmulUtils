#include "AtomKeyboard.h"
#include <iostream>

using namespace std;

AtomKeyboard::AtomKeyboard() : Keyboard()
{
	// Create 10 rows by 6 columns vector with key data
	// Also get keycodes for SHIFT, CTRL & REPEAT keys
	map<int, AtomKey>::iterator keys_it;
	for (keys_it = mKeycodes.begin(); keys_it != mKeycodes.end(); keys_it++) {
		AtomKey *key = &(keys_it->second);
		if (key->row >= 0 && key->row <= 9 && key->col >= 0 && key->col <= 5)
			mKeyboardMatrix[key->row][key->col] = key;
		else if (key->atomKeyName == "SHIFT")
			mShiftKeyCode = key->keyCode;
		else if (key->atomKeyName == "CTRL")
			mCtrlKeyCode = key->keyCode;
		else if (key->atomKeyName == "REPEAT")
			mRepeatKeyCode = key->keyCode;
	}

	cout << "KeyBoard initialised\n";
	cout << "SHIFT key has keycode 0x" << hex << mShiftKeyCode << dec << "\n";
	cout << "CTRL key has keycode 0x" << hex << mCtrlKeyCode << dec << "\n";
	cout << "REPEAT key has keycode 0x" << hex << mRepeatKeyCode << dec << "\n";
	cout << "Keyboard matrix is:\n";
	for (int r = 0; r < 10; r++) {
		for (int c = 0; c < 6; c++) {
			AtomKey *key = mKeyboardMatrix[r][c];
			if (key != NULL && key->row != -1 && key->col != -1)
				cout << key->atomKeyName << "\t(0x" << hex << key->keyCode << dec << ")\t";
			else
				cout << "-\t\t";
		}
		cout << "\n";
	}

}


uint8_t AtomKeyboard::readColumn()
{
	uint8_t column = 0xff;
	for (uint8_t c = 0; c < 6; c++) {
		AtomKey* key = mKeyboardMatrix[mSelectedRow][c];
		if (key != NULL) {
			if (keyDown(key->keyCode))
				column &= ~(0x1 >> c);
		}
	}
	if (column != 0xff)
		cout << "Key pressed => Keyboard Column: 0x" << hex << (int) column << dec << "\n";
	return column;
}

void AtomKeyboard::selectRow(uint8_t row)
{
	mSelectedRow = row % 10;
}

bool AtomKeyboard::ctrlPressed()
{
	bool key_pressed = keyDown(mCtrlKeyCode);
	if (key_pressed)
		cout << "CTRL pressed\n";
	return key_pressed;
}

bool AtomKeyboard::shiftPressed()
{
	bool key_pressed = keyDown(mShiftKeyCode);
	if (key_pressed)
		cout << "SHIFT pressed\n";
	return key_pressed;
}

bool AtomKeyboard::repeatPressed()
{
	bool key_pressed = keyDown(mRepeatKeyCode);
	if (key_pressed)
		cout << "REPEAT pressed\n";
	return key_pressed;
}