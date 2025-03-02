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

	// Ports
	int RxD, TxD, RTS, CTS, DCD, RxCLK, TxCLK;
	uint8_t mRxD = 0x0;
	uint8_t mTxD = 0x0;
	uint8_t mRTS = 0x1;
	uint8_t mCTS = 0x1;
	uint8_t mDCD = 0x1;
	uint8_t mRxCLK = 1;
	uint8_t mTxCLK = 1;

	// Registers
	uint8_t mCR = 0x0;	// base address + 0 - Write-only
	uint8_t mSR = 0x0;	// base address + 0 - Read-only
	uint8_t mTDR = 0x0;	// base address + 1 - Write-only
	uint8_t mRDR = 0x0;	// base address + 1 - Read-only

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