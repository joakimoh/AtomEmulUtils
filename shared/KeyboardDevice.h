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
	bool mKeyDown = false;
	Display* mDisplay = nullptr;	

protected:

	typedef struct KeyboardKey_struct {
		char			ASCII;
		vector<string>	keys;
	} KeyboardKey;

	ALLEGRO_KEYBOARD_STATE mKeyboardState;
	uint64_t mMinKeyDownTicks = 0;

	map<int, vector<int>> mASCII2KeyCodesMap; // map from ASCII character to combination of modifier keys and regular keys

	bool keyDown(int keyCode);
	void pollKeyboardState();

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

};

#endif