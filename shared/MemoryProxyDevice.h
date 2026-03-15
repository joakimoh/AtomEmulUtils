#ifndef PROXY_MEMORY_DEVICE_H
#define PROXY_MEMORY_DEVICE_H

#include "MemoryMappedDevice.h"
#include "DeviceTypes.h"

class MemoryProxyDevice : public MemoryMappedDevice {

	class MemoryDeviceInfo {
	public:
		MemoryMappedDevice* dev;
		PortVal				CS = 0;				// Current value of the proxy device's CS associated with a device
		int					CSPortindex = 0;	// Port index of the proxy device's CS input associated with a device
	};

private:
	vector<MemoryDeviceInfo*> mDeviceInfos;
	AddressSpaceInfo mSpace;
	int mCSPortIndex = 0;
	MemoryMappedDevice* mSelectedDevice = NULL;

public:

	MemoryProxyDevice(string name, BusAddress adr, BusAddress sz, MemoryMappedDevice* firstDevice, DebugTracing* debugTracing,
		ConnectionManager* connectionManager, DeviceManager* deviceManager);

	~MemoryProxyDevice();

	bool addDevice(MemoryMappedDevice* dev);

	// Intrusive read of a device's memory that could trigger actions on the device's side
	bool readByte(BusAddress adr, BusByte& data) {
		if (mSelectedDevice != NULL) return mSelectedDevice->readByte(adr, data);
		data = 0xff; // Return success and a tri-state value if no device is currently selected
		return true; //
	}

	bool writeByte(BusAddress adr, BusByte data) {
		if (mSelectedDevice != NULL) return mSelectedDevice->writeByte(adr, data);
		return false; // Writing when no device is selected will always be an erroneous action
	}

	bool selected(BusAddress adr) {
		if (mSelectedDevice != NULL) return mSelectedDevice->selected(adr);
		return false;
	}

	uint8_t getAccessRatio() {
		if (mSelectedDevice != NULL) return mSelectedDevice->getAccessRatio();
		return 0;
	}

	// Process a port update directly
	void processPortUpdate(int port) override;

};


#endif