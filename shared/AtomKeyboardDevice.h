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
// input port (ROW) and two output ports (COL_L & COL_H).
// 
// 
// PORT CONNECTIONS
// 
// The ROW input port shall be connected to using the directive "CONNECT:P"
// to ensure that the device is triggered immediately on changes to the ROW
// input port. The COL_L and COL_H output ports shall be connected to a device
// that can read these ports (typically a PIA 8255).
// 
// 
// FUNCTIONAL DESCRIPTION
// 
// To detect a key, the ROW input shall first select a row in the keyboard matrix.
// Then the key columns in which a key is being pressed can be determined
// by reading the COL_L and COL_H outputs. A cleared bit means that a key
// in the associated column is being pressed, a set bit that no key in the associated
// column is being pressed.
// 
// KEYBOARD MATRIX
// 
//		COLUMN
//      COL_L                                           COL_H
//      b0    b1    b2    b3    b4    b5    b6    b7    b0   b1
//      ============================================    ==========
// ROW  0     1     2     3     4     5     6     7     8     9
// 0	NC    3     -     G     Q    ESC  CTRL  SHIFT REPEAT BREAK
// 1    NC    2     ,     F     P     Z
// 2    HZ    1     ;     E     O     Y
// 3    VT    0     :     D     N     X
// 4   LOCK  DEL    9     C     M     W
// 5    UP   COPY   8     B     L     V
// 6    ]    RET    7     A     K     U
// 7    \    NC     6     @     J     T
// 8    [    NC     5     /     I     S
// 9  SPACE  NC     4     .     H     R
// 
// 



