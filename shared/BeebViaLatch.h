#ifndef BEEB_VIA_LATCH_H
#define BEEB_VIA_LATCH_H

#include <cstdint>
#include "Device.h"
#include <vector>
#include "ROM.h"


using namespace std;

//
// Emulation of BBC Micro's IC31 & IC32
//

class BeebViaLatch : public Device {

private:

	int CTRL, Q;
	uint8_t mCTRL = 0x0;	// Latch Selection & Data In
	uint8_t mQ = 0xff;		// Latch outputs

public:


	BeebViaLatch(string name, double cpuClock, DebugManager* debugManager, ConnectionManager* connectionManager);

	// Process a port update directly (and not just next time the advance() method is called)
	void processPortUpdate(int port) override;

};

#endif