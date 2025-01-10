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