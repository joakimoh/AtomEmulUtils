#ifndef RAM_H
#define RAM_H

#include <cstdint>
#include "MemoryMappedDevice.h"
#include <vector>
#include "DeviceTypes.h"


using namespace std;

class RAM : public MemoryMappedDevice {

private:
	

public:


	RAM(string name, uint8_t accessSpeed, bool DRAM, BusAddress adr, BusAddress sz, DebugTracing  *debugTracing,
		ConnectionManager * connectionManager, DeviceManager* deviceManager);

	bool readByte(BusAddress adr, BusByte& data);
	bool dump(BusAddress adr, uint8_t& data) override;
	bool writeByte(BusAddress adr, BusByte data);
	bool writeBytes(BusAddress adr, vector<BusByte>& data, BusAddress sz);
	bool readBytes(BusAddress adr, vector<BusByte>& data, BusAddress sz);

	// Outputs the internal state of the device
	bool outputState(ostream& sout) override;
};

#endif