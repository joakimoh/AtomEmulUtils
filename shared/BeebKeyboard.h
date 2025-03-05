#ifndef BEEB_KEYBOARD_H
#define BEEB_KEYBOARD_H

#include <vector>
#include <map>
#include "Device.h"
#include <allegro5/allegro5.h>
#include "DebugInfo.h"


typedef struct Key_struct {
	int		keyCode;
	string	keyName; // e.g, "COPY", "ESC" and "A"
}  Key;

class BeebKeyboard : public Device {

private:

	ALLEGRO_KEYBOARD_STATE mKeyboardState;

	vector<vector<Key>> mKeyboardMatrix = {
		{
			{ALLEGRO_KEY_LSHIFT,	"SHIFT"},				// 0,  0
			{ALLEGRO_KEY_LCTRL,		"CTRL"},				// 0,  1
			{-1,					"START UP OPTION 8"},	// 0,  2
			{-1,  					"START UP OPTION 7"},	// 0,  3
			{-1,  					"START UP OPTION 6"},	// 0,  4
			{-1,  					"START UP OPTION 5"},	// 0,  5
			{-1,  					"START UP OPTION 4"},	// 0,  6
			{-1,  					"START UP OPTION 3"},	// 0,  7
			{-1,  					"START UP OPTION 2"},	// 0,  8
			{-1,  					"START UP OPTION 1"}	// 0,  9
		},
		{
			{ALLEGRO_KEY_Q,			"Q"},					// 1,  0
			{ALLEGRO_KEY_3,			"3"},					// 1,  1
			{ALLEGRO_KEY_4,			"4"},					// 1,  2
			{ALLEGRO_KEY_5,			"5"},					// 1,  3
			{ALLEGRO_KEY_F4,		"F4"},					// 1,  4
			{ALLEGRO_KEY_8,			"8"},					// 1,  5
			{ALLEGRO_KEY_F7,		"F7"},					// 1,  6
			{ALLEGRO_KEY_MINUS,		"=-"},					// 1,  7
			{ALLEGRO_KEY_TILDE,		"~^"},					// 1,  8
			{ALLEGRO_KEY_LEFT,		"LEFT ARROW"}			// 1,  9
		},
		{
			{-1,				    "F0"},					// 2,  0	-  Not supported!
			{ALLEGRO_KEY_W,			"W"},					// 2,  1
			{ALLEGRO_KEY_E,			"E"},					// 2,  2
			{ALLEGRO_KEY_T,			"T"},					// 2,  3
			{ALLEGRO_KEY_7,			"7"},					// 2,  4
			{ALLEGRO_KEY_I,			"I"},					// 2,  5
			{ALLEGRO_KEY_9,			"9"},					// 2,  6
			{ALLEGRO_KEY_0,			"0"},					// 2,  7
			{ALLEGRO_KEY_ALTGR,		"POUND"},				// 2,  8
			{ALLEGRO_KEY_DOWN,		"DOWN ARROW"}			// 2,  9
		},{
			{ALLEGRO_KEY_1,			"1"},					// 3,  0
			{ALLEGRO_KEY_2,			"2"},					// 3,  1
			{ALLEGRO_KEY_D,			"D"},					// 3,  2
			{ALLEGRO_KEY_R,			"R"},					// 3,  3
			{ALLEGRO_KEY_6,			"6"},					// 3,  4
			{ALLEGRO_KEY_U,			"U"},					// 3,  5
			{ALLEGRO_KEY_O,			"O"},					// 3,  6
			{ALLEGRO_KEY_P,			"P"},					// 3,  7
			{ALLEGRO_KEY_PGUP,		"{["},					// 3,  8
			{ALLEGRO_KEY_UP,		"UP ARROW"}				// 3,  9
		},
		{
			{ALLEGRO_KEY_CAPSLOCK,  "CAPS LOCK"},			// 4,  0
			{ALLEGRO_KEY_A,			"A"},					// 4,  1
			{ALLEGRO_KEY_X,			"X"},					// 4,  2
			{ALLEGRO_KEY_F,			"F"},					// 4,  3
			{ALLEGRO_KEY_Y,			"Y"},					// 4,  4
			{ALLEGRO_KEY_J,			"J"},					// 4,  5
			{ALLEGRO_KEY_K,			"K"},					// 4,  6
			{ALLEGRO_KEY_RSHIFT,	"@"},					// 4,  7
			{ALLEGRO_KEY_DELETE,	"*:"},					// 4,  8
			{ALLEGRO_KEY_ENTER,		"RETURN"}				// 4,  9
		},
		{
			{ALLEGRO_KEY_ALT,		"SHIFT LOCK"},			// 5,  0
			{ALLEGRO_KEY_S,			"S"},					// 5,  1
			{ALLEGRO_KEY_C,			"C"},					// 5,  2
			{ALLEGRO_KEY_G,			"G"},					// 5,  3
			{ALLEGRO_KEY_H,			"H"},					// 5,  4
			{ALLEGRO_KEY_N,			"N"},					// 5,  5
			{ALLEGRO_KEY_L,			"L"},					// 5,  6
			{ALLEGRO_KEY_RCTRL,		"+;"},					// 5,  7
			{ALLEGRO_KEY_PGDN,		")}"},					// 5,  8
			{ALLEGRO_KEY_BACKSPACE,	"DELETE"}				// 5,  9
		},{
			{ALLEGRO_KEY_TAB,		"TAB"},					// 6,  0
			{ALLEGRO_KEY_Z,			"Z"},					// 6,  1
			{ALLEGRO_KEY_SPACE,		" "},					// 6,  2
			{ALLEGRO_KEY_V,			"V"},					// 6,  3
			{ALLEGRO_KEY_B,			"B"},					// 6,  4
			{ALLEGRO_KEY_M,			"M"},					// 6,  5
			{ALLEGRO_KEY_COMMA,		"<,"},					// 6,  6
			{ALLEGRO_KEY_FULLSTOP,	">."},					// 6,  7
			{ALLEGRO_KEY_END,		"?/"},					// 6,  8
			{ALLEGRO_KEY_HOME,		"COPY"}					// 6,  9
		},{
			{ALLEGRO_KEY_ESCAPE,	"ESC"},					// 7,  0
			{ALLEGRO_KEY_F1,		"F1"},					// 7,  1
			{ALLEGRO_KEY_F2,		"F2"},					// 7,  2
			{ALLEGRO_KEY_F3,		"F3"},					// 7,  3
			{ALLEGRO_KEY_F5,		"F5"},					// 7,  4
			{ALLEGRO_KEY_F6,		"F6"},					// 7,  5
			{ALLEGRO_KEY_F8,		"F8"},					// 7,  6
			{ALLEGRO_KEY_F9,		"F9"},					// 7,  7
			{ALLEGRO_KEY_BACKSLASH,	"\\"},					// 7,  8
			{ALLEGRO_KEY_RIGHT,		"RIGHT ARROW"}			// 7,  9
		}
	};

