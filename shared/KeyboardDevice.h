#ifndef KEYBOARD_DEVICE_H
#define KEYBOARD_DEVICE_H

#include <vector>
#include <map>
#include "Device.h"
#include <allegro5/allegro5.h>
#include "DebugTracing.h"


class Display;



class KeyboardDevice : public Device {

private:
	bool mPasting = false;
	bool mKeyDown = true;
	Display* mDisplay = nullptr;
	char mPastedChar = 0;
	vector<int> mPastedkeyCodes;

	typedef struct KeyCode2Str_struct {
		int		keyCode;
		string	keyName;
	} KeyCode2Str;

	vector<KeyCode2Str> mKeyCode2StrMap = {
		{ALLEGRO_KEY_LSHIFT, "ALLEGRO_KEY_LSHIFT"},
		{ALLEGRO_KEY_LCTRL, "ALLEGRO_KEY_LCTRL"},
		{ALLEGRO_KEY_Q, "ALLEGRO_KEY_Q"},
		{ALLEGRO_KEY_3, "ALLEGRO_KEY_3"},
		{ALLEGRO_KEY_4, "ALLEGRO_KEY_4"},
		{ALLEGRO_KEY_5, "ALLEGRO_KEY_5"},
		{ALLEGRO_KEY_F4, "ALLEGRO_KEY_F4"},
		{ALLEGRO_KEY_8, "ALLEGRO_KEY_8"},
		{ALLEGRO_KEY_F7, "ALLEGRO_KEY_F7"},
		{ALLEGRO_KEY_MINUS, "ALLEGRO_KEY_MINUS"},
		{ALLEGRO_KEY_TILDE, "ALLEGRO_KEY_TILDE"},
		{ALLEGRO_KEY_LEFT, "ALLEGRO_KEY_LEFT"},
		{ALLEGRO_KEY_W, "ALLEGRO_KEY_W"},
		{ALLEGRO_KEY_E, "ALLEGRO_KEY_E"},
		{ALLEGRO_KEY_T, "ALLEGRO_KEY_T"},
		{ALLEGRO_KEY_7, "ALLEGRO_KEY_7"},
		{ALLEGRO_KEY_I, "ALLEGRO_KEY_I"},
		{ALLEGRO_KEY_9, "ALLEGRO_KEY_9"},
		{ALLEGRO_KEY_0, "ALLEGRO_KEY_0"},
		{ALLEGRO_KEY_ALTGR, "ALLEGRO_KEY_ALTGR"},
		{ALLEGRO_KEY_DOWN, "ALLEGRO_KEY_DOWN"},
		{ALLEGRO_KEY_1, "ALLEGRO_KEY_1"},
		{ALLEGRO_KEY_2, "ALLEGRO_KEY_2"},
		{ALLEGRO_KEY_D, "ALLEGRO_KEY_D"},
		{ALLEGRO_KEY_R, "ALLEGRO_KEY_R"},
		{ALLEGRO_KEY_6, "ALLEGRO_KEY_6"},
		{ALLEGRO_KEY_U, "ALLEGRO_KEY_U"},
		{ALLEGRO_KEY_O, "ALLEGRO_KEY_O"},
		{ALLEGRO_KEY_P, "ALLEGRO_KEY_P"},
		{ALLEGRO_KEY_PGUP, "ALLEGRO_KEY_PGUP"},
		{ALLEGRO_KEY_UP, "ALLEGRO_KEY_UP"},
		{ALLEGRO_KEY_CAPSLOCK, "ALLEGRO_KEY_CAPSLOCK"},
		{ALLEGRO_KEY_A, "ALLEGRO_KEY_A"},
		{ALLEGRO_KEY_X, "ALLEGRO_KEY_X"},
		{ALLEGRO_KEY_F, "ALLEGRO_KEY_F"},
		{ALLEGRO_KEY_Y, "ALLEGRO_KEY_Y"},
		{ALLEGRO_KEY_J, "ALLEGRO_KEY_J"},
		{ALLEGRO_KEY_K, "ALLEGRO_KEY_K"},
		{ALLEGRO_KEY_RSHIFT, "ALLEGRO_KEY_RSHIFT"},
		{ALLEGRO_KEY_DELETE, "ALLEGRO_KEY_DELETE"},
		{ALLEGRO_KEY_ENTER, "ALLEGRO_KEY_ENTER"},
		{ALLEGRO_KEY_ALT, "ALLEGRO_KEY_ALT"},
		{ALLEGRO_KEY_S, "ALLEGRO_KEY_S"},
		{ALLEGRO_KEY_C, "ALLEGRO_KEY_C"},
		{ALLEGRO_KEY_G, "ALLEGRO_KEY_G"},
		{ALLEGRO_KEY_H, "ALLEGRO_KEY_H"},
		{ALLEGRO_KEY_N, "ALLEGRO_KEY_N"},
		{ALLEGRO_KEY_L, "ALLEGRO_KEY_L"},
		{ALLEGRO_KEY_RCTRL, "ALLEGRO_KEY_RCTRL"},
		{ALLEGRO_KEY_PGDN, "ALLEGRO_KEY_PGDN"},
		{ALLEGRO_KEY_BACKSPACE, "ALLEGRO_KEY_BACKSPACE"},
		{ALLEGRO_KEY_TAB, "ALLEGRO_KEY_TAB"},
		{ALLEGRO_KEY_Z, "ALLEGRO_KEY_Z"},
		{ALLEGRO_KEY_SPACE, "ALLEGRO_KEY_SPACE"},
		{ALLEGRO_KEY_V, "ALLEGRO_KEY_V"},
		{ALLEGRO_KEY_B, "ALLEGRO_KEY_B"},
		{ALLEGRO_KEY_M, "ALLEGRO_KEY_M"},
		{ALLEGRO_KEY_COMMA, "ALLEGRO_KEY_COMMA"},
		{ALLEGRO_KEY_FULLSTOP, "ALLEGRO_KEY_FULLSTOP"},
		{ALLEGRO_KEY_END, "ALLEGRO_KEY_END"},
		{ALLEGRO_KEY_HOME, "ALLEGRO_KEY_HOME"},
		{ALLEGRO_KEY_ESCAPE, "ALLEGRO_KEY_ESCAPE"},
		{ALLEGRO_KEY_F1, "ALLEGRO_KEY_F1"},
		{ALLEGRO_KEY_F2, "ALLEGRO_KEY_F2"},
		{ALLEGRO_KEY_F3, "ALLEGRO_KEY_F3"},
		{ALLEGRO_KEY_F5, "ALLEGRO_KEY_F5"},
		{ALLEGRO_KEY_F6, "ALLEGRO_KEY_F6"},
		{ALLEGRO_KEY_F8, "ALLEGRO_KEY_F8"},
		{ALLEGRO_KEY_F9, "ALLEGRO_KEY_F9"},
		{ALLEGRO_KEY_BACKSLASH, "ALLEGRO_KEY_BACKSLASH"},
		{ALLEGRO_KEY_RIGHT, "ALLEGRO_KEY_RIGHT"}
	};

