#ifndef DEVICE_H
#define DEVICE_H

#include <cstdint>
#include <vector>
#include <string>
#include "DebugInfo.h"
#include "Keyboard.h"
#include <map>
#include <cstdint>
#include "Connection.h"
#include "ConnectionManager.h"

using namespace std;

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

typedef struct DevicePort_struct {
	string		name = "";			// name of the I/O port
	int			index = -1;			// local device index for the I/O port
	int			globalIndex = -1;	// unique global index for the I/O port
	uint8_t		*val;				// pointer to variable holding the port's value
} DevicePort;

class Device {

protected:

	uint16_t mDevAdr;
	uint16_t mDevSz;
	vector<uint8_t> mMem;

	DebugInfo mDebugInfo;

	uint64_t mCycleCount = 0;

	vector<Device*> mConnectedDevices; // other devices that connects to inputs of the device
	
	map<int, DevicePort> mPorts; // the device's input ports that can be updated by other device's

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
	bool updateInput(Connection &connection, uint8_t val);

	// Update an output and propagate it to inputs of potentially connected other devices via the connection manager
	bool updateOutput(int index, uint8_t val);

	// Get local port index for a named I/O (used by connection manager at initialisation)
	bool getPortIndex(string name, int &index);

	// Update local port info with global index assigned by the connection manager
	bool assingGlobalPortIndex(int localIndex, int globalIndex);


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

#endif