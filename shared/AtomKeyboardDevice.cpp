#include "AtomKeyboardDevice.h"
#include <iostream>
#include <cstdint>
#include <map>
#include <chrono>
#include "Utility.h"
#include <cmath>

using namespace std;



//
// Emulates an Acorn Atom Keyboard (that in the Atom is connected to the PIA 8255).
// 
// It emulates IC26 (7445) and the keyboard matrix with the pull ups to 5V
// and the (at keypress) pull downs to 0V.
// 
// CIRCUIT REFERENCE (Acorn Atom)
// 
// The BCD-to-Decimal Decoder is controlled by the PIA's PA0:3 pins
// and will select a row. The selected row will be LOW and all other
// rows will be HIGH.
// 
// A Column is HIGH unless driven LOW by the related PIA's output pin
// 
// The PIA connected to the Keyboard in the Atom is located at addresses
// 0xb000 to 0xb003. The ROW is selected by the PIA by writing to Port A at address 0xb000,
// bits b3:b0 and the value of the columms - COL_L + COL_H are read by
// reading from PIA port B, bits b0:b7 at address 0xb001. The REPEAT key is read by
// reading from PIA port C at address 0xb002, bit b7. The BREAK key should be connected to
// the RESET input pins of devices (including the PIA).
// 
// |		             IC25 PIA 8255 (not part of emulation)               |
// |                                                                         |
// |   PA0:3      PB0   PB1   PB2   PB3   PB4   PB5   PB6   PB7   PC6  RESET |
// |_________________________________________________________________________|
//      |          |     |     |     |     |     |     |     |     |     |
//    "ROW"       .........."COL_L"...........................     "COL_H"     <= Keyboard Interface
// _____|_____     |     |     |     |     |     |     |     |     |     |
// |          |    |     |     |     |     |     |     |     |     |     |
// |        0 +----+----"3"---"-"---"G"---"Q"--"ESC    |     |     |     |
// |          |    |     |     |     |     |     |     |     |     |     |
// |        1 +----+----"2"---","---"F"---"P"---"Z"    |     |     |     |
// |          |    |     |     |     |     |     |     |     |     |     |
// |        2 +---"HZ"--"1"---";"---"E"---"O"---"Y"    |     |     |     |
// |   IC26   |    |     |     |     |     |     |     |     |     |     |
// |        3 +---"VT"--"0"---":"---"D"---"N"---"X"    |     |     |     |
// |          |    |     |     |     |     |     |     |     |     |     |
// |  7445  4 +--"LOCK"-"DEL"-"9"---"C"---"M"---"W"    |     |     |    BRK
// |          |    |     |     |     |     |     |     |     |     |   / |
// |   BCD  5 +--"UP"--"COPY"-"8"---"B"---"L"---"V"    |     |     |  0V |
// |    to    |    |     |     |     |     |     |     |     |     |     |
// |   Dec  6 +---"]"--"RET"--"7"---"A"---"K"---"U"    |     |     |     |
// |          |    |     |     |     |     |     |     |     |    RPT    |
// |        7 +---"\"----+----"6"---"@"---"J"---"T"    |     |   / |     |
// |          |    |     |     |     |     |     |     |     |  0V |     |
// |        8 +---"["----+----"5"---"/"---"I"---"S"    |     |     |     |    
// |          |    |     |     |     |     |     |     |     |     |     |
// |        9 +---" "----+----"4"---"."---"H"---"R"    |     |     |     |
// |          |   /|    /|    /|    /|    /|    /|    CTRL   |     |     |
// |__________| 0V |  0V |  0V |  0V |  0V |  0V |   / |   SHIFT   |     |
//                 |     |     |     |     |     |  0V |   / |     |     |
//                 |     |     |     |     |     |     |  0V |     |     |
//                5V    5V    5V    5V    5V    5V    5V    5V    5V    5V

AtomKeyboardDevice::AtomKeyboardDevice(string name, double tickRate, DebugTracing  *debugTracing, ConnectionManager* connectionManager) :
	KeyboardDevice(name, ATOM_KB_DEV, debugTracing, connectionManager), TimedDevice(tickRate)
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
		if (key->row >= 0 && key->row < ATOM_KB_ROWS && key->col >= 0 && key->col < ATOM_COLS)
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

	// Make sure Keyboard refresh rate always is 60 Hz (or less) - add a 10% margin
	mKeyboardRefreshCycles = max(1, (int)round(1.1 * tickRate * 1e6 * mEmulationSpeed / 60));
}

bool AtomKeyboardDevice::advanceUntil(uint64_t stopTick)
{
	// Don't update keyboard state too often as it creates a lot of load...
	int next_refresh_cycle = mTicks + mKeyboardRefreshCycles;
	if (stopTick < next_refresh_cycle)
		return true;

	mTicks = stopTick;

	return checkKeyBoard();
}

bool AtomKeyboardDevice::checkKeyBoard()
{

	al_get_keyboard_state(&mKeyboardState);

	uint8_t column_L = 0xff;
	uint8_t column_H = 0x03;

	if (mSelectedRow < ATOM_KB_ROWS) {

		// Get non-modifier keys
		vector<AtomKey*> &key_vec = mKeyboardMatrix[mSelectedRow];
		for (uint8_t c = 0; c < ATOM_COLS; c++) {
			AtomKey* key = key_vec[c];
			if (key != nullptr) {		
				int key_state_index = mSelectedRow * ATOM_COLS + c;
				bool key_depressed_state = keyDepressedState[key_state_index];
				keyDepressedState[key_state_index] = false;
				if (al_key_down(&mKeyboardState, key->keyCode)) {
					column_L &= ~(0x1 << c);
					keyDepressedState[key_state_index] = true;
					DBG_LOG_COND(!key_depressed_state, this, DBG_KEYBOARD, "Key " + key->atomKeyName + " depressed\n");
				}
			}
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


	DBG_LOG_COND(column_L != 0xff || column_H != 0x3, this, DBG_KEYBOARD | DBG_EXTENSIVE,
		"column L = 0x" + Utility::int2HexStr(column_L,2) + ", column H = 0x" + Utility::int2HexStr(column_H,2)
		);
	
	return true;
}

// Handle input port changes directly on change
void AtomKeyboardDevice::processPortUpdate(int index)
{
	if (index == KB_ROW) {
		checkKeyBoard();
	}
}

void AtomKeyboardDevice::processPortUpdate()
{
		checkKeyBoard();
}

// Outputs the internal state of the device
bool AtomKeyboardDevice::outputState(ostream& sout)
{
	sout << "COL_L = " << (int)KB_COL_L << "\n";
	sout << "COL_H = " << (int)KB_COL_H << "\n";

	return true;
}

// Set emulation speed
void AtomKeyboardDevice::setEmulationSpeed(double speed)
{
	KeyboardDevice::setEmulationSpeed(speed);

	// Make sure Keyboard refresh rate always is 60 Hz (or less) - add a 10% margin
	mKeyboardRefreshCycles = max(1, (int)round(1.1*mTickRate * 1e6 * mEmulationSpeed / 60));

}
