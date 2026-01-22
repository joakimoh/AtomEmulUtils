#ifndef CONNECTION_MANAGER_H
#define CONNECTION_MANAGER_H

#include <cstdint>
#include <vector>
#include <string>
#include <map>
#include "DebugManager.h"
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


	DebugManager* mDM = NULL;


public:

	Device *getDevice(string name);
	bool extractPort(string name, PortSelection& port);
	void printRouting();
	string printDevicePort(DevicePort* device_port);
	string printPortSelection(PortSelection& port_selection);

	ConnectionManager(DebugManager* debugManager);
	~ConnectionManager();

	void setDeviceManager(DeviceManager* devices);

	// Used by a device to make a port available for routing
	bool addDevicePort(Device* dev, DevicePort* localPort);

	// Connect one device's output with the input of another device
	bool connect(string srcName, string dstName, bool invert, bool process);

};

#endif
