#ifndef BEEB_KEYBOARD_H
#define BEEB_KEYBOARD_H

#include <vector>
#include <map>
#include "Device.h"
#include <allegro5/allegro5.h>
#include "DebugInfo.h"


typedef struct Key_struct {
	int		keyCode;
	int		row;	// 11 rows
	int		col;	// 6 columns
	string	keyName; // e.g, "COPY", "ESC" and "A"
}  Key;

class BeebKeyboard : public Device {

private:

	ALLEGRO_KEYBOARD_STATE mKeyboardState;

	vector<vector<Key>> mKeyboardMatrix = {
		{
			{ALLEGRO_KEY_ESCAPE,	7,  0, "ESC"},
			{ALLEGRO_KEY_F1,		7,  1, "F1"},
			{ALLEGRO_KEY_F2,		7,  2, "F2"},
			{ALLEGRO_KEY_F3,		7,  3, "F3"},
			{ALLEGRO_KEY_F5,		7,  4, "F5"},
			{ALLEGRO_KEY_F6,		7,  5, "F6"},
			{ALLEGRO_KEY_F8,		7,  6, "F8"},
			{ALLEGRO_KEY_F9,		7,  7, "F9"},
			{ALLEGRO_KEY_BACKSLASH,	7,  8, "\\"},
			{ALLEGRO_KEY_RIGHT,		7,  9, "RIGHT ARROW"}
		},
		{
			{ALLEGRO_KEY_TAB,		6,  0, "TAB"},
			{ALLEGRO_KEY_Z,			6,  1, "Z"},
			{ALLEGRO_KEY_SPACE,		6,  2, " "},
			{ALLEGRO_KEY_V,			6,  3, "V"},
			{ALLEGRO_KEY_B,			6,  4, "B"},
			{ALLEGRO_KEY_M,			6,  5, "M"},
			{ALLEGRO_KEY_COMMA,		6,  6, "<,"},
			{ALLEGRO_KEY_FULLSTOP,	6,  7, ">."},
			{ALLEGRO_KEY_END,		6,  8, "?/"},
			{ALLEGRO_KEY_HOME,		6,  9, "COPY"}
		},
		{
			{ALLEGRO_KEY_ALT,		5,  0, "SHIFT LOCK"},
			{ALLEGRO_KEY_S,			5,  1, "S"},
			{ALLEGRO_KEY_C,			5,  2, "C"},
			{ALLEGRO_KEY_G,			5,  3, "G"},
			{ALLEGRO_KEY_H,			5,  4, "H"},
			{ALLEGRO_KEY_N,			5,  5, "N"},
			{ALLEGRO_KEY_L,			5,  6, "L"},
			{ALLEGRO_KEY_RCTRL,		5,  7, "+;"},
			{ALLEGRO_KEY_PGDN,		5,  7, ")}"},
			{ALLEGRO_KEY_BACKSPACE,	5,  9, "DELETE"}
		},
		{
			{ALLEGRO_KEY_CAPSLOCK,  4,  0, "CAPS LOCK"},
			{ALLEGRO_KEY_A,			4,  1, "A"},
			{ALLEGRO_KEY_X,			4,  2, "X"},
			{ALLEGRO_KEY_F,			4,  3, "F"},
			{ALLEGRO_KEY_Y,			4,  4, "Y"},
			{ALLEGRO_KEY_J,			4,  5, "J"},
			{ALLEGRO_KEY_K,			4,  6, "K"},
			{ALLEGRO_KEY_RSHIFT,	4,  6, "@"},
			{ALLEGRO_KEY_DELETE,	4,  8, "*:"},
			{ALLEGRO_KEY_ENTER,		4,  9, "RETURN"}
		},{
			{ALLEGRO_KEY_1,			3,  0, "1"},
			{ALLEGRO_KEY_2,			3,  1, "2"},
			{ALLEGRO_KEY_D,			3,  2, "D"},
			{ALLEGRO_KEY_R,			3,  3, "R"},
			{ALLEGRO_KEY_6,			3,  4, "6"},
			{ALLEGRO_KEY_U,			3,  5, "U"},
			{ALLEGRO_KEY_O,			3,  6, "O"},
			{ALLEGRO_KEY_P,			3,  7, "P"},
			{ALLEGRO_KEY_PGUP,		3,  7, "{["},
			{ALLEGRO_KEY_UP,		3,  9, "UP ARROW"}
		},{
			{ALLEGRO_KEY_F,			2,  0, "F0"},
			{ALLEGRO_KEY_W,			2,  1, "W"},
			{ALLEGRO_KEY_E,			2,  2, "E"},
			{ALLEGRO_KEY_T,			2,  3, "T"},
			{ALLEGRO_KEY_7,			2,  4, "7"},
			{ALLEGRO_KEY_L,			2,  5, "L"},
			{ALLEGRO_KEY_9,			2,  6, "9"},
			{ALLEGRO_KEY_0,			2,  7, "0"},
			{ALLEGRO_KEY_ALTGR,		2,  8, "£_"},
			{ALLEGRO_KEY_DOWN,		2,  9, "DOWN ARROW"}
		},{
			{ALLEGRO_KEY_Q,			1,  0, "Q"},
			{ALLEGRO_KEY_3,			1,  1, "3"},
			{ALLEGRO_KEY_4,			1,  2, "4"},
			{ALLEGRO_KEY_5,			1,  3, "5"},
			{ALLEGRO_KEY_F4,		1,  4, "F4"},
			{ALLEGRO_KEY_8,			1,  5, "8"},
			{ALLEGRO_KEY_F7,		1,  6, "F7"},
			{ALLEGRO_KEY_MINUS,		1,  7, "=-"},
			{ALLEGRO_KEY_TILDE,		1,  8, "~^"},
			{ALLEGRO_KEY_LEFT,		1,  9, "LEFT ARROW"}
		},{
			{ALLEGRO_KEY_LSHIFT,	0,  0, "SHIFT"},
			{ALLEGRO_KEY_LCTRL,		0,  1, "CTRL"},
			{-1,					0,  2, "START UP OPTION 8"},
			{-1,  					0,  3, "START UP OPTION 7"},
			{-1,  					0,  4, "START UP OPTION 6"},
			{-1,  					0,  5, "START UP OPTION 5"},
			{-1,  					0,  6, "START UP OPTION 4"},
			{-1,  					0,  7, "START UP OPTION 3"},
			{-1,  					0,  8, "START UP OPTION 2"},
			{-1,  					0,  9, "START UP OPTION 1"}
		},
	};

