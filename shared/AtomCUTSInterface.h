#ifndef CUTS_INTERFACE
#define CUTS_INTERFACE

#include "Device.h"
#include <fstream>
#include <iostream>
#include "CSWCodec.h"
#include <cmath>

class AtomCUTSInterface : public Device {

private:

	uint8_t mTONE = 0x0;
	uint8_t mENA_TONE = 0x0;
	uint8_t mTAPE_OUT = 0x0;
	uint8_t mTAPE_IN = 0x0;
	uint8_t mCAS_IN = 0x0;
	uint8_t mCAS_OUT = 0x0;
	int TONE, CAS_IN, ENA_TONE, TAPE_OUT, TAPE_IN, CAS_OUT;

	int mToneHalfcycle = 0;

public:

	AtomCUTSInterface(string name, double systemClock, DebugManager  *debugManager, ConnectionManager* connectionManager);

	bool advanceUntil(uint64_t stopCycle);

	// Outputs the internal state of the device
	bool outputState(ostream& sout) override;

};





#endif