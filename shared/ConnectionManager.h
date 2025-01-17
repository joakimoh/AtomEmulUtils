#ifndef CONNECTION_MANAGER_H
#define CONNECTION_MANAGER_H

#include "Connection.h"
#include <map>
#include <cstdint>
#include <string>

using namespace std;

class Devices;

class ConnectionManager {

public:


	// Routing examples:
	// 
	// CONNECT	VDU:FS			PIA:PortC;7			- Connect VDU FS output to PIA Port C b7			
	// CONNECT	PIA:PortA;7		VDU:A/G				- Connect PIA Port A b7:4 to VDU A/G input and GM0:2 inputs
	// CONNECT	PIA:PortA;6;4	VDU:GM
	// CONNECT	PIA:_;7			VDU:INV				- Connect data bus b7:6 to VDU inputs INV,A/S & INT/EXT
	// CONNECT	PIA:_;6			VDU:A/S
	// CONNECT	PIA:_;6			VUD:INT/EXT

private:

	map<int, vector<Routing>> mRouting; // each output will have a device-independent unique index which is used to lookup the routing
	Devices* mDevices;

	int mIndex = 0;

	bool getRoutingIndex(Port port, Routing &routing);

	bool extractPort(string name, Port &port);

public:

	ConnectionManager(Devices* devices);

	// Based on a the device-independent unique index of a device's output, propagate the update to all connected devices
	bool receiveUpdate(int index, uint8_t val);

	// Connect one device's output with the input of another device and return the unqiue index for this output
	bool connect(string srcName, string dstName, int& index);
};


#endif

