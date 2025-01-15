#ifndef DEVICE_H
#define DEVICE_H

#include <cstdint>
#include <vector>
#include <string>
#include "DebugInfo.h"
#include "Keyboard.h"

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
	std::string fileName = "";
	int loadAdr = -1;
} Program;

class Device {

protected:

	uint16_t mDevAdr;
	uint16_t mDevSz;
	vector<uint8_t> mMem;

	DebugInfo mDebugInfo;

	uint64_t mCycleCount = 0;

public:

	DeviceEnum devType;

	Device(DeviceEnum typ, uint16_t adr, uint16_t sz, DebugInfo debugInfo);

	virtual bool read(uint16_t adr, uint8_t& data) = 0;
	virtual bool write(uint16_t adr, uint8_t data) = 0;

	bool selected(uint16_t adr);
	bool validAdr(uint16_t adr);

	// Reset device
	virtual bool reset() { mCycleCount = 0; return true; }

	//  Advance until clock cycle stopcycle has been reached
	virtual bool advance(uint64_t stopCycle) { mCycleCount = stopCycle; return true; }


};

class Devices {

private:

	vector<Device*> mDevices;
	DebugInfo mDebugInfo;


public:

	Devices(std::string memMapFile, int n60HzCycles, ALLEGRO_BITMAP* disp, Keyboard *keyboard, DebugInfo debugInfo, Program program, Program data);

	~Devices();

	bool loadData(Program data);

	bool getDevice(int n, Device* &device) {
		device = NULL;
		if (n >= 0 && n < mDevices.size()) {
			device = mDevices[n];
			return true;
		}
		else
			return false;
	}

	int size() { return (int) mDevices.size(); }


	bool read(uint16_t adr, uint8_t& data);
	bool write(uint16_t adr, uint8_t data);
};

#endif