#include "KeyboardDevice.h"
#include <iostream>
#include <cstdint>
#include <map>
#include <chrono>
#include "Utility.h"

using namespace std;


KeyboardDevice::KeyboardDevice(string name, DeviceId typ, double cpuClock, DebugTracing* debugTracing, ConnectionManager* connectionManager) :
	Device(name, typ, KEYBOARD_DEVICE, cpuClock, debugTracing, connectionManager)
{
	al_get_keyboard_state(&mKeyboardState);
}

// Set emulation speed
void KeyboardDevice::setEmulationSpeed(double speed)
{
	Device::setEmulationSpeed(speed);
}