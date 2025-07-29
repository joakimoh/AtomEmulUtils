#ifndef PROXY_MEMORY_DEVICE_H
#define PROXY_MEMORY_DEVICE_H

#include "MemoryMappedDevice.h"

class MemoryProxyDevice : public MemoryMappedDevice {

	class MemoryDeviceInfo {
	public:
		MemoryMappedDevice* dev;
		uint8_t				val = 0;
		int					index = 0;
	};

private:
	vector<MemoryDeviceInfo*> mDevices;
	AddressSpaceInfo mSpace;
	int mIndex = 0;
	MemoryMappedDevice* mSelectedDevice = NULL;

public:

	MemoryProxyDevice(string name, uint16_t adr, uint16_t sz, MemoryMappedDevice* firstDevice, DebugManager* debugManager,
		ConnectionManager* connectionManager, DeviceManager* deviceManager);

	bool addDevice(MemoryMappedDevice* dev);

	// Intrusive read of a device's memory that could trigger actions on the device's side
	virtual bool read(uint16_t adr, uint8_t& data) {
		if (mSelectedDevice != NULL) return mSelectedDevice->read(adr, data);
		return false;
	}

	virtual bool write(uint16_t adr, uint8_t data) {
		if (mSelectedDevice != NULL) return mSelectedDevice->write(adr, data);
		return false;
	}

	bool selected(uint16_t adr) {
		if (mSelectedDevice != NULL) return mSelectedDevice->selected(adr);
		return false;
	}

	uint8_t getWaitStates() {
		if (mSelectedDevice != NULL) return mSelectedDevice->getWaitStates();
		return 0;
	}

	// Process a port update directly
	void processPortUpdate(int port) override;

};


#endif