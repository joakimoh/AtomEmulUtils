#ifndef DEVICE_H
#define DEVICE_H

#include <cstdint>
#include <vector>
#include <string>

using namespace std;

enum DeviceEnum { ROM_DEV, RAM_DEV, PIA8255_DEV, VDU6847_DEV, VIA6522_DEV};

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

public:

	DeviceEnum devType;

	Device(DeviceEnum typ, uint16_t adr, uint16_t sz);

	virtual bool read(uint16_t adr, uint8_t& data) = 0;
	virtual bool write(uint16_t adr, uint8_t data) = 0;

	bool selected(uint16_t adr);
	bool validAdr(uint16_t adr);

	static bool crMemMap(vector< DeviceAllocation> devAllocation, vector<Device*> &devices);
	static bool freeMemMap(vector<Device*>& devices)

};

#endif