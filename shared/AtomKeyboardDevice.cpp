#include "AtomKeyboardDevice.h"
#include <iostream>
#include <cstdint>
#include <map>
#include <chrono>

using namespace std;


AtomKeyboardDevice::AtomKeyboardDevice(string name, double cpuClock, DebugManager  *debugManager, ConnectionManager* connectionManager) :
	Device(name, ATOM_KB_DEV, KEYBOARD_DEVICE, cpuClock, debugManager, connectionManager)
{
	// Specify ports that can be connected to other devices	
	registerPort("ROW", IN_PORT, 0x0f, KB_ROW, &mSelectedRow);
	registerPort("COL_L", OUT_PORT, 0xff, KB_COL_L, &mColumnL);
	registerPort("COL_H", OUT_PORT, 0x03, KB_COL_H, &mColumnH);

	// Create 10 rows by 6 columns vector with key data
	// Also get keycodes for SHIFT, CTRL & REPEAT keys
	map<int, AtomKey>::iterator keys_it;
	for (keys_it = mKeycodes.begin(); keys_it != mKeycodes.end(); keys_it++) {
		AtomKey* key = &(keys_it->second);
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
		else if (key->atomKeyName == "BREAK")
			mBreakKeyCode = key->keyCode;
	}

	if (mDM->debug(DBG_VERBOSE)) {
		cout << "KeyBoard initialised\n";
		cout << "SHIFT keys have keycodes 0x" << hex << mShiftKeyCodes[0] << " and 0x" << mShiftKeyCodes[1] << dec << "\n";
		cout << "CTRL key has keycode 0x" << hex << mCtrlKeyCode << dec << "\n";
		cout << "REPEAT key has keycode 0x" << hex << mRepeatKeyCode << dec << "\n";
		cout << "BREAK key has keycode 0x" << hex << mBreakKeyCode << dec << "\n";
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

	al_get_keyboard_state(&mKeyboardState);
}

bool AtomKeyboardDevice::advance(uint64_t stopCycle)
{
	mCnt += stopCycle - mCycleCount;

	mCycleCount = stopCycle;

	auto kb_start = chrono::high_resolution_clock::now();

	al_get_keyboard_state(&mKeyboardState);

	uint8_t column_L = 0xff;
	uint8_t column_H = 0x03;

	if (mSelectedRow <= 9) {

		// Get non-modifier keys
		vector<AtomKey*> key_vec = mKeyboardMatrix[mSelectedRow];
		for (uint8_t c = 0; c < 6; c++) {
			AtomKey* key = key_vec[c];
			if (key != NULL && al_key_down(&mKeyboardState, key->keyCode))
				column_L &= ~(0x1 << c);
		}

	}

	// Get CTRL key
	if (al_key_down(&mKeyboardState, mCtrlKeyCode))
		column_L &= ~0x40;

	// Get SHIFT keys
	if (al_key_down(&mKeyboardState, mShiftKeyCodes[0]) || al_key_down(&mKeyboardState, mShiftKeyCodes[1]))
		column_L &= ~0x80;

	// Get REPEAT key
	if (al_key_down(&mKeyboardState, mRepeatKeyCode))
		column_H &= ~0x1;


	// Get BREAK key
	if (al_key_down(&mKeyboardState, mBreakKeyCode))
		column_H &= ~0x2;

	// Update outputs "COL" and "RPT"
	if (!updatePort(KB_COL_L, column_L) || !updatePort(KB_COL_H, column_H))
		return false;


	if (mDM->debug(DBG_KEYBOARD) && (column_L != 0xff || column_H != 0x3))
		cout << "column L = 0x" << hex << (int)column_L << ", column H = 0x" << (int)column_H << "\n";


	auto kb_stop = chrono::high_resolution_clock::now();
	auto vdu_dur = chrono::duration_cast<chrono::microseconds>(kb_stop - kb_start);
	mKBCnt += vdu_dur.count();

	if (false && mCnt >= 1000000) {
		cout << "Keyboard ms per sec: " << mKBCnt / 1000 << "\n";
		mKBCnt = 0;
		mCnt = 0;
	}
	
	return true;
}