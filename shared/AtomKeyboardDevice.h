#ifndef ATOM_KEYBOARD_DEVICE_H
#define ATOM_KEYBOARD_DEVICE_H

#include <vector>
#include <map>
#include "ClockedDevice.h"
#include "Device.h"
#include <allegro5/allegro5.h>
#include "DebugTracing.h"
#include "KeyboardDevice.h"

//
// 
// Emulates an Acorn Atom Keyboard.
// 
// 
// SCHEDULING
// 
// The Keyboard device can be schedulated at low rate (typically 60 Hz)
// using the SCHED directive
// 
// 
// PORTS
// 
// The Keyboard presents a port interface consisting of one
// input port (ROW) and one output port (COL).
// 
// 
// PORT CONNECTIONS
// 
// The ROW input port shall be connected to using the directive "CONNECT:P"
// to ensure that the device is triggered immediately on changes to the ROW
// input port. The COL output port shall be connected to a device
// that can read these ports (typically a PIA 8255).
// 
// 
// FUNCTIONAL DESCRIPTION
// 
// To detect a key, the ROW input shall first select a row in the keyboard matrix.
// Then the key columns in which a key is being pressed can be determined
// by reading the COL output. A cleared bit means that a key
// in the associated column is being pressed, a set bit that no key in the associated
// column is being pressed.
// 
// KEYBOARD MATRIX
// 
//		COLUMN
//		0		1		2		3		4		5		6		7		8
// ROW
// 0	  		3		-		G		Q		ESC
// 1      		2		,		F		P		Z
// 2	HZ		1		;		E		O		Y
// 3	VT		0		:		D		N		X
// 4	LOCK	DEL		9		C		M		W
// 5	UP		COPY	8		B		L		V
// 6	]		RETTURN	7		A		K		U
// 7	\		  		6		@		J		T
// 8	[		  		5		/		I		S
// 9	SPACE	  		4		.		H		R
//																LSHIFT
//		  		  		  		  		  		  		CTRL	RSHIFT	REPEAT
// 


#define ATOM_KB_ROWS 10
#define ATOM_COLS 6

typedef struct AtomModifier_struct {
	int	keyCode;
	bool ctrl;
	bool shift;
	bool repeat;
} AtomModifier;


class AtomKeyboardDevice : public KeyboardDevice, public TimedDevice {

private:

	array<bool, ATOM_KB_ROWS * ATOM_COLS> keyDepressedState = { false };

	vector<KeyPastingInfo> mPasteKeyMap = {

		{' ', {"SPACE"}},
		{'\r', {"RETURN"}},
		{'\t', {"HZARROW"}},

		{'@', {"@"}},
		{'A', {"Aa"}},
		{'B', {"Bb"}},
		{'C', {"Cc"}},
		{'D', {"Dd"}},
		{'E', {"Ee"}},
		{'F', {"Ff"}},
		{'G', {"Gg"}},
		{'H', {"Hh"}},
		{'I', {"Ii"}},
		{'J', {"Jj"}},
		{'K', {"Kk"}},
		{'L', {"Ll"}},
		{'M', {"Mm"}},
		{'N', {"Nn"}},
		{'O', {"Oo"}},
		{'P', {"Pp"}},
		{'Q', {"Qq"}},
		{'R', {"Rr"}},
		{'S', {"Ss"}},
		{'T', {"Tt"}},
		{'U', {"Uu"}},
		{'V', {"Vv"}},
		{'W', {"Ww"}},
		{'X', {"Xx"}},
		{'Y', {"Yy"}},
		{'Z', {"Zz"}},

		{'a', {"Aa", "SHIFT"}},
		{'b', {"Bb", "SHIFT"}},
		{'c', {"Cc", "SHIFT"}},
		{'d', {"Dd", "SHIFT"}},
		{'e', {"Ee", "SHIFT"}},
		{'f', {"Ff", "SHIFT"}},
		{'g', {"Gg", "SHIFT"}},
		{'h', {"Hh", "SHIFT"}},
		{'i', {"Ii", "SHIFT"}},
		{'j', {"Jj", "SHIFT"}},
		{'k', {"Kk", "SHIFT"}},
		{'l', {"Ll", "SHIFT"}},
		{'m', {"Mm", "SHIFT"}},
		{'n', {"Nn", "SHIFT"}},
		{'o', {"Oo", "SHIFT"}},
		{'p', {"Pp", "SHIFT"}},
		{'q', {"Qq", "SHIFT"}},
		{'r', {"Rr", "SHIFT"}},
		{'s', {"Ss", "SHIFT"}},
		{'t', {"Tt", "SHIFT"}},
		{'u', {"Uu", "SHIFT"}},
		{'v', {"Vv", "SHIFT"}},
		{'w', {"Ww", "SHIFT"}},
		{'x', {"Xx", "SHIFT"}},
		{'y', {"Yy", "SHIFT"}},
		{'z', {"Zz", "SHIFT"}},

		{'0', {"0"}},
		{'1', {"1!"}},
		{'2', {"2\""}},
		{'3', {"3#"}},
		{'4', {"4$"}},
		{'5', {"5%"}},
		{'6', {"6&"}},
		{'7', {"7'"}},
		{'8', {"8("}},
		{'9', {"9)"}},

		{'!', {"1!", "SHIFT"}},
		{'"', {"2\"", "SHIFT"}},
		{'#', {"3#", "SHIFT"}},
		{'$', {"4$", "SHIFT"}},
		{'%', {"5%", "SHIFT"}},
		{'&', {"6&", "SHIFT"}},
		{'\'', {"7'", "SHIFT"}},
		{'(', {"8(", "SHIFT"}},
		{')', {"9)", "SHIFT"}},

		{'-', {"-="}},
		{':', {":*"}},
		{';', {";+"}},
		{',', {",<"}},
		{'.', {".>"}},

		{'=', {"-=", "SHIFT"}},
		{'*', {":*", "SHIFT"}},
		{'+', {";+", "SHIFT"}},
		{'<', {",<", "SHIFT"}},
		{'>', {".>", "SHIFT"}},

		{'\\', {"\\"}},

		{'[', {"["}},
		{']', {"]",}},


		{'/', {"/?"}},
		{'?', {"/?", "SHIFT"} }



	};

