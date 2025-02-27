#ifndef ACIA_6850_H
#define ACIA_6850_H

#include <cstdint>
#include <string>
#include "MemoryMappedDevice.h"

using namespace std;


class ACIA6850 : public MemoryMappedDevice {

private:

	double mClock = 1.0;
	double mCPUClock = 2.0;

public:

	ACIA6850(string name, uint16_t adr, double clock, double cpuClock, DebugInfo  *debugInfo, ConnectionManager* connectionManager);

	bool read(uint16_t adr, uint8_t& data);
	bool write(uint16_t adr, uint8_t data);

	// Reset device
	bool reset();

	// Advance until clock cycle stopcycle has been reached
	bool advance(uint64_t stopCycle);

};

#endif