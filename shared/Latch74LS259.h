#ifndef BEEB_VIA_LATCH_H
#define BEEB_VIA_LATCH_H

#include <cstdint>
#include "Device.h"
#include <vector>
#include "ROM.h"


using namespace std;

//
// Emulation of an 8-bit Addressable Latch 74LS259
//

class Latch74LS259 : public Device {

private:

	int CTRL, Q, ENA, CLR;
	uint8_t mCTRL = 0x0;	// Latch Selection & Data In
	uint8_t mQ = 0xff;		// Latch outputs
	uint8_t mENA = 0;		// Latch enable
	uint8_t mCLR = 1;		// Clear latch

public:


	Latch74LS259(string name, double tickRate, DebugTracing* debugTracing, ConnectionManager* connectionManager);

	// Process a port update directly (and not just next time the advanceUntil() method is called)
	void processPortUpdate(int port) override;

	// Outputs the internal state of the device
	bool outputState(ostream& sout) override;

};

#endif