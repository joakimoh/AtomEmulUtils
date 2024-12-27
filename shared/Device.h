#ifndef DEVICE_H
#define DEVICE_H

#include <cstdint>
#include <vector>
#include <string>

using namespace std;

enum DeviceEnum { ROM_DEV, RAM_DEV, PIA8255_DEV, VDU6847_DEV, VIA6522_DEV};
#define _DEVICE_TYPE(x) (x==ROM_DEV?"ROM":(x==RAM_DEV?"RAM":(x==PIA8255_DEV?"PIA 8255":(x==VDU6847_DEV?"VDU 6847":("VIA 6522")))))

typedef struct DeviceAllocation_struct {
	DeviceEnum deviceType;
	uint16_t startAdr;
	uint16_t size;
	string ROMFileName;
} DeviceAllocation;

class Device {

protected:

	uint16_t mDevAdr;
	uint16_t mDevSz;
	vector<uint8_t> mMem;

	bool mVerbose = false;

public:

	DeviceEnum devType;

	Device(DeviceEnum typ, uint16_t adr, uint16_t sz, bool verbose = false);

	virtual bool read(uint16_t adr, uint8_t& data) = 0;
	virtual bool write(uint16_t adr, uint8_t data) = 0;

	bool selected(uint16_t adr);
	bool validAdr(uint16_t adr);

	static bool crMemMap(const vector< DeviceAllocation> &devAllocation, vector<Device*> &devices, string ROMDirPath, bool verbose = false);
	static bool freeMemMap(vector<Device*>& devices);

};

#endif