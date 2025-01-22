#ifndef PIA8255_H
#define PIA8255_H

#include <cstdint>
#include <vector>
#include <string>
#include <allegro5/allegro.h>
#include "Device.h"


using namespace std;

class PIA8255 : public Device {

private:

	uint8_t					mSync60HzEvent = 0; // 60 Hz sync signal - high (1) or low (0)
	int						mN60HzCycles = 0;

	// PIA registers
	uint8_t mPortA, mPortB, mPortC, mCR;
	
	// Ports that can be connected to other devices
	int PIA_PORT_A, PIA_PORT_B, PIA_PORT_C;



public:

	PIA8255(string name, uint16_t adr, int n60HzCycles, DebugInfo debugInfo, ConnectionManager *connectionManager);

	bool read(uint16_t adr, uint8_t& data);
	bool write(uint16_t adr, uint8_t data);

	// Reset device
	bool reset();

	// Advance until clock cycle stopcycle has been reached
	bool advance(uint64_t stopCycle);

};

#endif