	Key mBreakKey = { ALLEGRO_KEY_PAUSE, "BREAK" };

	// Ports that can be conALLEGROnected to other devices
	int  CTRL, COL_SEL, ROW_SEL, ROW, BREAK, PRESSED;
	uint8_t mCTRL = 0xf;			// CTRL input; b3: data; b2:b0 selects #3 KB:ENA, #6: LED2_CAPS_LOCK & #7: LED1_SHIFT_LOCK
	uint8_t mKB_ENA = 0x1;			// Not a port but updated based on the CTRL input
	uint8_t pKB_ENA = 0x1;
	uint8_t mLED1_SHIFT_LOCK = 0x0;			//
	uint8_t mLED2_CAPS_LOCK = 0x0;			//
	uint8_t mCOL_SEL = 0x0;			// Column Select: 0-9 BCD-coded;								connected to VIA 6522 (IC3) PA0:3 output
	uint8_t mROW_SEL = 0x0;			// Row Select:  0-9 BCD-coded;									connected to VIA 6522 (IC3) PA4:6 output
	uint8_t pCOL_SEL = 0x1;
	uint8_t pROW_SEL = 0x1;
	uint8_t mROW = 0x0;				// Value of selected row (HIGH if any key on row is pressed);	connected to VIA 6522 (IC3) PA7 input
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

	uint8_t mColCounter = 0;
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

	BeebKeyboard(string name, uint8_t startupOptions, DebugInfo  *debugInfo, ConnectionManager* connectionManager);

	// Reset device
	bool reset();

	//  Advance until clock cycle stopcycle has been reached
	bool advance(uint64_t stopCycle);
};

#endif