	int mLastPressedKeyCode = 0;

	

protected:

	// This struct is used to set up the mapping from ASCII characters to key codes (including modifiers) for the paste function
	typedef struct KeyPastingInfo_struct {
		char			ASCII;
		vector<string>	keys;
	} KeyPastingInfo;

	// This struct is used to set up the keyboard matrix with key codes and key names
	typedef struct KeyCodeMapping_struct {
		int		keyCode;
		string	keyName; // e.g, "ESC" and "Aa"
	}  KeyCodeMapping;

	ALLEGRO_KEYBOARD_STATE mKeyboardState;
	uint64_t mMinKeyDownTicks = 0;

	map<int, vector<int>> mASCII2KeyCodesMap; // map from ASCII character to combination of modifier keys and regular keys

	//  Check if a key is being pressed - used by the derived class to decouple from the host implementation libray used
	bool keyDown(int keyCode);

	// Poll for key presses - used by the derived class to decouple from the host implementation libray used but also to allow for pasting
	void pollKeyboardState();

	// Must be called from the derived class constructor to set up the key code maps
	bool init(vector<KeyPastingInfo>& pasteKeyMap, vector<vector<KeyCodeMapping>>& keyboardMatrix);

	// Get the key name from the keyboard matrix
	string getKeyName(int keyCode, vector<vector<KeyCodeMapping>>& keyboardMatrix) {
		for (int row = 0; row < keyboardMatrix.size(); row++) {
			for (int col = 0; col < keyboardMatrix[row].size(); col++) {
				if (keyboardMatrix[row][col].keyCode == keyCode) {
					return keyboardMatrix[row][col].keyName;
				}
			}
		}
		return "???";
	}

	// Get the key code name
	string getKeyCodeName(int keyCode)
	{
		for (int i = 0; i < mKeyCode2StrMap.size(); i++) {
			if (mKeyCode2StrMap[i].keyCode == keyCode) {
				return mKeyCode2StrMap[i].keyName;
			}
		}
		return "???";
	}

public:

	KeyboardDevice(string name, DeviceId typ, DebugTracing* debugTracing, ConnectionManager* connectionManager);

	// Set emulation speed
	void setEmulationSpeed(double speed) override;

	// Set the display to which the keyboard is connected. This is needed to be able to chech for clipboard data that is linked to the display
	bool setDisplay(Display* display) { mDisplay = display; return true; }

	// Switch to paste mode in which characters are picked from the clipboard instead of from the keyboard.
	bool startPasting();

	// Check if the minimum key down time has passed (for the paste function)
	virtual bool minKeyDownTimePassed() = 0;

	virtual string getKeyName(int keyCode) = 0;

};

#endif