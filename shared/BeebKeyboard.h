#ifndef BEEB_KEYBOARD_H
#define BEEB_KEYBOARD_H

#include <vector>
#include <map>
#include "Device.h"
#include "ClockedDevice.h"
#include <allegro5/allegro5.h>
#include "DebugTracing.h"
#include "KeyboardDevice.h"

//
// 
// Emulates a BBC Nicro Keyboard.
// 
// The emulation uses the interface tke Keyboard exposes towards the VIA 6522 circuit (and the RESET line).
// 
// SCHEDULING
// 
// The Keyboard device can be schedulated at low rate (typically 50 Hz)
// using the SCHED directive
// 
// 
// PORTS
// 
// The Keyboard presents a port interface consisting of five input ports (ENA, ROW_SEL and COL_SEL) and
// three output ports (BREAK, ROW and PRESSED).
//
// 
// PORT CONNECTIONS
// 
// The COL_SEL and ROW_SEL input ports shall be connected to using the directive "CONNECT:P"
// to ensure that the device is triggered immediately on changes to the these ports. The ENA input
// can be connected with just "CONNECT" as changes to it don't require immediate action. The ROW and PRESSED
// outputs shall be connceted to a device that can read these ports (typically a VIA 6522) whereas
// the BREAK output usually is connected to the RESET input pin of all devices that can be reset
// (including the microprocessor).
// 
// 
// FUNCTIONAL DESCRIPTION
// 
// The Keyboard has two modes of operation:
// 
// 1) No scan - Key at a certain row and column is explictly selected and the ROW output will give its value (LOW if pressed)
// 2) Auto scan - the columns are scanned repeatedly and the PRESSED output will go high if any key is pressed
//    (except for the keys and switches on row one)
// 
//
// 
// KEYBOARD MATRIX
//      
//      COLUMN
// ROW  0          1          2          3          4          5          6          7          8          9
// ===  ========= =========== ========== ========== ========== ========== ========== ========== ========== ===========
// 7	ESC        F1         F2         F3         F5         F6         F8         F9         \|         RIGHT ARROW
// 6    TAB        Z         SPACE       V          B          M          <,         >.         ?/         COPY
// 5    SHIFT LOCK S          C          G          H          N          L          +;         }]         DELETE
// 4    CAPS LOCK  A          X          F          Y          J          K          @          #:         RETURN
// 3    1          2          D          R          6          U          O          P          {[         UP ARROW
// 2    F0         W          E          T          7          1          9          0          -£         DOWN ARROW
// 1    Q          3          4          5          F4         8          F7         =-         ~^         LEFT ARROW
// 0    SHIFT      CTRL       Link 1     Link 2     Link 3     Link 4     Link 5     Link 6     Link 7     Link 8     
//



typedef struct Key_struct {
	int		keyCode;
	string	keyName; // e.g, "COPY", "ESC" and "A"
}  Key;

class BeebKeyboard : public KeyboardDevice, public ClockedDevice {

private:

	int mKeyboardRefreshCycles = 1;

