#ifndef MEMORY_MAPPED_DEVICE_H
#define MEMORY_MAPPED_DEVICE_H

#include <iostream>
#include "Device.h"
#include "AddressSpaceInfo.h"
#include "TimedDevice.h"

class DeviceManager;

class MemoryMappedDevice : public Device {

	typedef struct DeviceAccessScheduling_struct{
		Device *	device;
		uint16_t	triggeringAdr;
	} DeviceAccessScheduling;

protected:

	uint16_t mStartOfSpace = 0;

	int CS;
	uint8_t mCS = 0x0;

	bool mTriggerOnWrite = false;
	bool mTriggerOnRead = false;

	AddressSpaceInfo mAddressSpace;

	vector<uint8_t> mMem;
	vector<DeviceAccessScheduling> mScheduleOnRead;
	vector<DeviceAccessScheduling> mScheduleOnWrite;

	double mAccessSpeed = 1.0;
	uint8_t mAccessRatio = 0; // access speed in relation to the CPU speed

	DeviceManager* mDeviceManager = NULL;

public:

	MemoryMappedDevice(string name, DeviceId typ, DeviceCategory cat, double accessSpeed, uint16_t adr, uint16_t sz,
		DebugTracing  *debugTracing, ConnectionManager* connectionManager, DeviceManager *deviceManager);

	// Intrusive read of a device's memory that could trigger actions on the device's side
	virtual bool read(uint16_t adr, uint8_t& data);

	// Non-intrusive read of a device's memory
	virtual bool dump(uint16_t adr, uint8_t& data) { return read(adr,data); }

	virtual bool write(uint16_t adr, uint8_t data);

	bool triggerBeforeRead(uint16_t adr, uint8_t data);

	bool triggerAfterWrite(uint16_t adr, uint8_t data);
	bool selected(uint16_t adr);

	// Register that another device shall be triggered on R/W accesses to this device
	bool registerAccess(Device* dev, uint16_t adr, bool writeAccess);

	// Register a gap in the device's memory map
	void registerMemoryGap(uint16_t adr, uint16_t sz);

	uint8_t getAccessRatio() { return mAccessRatio; }

	AddressSpaceInfo getClaimedAddressSpace() { return mAddressSpace;  }

	void setCPUClockSpeed(double cpuClockSpeed) {
		mAccessRatio = (uint8_t)round(cpuClockSpeed / mAccessSpeed);
		if (mAccessRatio == 0)
			mAccessRatio = 1;
	}

};

#endif