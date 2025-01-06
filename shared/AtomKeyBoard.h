#ifndef ATOM_KEYBOARD_H
#define ATOM_KEYBOARD_H
#include <map>
#include <allegro5/allegro.h>
#include "Keyboard.h"
#include <string>

using namespace std;

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

class AtomKeyboard : public Keyboard {

private:

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
		{ALLEGRO_KEY_COLON2,		{ALLEGRO_KEY_COLON2,		3,		2,	":"}},		// :*
		{ALLEGRO_KEY_TILDE,			{ALLEGRO_KEY_TILDE,			5,		0,	"^"}},		// ^~
		{ALLEGRO_KEY_PAUSE,			{ALLEGRO_KEY_PAUSE,			-1,    -1,  "BREAK"}},	// BREAK (not mapped to rows/cols)


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
		{ALLEGRO_KEY_DELETE,		{ALLEGRO_KEY_DELETE,		4,		1,	"DEL"}},	// DEL

		// Third row UP_DOWN_ARROW CTRL A S D F G H J K L ;+ [ ] RET

		// UP/DOWN arrow
		{ALLEGRO_KEY_UP,			{ALLEGRO_KEY_UP,			5,		0,	"VTARROW"}}, // UP/DOWN ARROW
		{ALLEGRO_KEY_LCTRL,			{ALLEGRO_KEY_LCTRL,			-1,		-1,	"CTRL"}},	// CTRL (PB6)
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
		{ALLEGRO_KEY_LSHIFT,		{ALLEGRO_KEY_LSHIFT,		-1,    -1,	"SHIFT"}},	// SHIFT (PB7)
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
		{ALLEGRO_KEY_RSHIFT,		{ALLEGRO_KEY_RSHIFT,		-1,    -1,	"SHIFT"}},	// SHIFT (PB7)
		{ALLEGRO_KEY_RIGHT,			{ALLEGRO_KEY_RIGHT,			-1,    -1,	"REPEAT"}},	// REPEAT (PC6)

		// Fifth row
		{ALLEGRO_KEY_SPACE,			{ALLEGRO_KEY_SPACE,			9,   0,    " "}},

