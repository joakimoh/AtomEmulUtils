#ifndef DEVICE_H
#define DEVICE_H

#include <cstdint>
#include <vector>
#include <string>
#include <map>
#include "DebugInfo.h"
#include <allegro5/allegro_primitives.h>


using namespace std;

// Forward references to classed defined later on to allow them being referenced before their the are declared
class ConnectionManager; 
class Device;
class Devices;
class MemoryMappedDevice;

enum DeviceId {
	TAPE_RECORDER_DEV, CUTS_DEV, ROM_DEV, RAM_DEV, PIA8255_DEV, VDU6847_DEV, VIA6522_DEV, ATOM_KB_DEV, P6502_DEV, UNDEFINED_DEV
};
#define _DEVICE_ID(x) (\
	x==ROM_DEV?"ROM":(x==RAM_DEV?"RAM":(x==PIA8255_DEV?"PIA 8255":(x==VDU6847_DEV?"VDU 6847":(x==VIA6522_DEV?"VIA 6522":\
(x==ATOM_KB_DEV?"ATOM KB":(x==P6502_DEV?"6502":(x==CUTS_DEV?"CUTS":(x==TAPE_RECORDER_DEV?"Tape Recorder":"???")))))))))

enum DeviceCategory {
	MICROROCESSOR_DEVICE, PERIPHERAL, MEMORY_DEVICE, OTHER_DEVICE
};
#define _DEVICE_CATEGORY(x) (\
	x==MICROROCESSOR_DEVICE?"Microprocessor":(x== PERIPHERAL?"Peripheral":(x==MEMORY_DEVICE?"Memory":"Other Device")))

typedef struct Program_struct {
	string fileName = "";
	int loadAdr = -1;
} Program;


class DevicePort;
//typedef struct DevicePort_struct DevicePort;

// dst = dst & ~mask | ((src >> shifts) & mask)
typedef struct InputReference_struct {
	DevicePort *	port;
	int				shifts = 0;		// no of steps to downshift src value to fit dst start bit
	uint8_t			mask = 0xff;	// mask specifiyng the bits of the dst to be updated (set bit <= update)
} InputReference;

enum PortDirection {IN_PORT, OUT_PORT, IO_PORT};
#define _PORT_DIR(x) (x==IN_PORT?"IN":(x==OUT_PORT?"OUT":"IN/OUT"))

class DevicePort {
public:
	Device *				dev = NULL;			// name of the device
	string					name = "";			// name of the I/O port
	int						localIndex = -1;	// local device index for the I/O port
	int						globalIndex = -1;	// unique global index for the port
	PortDirection			dir = IO_PORT;		// I/O direction
	uint8_t					mask = 0x1;			// mask to select only the implemented bits
	uint8_t	*				val;				// pointer to variable holding the port's value
	vector<InputReference>	inputs;				// connected inputs (used only if the port is an output port)
};

typedef struct BitsSelection_struct {
	uint8_t mask = 0x0;	// specifies the bits of the I/O port to be connected
	uint8_t lowBit = 0;	// specifies the lowest bit if the port I/O to be connected (already identified by the mask but pre-calculated for speed reasons)
} BitsSelection;

typedef struct PortBitsSel_struct {
	DevicePort *	port;	// port identity
	BitsSelection	bits;	// bits selection
} PortSelection;

typedef struct Connector_struct { // specifies the receiving unique part of a routing
	BitsSelection srcBits; // specifies the bits of the output port to be connected
	PortSelection dstPort; // specifies the the input port to be connected
} Connection;

typedef struct Routing_struct { // specifies how an output port of one device is connected to the input ports of one or more receving devices
	DevicePort *		srcPort;
	vector <Connection>	connections;
} Routing;

class Device {

protected:



	DebugInfo mDebugInfo;

	uint64_t mCycleCount = 0;
	
	vector<DevicePort*> mPorts; // the device's input ports that can be connected to by other devices

	ConnectionManager* mConnectionManager;

	int mPortIndex = 0;

public:


	string name;

	DeviceId devType;

	DeviceCategory category;

	Device(string name, DeviceId typ, DeviceCategory cat, DebugInfo debugInfo, ConnectionManager *connectionManager);
	~Device();

	// Reset device
	virtual bool reset() { return true; }

	//  Advance until clock cycle stopcycle has been reached
	virtual bool advance(uint64_t stopCycle) { mCycleCount = stopCycle; return true; }

	// Update an output and propagate it to inputs of potentially connected other devices via the connection manager
	bool updatePort(int index, uint8_t val);

	// Get local port index for a named I/O (used by connection manager at initialisation)
	bool getPortIndex(string name, DevicePort * &port);

	// Used by a device to make a port available for routing
	bool registerPort(string name, PortDirection dir, uint8_t mask, int& index, uint8_t* val);

};

class Devices {

private:

	vector<Device*> mDevices;
	DebugInfo mDebugInfo;

public:

	Devices(
		string memMapFile, int n60HzCycles, double clockSpeed, ALLEGRO_BITMAP* disp, DebugInfo debugInfo,
		Program program, Program data, ConnectionManager &connectionManager, Device * &vdu,
		vector<Device *> &nonVduDevices
	);

	~Devices();

	bool getPeripherals(vector<Device*> &devices);
	bool getNonVduTimeAwareDevices(vector<Device*> &devices);
	bool getMemoryMappedDevices(vector<MemoryMappedDevice*> &devices);

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

	int size() { return (int) mDevices.size(); }

};

class ConnectionManager {

private:

	//map<int, Routing>					mRouting;		// each output will have a device-independent unique index which is used to lookup the routing
	Devices*							mDevices = NULL;
	map<Device*, map<int,DevicePort*>>	mDevicePorts;	// device port to global index mapping
	int									mDevicePortIndex = 0;

	bool extractPort(string name, PortSelection& port);

	DebugInfo mDebugInfo;
	

public:

	void printRouting();
	string printDevicePort(DevicePort * device_port);
	string printPortSelection(PortSelection & port_selection);

	ConnectionManager(DebugInfo debugInfo);
	~ConnectionManager();

	void setDevices(Devices* devices);

	// Used by a device to make a port available for routing
	bool addDevicePort(Device* dev, DevicePort *localPort);

	// Based on the device-independent unique index of a device's output, propagate the update to all connected devices
	//bool receiveUpdate(Device *dev, int index, uint8_t val);

	// Connect one device's output with the input of another device
	bool connect(string srcName, string dstName);

};

#endif