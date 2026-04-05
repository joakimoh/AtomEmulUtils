#ifndef CONNECTION_MANAGER_H
#define CONNECTION_MANAGER_H

#include <cstdint>
#include <vector>
#include <string>
#include <map>
#include "DebugTracing.h"
#include <allegro5/allegro_primitives.h>
#include <iostream>


using namespace std;

// Forward references to classed defined later on to allow them being referenced before their the are declared
class Device;
class DeviceManager;
class PortSelection;

class ConnectionManager {

private:

	DeviceManager* mDevices = NULL;
	map<Device*, map<int, DevicePort*>>	mDevicePorts;	// device port to global index mapping
	int									mDevicePortIndex = 0;

	map<Device*, map<int, AnaloguePort*>>	mAnaloguePorts;	// analogue port to global index mapping
	int										mAnaloguePortIndex = 0;

	DebugTracing* mDM = NULL;


public:

	Device *getDevice(string name);
	bool extractPort(string name, PortSelection& port);

	bool extractAnaloguePort(string name, AnaloguePort * &port);

	void printRouting();
	string printDevicePort(DevicePort* device_port);
	string printPortSelection(PortSelection& port_selection);

	string printAnaloguePort(AnaloguePort* device_port);

	ConnectionManager(DebugTracing* debugTracing);
	~ConnectionManager();

	void setDeviceManager(DeviceManager* devices);

	// Used by a device to make a port available for routing
	bool addDevicePort(Device* dev, DevicePort* localPort);

	// Used by a device to make an analogue port available externally (e.g., to the debugger)
	bool addAnaloguePort(Device* dev, AnaloguePort* localPort);

	// Connect one device's output with the input of another device
	bool connect(string srcName, string dstName, bool invert, bool process);

};

#endif
