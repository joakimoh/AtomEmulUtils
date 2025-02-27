#include "AtomCUTSInterface.h"
#include <cmath>

//
// Model of the CUTS Interface implemented by the Acorn Atom
//
// It connects the PIA 8255 and a Tape Recorder
//
// PIA Port	PIA Direction	CUTS Port	Usage
// PC0		OUT				TAPE_OUT	Tape output - both provides and enables cassette output
// PC1		OUT				ENA_TONE	Enable 2.4 kHz tone generation to the cassette output
// PC4		IN				TONE		2.4 KHz tone input from the CUTS Interface
// PC5		IN				TAPE_IN		Cassette input
// 
// CUTS Port	Usage
// CAS_IN		Cassette input from the Tape Recorder
// CAS_OUT		Cassette output to the Tape Recorder
//

AtomCUTSInterface::AtomCUTSInterface(string name, double systemClock, DebugInfo  *debugInfo, ConnectionManager* connectionManager) :
	Device(name, ATOM_CUTS_DEV, OTHER_DEVICE, debugInfo, connectionManager)
{
	registerPort("TAPE_OUT",	IN_PORT,	0x01, TAPE_OUT, &mTAPE_OUT);	// From PIA PC0
	registerPort("ENA_TONE",	IN_PORT,	0x01, ENA_TONE, &mENA_TONE);	// From PIA PC1
	registerPort("CAS_IN",		IN_PORT,	0x01, CAS_IN, &mCAS_IN);		// From Tape Recorder
	registerPort("TONE",		OUT_PORT,	0x01, TONE,		&mTONE);		// To PIA PC4
	registerPort("TAPE_IN",		OUT_PORT,	0x01, TAPE_IN, &mTAPE_IN);		// To PIA PC5
	registerPort("CAS_OUT",		OUT_PORT,	0x01, CAS_OUT,	&mCAS_OUT);		// To Tape Recorder

	mToneHalfcycle = (int)round(systemClock * 1e6 / 2400 / 2);

}


bool AtomCUTSInterface::advance(uint64_t stopCycle)
{
	while (mCycleCount < stopCycle) {

		// Generate TONE - toggles the 2.4 kHz tone when one 1/2 cycle of 2.4 Khz has elapsed
		if (mCycleCount % mToneHalfcycle == 0)
			updatePort(TONE, 1 - mTONE);

		// Generate CAS OUT based on ENA_TONE, TONE and TAPE OUT
		uint8_t cas_out = (1 - (((1 - mENA_TONE) | mTONE) & mTAPE_OUT));
		updatePort(CAS_OUT, cas_out);

		// Generate TAPE_IN based on CAS_IN
		updatePort(TAPE_IN, mCAS_IN);

		mCycleCount++;

		
	}


	return true;
}