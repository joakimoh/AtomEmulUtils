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
	return true;
}


bool KeyboardDevice::keyDown(int keyCode)
{

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
			if (mPastedkeyCodes[i] == keyCode)
				return true;
		}

		// No mtaching key code found for the pasted character
		return false;
	}

	// Not in pasting mode => check the keyboard state for the given key code
	return al_key_down(&mKeyboardState, keyCode);
}


void KeyboardDevice::pollKeyboardState()
{
	al_get_keyboard_state(&mKeyboardState);
}