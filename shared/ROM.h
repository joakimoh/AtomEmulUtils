#ifndef ROM_H
#define ROM_H

#include <cstdint>
#include "MemoryMappedDevice.h"
#include <string>
#include <vector>


using namespace std;

class ROM : public MemoryMappedDevice {

public:

	int CS;
	uint8_t mCS = 0x0;

public:


	ROM(string name, double clockSpeed, uint8_t waitStates, uint16_t adr, uint16_t sz, string binaryContent, DebugManager  *debugManager, ConnectionManager* connectionManager);

	bool read(uint16_t adr, uint8_t& data);
	bool write(uint16_t adr, uint8_t data);
};

#endif