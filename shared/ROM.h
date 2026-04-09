#ifndef ROM_H
#define ROM_H

#include <cstdint>
#include "MemoryMappedDevice.h"
#include <string>
#include <vector>
#include "DeviceTypes.h"


using namespace std;

class ROM : public MemoryMappedDevice {

public:

	bool mRead = false;

public:


	ROM(string name, uint8_t accessSpeed, BusAddress adr, BusAddress sz, string binaryContent, DebugTracing  *debugTracing,
		ConnectionManager* connectionManager, DeviceManager* deviceManager);

	bool readByte(BusAddress adr, BusByte& data);
	bool dump(BusAddress adr, uint8_t& data) override;
	bool writeByte(BusAddress adr, BusByte data);

	// Outputs the internal state of the device
	bool outputState(ostream& sout) override;
};

#endif