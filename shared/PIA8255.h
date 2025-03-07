#ifndef PIA8255_H
#define PIA8255_H

#include <cstdint>
#include <vector>
#include <string>
#include <allegro5/allegro.h>
#include "MemoryMappedDevice.h"


using namespace std;

class PIA8255 : public MemoryMappedDevice {

private:

	// PIA registers
	uint8_t mPortA, mPortB, mPortC, mCR;
	uint8_t pPortA = 0x0, pPortB = 0x0, pPortC = 0x0;
	
	// Ports that can be connected to other devices
	int PIA_PORT_A, PIA_PORT_B, PIA_PORT_C;




public:

	PIA8255(string name, double cpuclock, uint16_t adr, DebugManager  *debugManager, ConnectionManager *connectionManager);

	bool read(uint16_t adr, uint8_t& data);
	bool write(uint16_t adr, uint8_t data);

	// Reset device
	bool reset();

	// Advance until clock cycle stopcycle has been reached
	bool advance(uint64_t stopCycle);

};

#endif