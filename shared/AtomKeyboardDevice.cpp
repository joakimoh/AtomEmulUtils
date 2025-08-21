#include "AtomKeyboardDevice.h"
#include <iostream>
#include <cstdint>
#include <map>
#include <chrono>
#include "Utility.h"

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

	al_get_keyboard_state(&mKeyboardState);
}

bool AtomKeyboardDevice::advanceUntil(uint64_t stopCycle)
{
	mCnt += stopCycle - mCycleCount;
	mCycleCount = stopCycle;

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


	DBG_LOG_COND(column_L != 0xff || column_H != 0x3, this, DBG_KEYBOARD, 
		"column L = 0x" + Utility::int2HexStr(column_L,2) + ", column H = 0x" + Utility::int2HexStr(column_H,2)
		);
	
	return true;
}

// Outputs the internal state of the device
bool AtomKeyboardDevice::outputState(ostream& sout)
{
	sout << "COL_L = " << (int)KB_COL_L << "\n";
	sout << "COL_H = " << (int)KB_COL_H << "\n";

	return true;
}