	// 11 rows and 9 columns (the last row - row 10 -  is however not connected to the ROW input)
	vector<vector<KeyCodeMapping>> mKeyboardMatrix = {
		{
			{-1,							""},		// 0,  0
			{ALLEGRO_KEY_3,					"3#"},		// 0,  1
			{ALLEGRO_KEY_MINUS,				"-="},		// 0,  2 
			{ALLEGRO_KEY_G,             	"Gg"},		// 0,  3
			{ALLEGRO_KEY_Q,					"Qq"},		// 0,  4
			{ALLEGRO_KEY_ESCAPE,			"ESC"},		// 0,  5
			{-1,							"" },		// 0,  6	Columns 6 to 8 are not connected to the ROW input
			{-1,							""} ,		// 0,  7
			{-1,							""}			// 0,  8
		},

		{
			{-1,							""},		// 1,  0
			{ALLEGRO_KEY_2,					"2\""},		// 1,  1
			{ALLEGRO_KEY_COMMA,				",<"},		// 1,  2
			{ALLEGRO_KEY_F,             	"Ff"},		// 1,  3
			{ALLEGRO_KEY_P,					"Pp"},		// 1,  4
			{ALLEGRO_KEY_Z,					"Zz"},		// 1,  5
			{-1,							"" },		// 1,  6	Columns 6 to 8 are not connected to the ROW input
			{-1,							""} ,		// 1,  7
			{-1,							""}			// 1,  8
		},

		{
			{-1	,							"VTARROW"},	// 2,  0
			{ALLEGRO_KEY_1,					"1!"},		// 2,  1
			{ALLEGRO_KEY_PGUP,				";+"},		// 2,  2
			{ALLEGRO_KEY_E,             	"Ee"},		// 2,  3
			{ALLEGRO_KEY_O,             	"Oo"},		// 2,  4
			{ALLEGRO_KEY_Y,					"Yy"},		// 2,  5
			{-1,							"" },		// 2,  6	Columns 6 to 8 are not connected to the ROW input
			{-1,							""} ,		// 2,  7
			{-1,							""}			// 2,  8
		},

		{
			{ALLEGRO_KEY_TAB,				"HZARROW"},	// 3,	0
			{ALLEGRO_KEY_0,					"0"},		// 3,	1
			{ALLEGRO_KEY_DELETE,			":*"},		// 3,	2
			{ALLEGRO_KEY_D,             	"Dd"},		// 3,	3
			{ALLEGRO_KEY_N,             	"Nn"},		// 3,	4
			{ALLEGRO_KEY_X,					"Xx"},		// 3,	5
			{-1,							"" },		// 3,	6	Columns 6 to 8 are not connected to the ROW input
			{-1,							""} ,		// 3,	7
			{-1,							""}			// 3,	8
		},

		{
			{ALLEGRO_KEY_CAPSLOCK,			"LOCK"},	// 4,	0
			{ALLEGRO_KEY_BACKSPACE,			"DEL"},		// 4,	1
			{ALLEGRO_KEY_9,					"9)"},		// 4,	2	
			{ALLEGRO_KEY_C,             	"Cc"},		// 4,	3
			{ALLEGRO_KEY_M,             	"Mm"},		// 4,	4
			{ALLEGRO_KEY_W,					"Ww"},		// 4,	5
			{-1,							"" },		// 4,	6	Columns 6 to 8 are not connected to the ROW input
			{-1,							""} ,		// 4,	7
			{-1,							""}			// 4,	8
		},

		{
			{ALLEGRO_KEY_UP,				"UPARROW"},	// 5,	0
			{ALLEGRO_KEY_HOME,				"COPY"},	// 5,	1
			{ALLEGRO_KEY_8,					"8("},		// 5,	2
			{ALLEGRO_KEY_B,             	"Bb"},		// 5,	3
			{ALLEGRO_KEY_L,             	"Ll"},		// 5,	4
			{ALLEGRO_KEY_V,					"Vv"},		// 5,	5
			{-1,							"" },		// 5,	6	Columns 6 to 8 are not connected to the ROW input
			{-1,							""} ,		// 5,	7
			{-1,							""}			// 5,	8
		},

		{
			{ALLEGRO_KEY_DOWN,				"]"},		// 6,	0
			{ALLEGRO_KEY_ENTER,				"RETURN"},	// 6,	1
			{ALLEGRO_KEY_7,					"7'"},		// 6,	2
			{ALLEGRO_KEY_A,             	"Aa"},		// 6,	3
			{ALLEGRO_KEY_K,             	"Kk"},		// 6,	4
			{ALLEGRO_KEY_U,					"Uu"},		// 6,	5
			{-1,							"" },		// 6,	6	Columns 6 to 8 are not connected to the ROW input
			{-1,							""} ,		// 6,	7
			{-1,							""}			// 6,	8
		},

		{
			{ALLEGRO_KEY_BACKSLASH,			"\\"},		// 7,	0
			{-1,							""},		// 7,	1
			{ALLEGRO_KEY_6,					"6&"},		// 7,	2
			{ALLEGRO_KEY_PGDN,				"@"},		// 7,	3
			{ALLEGRO_KEY_J,             	"Jj"},		// 7,	4
			{ALLEGRO_KEY_T,					"Tt"},		// 7,	5	
			{-1,							"" },		// 7,	6	Columns 6 to 8 are not connected to the ROW input
			{-1,							""} ,		// 7,	7
			{-1,							""}			// 7,	8
		},

		{
			{ALLEGRO_KEY_LEFT,				"["},		// 8,	0
			{-1,							""} ,		// 8,	1
			{ALLEGRO_KEY_5,					"5%"},		// 8,	2
			{ALLEGRO_KEY_END,				"/?"},		// 8,	3
			{ALLEGRO_KEY_I,             	"Ii"},		// 8,	4
			{ALLEGRO_KEY_S,					"Ss"},		// 8,	5
			{-1,							"" },		// 8,	6	Columns 6 to 8 are not connected to the ROW input
			{-1,							""} ,		// 8,	7
			{-1,							""}			// 8,	8
		},

		{
			{ALLEGRO_KEY_SPACE,				"SPACE"},	// 9,	0
			{-1,							""} ,		// 9,	1
			{ALLEGRO_KEY_4,					"4$"},		// 9,	2
			{ALLEGRO_KEY_FULLSTOP,			".>"},		// 9,	3
			{ALLEGRO_KEY_H,					"Hh"},		// 9,	4
			{ALLEGRO_KEY_R,					"Rr"},		// 9,	5
			{-1,							"" },		// 9,	6	Columns 6 to 8 are not connected to the ROW input
			{ALLEGRO_KEY_LSHIFT,			"SHIFT"},	// 9,	7	
			{-1,							""}			// 9,	8
		},

		// Row 10 operates independently of the ROW input.
		// A cleared bit means that the associated key is being pressed, a set bit that it is not being pressed.
		{
			{-1,							""} ,		// 10,	0
			{-1,							""} ,		// 10,	1
			{-1,							""} ,		// 10,	2
			{-1,							""} ,		// 10,	3
			{-1,							""} ,		// 10,	4
			{-1,							""} ,		// 10,	5	
			{ ALLEGRO_KEY_LCTRL,			"CTRL"},	// 10,	6					
			{ ALLEGRO_KEY_RSHIFT,			"SHIFT"},	// 10,	7
			{ ALLEGRO_KEY_RIGHT,			"REPEAT"}	// 10,	8
		}
	};


