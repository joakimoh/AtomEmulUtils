#ifndef RAM_H
#define RAM_H

#include <cstdint>
#include "MemoryMappedDevice.h"
#include <vector>


using namespace std;

class RAM : public MemoryMappedDevice {

private:
	

public:


	RAM(string name, uint8_t waitStates, bool DRAM, uint16_t adr, uint16_t sz, DebugTracing  *debugTracing,
		ConnectionManager * connectionManager, DeviceManager* deviceManager);

	bool read(uint16_t adr, uint8_t& data);
	bool dump(uint16_t adr, uint8_t& data) override;
	bool write(uint16_t adr, uint8_t data);
	bool write(uint16_t adr, vector<uint8_t>& data, uint16_t sz);
	bool read(uint16_t adr, vector<uint8_t>& data, uint16_t sz);

	// Outputs the internal state of the device
	bool outputState(ostream& sout) override;
};

#endif