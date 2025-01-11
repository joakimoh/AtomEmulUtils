#include "AtomKeyboard.h"
#include <iostream>

using namespace std;


void AtomKeyboard::keyDown(int k, unsigned modifiers)
{
	if (mKeycodes.find(k) != mKeycodes.end()) {
		AtomKey key = mKeycodes[k];
		if (key.row != -1 && key.col != -1) {
			PA0_03_selectedRow_keys[key.row][key.col]++;
			cout << "KEY '" << key.atomKeyName << "' sets row " << (int)key.row << " and col " << (int)key.col <<
				"=> count = " << (int) PA0_03_selectedRow_keys[key.row][key.col] << "\n";
		}
		else if (key.atomKeyName == "SHIFT")
			PB7_shift = true;
		else if (key.atomKeyName == "CTRL")
			PB6_ctrl = true;
		else if (key.atomKeyName == "REPEAT")
			PC6_repeat = true;
		else if (key.atomKeyName == "BREAK") {
			// resetAction();
		}
		//cout << "KEY '" << key.atomKeyName << "' pressed!\n";
	}
}

void AtomKeyboard::keyUp(int k)
{
	if (mKeycodes.find(k) != mKeycodes.end()) {
		AtomKey key = mKeycodes[k];
		if (key.row != -1 && key.col != -1) {
			PA0_03_selectedRow_keys[key.row][key.col]--;
		}
		else if (key.atomKeyName == "SHIFT")
			PB7_shift = false;
		else if (key.atomKeyName == "CTRL")
			PB6_ctrl = false;
		else if (key.atomKeyName == "REPEAT")
			PC6_repeat = false;
		//cout << "KEY '" << key.atomKeyName << "' released!\n";
	}
}

AtomKeyboard::AtomKeyboard() : Keyboard()
{
	// Create 10 rows by 6 columns vector with key data
	map<int, AtomKey>::iterator keys_it;
	for (keys_it = mKeycodes.begin(); keys_it != mKeycodes.end(); keys_it++) {
		AtomKey *key = &(keys_it->second);
		if (key->row >= 0 && key->row <= 9 && key->col >= 0 && key->col <= 5)
			mKeyboardRows[key->row][key->col] = key;
	}

	for (int r = 0; r < 10; r++) {
		for (int c = 0; c < 6; c++) {
			AtomKey* key = mKeyboardRows[r][c];
			if (key != NULL)
				cout << "'" << key->atomKeyName << "'";
			else
				cout << "-";
			if (c < 5)
				cout << " ";
		}
		cout << "\n";
	}
}