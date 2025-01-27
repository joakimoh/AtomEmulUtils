#ifndef CUTS_INTERFACE
#define CUTS_INTERFACE

#include "Device.h"
#include <fstream>
#include <iostream>
#include "CSWCodec.h"

class AtomCUTSInterface : public Device {

private:

	uint8_t mTONE = 0x0;
	uint8_t mENA_TONE = 0x0;
	uint8_t mTAPE_OUT = 0x0;
	uint8_t mTAPE_IN = 0x0;
	uint8_t mCAS_IN = 0x0;
	uint8_t mCAS_OUT = 0x0;
	int TONE, CAS_IN, ENA_TONE, TAPE_OUT, TAPE_IN, CAS_OUT;

	uint8_t mToneHalfcycle = 0;

	uint8_t pCAS_IN = 0x0;

public:

	AtomCUTSInterface(string name, DebugInfo debugInfo, ConnectionManager* connectionManager);

	bool advance(uint64_t stopCycle);

};





#endif