typedef struct AtomKey_struct {
	int		keyCode;
	int		row;	// 11 rows
	int		col;	// 6 columns
	string	atomKeyName; // e.g, "COPY", "ESC" and "A"
} AtomKey;
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

	vector<KeyboardKey> mPasteKeyMap = {

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

		{'^', {"^~"}},
		{'~', {"^~", "SHIFT"}},

		{'[', {"["}},
		{']', {"]",}},


		{'/', {"/?"}},
		{'?', {"/?", "SHIFT"} }



	};

	map<int, AtomKey> mKeycodes = {

		// Top row ESC 1 2 3 4 5 6 7 8 9 0 -= :* ^~ BREAK
		{ALLEGRO_KEY_ESCAPE,		{ALLEGRO_KEY_ESCAPE,		0,		5,	"ESC"}},	// ESC
		{ALLEGRO_KEY_0,				{ALLEGRO_KEY_0,				3,		1,	"0"}},		// 0
		{ALLEGRO_KEY_1,				{ALLEGRO_KEY_1,				2,		1,	"1!"}},		// 1 !
		{ALLEGRO_KEY_2,				{ALLEGRO_KEY_2,				1,		1,	"2\""}},	// 2 "
		{ALLEGRO_KEY_3,				{ALLEGRO_KEY_3,				0,		1,	"3#"}},		// 3 #
		{ALLEGRO_KEY_4,				{ALLEGRO_KEY_4,				9,		2,	"4$"}},		// 4 $
		{ALLEGRO_KEY_5,				{ALLEGRO_KEY_5,				8,		2,	"5%"}},		// 5 %
		{ALLEGRO_KEY_6,				{ALLEGRO_KEY_6,				7,		2,	"6&"}},		// 6 &
		{ALLEGRO_KEY_7,				{ALLEGRO_KEY_7,				6,		2,	"7'"}},		// 7 '
		{ALLEGRO_KEY_8,				{ALLEGRO_KEY_8,				5,		2,	"8("}},		// 8 (
		{ALLEGRO_KEY_9,				{ALLEGRO_KEY_9,				4,		2,	"9)"}},		// 9 )
		{ALLEGRO_KEY_MINUS,			{ALLEGRO_KEY_MINUS,			0,		2,	"-="}},		// -=	
		{ALLEGRO_KEY_DELETE,		{ALLEGRO_KEY_DELETE,		3,		2,	":*"}},		// :*
		{ALLEGRO_KEY_TILDE,			{ALLEGRO_KEY_TILDE,			5,		0,	"^~"}},		// ^~
		{ALLEGRO_KEY_PAUSE,			{ALLEGRO_KEY_PAUSE,			-1,    -1,  "BREAK"}},	// BREAK (mapped as COL_H b1)


		// Second row <-> COPY Q W E R T Y U I O P @ \ DEL

		{ALLEGRO_KEY_TAB,			{ALLEGRO_KEY_TAB,			3,		0,	"HZARROW"}},// <->
		{ALLEGRO_KEY_HOME,			{ALLEGRO_KEY_HOME,			5,		1,	"COPY"}},	// COPY
		{ALLEGRO_KEY_Q,				{ALLEGRO_KEY_Q,				0,		4,	"Qq"}},		// Q
		{ALLEGRO_KEY_W,				{ALLEGRO_KEY_W,				4,		5,	"Ww"}},		// W
		{ALLEGRO_KEY_E,				{ALLEGRO_KEY_E,             2,		3,	"Ee"}},		// E
		{ALLEGRO_KEY_R,				{ALLEGRO_KEY_R,				9,		5,	"Rr"}},		// R
		{ALLEGRO_KEY_T,				{ALLEGRO_KEY_T,				7,		5,	"Tt"}},		// T
		{ALLEGRO_KEY_Y,				{ALLEGRO_KEY_Y,				2,		5,	"Yy"}},		// Y
		{ALLEGRO_KEY_U,				{ALLEGRO_KEY_U,				6,		5,	"Uu"}},		// U
		{ALLEGRO_KEY_I,				{ALLEGRO_KEY_I,             8,		4,	"Ii"}},		// I
		{ALLEGRO_KEY_O,				{ALLEGRO_KEY_O,             2,		4,	"Oo"}},		// O
		{ALLEGRO_KEY_P,				{ALLEGRO_KEY_P,				1,		4,	"Pp"}},		// P
		{ALLEGRO_KEY_PGDN,			{ALLEGRO_KEY_PGDN,			7,		3,	"@"}},		// @
		{ALLEGRO_KEY_BACKSLASH,		{ALLEGRO_KEY_BACKSLASH,		7,		0,	"\\"}},		// "\"
		{ALLEGRO_KEY_BACKSPACE,		{ALLEGRO_KEY_BACKSPACE,		4,		1,	"DEL"}},	// DEL


		// Third row UP_DOWN_ARROW CTRL A S D F G H J K L ;+ [ ] RET

		// UP/DOWN arrow
		{ALLEGRO_KEY_UP,			{ALLEGRO_KEY_UP,			5,		0,	"VTARROW"}}, // UP/DOWN ARROW
		{ALLEGRO_KEY_LCTRL,			{ALLEGRO_KEY_LCTRL,			-1,		-1,	"CTRL"}},	// CTRL (mapped as COL_L b6)
		{ALLEGRO_KEY_A,				{ALLEGRO_KEY_A,             6,		3,	"Aa"}},
		{ALLEGRO_KEY_S,				{ALLEGRO_KEY_S,				8,		5,	"Ss"}},
		{ALLEGRO_KEY_D,				{ALLEGRO_KEY_D,             3,		3,	"Dd"}},
		{ALLEGRO_KEY_F,				{ALLEGRO_KEY_F,             1,		3,	"Ff"}},
		{ALLEGRO_KEY_G,				{ALLEGRO_KEY_G,             0,		3,	"Gg"}},
		{ALLEGRO_KEY_H,				{ALLEGRO_KEY_H,				9,		4,	"Hh"}},
		{ALLEGRO_KEY_J,				{ALLEGRO_KEY_J,             7,		4,	"Jj"}},
		{ALLEGRO_KEY_K,				{ALLEGRO_KEY_K,             6,		4,	"Kk"}},
		{ALLEGRO_KEY_L,				{ALLEGRO_KEY_L,             5,		4,	"Ll"}},
		{ALLEGRO_KEY_PGUP,			{ALLEGRO_KEY_PGUP,			2,		2,	";+"}},		// ;+
		{ALLEGRO_KEY_LEFT,			{ALLEGRO_KEY_LEFT,			8,		0,  "["}},		// [
		{ALLEGRO_KEY_DOWN,			{ALLEGRO_KEY_DOWN,			6,		0,	"]"}},		// ]
		{ALLEGRO_KEY_ENTER,			{ALLEGRO_KEY_ENTER,			6,		1,	"RETURN"}}, // RETURN

		// Forth row LOCK SHIFT Z X C V B N M ,< .> /? SHIFT REPEAT
		{ALLEGRO_KEY_CAPSLOCK,		{ALLEGRO_KEY_CAPSLOCK,		4,		0,	"LOCK"}},	// LOCK
		{ALLEGRO_KEY_LSHIFT,		{ALLEGRO_KEY_LSHIFT,		-1,    -1,	"SHIFT"}},	// SHIFT (mapped as COL_L b7)
		{ALLEGRO_KEY_Z,				{ALLEGRO_KEY_Z,				1,		5,	"Zz"}},		// Z
		{ALLEGRO_KEY_X,				{ALLEGRO_KEY_X,				3,		5,	"Xx"}},		// X
		{ALLEGRO_KEY_C,				{ALLEGRO_KEY_C,             4,		3,	"Cc"}},		// C
		{ALLEGRO_KEY_V,				{ALLEGRO_KEY_V,				5,		5,	"Vv"}},		// V
		{ALLEGRO_KEY_B,				{ALLEGRO_KEY_B,             5,		3,	"Bb"}},		// B
		{ALLEGRO_KEY_N,				{ALLEGRO_KEY_N,             3,		4,	"Nn"}},		// N
		{ALLEGRO_KEY_M,				{ALLEGRO_KEY_M,             4,		4,	"Mm"}},		// M
		{ALLEGRO_KEY_COMMA,			{ALLEGRO_KEY_COMMA,			1,		2,	",<"}},		// ,<
		{ALLEGRO_KEY_FULLSTOP,		{ALLEGRO_KEY_FULLSTOP,		9,		3,	".>"}},		// .>
		{ALLEGRO_KEY_END,			{ALLEGRO_KEY_END,			8,		3,	"/?"}},		// /?
		{ALLEGRO_KEY_RSHIFT,		{ALLEGRO_KEY_RSHIFT,		-1,    -1,	"SHIFT"}},	// SHIFT (mapped as COL_L b7)
		{ALLEGRO_KEY_RIGHT,			{ALLEGRO_KEY_RIGHT,			-1,    -1,	"REPEAT"}},	// REPEAT (mapped as COL_H b0)

		// Fifth row
		{ALLEGRO_KEY_SPACE,			{ALLEGRO_KEY_SPACE,			9,   0,    "SPACE"}}
	};

	vector<vector<AtomKey*>> mKeyboardMatrix = {
		{NULL, NULL, NULL, NULL, NULL, NULL},
		{NULL, NULL, NULL, NULL, NULL, NULL},
		{NULL, NULL, NULL, NULL, NULL, NULL},
		{NULL, NULL, NULL, NULL, NULL, NULL},
		{NULL, NULL, NULL, NULL, NULL, NULL},
		{NULL, NULL, NULL, NULL, NULL, NULL},
		{NULL, NULL, NULL, NULL, NULL, NULL},
		{NULL, NULL, NULL, NULL, NULL, NULL},
		{NULL, NULL, NULL, NULL, NULL, NULL},
		{NULL, NULL, NULL, NULL, NULL, NULL}
	};

	// Ports that can be connected to other devices
	int KB_ROW, KB_COL_L, KB_COL_H;
	PortVal mSelectedRow = 0x0;
	PortVal mColumnL = 0xff;
	PortVal mColumnH = 0x3;

	int mCtrlKeyCode = -1;
	int mShiftKeyCodes[2] = { -1 };
	int mRepeatKeyCode = -1;
	int mBreakKeyCode = -1;

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

};

#endif