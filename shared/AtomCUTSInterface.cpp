#include "AtomCUTSInterface.h"
#include <cmath>
#include <chrono>
#include <iomanip>

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

AtomCUTSInterface::AtomCUTSInterface(string name, double systemClock, DebugManager  *debugManager, ConnectionManager* connectionManager) :
	Device(name, ATOM_CUTS_DEV, OTHER_DEVICE, systemClock, debugManager, connectionManager)
{
	registerPort("TAPE_OUT",	IN_PORT,	0x01, TAPE_OUT, &mTAPE_OUT);	// From PIA PC0
	registerPort("ENA_TONE",	IN_PORT,	0x01, ENA_TONE, &mENA_TONE);	// From PIA PC1
	registerPort("CAS_IN",		IN_PORT,	0x01, CAS_IN, &mCAS_IN);		// From Tape Recorder
	registerPort("TONE",		OUT_PORT,	0x01, TONE,		&mTONE);		// To PIA PC4
	registerPort("TAPE_IN",		OUT_PORT,	0x01, TAPE_IN, &mTAPE_IN);		// To PIA PC5
	registerPort("CAS_OUT",		OUT_PORT,	0x01, CAS_OUT,	&mCAS_OUT);		// To Tape Recorder

	mToneHalfcycle = (int)round(systemClock * 1e6 / 2400 / 2);

}


bool AtomCUTSInterface::advanceUntil(uint64_t stopCycle)
{
	while (mCycleCount < stopCycle) {

		// Generate TONE - toggles the 2.4 kHz tone when one 1/2 cycle of 2.4 Khz has elapsed
		if (mCycleCount % mToneHalfcycle == 0)
			updatePort(TONE, 1 - mTONE);

		// Generate CAS OUT based on ENA_TONE, TONE and TAPE OUT
		uint8_t cas_out = (1 - (((1 - mENA_TONE) | mTONE) & mTAPE_OUT));
		updatePort(CAS_OUT, cas_out);

		// Generate TAPE_IN based on CAS_IN
		
		if (DBG_LEVEL(DBG_TAPE) && mTAPE_IN != mCAS_IN) {
			int prev_half_cycle_time_point = mDebugPrevHalfCycleTimePoint;
			mDebugPrevHalfCycleTimePoint = mCycleCount;
			double cycle_len = 2 * (mDebugPrevHalfCycleTimePoint- prev_half_cycle_time_point) / (mCPUClock * 1e6);
			DBG_LOG(this, DBG_TAPE, to_string(cycle_len > 0 ? 1 / cycle_len : 9999) + " Hz 1/2 cycle detected");
		}
		updatePort(TAPE_IN, mCAS_IN);

		mCycleCount++;

		
	}


	return true;
}

// Outputs the internal state of the device
bool AtomCUTSInterface::outputState(ostream& sout)
{
	sout << "The Acorn Atom CUTS interface is stateless!\n";
	return true;
}