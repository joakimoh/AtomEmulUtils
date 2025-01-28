#ifndef KEYBOARD_DEVICE_H
#define KEYBOARD_DEVICE_H

#include <vector>
#include <map>
#include "Device.h"
#include <allegro5/allegro5.h>
#include "DebugInfo.h"


typedef struct AtomKey_struct {
	int		keyCode;
	int		row;	// 11 rows
	int		col;	// 6 columns
	string	atomKeyName; // e.g, "COPY", "ESC" and "A"
} AtomKey;

typedef struct AtomModifier_struct {
	int	keyCode;
	bool ctrl;
	bool shift;
	bool repeat;
} AtomModifier;

class AtomKeyboardDevice : public Device {

private:

	ALLEGRO_KEYBOARD_STATE mKeyboardState;

	map<int, AtomKey> mKeycodes = {

		// Top row ESC 1 2 3 4 5 6 7 8 9 0 -= :* ^~ BREAK
		{ALLEGRO_KEY_ESCAPE,		{ALLEGRO_KEY_ESCAPE,		0,		5,	"ESC"}},	// ESC
		{ALLEGRO_KEY_0,				{ALLEGRO_KEY_0,				3,		1,	"0"}},		// 0
		{ALLEGRO_KEY_1,				{ALLEGRO_KEY_1,				2,		1,	"1"}},		// 1 !
		{ALLEGRO_KEY_2,				{ALLEGRO_KEY_2,				1,		1,	"2"}},		// 2 "
		{ALLEGRO_KEY_3,				{ALLEGRO_KEY_3,				0,		1,	"3"}},		// 3 #
		{ALLEGRO_KEY_4,				{ALLEGRO_KEY_4,				9,		2,	"4"}},		// 4 $
		{ALLEGRO_KEY_5,				{ALLEGRO_KEY_5,				8,		2,	"5"}},		// 5 %
		{ALLEGRO_KEY_6,				{ALLEGRO_KEY_6,				7,		2,	"6"}},		// 6 &
		{ALLEGRO_KEY_7,				{ALLEGRO_KEY_7,				6,		2,	"7"}},		// 7 '
		{ALLEGRO_KEY_8,				{ALLEGRO_KEY_8,				5,		2,	"8"}},		// 8 (
		{ALLEGRO_KEY_9,				{ALLEGRO_KEY_9,				4,		2,	"9"}},		// 9 )
		{ALLEGRO_KEY_MINUS,			{ALLEGRO_KEY_MINUS,			0,		2,	"-"}},		// -=	
		{ALLEGRO_KEY_DELETE,		{ALLEGRO_KEY_DELETE,		3,		2,	":"}},		// :*
		{ALLEGRO_KEY_TILDE,			{ALLEGRO_KEY_TILDE,			5,		0,	"^"}},		// ^~
		{ALLEGRO_KEY_PAUSE,			{ALLEGRO_KEY_PAUSE,			-1,    -1,  "BREAK"}},	// BREAK (mapped as COL_H b1)


		// Second row <-> COPY Q W E R T Y U I O P @ \ DEL

		{ALLEGRO_KEY_TAB,			{ALLEGRO_KEY_TAB,			3,		0,	"HZARROW"}},// <->
		{ALLEGRO_KEY_HOME,			{ALLEGRO_KEY_HOME,			5,		1,	"COPY"}},	// COPY
		{ALLEGRO_KEY_Q,				{ALLEGRO_KEY_Q,				0,		4,	"Q"}},		// Q
		{ALLEGRO_KEY_W,				{ALLEGRO_KEY_W,				4,		5,	"W"}},		// W
		{ALLEGRO_KEY_E,				{ALLEGRO_KEY_E,             2,		3,	"E"}},		// E
		{ALLEGRO_KEY_R,				{ALLEGRO_KEY_R,				9,		5,	"R"}},		// R
		{ALLEGRO_KEY_T,				{ALLEGRO_KEY_T,				7,		5,	"T"}},		// T
		{ALLEGRO_KEY_Y,				{ALLEGRO_KEY_Y,				2,		5,	"Y"}},		// Y
		{ALLEGRO_KEY_U,				{ALLEGRO_KEY_U,				6,		5,	"U"}},		// U
		{ALLEGRO_KEY_I,				{ALLEGRO_KEY_I,             8,		4,	"I"}},		// I
		{ALLEGRO_KEY_O,				{ALLEGRO_KEY_O,             2,		4,	"O"}},		// O
		{ALLEGRO_KEY_P,				{ALLEGRO_KEY_P,				1,		4,	"P"}},		// P
		{ALLEGRO_KEY_PGDN,			{ALLEGRO_KEY_PGDN,			7,		3,	"@"}},		// @
		{ALLEGRO_KEY_BACKSLASH,		{ALLEGRO_KEY_BACKSLASH,		7,		0,	"\\"}},		// "\"
		{ALLEGRO_KEY_BACKSPACE,		{ALLEGRO_KEY_BACKSPACE,		4,		1,	"DEL"}},	// DEL


		// Third row UP_DOWN_ARROW CTRL A S D F G H J K L ;+ [ ] RET

		// UP/DOWN arrow
		{ALLEGRO_KEY_UP,			{ALLEGRO_KEY_UP,			5,		0,	"VTARROW"}}, // UP/DOWN ARROW
		{ALLEGRO_KEY_LCTRL,			{ALLEGRO_KEY_LCTRL,			-1,		-1,	"CTRL"}},	// CTRL (mapped as COL_L b6)
		{ALLEGRO_KEY_A,				{ALLEGRO_KEY_A,             6,		3,	"A"}},
		{ALLEGRO_KEY_S,				{ALLEGRO_KEY_S,				8,		5,	"S"}},
		{ALLEGRO_KEY_D,				{ALLEGRO_KEY_D,             3,		3,	"D"}},
		{ALLEGRO_KEY_F,				{ALLEGRO_KEY_F,             1,		3,	"F"}},
		{ALLEGRO_KEY_G,				{ALLEGRO_KEY_G,             0,		3,	"G"}},
		{ALLEGRO_KEY_H,				{ALLEGRO_KEY_H,				9,		4,	"H"}},
		{ALLEGRO_KEY_J,				{ALLEGRO_KEY_J,             7,		4,	"J"}},
		{ALLEGRO_KEY_K,				{ALLEGRO_KEY_K,             6,		4,	"K"}},
		{ALLEGRO_KEY_L,				{ALLEGRO_KEY_L,             5,		4,	"L"}},
		{ALLEGRO_KEY_PGUP,			{ALLEGRO_KEY_PGUP,			2,		2,	";"}},		// ;+
		{ALLEGRO_KEY_LEFT,			{ALLEGRO_KEY_LEFT,			8,		0,  "["}},		// [
		{ALLEGRO_KEY_DOWN,			{ALLEGRO_KEY_DOWN,			6,		0,	"]"}},		// ]
		{ALLEGRO_KEY_ENTER,			{ALLEGRO_KEY_ENTER,			6,		1,	"RETURN"}}, // RETURN

		// Forth row LOCK SHIFT Z X C V B N M ,< .> /? SHIFT REPEAT
		{ALLEGRO_KEY_CAPSLOCK,		{ALLEGRO_KEY_CAPSLOCK,		4,		0,	"LOCK"}},	// LOCK
		{ALLEGRO_KEY_LSHIFT,		{ALLEGRO_KEY_LSHIFT,		-1,    -1,	"SHIFT"}},	// SHIFT (mapped as COL_L b7)
		{ALLEGRO_KEY_Z,				{ALLEGRO_KEY_Z,				1,		5,	"Z"}},		// Z
		{ALLEGRO_KEY_X,				{ALLEGRO_KEY_X,				3,		5,	"X"}},		// X
		{ALLEGRO_KEY_C,				{ALLEGRO_KEY_C,             4,		3,	"C"}},		// C
		{ALLEGRO_KEY_V,				{ALLEGRO_KEY_V,				5,		5,	"V"}},		// V
		{ALLEGRO_KEY_B,				{ALLEGRO_KEY_B,             5,		3,	"B"}},		// B
		{ALLEGRO_KEY_N,				{ALLEGRO_KEY_N,             3,		4,	"N"}},		// N
		{ALLEGRO_KEY_M,				{ALLEGRO_KEY_M,             4,		4,	"M"}},		// M
		{ALLEGRO_KEY_COMMA,			{ALLEGRO_KEY_COMMA,			1,		2,	","}},		// ,<
		{ALLEGRO_KEY_FULLSTOP,		{ALLEGRO_KEY_FULLSTOP,		9,		3,	"."}},		// .>
		{ALLEGRO_KEY_END,			{ALLEGRO_KEY_END,			8,		3,	"/"}},		// /?
		{ALLEGRO_KEY_RSHIFT,		{ALLEGRO_KEY_RSHIFT,		-1,    -1,	"SHIFT"}},	// SHIFT (mapped as COL_L b7)
		{ALLEGRO_KEY_RIGHT,			{ALLEGRO_KEY_RIGHT,			-1,    -1,	"REPEAT"}},	// REPEAT (mapped as COL_H b0)

		// Fifth row
		{ALLEGRO_KEY_SPACE,			{ALLEGRO_KEY_SPACE,			9,   0,    " "}}
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
	uint8_t mSelectedRow = 0x0;
	uint8_t mColumnL = 0xff;
	uint8_t mColumnH = 0x3;

	int mCtrlKeyCode = -1;
	int mShiftKeyCodes[2] = { -1 };
	int mRepeatKeyCode = -1;
	int mBreakKeyCode = -1;

public:

	AtomKeyboardDevice(string name, DebugInfo debugInfo, ConnectionManager* connectionManager);

	// The Keyboard Device is not a memory-mapped device so we have to
	// return false on all attempts to access it...
	bool read(uint16_t adr, uint8_t& data) { data = 0xff; return false; }
	bool write(uint16_t adr, uint8_t data) { return false;  }
	bool selected(uint16_t adr) { return false; }
	bool validAdr(uint16_t adr) { return false; }

	// Reset device
	bool reset() { mCycleCount = 0; return true; }

	//  Advance until clock cycle stopcycle has been reached
	bool advance(uint64_t stopCycle);
};

#endif