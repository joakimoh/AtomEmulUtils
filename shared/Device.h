#ifndef DEVICE_H
#define DEVICE_H

#include <cstdint>
#include <vector>
#include <string>
#include <map>
#include "DebugInfo.h"
#include <allegro5/allegro_primitives.h>
#include <iostream>


using namespace std;

// Forward references to classed defined later on to allow them being referenced before their the are declared
class ConnectionManager; 
class Device;
class Devices;
class MemoryMappedDevice;
class VideoDisplayUnit;
class P6502;

enum Scheduling {FRAME, HLINE, INSTR, NONE};
#define _SCHEDULING(x) (x==FRAME?"Frame":(x==HLINE?"1/2 line":(x==INSTR?"Instruction":"None")))

enum DeviceId {
	ACIA6850_DEV, BEEB_PAGED_ROM_SEL_DEV, CRTC6845_DEV, TT_5050_DEV, BEEB_KEYBOARD_DEV, BEEB_VDU_DEV, TAPE_RECORDER_DEV, ATOM_SPEAKER_DEV, ATOM_CUTS_DEV, ROM_DEV, RAM_DEV, PIA8255_DEV, VDU6847_DEV, VIA6522_DEV, ATOM_KB_DEV, P6502_DEV, UNDEFINED_DEV
};
#define _DEVICE_ID(x) (\
	x==ROM_DEV?"ROM":(x==RAM_DEV?"RAM":(x==PIA8255_DEV?"PIA 8255":(x==VDU6847_DEV?"VDU 6847":(x==VIA6522_DEV?"VIA 6522":\
(x==ATOM_KB_DEV?"ATOM KB":(x==P6502_DEV?"6502":(x==ATOM_CUTS_DEV?"CUTS":(x==TAPE_RECORDER_DEV?"Tape Recorder":\
(x==ATOM_SPEAKER_DEV?"Atom Speaker":(x==BEEB_VDU_DEV?"Beeb VDU":(x==BEEB_KEYBOARD_DEV?"Beeb Keyboard":(x==CRTC6845_DEV?"CRTC 6845":\
(x==TT_5050_DEV?"TT SA5050":(x==BEEB_PAGED_ROM_SEL_DEV?"Beeb Paged Rom Register":(x==ACIA6850_DEV?"ACIA 6850":"???"))\
))))))))))))))

enum DeviceCategory {
	SOUND_DEVICE, MICROROCESSOR_DEVICE, VDU_DEVICE, KEYBOARD_DEVICE, PERIPHERAL, MEMORY_DEVICE, OTHER_DEVICE
};
#define _DEVICE_CATEGORY(x) (\
	x==MICROROCESSOR_DEVICE?"Microprocessor":(x== PERIPHERAL?"Peripheral":(x==MEMORY_DEVICE?"Memory":(x==VDU_DEVICE?"Video Data Unit":(x==SOUND_DEVICE?"Sound Device":(x==KEYBOARD_DEVICE?"Keyboard":"Other Device"))))))

typedef struct Program_struct {
	string fileName = "";
	int loadAdr = -1;
} Program;


class DevicePort;

// dst = dst & ~mask | ((src >> shifts) & mask)
typedef struct InputReference_struct {
	DevicePort *	port;
	int				shifts = 0;				// no of steps to downshift src value to fit dst start bit
	uint8_t			mask = 0xff;			// mask specifiyng the bits of the dst to be updated (set bit <= update)
} InputReference;

enum PortDirection {IN_PORT, OUT_PORT, IO_PORT};
#define _PORT_DIR(x) (x==IN_PORT?"IN":(x==OUT_PORT?"OUT":"IN/OUT"))

