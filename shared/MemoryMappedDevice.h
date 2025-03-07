#ifndef MEMORY_MAPPED_DEVICE_H
#define MEMORY_MAPPED_DEVICE_H

#include "Device.h"

class MemoryMappedDevice : public Device {

	typedef struct DeviceAccessScheduling_struct{
		Device *	device;
		uint16_t	triggeringAdr;
	} DeviceAccessScheduling;

	typedef struct MemoryRange_struct {
		uint16_t	adr;
		uint16_t	sz;
	} MemoryRange;

protected:

	MemoryRange mMemorySpace;
	vector< MemoryRange> mMemoryGaps;

	vector<uint8_t> mMem;
	vector<DeviceAccessScheduling> mScheduleOnRead;
	vector<DeviceAccessScheduling> mScheduleOnWrite;

public:

	MemoryMappedDevice(string name, DeviceId typ, DeviceCategory cat, double cpuClock, uint16_t adr, uint16_t sz, DebugManager  *debugManager, ConnectionManager* connectionManager);

	virtual bool read(uint16_t adr, uint8_t& data);

	virtual bool write(uint16_t adr, uint8_t data);

	bool triggerBeforeRead(uint16_t adr, uint8_t data);

	bool triggerAfterWrite(uint16_t adr, uint8_t data);
	bool selected(uint16_t adr);

	bool registerAccess(Device* dev, uint16_t adr, bool writeAccess);
	void addMemoryGap(uint16_t adr, uint16_t sz);

};

#endif