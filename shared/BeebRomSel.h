#ifndef BEEB_ROM_SEL_H
#define BEEB_ROM_SEL_H

#include <cstdint>
#include "MemoryMappedDevice.h"
#include <vector>
#include "Rom.h"


using namespace std;

class BeebROMSel : public MemoryMappedDevice {

private:

	int NW,NE,SW,SE;
	uint8_t mNW = 0x1; // IC52 "BASIC ROM" -		typically containing the DFS (slot 12, IC20 output #0)
	uint8_t mNE = 0x1; // IC88 "DISC & NET ROM" -	typically empty (slot 13, IC20 output #1)
	uint8_t mSW = 0x1; // IC100 "AUX ROM" -			typically empty  (slot 14, IC20 output #2)
	uint8_t mSE = 0x1; // IC101 "AUX ROM" -			typically containing BASIC or BASIC II (slot 15, IC20 output #3)

	uint8_t mReg = 15;	// 4-bit ROM selection register; 15 <=> BBC BASIC ROM

	vector<ROM*> mROMs;

public:


	BeebROMSel(string name, uint16_t adr, DebugInfo debugInfo, ConnectionManager * connectionManager);

	bool read(uint16_t adr, uint8_t& data);
	bool write(uint16_t adr, uint8_t data);

	bool addROMs(vector<ROM*> &ROMs);

};

#endif