		// Unmapped keys
		{ALLEGRO_KEY_BACKSPACE,		{ALLEGRO_KEY_BACKSPACE,		-1,    -1,    ""}},
		{ALLEGRO_KEY_SEMICOLON,		{ALLEGRO_KEY_SEMICOLON,		-1,    -1,    ""}},
		{ALLEGRO_KEY_SLASH,			{ALLEGRO_KEY_SLASH,			-1,	   -1,    ""}},
		{ALLEGRO_KEY_PAD_0,			{ALLEGRO_KEY_PAD_0,			-1,    -1,    ""}},
		{ALLEGRO_KEY_PAD_1,			{ALLEGRO_KEY_PAD_1,			-1,    -1,    ""}},
		{ALLEGRO_KEY_PAD_2,			{ALLEGRO_KEY_PAD_2,			-1,    -1,    ""}},
		{ALLEGRO_KEY_PAD_3,			{ALLEGRO_KEY_PAD_3,			-1,    -1,    ""}},
		{ALLEGRO_KEY_PAD_4,			{ALLEGRO_KEY_PAD_4,			-1,    -1,    ""}},
		{ALLEGRO_KEY_PAD_5,			{ALLEGRO_KEY_PAD_5,			-1,    -1,    ""}},
		{ALLEGRO_KEY_PAD_6,			{ALLEGRO_KEY_PAD_6,			-1,    -1,    ""}},
		{ALLEGRO_KEY_PAD_7,			{ALLEGRO_KEY_PAD_7,			-1,    -1,    ""}},
		{ALLEGRO_KEY_PAD_8,			{ALLEGRO_KEY_PAD_8,			-1,    -1,    ""}},
		{ALLEGRO_KEY_PAD_9,			{ALLEGRO_KEY_PAD_9,			-1,    -1,    ""}},
		{ALLEGRO_KEY_F1,			{ALLEGRO_KEY_F1,			-1,    -1,    ""}},
		{ALLEGRO_KEY_F2,			{ALLEGRO_KEY_F2,			-1,    -1,    ""}},
		{ALLEGRO_KEY_F3,			{ALLEGRO_KEY_F3,			-1,    -1,    ""}},
		{ALLEGRO_KEY_F4,			{ALLEGRO_KEY_F4,			-1,    -1,    ""}},
		{ALLEGRO_KEY_F5,			{ALLEGRO_KEY_F5,			-1,    -1,    ""}},
		{ALLEGRO_KEY_F6,			{ALLEGRO_KEY_F6,			-1,    -1,    ""}},
		{ALLEGRO_KEY_F7,			{ALLEGRO_KEY_F7,			-1,    -1,    ""}},
		{ALLEGRO_KEY_F8,			{ALLEGRO_KEY_F8,			-1,    -1,    ""}},
		{ALLEGRO_KEY_F9,			{ALLEGRO_KEY_F9,			-1,    -1,    ""}},
		{ALLEGRO_KEY_F10,			{ALLEGRO_KEY_F10,			-1,    -1,    ""}},
		{ALLEGRO_KEY_F11,			{ALLEGRO_KEY_F11,			-1,    -1,    ""}},
		{ALLEGRO_KEY_F12,			{ALLEGRO_KEY_F12,			-1,    -1,    ""}},
		{ALLEGRO_KEY_EQUALS,		{ALLEGRO_KEY_EQUALS,		-1,    -1,    ""}},
		{ALLEGRO_KEY_OPENBRACE,		{ALLEGRO_KEY_OPENBRACE,		-1,    -1,    ""}},
		{ALLEGRO_KEY_CLOSEBRACE,    {ALLEGRO_KEY_CLOSEBRACE,    -1,    -1,    ""}},
		{ALLEGRO_KEY_QUOTE,			{ALLEGRO_KEY_QUOTE,			-1,    -1,    ""}},
		{ALLEGRO_KEY_BACKSLASH2,    {ALLEGRO_KEY_BACKSLASH2,    -1,    -1,    ""}},
		{ALLEGRO_KEY_INSERT,		{ALLEGRO_KEY_INSERT,		-1,    -1,    ""}},
		{ALLEGRO_KEY_PAD_SLASH,		{ALLEGRO_KEY_PAD_SLASH,		-1,    -1,    ""}},
		{ALLEGRO_KEY_PAD_ASTERISK,	{ALLEGRO_KEY_PAD_ASTERISK,	-1,    -1,    ""}},
		{ALLEGRO_KEY_PAD_MINUS,		{ALLEGRO_KEY_PAD_MINUS,		-1,    -1,    ""}},
		{ALLEGRO_KEY_PAD_PLUS,		{ALLEGRO_KEY_PAD_PLUS,		-1,    -1,    ""}},
		{ALLEGRO_KEY_PAD_DELETE,    {ALLEGRO_KEY_PAD_DELETE,    -1,    -1,    ""}},
		{ALLEGRO_KEY_PAD_ENTER,		{ALLEGRO_KEY_PAD_ENTER,		-1,    -1,    ""}},
		{ALLEGRO_KEY_PRINTSCREEN,	{ALLEGRO_KEY_PRINTSCREEN,   -1,    -1,    ""}},
		{ALLEGRO_KEY_ABNT_C1,		{ALLEGRO_KEY_ABNT_C1,		-1,    -1,    ""}},
		{ALLEGRO_KEY_YEN,			{ALLEGRO_KEY_YEN,			-1,    -1,    ""}},
		{ALLEGRO_KEY_KANA,			{ALLEGRO_KEY_KANA,			-1,    -1,    ""}},
		{ALLEGRO_KEY_CONVERT,		{ALLEGRO_KEY_CONVERT,		-1,    -1,    ""}},
		{ALLEGRO_KEY_NOCONVERT,		{ALLEGRO_KEY_NOCONVERT,		-1,    -1,    ""}},
		{ALLEGRO_KEY_AT,			{ALLEGRO_KEY_AT,			-1,    -1,    ""}},
		{ALLEGRO_KEY_KANJI,			{ALLEGRO_KEY_KANJI,			-1,    -1,    ""}},
		{ALLEGRO_KEY_RCTRL,			{ALLEGRO_KEY_RCTRL,			-1,    -1,    ""}},
		{ALLEGRO_KEY_ALT,			{ALLEGRO_KEY_ALT,			-1,    -1,    ""}},
		{ALLEGRO_KEY_ALTGR,			{ALLEGRO_KEY_ALTGR,			-1,    -1,    ""}},
		{ALLEGRO_KEY_LWIN,			{ALLEGRO_KEY_LWIN,			-1,    -1,    ""}},
		{ALLEGRO_KEY_RWIN,			{ALLEGRO_KEY_RWIN,			-1,    -1,    ""}},
		{ALLEGRO_KEY_MENU,			{ALLEGRO_KEY_MENU,			-1,    -1,    ""}},
		{ALLEGRO_KEY_SCROLLLOCK,    {ALLEGRO_KEY_SCROLLLOCK,    -1,    -1,    ""}},
		{ALLEGRO_KEY_NUMLOCK,		{ALLEGRO_KEY_NUMLOCK,		-1,    -1,    ""}},
		{ALLEGRO_KEY_PAD_EQUALS,    {ALLEGRO_KEY_PAD_EQUALS,    -1,    -1,    ""}},
		{ALLEGRO_KEY_BACKQUOTE,		{ALLEGRO_KEY_BACKQUOTE,		-1,    -1,    ""}},
		{ALLEGRO_KEY_SEMICOLON2,    {ALLEGRO_KEY_SEMICOLON2,    -1,    -1,    ""}},
		{ALLEGRO_KEY_COMMAND,		{ALLEGRO_KEY_COMMAND,		-1,    -1,    ""}},
	};

	map<unsigned, AtomModifier> mModifiers = {
	{ALLEGRO_KEYMOD_SHIFT,		{ALLEGRO_KEYMOD_SHIFT,		false, true, false}},
	{ALLEGRO_KEYMOD_CTRL,		{ALLEGRO_KEYMOD_CTRL,		true, false, false}},
	{ALLEGRO_KEYMOD_ALT,		{ALLEGRO_KEYMOD_ALT,		false, false, false}},
	{ALLEGRO_KEYMOD_LWIN,		{ALLEGRO_KEYMOD_LWIN,		false, false, false}},
	{ALLEGRO_KEYMOD_RWIN,		{ALLEGRO_KEYMOD_RWIN,		false, false, false}},
	{ALLEGRO_KEYMOD_MENU,		{ALLEGRO_KEYMOD_MENU,		false, false, false}},
	{ALLEGRO_KEYMOD_ALTGR,		{ALLEGRO_KEYMOD_ALTGR,		false, false, false}},
	{ALLEGRO_KEYMOD_COMMAND,	{ALLEGRO_KEYMOD_COMMAND,	false, false, false}},
	{ALLEGRO_KEYMOD_SCROLLLOCK,	{ALLEGRO_KEYMOD_SCROLLLOCK,	false, false, false}},
	{ALLEGRO_KEYMOD_NUMLOCK,	{ALLEGRO_KEYMOD_NUMLOCK,	false, false, false}},
	{ALLEGRO_KEYMOD_CAPSLOCK,	{ALLEGRO_KEYMOD_CAPSLOCK,	false, false, false}},
	{ALLEGRO_KEYMOD_INALTSEQ,	{ALLEGRO_KEYMOD_INALTSEQ,	false, false, false}},
	{ALLEGRO_KEYMOD_ACCENT1,	{ALLEGRO_KEYMOD_ACCENT1,	false, false, false}},
	{ALLEGRO_KEYMOD_ACCENT2,	{ALLEGRO_KEYMOD_ACCENT2,	false, false, false}},
	{ALLEGRO_KEYMOD_ACCENT3,	{ALLEGRO_KEYMOD_ACCENT3,	false, false, false}},
	{ALLEGRO_KEYMOD_ACCENT4,	{ALLEGRO_KEYMOD_ACCENT4,	false, false, false}}
	};

public:
	int PA0_03_selectedRow_keys[10][6] = {0}; // selected row (PA0:3) and key count for each related column PB0:5
	int PB6_ctrl = 0; // ctrl key status
	int PB7_shift = 0; // shift keys status
	int PC6_repeat = 0; // repeat key status

public:
	void keyDown(int key, unsigned modifiers);
	void keyUp(int key);
};

#endif