	// Ports that can be connected to other devices
	int KB_ROW, KB_COL;
	PortVal mSelectedRow = 0x0;
	PortVal mColumn = 0x3ff;

	int mCtrlKeyCode = mKeyboardMatrix[10][6].keyCode;
	int mShiftKeyCodes[2] = { mKeyboardMatrix[9][7].keyCode, mKeyboardMatrix[10][7].keyCode };
	int mRepeatKeyCode = mKeyboardMatrix[10][8].keyCode;
	int mBreakKeyCode = ALLEGRO_KEY_PAUSE;

	int mKeyboardRefreshCycles = 1;

	bool checkKeyBoard();

public:

	AtomKeyboardDevice(string name, double tickRate, DebugTracing  *debugTracing, ConnectionManager* connectionManager);

	// Reset device
	bool power() { mTicks = 0; return true; }


	//  Advance until time tickTime
	bool advanceUntil(uint64_t tickTime) override;

	// Handle input port changes directly on change
	void processPortUpdate(int index) override;
	void processPortUpdate() override;

	// Outputs the internal state of the device
	bool outputState(ostream& sout) override;

	// Set emulation speed
	void setEmulationSpeed(double speed) override;

	// Check if the minimum key down time has passed (for the paste function)
	bool minKeyDownTimePassed() override;

	// Get the key name from the keyboard matrix
	string getKeyName(int keyCode) override { return KeyboardDevice::getKeyName(keyCode, mKeyboardMatrix); }

};

#endif