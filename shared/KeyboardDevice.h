#ifndef KEYBOARD_DEVICE_H
#define KEYBOARD_DEVICE_H

#include <vector>
#include <map>
#include "Device.h"
#include <allegro5/allegro5.h>
#include "DebugManager.h"




class KeyboardDevice : public Device {


protected:
	ALLEGRO_KEYBOARD_STATE mKeyboardState;

public:

	KeyboardDevice(string name, DeviceId typ, double cpuClock, DebugManager* debugManager, ConnectionManager* connectionManager);

	// Set emulation speed
	void setEmulationSpeed(double speed) override;

};

#endif