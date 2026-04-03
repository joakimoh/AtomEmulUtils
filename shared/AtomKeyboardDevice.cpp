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
// 0xb000 to 0xb003.
// The row (ROW) is selected by the PIA by writing to
//		bits b3:b0 of Port A at address 0xb000 (rows 0 to 9)
// 
// The column (COL) state is read by reading
//		bits b0:b7 of port B at address 0xb001 (columns 0 to 7) - here also the CTRL and SHIFT keys are connected
//		bits b6 of port C at address 0xb002 (column 8) - here only the REPEAT (RPT) key is connected
// 
// The BREAK key should be connected to the RESET input pins of devices (including the PIA).
// 
// |		             IC25 PIA 8255 (not part of emulation)               |
// |                                                                         |
// |   PA0:3      PB0   PB1   PB2   PB3   PB4   PB5   PB6   PB7   PC6  RESET |
// |_________________________________________________________________________|
//      |          |     |     |     |     |     |     |     |     |     |
//    "ROW"       .........."COL"........................... ........    |		<= Keyboard Interface
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
	registerPort("COL", OUT_PORT, 0x3ff, KB_COL, &mColumn);

	// Minimum key down time of 50 ms for the paste function
	mMinKeyDownTicks = ms2Ticks(50);

	// Call base class init to set up the key code maps
	init(mPasteKeyMap, mKeyboardMatrix);

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

	pollKeyboardState();

	uint16_t column = 0x3ff;

	if (mSelectedRow < ATOM_KB_ROWS) {

		// Get non-modifier keys
		for (uint8_t c = 0; c < ATOM_COLS; c++) {
			KeyCodeMapping& key = mKeyboardMatrix[mSelectedRow][c];
			if (key.keyCode != -1) {
				int key_state_index = mSelectedRow * ATOM_COLS + c;
				bool key_depressed_state = keyDepressedState[key_state_index];
				keyDepressedState[key_state_index] = false;
				if (keyDown(key.keyCode)) {
					column &= ~(0x1 << c);
					keyDepressedState[key_state_index] = true;
					DBG_LOG_COND(!key_depressed_state, this, DBG_KEYBOARD, "KeyCodeMapping " + key->atomKeyName + " depressed\n");
				}
			}
		}

	}

	// Get CTRL key
	if (keyDown(mCtrlKeyCode))
		column &= ~0x40;

	// Get SHIFT keys
	if (keyDown(mShiftKeyCodes[0]) || keyDown(mShiftKeyCodes[1]))
		column &= ~0x80;

	// Get REPEAT key
	if (keyDown(mRepeatKeyCode))
		column &= ~0x100;

	// Get BREAK key
	if (keyDown(mBreakKeyCode))
		column &= ~0x200;

	// Update output "COL"
	if (!updatePort(KB_COL, column))
		return false;


	DBG_LOG_COND(column != 0x3ff, this, DBG_KEYBOARD | DBG_EXTENSIVE, "column = 0x" + Utility::int2HexStr(column,3));
	
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
	sout << "ROW = " << (int) KB_ROW << "\n";
	sout << "COL = " << (int) KB_COL<< "\n";

	return true;
}

// Set emulation speed
void AtomKeyboardDevice::setEmulationSpeed(double speed)
{
	KeyboardDevice::setEmulationSpeed(speed);

	// Make sure Keyboard refresh rate always is 60 Hz (or less) - add a 10% margin
	mKeyboardRefreshCycles = max(1, (int)round(1.1*mTickRate * 1e6 * mEmulationSpeed / 60));

}


// Check if the minimum key down time has passed (for the paste function)
bool AtomKeyboardDevice::minKeyDownTimePassed()
{
	return ticksPassed(mMinKeyDownTicks);
}