	Key mBreakKey = { ALLEGRO_KEY_PAUSE , -1, -1, "BREAK" };

	// Ports that can be conALLEGROnected to other devices
	int  COL_SEL, ROW_SEL, ROW, BREAK, PRESSED;
	uint8_t mCOL_SEL = 0x0;			// Column Select: 0-9 BCD-coded;								connected to VIA 6522 (IC3) PA0:3 output
	uint8_t mROW_SEL = 0x0;			// Row Select:  0-9 BCD-coded;									connected to VIA 6522 (IC3) PA4:6 output
	uint8_t mROW = 0x1;				// Value of selected row (LOW if any key on row is pressed);	connected to VIA 6522 (IC3) PA7 input
	uint8_t mBREAK = 0x1;			// BREAK key (LOW when pressed <=> RESET)
	uint8_t mPRESSED = 0x0;			// HIGH when any key is pressed;								connected to VIA 6522 (IC3) CA2 input
	uint8_t mSW = 0x0;				// Start-up options DIL switches (8-bit) - see table below...
									//	Link	Bit	SET	(OFF)						CLR (ON)
									//	1		7	Default filing system : DFS		Default filing system : NFS
									//	2		6	Not used						Not used
									//	3		5	Disc drive timings				Disc drive timings
									//	4		4	Disc drive timings				Disc drive timings
									//	5		3	SHIFT - BREAK to boot			BREAK to boot
									//	6		2	+ 4 to screen mode				+ 0 to screen mode
									//	7		1	+ 2 to screen mode				+ 0 to screen mode
									//	8		0	+ 1 to screen mode				+ 0 to screen mode
	enum SwitchEnum { SW_FILING_SYSTEM = 0x80, SW_DISC_SETTINGS = 0x30, SW_BOOT = 0x08, SW_MOD_PL_4 = 0x4, SW_MOD_PL_2 = 0x2, SW_MOD_PL_1 = 0x1};
	typedef struct SWField_struct {
		uint8_t mask;
		uint8_t lowBit;
		SwitchEnum field;
	} SWField;

	SWField mSWField[6] = {
		{0x1, 7, SW_FILING_SYSTEM},
		{0x3, 4, SW_DISC_SETTINGS},
		{0x1, 3, SW_BOOT},
		{0x1, 2, SW_MOD_PL_4},
		{0xe, 1, SW_MOD_PL_2},
		{0x0, 0, SW_MOD_PL_1}
	};
	inline uint8_t getSWField(SwitchEnum field) {
		return (mSW >> mSWField[field].lowBit) & mSWField[field].mask;
	}




public:

	BeebKeyboard(string name, uint8_t startupOptions, DebugInfo debugInfo, ConnectionManager* connectionManager);

	// Reset device
	bool reset();

	//  Advance until clock cycle stopcycle has been reached
	bool advance(uint64_t stopCycle);
};

#endif