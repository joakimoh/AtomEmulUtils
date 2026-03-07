#ifndef ROM_H
#define ROM_H

#include <cstdint>
#include "MemoryMappedDevice.h"
#include <string>
#include <vector>


using namespace std;

class ROM : public MemoryMappedDevice {

public:

	bool mRead = false;

public:


	ROM(string name, uint8_t waitStates, uint16_t adr, uint16_t sz, string binaryContent, DebugTracing  *debugTracing,
		ConnectionManager* connectionManager, DeviceManager* deviceManager);

	bool read(uint16_t adr, uint8_t& data);
	bool dump(uint16_t adr, uint8_t& data) override;
	bool write(uint16_t adr, uint8_t data);

	// Outputs the internal state of the device
	bool outputState(ostream& sout) override;
};

#endif