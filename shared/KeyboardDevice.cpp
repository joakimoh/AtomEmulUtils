#include "KeyboardDevice.h"
#include <iostream>
#include <cstdint>
#include <map>
#include <chrono>
#include "Utility.h"
#include "Display.h"

using namespace std;


KeyboardDevice::KeyboardDevice(string name, DeviceId typ, DebugTracing* debugTracing, ConnectionManager* connectionManager) :
	Device(name, typ, KEYBOARD_DEVICE, debugTracing, connectionManager)
{	

	pollKeyboardState();
}

// Must be called from the derived class constructor to set up the key code maps
bool KeyboardDevice::init(vector<KeyPastingInfo>& pasteKeyMap, vector<vector<KeyCodeMapping>>& keyboardMatrix)
{
	// Create map from ASCII characters to key codes (including modifiers)
	for (int i = 0; i < pasteKeyMap.size(); i++) {
		KeyPastingInfo& key = pasteKeyMap[i];
		bool found = false;
		vector <string> keys;
		for (int j = 0; j < key.keys.size(); j++) {
			string keyName = key.keys[j];
			for (int row = 0; row < keyboardMatrix.size(); row++) {
				for (int col = 0; col < keyboardMatrix[row].size(); col++) {
					KeyCodeMapping& k = keyboardMatrix[row][col];
					if (k.keyName == keyName) {
						mASCII2KeyCodesMap[key.ASCII].push_back(k.keyCode);
						found = true;
						break;
					}
				}
			}
			if (!found) {
				cout << "Failed to find keycode for key name '" << keyName << "' for ASCII character '" << string(1, key.ASCII) << "'\n";
				throw runtime_error("Error in AtomKeyboardDevice constructor: no keycode found for key name '" + keyName + "' for ASCII character '" + string(1, key.ASCII) + "'");
			}
			keys.push_back(keyName);
		}
		/*
		cout << "Mapped ASCII character '" << key.ASCII << "' to key codes ";
		for (int i = 0; i < mASCII2KeyCodesMap[key.ASCII].size(); i++) {
			cout << mASCII2KeyCodesMap[key.ASCII][i] << " ";
		}
		cout << " <=> ";
		for (int i = 0; i < keys.size(); i++) {
			cout << keys[i] << " ";
		}
		cout << "\n";
		*/
	}

	return true;
}


// Set emulation speed
void KeyboardDevice::setEmulationSpeed(double speed)
{
	Device::setEmulationSpeed(speed);
}


// Switch to paste mode in which characters are picked from the clipboard instead of from the keyboard.
bool KeyboardDevice::startPasting() {

	if (mDisplay == nullptr)
		return false;

	mPasting = true;
	mKeyDown = true;
	return true;
}

// 
// Check if a certain key is being pressed
//
// In pasting mode, the given key code is instead compared with the key codes for the current pasted character.
//
bool KeyboardDevice::keyDown(int keyCode)
{
	bool pressed = false;

	if (mPasting) {

		// Pasting mode

		// Get a new pasted character if the minimum key down time has passed.
		if (minKeyDownTimePassed()) {

			// Toggle between a pasted character and no character to emulate a key being pressed and then realeased.
			if (mKeyDown) {
				if (mDisplay->nextClipboardChar(mPastedChar)) {
					mPastedkeyCodes.clear();
					mPastedkeyCodes = mASCII2KeyCodesMap[mPastedChar];
				}
				else {
					// No more characters to paste => stop pasting
					mPasting = false;
				}
			} else {
				mPastedkeyCodes.clear();
				mPastedChar = 0;
			}

			mKeyDown = !mKeyDown;
		}

		// Compare the pasted character's key codes with the given key code
		for (int i = 0; i < mPastedkeyCodes.size(); i++) {
			if (mPastedkeyCodes[i] == keyCode) {
				pressed = true;
				break;
			}
		}

	}

	else

		// Not in pasting mode => check the keyboard state for the given key code
		pressed = al_key_down(&mKeyboardState, keyCode);

	if (pressed) {
		if (keyCode != mLastPressedKeyCode) {
			mLastPressedKeyCode = keyCode;
			//cout << getKeyCodeName(keyCode) << " (0X" << setw(2) << setfill('0') << hex << (int)keyCode << ")" << " <=> " << getKeyName(keyCode) << "\n";
		}
	}		

	return pressed;
}


void KeyboardDevice::pollKeyboardState()
{
	al_get_keyboard_state(&mKeyboardState);
}