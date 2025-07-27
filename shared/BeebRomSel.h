#ifndef BEEB_ROM_SEL_H
#define BEEB_ROM_SEL_H

#include <cstdint>
#include "MemoryMappedDevice.h"
#include <vector>
#include "ROM.h"
#include "DeviceManager.h"


using namespace std;

class BeebROMSel : public MemoryMappedDevice {
	//
	// Selects one of the sixteen sideways RAM/ROMs
	// A standard Model B can only select ROM sockets (IC52, IC88, IC100, IC101, left to right on the board after the OS socket)
	// but this device implementation allows selection of even more RAM/ROMs.
	//

private:

	int NW, NE, SW, SE;
	int B0, B1, B2;
	uint8_t mB0 = 0x3;		// Selection of slots 0-3
	uint8_t mB1 = 0x3;		// Selection of slots 4-7
	uint8_t mB2 = 0x3;		// Selection of slots 8-11
	uint8_t mNW = 0x1;		// Slot 12 Selection:	IC52	"DISC & NET ROM	- typically containing the DFS	(IC20 output #0)
	uint8_t mNE = 0x1;		// Slot 13 Selection:	IC88	"AUX ROM"		- typically empty				(IC20 output #1)
	uint8_t mSW = 0x1;		// Slot 14 Selection:	IC100	"AUX ROM"		- typically empty				(IC20 output #2)
	uint8_t mSE = 0x1;		// Slot 15 Selection:	IC101	"BASIC ROM"		- typically containing BASIC	(IC20 output #3)

	uint8_t mReg = 15;	// 4-bit ROM selection register; 15 <=> BBC BASIC ROM


public:


	BeebROMSel(string name, double cpuClock, uint8_t waitStates, uint16_t adr, DebugManager  *debugManager, ConnectionManager * connectionManager, DeviceManager* deviceManager);

	bool read(uint16_t adr, uint8_t& data);
	bool dump(uint16_t adr, uint8_t& data) override;
	bool write(uint16_t adr, uint8_t data);

	// Outputs the internal state of the device
	bool outputState(ostream& sout) override;

};

#endif