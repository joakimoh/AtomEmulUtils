#ifndef PROXY_MEMORY_DEVICE_H
#define PROXY_MEMORY_DEVICE_H

#include "MemoryMappedDevice.h"

class MemoryProxyDevice : public MemoryMappedDevice {

	class MemoryDeviceInfo {
	public:
		MemoryMappedDevice* dev;
		uint8_t				CS = 0;				// Current value of the proxy device's CS associated with a device
		int					CSPortindex = 0;	// Port index of the proxy device's CS input associated with a device
	};

private:
	vector<MemoryDeviceInfo*> mDeviceInfos;
	AddressSpaceInfo mSpace;
	int mCSPortIndex = 0;
	MemoryMappedDevice* mSelectedDevice = NULL;

public:

	MemoryProxyDevice(string name, uint16_t adr, uint16_t sz, MemoryMappedDevice* firstDevice, DebugTracing* debugTracing,
		ConnectionManager* connectionManager, DeviceManager* deviceManager);

	~MemoryProxyDevice();

	bool addDevice(MemoryMappedDevice* dev);

	// Intrusive read of a device's memory that could trigger actions on the device's side
	bool read(uint16_t adr, uint8_t& data) {
		if (mSelectedDevice != NULL) return mSelectedDevice->read(adr, data);
		data = 0xff; // Return success and a tri-state value if no device is currently selected
		return true; //
	}

	bool write(uint16_t adr, uint8_t data) {
		if (mSelectedDevice != NULL) return mSelectedDevice->write(adr, data);
		return false; // Writing when no device is selected will always be an erroneous action
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