#ifndef ROMSEL_H
#define ROMSEL_H

#include <cstdint>
#include "MemoryMappedDevice.h"
#include <vector>
#include "BeebPagedRom.h"


using namespace std;

class BeebROMSel : public MemoryMappedDevice {

private:

	int SEL_L, SEL_H;
	uint8_t mSEL_L = 0x0;
	uint8_t mSEL_H = 0x0;

	uint8_t mReg = 15;	// 4-bit ROM selection register; 15 <=> BBC BASIC ROM

	vector<BeebPagedROM*> mROMs;

public:


	BeebROMSel(string name, uint16_t adr, DebugInfo debugInfo);

	bool read(uint16_t adr, uint8_t& data);
	bool write(uint16_t adr, uint8_t data);

	bool addROMs(vector<BeebPagedROM*> &ROMs);

};

#endif