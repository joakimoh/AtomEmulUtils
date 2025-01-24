#ifndef MEMORY_MAPPED_DEVICE_H
#define MEMORY_MAPPED_DEVICE_H

#include "Device.h"

class MemoryMappedDevice : public Device {

protected:

	uint16_t mDevAdr;
	uint16_t mDevSz;
	vector<uint8_t> mMem;

public:

	MemoryMappedDevice(string name, DeviceId typ, DeviceCategory cat, uint16_t adr, uint16_t sz, DebugInfo debugInfo, ConnectionManager* connectionManager);

	virtual bool read(uint16_t adr, uint8_t& data) = 0;
	virtual bool write(uint16_t adr, uint8_t data) = 0;

	bool selected(uint16_t adr);
	bool validAdr(uint16_t adr);

};

#endif