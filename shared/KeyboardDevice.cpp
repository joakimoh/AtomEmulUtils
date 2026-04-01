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
	cout << "PASTING!\n";
	return true;
}


bool KeyboardDevice::keyDown(int keyCode)
{
	if (mPasting) {
		char c;
		if (minKeyDownTimePassed()) {
			if (!mKeyDown || mDisplay->nextClipboardChar(c)) {
				mKeyDown = !mKeyDown;
				if (!mKeyDown && mASCII2KeyCodesMap.find(c) != mASCII2KeyCodesMap.end()) {
					vector<int>& keyCodes = mASCII2KeyCodesMap[c];
					if (keyCodes.size() == 1) {
						return keyCodes[0] == keyCode;
					}
					else {
						for (int i = 0; i < keyCodes.size(); i++) {
							if (keyCodes[i] == keyCode)
								return true;
						}
					}
					return false;
				}
				return false;
			}
		mPasting = false;
		cout << "DONE PASTING!\n";
		return false;
	}
	}
	return al_key_down(&mKeyboardState, keyCode);
}


void KeyboardDevice::pollKeyboardState()
{
	al_get_keyboard_state(&mKeyboardState);
}