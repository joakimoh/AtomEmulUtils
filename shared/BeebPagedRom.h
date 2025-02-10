#ifndef BEEB_PAGED_ROM_H
#define BEEB_PAGED_ROM_H

#include <cstdint>
#include "MemoryMappedDevice.h"
#include <string>
#include <vector>


using namespace std;

class BeebPagedROM : public MemoryMappedDevice {

public:

	int SEL;
	uint8_t mSEL = 0x0;
	int mSlot = 0;

public:


	BeebPagedROM(string name, int slot,	uint16_t adr, uint16_t sz, string binaryContent, DebugInfo debugInfo, ConnectionManager * connectionManager);

	bool read(uint16_t adr, uint8_t& data);
	bool write(uint16_t adr, uint8_t data);

	int getSlot() { return mSlot; }
};

#endif