class DevicePort {
public:
	Device *				dev = NULL;				// name of the device
	string					name = "";				// name of the I/O port
	int						localIndex = -1;		// local device index for the I/O port
	int						globalIndex = -1;		// unique global index for the port
	PortDirection			dir = IO_PORT;			// I/O direction
	uint8_t					mask = 0x1;				// mask to select only the implemented bits
	uint8_t	*				val;					// pointer to variable holding the port's value
	vector<InputReference>	inputs;					// connected inputs (used only if the port is an output port)
	bool					triggerDevice = false;	// true if the device's trigger() method shall be called on an update of an input port
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

	vector<Device*> mConnectedDevices;

	bool mMemoryMapped = false;

public:

	Scheduling scheduling = INSTR; // default scheduling if nothing specified

	string name;

	DeviceId devType;

	DeviceCategory category;

	Device(string name, DeviceId typ, DeviceCategory cat, DebugInfo debugInfo, ConnectionManager *connectionManager);
	~Device();

	// Reset device
	virtual bool reset() { return true; }

	//  Advance until clock cycle stopcycle has been reached
	virtual bool advance(uint64_t stopCycle) { mCycleCount = stopCycle; return true; }

	// Executed on input port update (when configured)
	virtual bool trigger(int port) { return true; }

	// Update an output and propagate it to inputs of potentially connected other devices via the connection manager
	bool updatePort(int index, uint8_t val);

	// Get local port index for a named I/O (used by connection manager at initialisation)
	bool getPortIndex(string name, DevicePort * &port);

	// Used by a device to make a port available for routing
	bool registerPort(string name, PortDirection dir, uint8_t mask, int& index, uint8_t* val);

	uint64_t getCycleCount() { return mCycleCount; }

	// Get pointer to other device to be able to call its methods
	virtual bool connectDevice(Device* dev);

	bool memoryMapped() { return mMemoryMapped;  }

	// Called by other device to get next memory address to fetch data from
	// Mainly used my video devices like the M6845 that calculates memory addresses
	// for fetching char/graphics data from to be used by the mian video devices
	// like the Beeb Video ULA that consumes the data.
	virtual bool getMemFetchAdr(uint16_t& adr, uint16_t& cursor) { adr = 0xffff;  return false; }

	// Called by a other device when the device is asked to process/transform data.
	virtual bool getDeviceData(uint8_t dIn, uint8_t& dOut) { dOut = 0xff;  return false; }

};

class Devices {

private:

	vector<Device*> mDevices;
	DebugInfo mDebugInfo;

	string getFileName(string& path, stringstream& sin);
	uint16_t getHexVal(stringstream& sin);
	double getDoubleVal(stringstream& sin);

public:

	Devices(
		string memMapFile, double clockSpeed, int audioSampleFreq, ALLEGRO_BITMAP* disp, int dispW, int dispH, DebugInfo debugInfo,
		Program program, Program data, ConnectionManager &connectionManager, P6502* &microprocessor, VideoDisplayUnit* &vdu,
		vector<Device *> &frameScheduledDevices, vector<Device*> &halfLineScheduledDevices, vector<Device*> &instructionScheduledDevices
	);

	~Devices();

	bool getPeripherals(vector<Device*> &devices);
	bool getOtherDevices(vector<Device*> &devices);
	bool getMemoryMappedDevices(vector<MemoryMappedDevice*> &devices);
	bool getMemoryDevices(vector<MemoryMappedDevice*>& devices);
	bool getZPMemDevice(MemoryMappedDevice*& zpMem);

	bool loadData(Program data);


	bool getDevice(string name, Device * &device) {
		device = NULL;
		for (int i = 0; i < mDevices.size(); i++) {
			if (mDevices[i]->name == name) {
				device = mDevices[i];
				return true;
			}
		}
		return false;
	}

	//
	// Find a device of a certain type.
	// If more thn one device of this type is found,
	// the first one is returned but an error is also indicated.
	//
	bool getDevice(DeviceId id, Device*& device) {
		bool found = false;
		device = NULL;
		for (int i = 0; i < mDevices.size(); i++) {
			if (mDevices[i]->devType == id) {
				if (!found) {
					device = mDevices[i];
					found = true;
				}
				else
					return false;
			}
		}
		return found;
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