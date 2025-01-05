#ifndef ATOM_KEYBOARD_H
#define ATOM_KEYBOARD_H
#include <map>
#include <allegro5/allegro.h>
#include "KeyBoard.h"

typedef struct AtomKey_struct {
	int	keyCode;
	int	row;	// 11 rows
	int	col;	// 6 columns
	int	ascii;	// value without modifers used (e.g.,'A' for ALLEGRO_KEY_A and never 'a')
} AtomKey;

const map<int, AtomKey> keycodes={
	{ALLEGRO_KEY_A,				{ALLEGRO_KEY_A,             7,    4,    'A'}},    
	{ALLEGRO_KEY_B,				{ALLEGRO_KEY_B,             6,    3,    'B'}},    
	{ALLEGRO_KEY_C,				{ALLEGRO_KEY_C,             5,    4,    'C'}},    
	{ALLEGRO_KEY_D,				{ALLEGRO_KEY_D,             4,    4,    'D'}},    
	{ALLEGRO_KEY_E,				{ALLEGRO_KEY_E,             3,    4,    'E'}},    
	{ALLEGRO_KEY_F,				{ALLEGRO_KEY_F,             2,    4,    'F'}},    
	{ALLEGRO_KEY_G,				{ALLEGRO_KEY_G,             1,    3,    'G'}},    
	{ALLEGRO_KEY_H,				{ALLEGRO_KEY_H,             11,   5,    'H'}},    
	{ALLEGRO_KEY_I,				{ALLEGRO_KEY_I,             10,   5,    'I'}},    
	{ALLEGRO_KEY_J,				{ALLEGRO_KEY_J,             9,    5,    'J'}},    
	{ALLEGRO_KEY_K,				{ALLEGRO_KEY_K,             7,    5,    'K'}},    
	{ALLEGRO_KEY_L,				{ALLEGRO_KEY_L,             6,    5,    'L'}},    
	{ALLEGRO_KEY_M,				{ALLEGRO_KEY_M,             5,    5,    'M'}},    
	{ALLEGRO_KEY_N,				{ALLEGRO_KEY_N,             4,    5,    'N'}},    
	{ALLEGRO_KEY_O,				{ALLEGRO_KEY_O,             3,    5,    'O'}},    
	{ALLEGRO_KEY_P,				{ALLEGRO_KEY_P,				2,    5,    'P'}},    
	{ALLEGRO_KEY_Q,				{ALLEGRO_KEY_Q,				1,    4,    'Q'}},    
	{ALLEGRO_KEY_R,				{ALLEGRO_KEY_R,				11,   6,    'R'}},    
	{ALLEGRO_KEY_S,				{ALLEGRO_KEY_S,				10,   6,    'S'}},    
	{ALLEGRO_KEY_T,				{ALLEGRO_KEY_T,				9,    6,    'T'}},    
	{ALLEGRO_KEY_U,				{ALLEGRO_KEY_U,				7,    6,    'U'}},    
	{ALLEGRO_KEY_V,				{ALLEGRO_KEY_V,				6,    6,    'V'}},    
	{ALLEGRO_KEY_W,				{ALLEGRO_KEY_W,				0,    0,    'W'}},    
	{ALLEGRO_KEY_X,				{ALLEGRO_KEY_X,				4,    6,    'X'}},    
	{ALLEGRO_KEY_Y,				{ALLEGRO_KEY_Y,				3,    6,    'Y'}},    
	{ALLEGRO_KEY_Z,				{ALLEGRO_KEY_Z,				2,    6,    'Z'}},    
	{ALLEGRO_KEY_0,				{ALLEGRO_KEY_0,				4,    2,    '0'}},    
	{ALLEGRO_KEY_1,				{ALLEGRO_KEY_1,				3,    2,    '1'}},    
	{ALLEGRO_KEY_2,				{ALLEGRO_KEY_2,				2,    2,    '2'}},    
	{ALLEGRO_KEY_3,				{ALLEGRO_KEY_3,				1,    2,    '3'}},    
	{ALLEGRO_KEY_4,				{ALLEGRO_KEY_4,				11,   3,    '4'}},    
	{ALLEGRO_KEY_5,				{ALLEGRO_KEY_5,				10,   3,    '5'}},    
	{ALLEGRO_KEY_6,				{ALLEGRO_KEY_6,				9,    3,    '6'}},    
	{ALLEGRO_KEY_7,				{ALLEGRO_KEY_7,				7,    3,    '7'}},    
	{ALLEGRO_KEY_8,				{ALLEGRO_KEY_8,				6,    3,    '8'}},    
	{ALLEGRO_KEY_9,				{ALLEGRO_KEY_9,				5,    3,    '9'}},    
	{ALLEGRO_KEY_PAD_0,			{ALLEGRO_KEY_PAD_0,			0,    0,    '\x00'}},    
	{ALLEGRO_KEY_PAD_1,			{ALLEGRO_KEY_PAD_1,			0,    0,    '\x00'}},
	{ALLEGRO_KEY_PAD_2,			{ALLEGRO_KEY_PAD_2,			0,    0,    '\x00'}},
	{ALLEGRO_KEY_PAD_3,			{ALLEGRO_KEY_PAD_3,			0,    0,    '\x00'}},
	{ALLEGRO_KEY_PAD_4,			{ALLEGRO_KEY_PAD_4,			0,    0,    '\x00'}},
	{ALLEGRO_KEY_PAD_5,			{ALLEGRO_KEY_PAD_5,			0,    0,    '\x00'}},
	{ALLEGRO_KEY_PAD_6,			{ALLEGRO_KEY_PAD_6,			0,    0,    '\x00'}},
	{ALLEGRO_KEY_PAD_7,			{ALLEGRO_KEY_PAD_7,			0,    0,    '\x00'}},
	{ALLEGRO_KEY_PAD_8,			{ALLEGRO_KEY_PAD_8,			0,    0,    '\x00'}},
	{ALLEGRO_KEY_PAD_9,			{ALLEGRO_KEY_PAD_9,			0,    0,    '\x00'}},
	{ALLEGRO_KEY_F1,			{ALLEGRO_KEY_F1,			0,    0,    '\x00'}},    
	{ALLEGRO_KEY_F2,			{ALLEGRO_KEY_F2,			0,    0,    '\x00'}},    
	{ALLEGRO_KEY_F3,			{ALLEGRO_KEY_F3,			0,    0,    '\x00'}},    
	{ALLEGRO_KEY_F4,			{ALLEGRO_KEY_F4,			0,    0,    '\x00'}},    
	{ALLEGRO_KEY_F5,			{ALLEGRO_KEY_F5,			0,    0,    '\x00'}},    
	{ALLEGRO_KEY_F6,			{ALLEGRO_KEY_F6,			0,    0,    '\x00'}},    
	{ALLEGRO_KEY_F7,			{ALLEGRO_KEY_F7,			0,    0,    '\x00'}},    
	{ALLEGRO_KEY_F8,			{ALLEGRO_KEY_F8,			0,    0,    '\x00'}},    
	{ALLEGRO_KEY_F9,			{ALLEGRO_KEY_F9,			0,    0,    '\x00'}},    
	{ALLEGRO_KEY_F10,			{ALLEGRO_KEY_F10,			0,    0,    '\x00'}},    
	{ALLEGRO_KEY_F11,			{ALLEGRO_KEY_F11,			0,    0,    '\x00'}},    
	{ALLEGRO_KEY_F12,			{ALLEGRO_KEY_F12,			0,    0,    '\x00'}},    
	{ALLEGRO_KEY_ESCAPE,		{ALLEGRO_KEY_ESCAPE,		1,    5,    '\x1b'}},
	{ALLEGRO_KEY_TILDE,			{ALLEGRO_KEY_TILDE,			0,    0,    '~'}},    
	{ALLEGRO_KEY_MINUS,			{ALLEGRO_KEY_MINUS,			1,    2,    '-'}},    
	{ALLEGRO_KEY_EQUALS,		{ALLEGRO_KEY_EQUALS,		0,    0,    '='}},    
	{ALLEGRO_KEY_BACKSPACE,		{ALLEGRO_KEY_BACKSPACE,		9,    1,    '\x08'}},    
	{ALLEGRO_KEY_TAB,			{ALLEGRO_KEY_TAB,			0,    0,    '\x09'}},    
	{ALLEGRO_KEY_OPENBRACE,		{ALLEGRO_KEY_OPENBRACE,		0,    0,    '{'}},    
	{ALLEGRO_KEY_CLOSEBRACE,    {ALLEGRO_KEY_CLOSEBRACE,    0,    0,    '}'}},    
	{ALLEGRO_KEY_ENTER,			{ALLEGRO_KEY_ENTER,			7,    2,    '\x0d'}},    
	{ALLEGRO_KEY_SEMICOLON,		{ALLEGRO_KEY_SEMICOLON,		3,    3,    ';'}},    
	{ALLEGRO_KEY_QUOTE,			{ALLEGRO_KEY_QUOTE,			0,    0,    '\''}},    
	{ALLEGRO_KEY_BACKSLASH,		{ALLEGRO_KEY_BACKSLASH,		0,    0,    '\\'}},    
	{ALLEGRO_KEY_BACKSLASH2,    {ALLEGRO_KEY_BACKSLASH2,    0,    0,    '\x00'}},    
	{ALLEGRO_KEY_COMMA,			{ALLEGRO_KEY_COMMA,			2,    3,    ','}},    
	{ALLEGRO_KEY_FULLSTOP,		{ALLEGRO_KEY_FULLSTOP,		11,   4,    '.'}},    
	{ALLEGRO_KEY_SLASH,			{ALLEGRO_KEY_SLASH,			10,	  4,    '/'}},    
	{ALLEGRO_KEY_SPACE,			{ALLEGRO_KEY_SPACE,			11,   1,    ' '}},    
	{ALLEGRO_KEY_INSERT,		{ALLEGRO_KEY_INSERT,		0,    0,    '\x00'}},    
	{ALLEGRO_KEY_DELETE,		{ALLEGRO_KEY_DELETE,		5,    2,    '\x7f'}},    
	{ALLEGRO_KEY_HOME,			{ALLEGRO_KEY_HOME,			0,    0,    '\x00'}},    
	{ALLEGRO_KEY_END,			{ALLEGRO_KEY_END,			0,    0,    '\x00'}},    
	{ALLEGRO_KEY_PGUP,			{ALLEGRO_KEY_PGUP,			0,    0,    '\x00'}},    
	{ALLEGRO_KEY_PGDN,			{ALLEGRO_KEY_PGDN,			0,    0,    '\x00'}},    
	{ALLEGRO_KEY_LEFT,			{ALLEGRO_KEY_LEFT,			0,    0,    '\x00'}},    
	{ALLEGRO_KEY_RIGHT,			{ALLEGRO_KEY_RIGHT,			4,    1,    '\x00'}},    
	{ALLEGRO_KEY_UP,			{ALLEGRO_KEY_UP,			6,    1,    '\x00'}},    
	{ALLEGRO_KEY_DOWN,			{ALLEGRO_KEY_DOWN,			0,    0,    '\x00'}},    
	{ALLEGRO_KEY_PAD_SLASH,		{ALLEGRO_KEY_PAD_SLASH,		0,    0,    '\x00'}},    
	{ALLEGRO_KEY_PAD_ASTERISK,	{ALLEGRO_KEY_PAD_ASTERISK,	0,    0,    '\x00'}},    
	{ALLEGRO_KEY_PAD_MINUS,		{ALLEGRO_KEY_PAD_MINUS,		0,    0,    '\x00'}},    
	{ALLEGRO_KEY_PAD_PLUS,		{ALLEGRO_KEY_PAD_PLUS,		0,    0,    '\x00'}},    
	{ALLEGRO_KEY_PAD_DELETE,    {ALLEGRO_KEY_PAD_DELETE,    0,    0,    '\x00'}},    
	{ALLEGRO_KEY_PAD_ENTER,		{ALLEGRO_KEY_PAD_ENTER,		0,    0,    '\x00'}},    
	{ALLEGRO_KEY_PRINTSCREEN,	{ALLEGRO_KEY_PRINTSCREEN,   0,    0,    '\x00'}},    
	{ALLEGRO_KEY_PAUSE,			{ALLEGRO_KEY_PAUSE,			0,    0,    '\x00'}},    
	{ALLEGRO_KEY_ABNT_C1,		{ALLEGRO_KEY_ABNT_C1,		0,    0,    '\x00'}},    
	{ALLEGRO_KEY_YEN,			{ALLEGRO_KEY_YEN,			0,    0,    '\x00'}},    
	{ALLEGRO_KEY_KANA,			{ALLEGRO_KEY_KANA,			0,    0,    '\x00'}},    
	{ALLEGRO_KEY_CONVERT,		{ALLEGRO_KEY_CONVERT,		0,    0,    '\x00'}},    
	{ALLEGRO_KEY_NOCONVERT,		{ALLEGRO_KEY_NOCONVERT,		0,    0,    '\x00'}},    
	{ALLEGRO_KEY_AT,			{ALLEGRO_KEY_AT,			0,    0,    '@'}},    
	{ALLEGRO_KEY_CIRCUMFLEX,    {ALLEGRO_KEY_CIRCUMFLEX,    0,    0,    '^'}},    
	{ALLEGRO_KEY_COLON2,		{ALLEGRO_KEY_COLON2,		4,    3,    '\x00'}},    
	{ALLEGRO_KEY_KANJI,			{ALLEGRO_KEY_KANJI,			0,    0,    '\x00'}},    
	{ALLEGRO_KEY_LSHIFT,		{ALLEGRO_KEY_LSHIFT,		0,    0,    '\x00'}},    
	{ALLEGRO_KEY_RSHIFT,		{ALLEGRO_KEY_RSHIFT,		0,    0,    '\x00'}},    
	{ALLEGRO_KEY_LCTRL,			{ALLEGRO_KEY_LCTRL,			0,    0,    '\x00'}},    
	{ALLEGRO_KEY_RCTRL,			{ALLEGRO_KEY_RCTRL,			0,    0,    '\x00'}},    
	{ALLEGRO_KEY_ALT,			{ALLEGRO_KEY_ALT,			0,    0,    '\x00'}},    
	{ALLEGRO_KEY_ALTGR,			{ALLEGRO_KEY_ALTGR,			0,    0,    '\x00'}},    
	{ALLEGRO_KEY_LWIN,			{ALLEGRO_KEY_LWIN,			0,    0,    '\x00'}},    
	{ALLEGRO_KEY_RWIN,			{ALLEGRO_KEY_RWIN,			0,    0,    '\x00'}},    
	{ALLEGRO_KEY_MENU,			{ALLEGRO_KEY_MENU,			0,    0,    '\x00'}},    
	{ALLEGRO_KEY_SCROLLLOCK,    {ALLEGRO_KEY_SCROLLLOCK,    0,    0,    '\x00'}},    
	{ALLEGRO_KEY_NUMLOCK,		{ALLEGRO_KEY_NUMLOCK,		0,    0,    '\x00'}},    
	{ALLEGRO_KEY_CAPSLOCK,		{ALLEGRO_KEY_CAPSLOCK,		5,    1,    '\x00'}},    
	{ALLEGRO_KEY_PAD_EQUALS,    {ALLEGRO_KEY_PAD_EQUALS,    0,    0,    '\x00'}},    
	{ALLEGRO_KEY_BACKQUOTE,		{ALLEGRO_KEY_BACKQUOTE,		0,    0,    '`'}},    
	{ALLEGRO_KEY_SEMICOLON2,    {ALLEGRO_KEY_SEMICOLON2,    0,    0,    '\x00'}},    
	{ALLEGRO_KEY_COMMAND,		{ALLEGRO_KEY_COMMAND,		0,    0,    '\x00'}},    
};

typedef struct AtomModifier_struct {
	int	keyCode;
	bool ctrl;
	bool shift;
	bool repeat;
} AtomModifier;

const map<unsigned, AtomModifier> modifiers = {
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

class AtomKeyBoard : public KeyBoard {
public:
	int selectedRow;
	int readColStatus;
	bool ctrlPressed;
	bool shiftPressed;
	bool repeatPressed;

public:
	void keyAction(int pressedKey);
};

#endif