	vector<KeyboardKey> mPasteKeyMap = {

		{' ', {"SPACE"}},
		{'\r', {"RETURN"}},
		{'\t', {"TAB"}},

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

		{'^', {"^~"}},
		{'~', {"^~", "SHIFT"}},

		{'\\', {"\\|"}},
		{'|', {"\\|", "SHIFT"}},

		{'[', {"[{"}},
		{'{', {"[{", "SHIFT"}},
		{']', {"]}"} },
		{'}', {"]}", "SHIFT"} },

		{'£', {"£_"} },
		{'_', {"£_", "SHIFT"} },

		{'/', {"/?"}},
		{'?', {"/?", "SHIFT"} }

	};

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
			{ALLEGRO_KEY_Q,			"Qq"},					// 1,  0
			{ALLEGRO_KEY_3,			"3#"},					// 1,  1
			{ALLEGRO_KEY_4,			"4$"},					// 1,  2
			{ALLEGRO_KEY_5,			"5%"},					// 1,  3
			{ALLEGRO_KEY_F4,		"F4"},					// 1,  4
			{ALLEGRO_KEY_8,			"8("},					// 1,  5
			{ALLEGRO_KEY_F7,		"F7"},					// 1,  6
			{ALLEGRO_KEY_MINUS,		"-="},					// 1,  7
			{ALLEGRO_KEY_TILDE,		"^~"},					// 1,  8
			{ALLEGRO_KEY_LEFT,		"LEFT ARROW"}			// 1,  9
		},
		{
			{-1,				    "F0"},					// 2,  0	-  Not supported!
			{ALLEGRO_KEY_W,			"Ww"},					// 2,  1
			{ALLEGRO_KEY_E,			"Ee"},					// 2,  2
			{ALLEGRO_KEY_T,			"Tt"},					// 2,  3
			{ALLEGRO_KEY_7,			"7'"},					// 2,  4
			{ALLEGRO_KEY_I,			"Ii"},					// 2,  5
			{ALLEGRO_KEY_9,			"9)"},					// 2,  6
			{ALLEGRO_KEY_0,			"0"},					// 2,  7
			{ALLEGRO_KEY_ALTGR,		"£_"}	,				// 2,  8
			{ALLEGRO_KEY_DOWN,		"DOWN ARROW"}			// 2,  9
		},{
			{ALLEGRO_KEY_1,			"1!"},					// 3,  0
			{ALLEGRO_KEY_2,			"2\""},					// 3,  1
			{ALLEGRO_KEY_D,			"Dd"},					// 3,  2
			{ALLEGRO_KEY_R,			"Rr"},					// 3,  3
			{ALLEGRO_KEY_6,			"6&"},					// 3,  4
			{ALLEGRO_KEY_U,			"Uu"},					// 3,  5
			{ALLEGRO_KEY_O,			"Oo"},					// 3,  6
			{ALLEGRO_KEY_P,			"Pp"},					// 3,  7
			{ALLEGRO_KEY_PGUP,		"[{"},					// 3,  8
			{ALLEGRO_KEY_UP,		"UP ARROW"}				// 3,  9
		},
		{
			{ALLEGRO_KEY_CAPSLOCK,  "CAPS LOCK"},			// 4,  0
			{ALLEGRO_KEY_A,			"Aa"},					// 4,  1
			{ALLEGRO_KEY_X,			"Xx"},					// 4,  2
			{ALLEGRO_KEY_F,			"Ff"},					// 4,  3
			{ALLEGRO_KEY_Y,			"Yy"},					// 4,  4
			{ALLEGRO_KEY_J,			"Jj"},					// 4,  5
			{ALLEGRO_KEY_K,			"Kk"},					// 4,  6
			{ALLEGRO_KEY_RSHIFT,	"@"},					// 4,  7
			{ALLEGRO_KEY_DELETE,	":*"},					// 4,  8
			{ALLEGRO_KEY_ENTER,		"RETURN"}				// 4,  9
		},
		{
			{ALLEGRO_KEY_ALT,		"SHIFT LOCK"},			// 5,  0
			{ALLEGRO_KEY_S,			"Ss"},					// 5,  1
			{ALLEGRO_KEY_C,			"Cc"},					// 5,  2
			{ALLEGRO_KEY_G,			"Gg"},					// 5,  3
			{ALLEGRO_KEY_H,			"Hh"},					// 5,  4
			{ALLEGRO_KEY_N,			"Nn"},					// 5,  5
			{ALLEGRO_KEY_L,			"Ll"},					// 5,  6
			{ALLEGRO_KEY_RCTRL,		";+"},					// 5,  7
			{ALLEGRO_KEY_PGDN,		"]}"},					// 5,  8
			{ALLEGRO_KEY_BACKSPACE,	"DELETE"}				// 5,  9
		},{
			{ALLEGRO_KEY_TAB,		"TAB"},					// 6,  0
			{ALLEGRO_KEY_Z,			"Zz"},					// 6,  1
			{ALLEGRO_KEY_SPACE,		"SPACE"},				// 6,  2
			{ALLEGRO_KEY_V,			"Vv"},					// 6,  3
			{ALLEGRO_KEY_B,			"Bb"},					// 6,  4
			{ALLEGRO_KEY_M,			"Mm"},					// 6,  5
			{ALLEGRO_KEY_COMMA,		",<"},					// 6,  6
			{ALLEGRO_KEY_FULLSTOP,	".>"},					// 6,  7
			{ALLEGRO_KEY_END,		"/?"},					// 6,  8
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
			{ALLEGRO_KEY_BACKSLASH,	"\\|"},					// 7,  8
			{ALLEGRO_KEY_RIGHT,		"RIGHT ARROW"}			// 7,  9
		}
	};

	Key mBreakKey = { ALLEGRO_KEY_PAUSE, "BREAK" };

	// Ports that can be connected to other devices
	int  SW, ENA, COL_SEL, ROW_SEL, ROW, BREAK, PRESSED;
	PortVal mENA = 0x0;
	PortVal pENA = 0x0;
	PortVal mCOL_SEL = 0x0;			// Column Select: 0-9 BCD-coded;								connected to VIA 6522 (IC3) PA0:3 output
	PortVal mROW_SEL = 0x0;			// Row Select:  0-9 BCD-coded;									connected to VIA 6522 (IC3) PA4:6 output
	PortVal pCOL_SEL = 0x1;
	PortVal pROW_SEL = 0x1;
	PortVal mROW = 0x0;				// Value of selected row (HIGH if any key on row is pressed);	connected to VIA 6522 (IC3) PA7 input
	PortVal mBREAK = 0x1;			// BREAK key (LOW when pressed <=> RESET)
	PortVal mPRESSED = 0x0;			// HIGH when any key is pressed;								connected to VIA 6522 (IC3) CA2 input

	// Keyboard DIP swithes (link 1 to 8)
	// Mode 7 is set by having Links 6-8 OFF (LOW)
	//
	PortVal mSW =
		//												column		bit		LOW (OFF)					HIGH (ON)
		(0 << 7) ||	// Link 1 Default filing system		2			b7		DFS							NFS
		(0 << 6) ||	// Link 2 Not used					3			b6
		(0 << 5) ||	// Link 3 Disc drive timings		4			b5
		(0 << 4) ||	// Link 4 Disc drive timings		5			b4
		(0 << 3) ||	// Link 5 Boot						6			b3		SHIFT - BREAK to boot		BREAK to boot
		(0 << 2) ||	// Link 6 Screen mode				7			b2		+4							+0
		(0 << 1) || // Link 7 Screen mode				8			b1		+2							+0
		(0 << 0);	// Link 8 Screen mode				9			b0		+1							+0

	inline bool linkSet(uint8_t col) { return col >= 2 && col <= 9 && (mSW & (1 << (7-(col -2)))) != 0;}

	uint8_t mColCounter = 0;

	//
	// Auto scan keyboard
	// 
	// Iterates over all columns and sets the PRESSED output if any key was pressed.
	//
	bool autoScan();

	// Scans the next column
	bool scanNextColumn();


	// Scan a whole column and set the PRESSED output if any key was pressed in the column
	bool scanColumn(int col);

public:

	BeebKeyboard(string name, double tickRate, double deviceClockRate, uint8_t startupOptions, DebugTracing  *debugTracing, ConnectionManager* connectionManager);

	// Device power on
	bool power() { mTicks = 0; return true; }

	//  Advance until time tickTime
	bool advanceUntil(uint64_t tickTime) override;

	void processPortUpdate(int index);

	// Outputs the internal state of the device
	bool outputState(ostream& sout) override;

	// Set emulation speed
	void setEmulationSpeed(double speed) override;

	// Check if the minimum key down time has passed (for the paste function)
	bool minKeyDownTimePassed() override;

	// Get current device time in emulation ticks
	uint64_t getTicks() { return mTicks; }
};

#endif
