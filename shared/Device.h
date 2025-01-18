#ifndef DEVICE_H
#define DEVICE_H

#include <cstdint>
#include <vector>
#include <string>
#include <map>
#include "DebugInfo.h"
#include "Keyboard.h"


using namespace std;

// Forward references to classed defined later on to allow them being referenced before their the are declared
class ConnectionManager; 
class Device;
class Devices;

enum DeviceEnum {
	ROM_DEV, RAM_DEV, PIA8255_DEV, VDU6847_DEV, VIA6522_DEV, UNDEFINED_DEV
};
#define _DEVICE_TYPE(x) (\
	x==ROM_DEV?"ROM":(x==RAM_DEV?"RAM":(x==PIA8255_DEV?"PIA 8255":(x==VDU6847_DEV?"VDU 6847":(x==VIA6522_DEV?"VIA 6522":"???")))))

typedef struct DeviceAllocation_struct {
	DeviceEnum deviceType;
	uint16_t startAdr;
	uint16_t size;
	std::string ROMFileName;
	uint16_t videoMemAdr; // only for VDU6847
} DeviceAllocation;

typedef struct Program_struct {
	string fileName = "";
	int loadAdr = -1;
} Program;

typedef struct LocalPort_struct {
	string name = "";			// name of the I/O port
	int localIndex = -1;		// local device index for the I/O port ('-1' <=> data bus, >= 0 <=> other port)		
} LocalPort; 

typedef struct DevicePort_struct {
	LocalPort	port;	// port identity
	uint8_t		*val;	// pointer to variable holding the port's value
} DevicePort;

typedef struct UniquePort_struct {
	Device* dev = NULL;	// name of the device
	LocalPort localPort; // local port info
	int globalIndex = -1; // unique global index for the port
} UniquePort;

typedef struct BitsSelection_struct {
	uint8_t mask = 0x0;	// specifies the bits of the I/O port to be connected
	uint8_t lowBit = 0;	// specifies the lowest bit if the port I/O to be connected (already identified by the mask but pre-calculated for speed reasons)
} BitsSelection;

typedef struct PortBitsSel_struct {
	UniquePort port;	// port identity
	BitsSelection bits;	// bits selection
} PortSelection;

typedef struct Connector_struct { // specifies the receiving unique part of a routing
	BitsSelection srcBits; // specifies the bits of the output port to be connected
	PortSelection dstPort; // specifies the the input port to be connected
} Connection;

typedef struct Routing_struct { // specifies how an output port of one device is connected to the input ports of one or more receving devices
	UniquePort srcPort;
	vector <Connection> connections;
} Routing;

class Device {

protected:

	uint16_t mDevAdr;
	uint16_t mDevSz;
	vector<uint8_t> mMem;

	DebugInfo mDebugInfo;

	uint64_t mCycleCount = 0;

	vector<Device*> mConnectedDevices; // other devices that connects to inputs of the device
	
	map<int, DevicePort> mPorts; // the device's input ports that can be connected to by other devices

	ConnectionManager* mConnectionManager;


public:


	string name;

	DeviceEnum devType;

	Device(string name, DeviceEnum typ, uint16_t adr, uint16_t sz, DebugInfo debugInfo, ConnectionManager *connectionManager);

	virtual bool read(uint16_t adr, uint8_t& data) = 0;
	virtual bool write(uint16_t adr, uint8_t data) = 0;

	bool selected(uint16_t adr);
	bool validAdr(uint16_t adr);

	// Reset device
	virtual bool reset() { mCycleCount = 0; return true; }

	//  Advance until clock cycle stopcycle has been reached
	virtual bool advance(uint64_t stopCycle) { mCycleCount = stopCycle; return true; }

	// Update of an input by another device via the connection manager
	bool updateInput(PortSelection &port, uint8_t val);

	// Update an output and propagate it to inputs of potentially connected other devices via the connection manager
	bool updateOutput(int index, uint8_t val);

	// Get local port index for a named I/O (used by connection manager at initialisation)
	bool getPortIndex(string name, int &index);

	// Used by a device to make a port available for routing
	bool addPort(string name, int index, uint8_t* portVal);

};

class Devices {

private:

	vector<Device*> mDevices;
	DebugInfo mDebugInfo;

public:

	Devices(std::string memMapFile, int n60HzCycles, ALLEGRO_BITMAP* disp, Keyboard *keyboard, DebugInfo debugInfo, Program program, Program data);

	~Devices();

	bool loadData(Program data);


	bool getDevice(string name, Device * &device) {
		for (int i = 0; i < mDevices.size(); i++) {
			if (mDevices[i]->name == name) {
				device = mDevices[i];
				return true;
			}
		}
		return false;
	}

	bool getDevice(int n, Device* &device) {
		device = NULL;
		if (n >= 0 && n < mDevices.size()) {
			device = mDevices[n];
			return true;
		}	
		return false;
	}

	int size() { return (int) mDevices.size(); }


	bool read(uint16_t adr, uint8_t& data);
	bool write(uint16_t adr, uint8_t data);

};

class ConnectionManager {

private:

	map<int, Routing>					mRouting;		// each output will have a device-independent unique index which is used to lookup the routing
	Devices*							mDevices;
	map<Device*, map<int,UniquePort>>	mUniquePorts;	// device port to global index mapping
	int mUniqueIndex = 0;

	bool getRoutingIndex(PortSelection port, Routing *routing);

	bool extractPort(string name, PortSelection& port);

public:

	ConnectionManager(Devices* devices);

	// Used by a device to make a port available for routing
	bool addDevicePort(Device* dev, LocalPort localPort);

	// Based on the device-independent unique index of a device's output, propagate the update to all connected devices
	bool receiveUpdate(Device *dev, int index, uint8_t val);

	// Connect one device's output with the input of another device
	bool connect(string srcName, string dstName);

};

#endif