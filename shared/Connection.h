#ifndef CONNECTION_H
#define CONNECTION_H

#include <string>
#include "Device.h"
#include <vector>

using namespace std;

typedef struct LocalPort_struct {
	string name = "";	// name of the I/O port
	int index = -1;		// local device index for the I/O port ('-1' <=> data bus, >= 0 <=> other port)
} LocalPort;

typedef struct Port_struct {
	Device* dev = NULL;	// name of the device
	LocalPort localPort; // local port info
	uint8_t mask = 0x0;	// specifies the bits of the I/O port to be connected
	uint8_t lowBit = 0;	// specifies the lowest bit if the port I/O to be connected (already identified by the mask but pre-calculated for speed reasons)

} Port;

typedef struct Connector_struct { // specifies the receiving unique part of a routing
	Port srcPort; // specifies the bits of the output port to be connected
	Port dstPort; // specifies the bits of the input port to be connected
} Connection;

typedef struct Routing_struct { // specifies how an output port of one device is connected to the input ports of one or more receving devices
	int index = -1;
	Device* srcDev = NULL;
	string srcPortName = "";
	uint8_t srcPortIndex = 0; // the device's local port index
	vector <Connection> connections;
} Routing;

#endif