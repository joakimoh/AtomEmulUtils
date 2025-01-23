#ifndef VIA6522_H
#define VIA6522_H

#include <cstdint>
#include <string>
#include "Device.h"

using namespace std;


class VIA6522 : public Device {

private:

	// Ports that can be connected to other devices
	int IRQ, RESET;
	uint8_t mIRQ = 0x1;
	uint8_t mRESET = 0x1;
	uint8_t pRESET = 0x0;

public:

	VIA6522(string name, uint16_t adr, DebugInfo debugInfo, ConnectionManager* connectionManager);

	bool read(uint16_t adr, uint8_t& data);
	bool write(uint16_t adr, uint8_t data);

	// Reset device
	bool reset();

	// Advance until clock cycle stopcycle has been reached
	bool advance(uint64_t stopCycle);

};

#endif