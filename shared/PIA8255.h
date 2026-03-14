#ifndef PIA8255_H
#define PIA8255_H

#include <cstdint>
#include <vector>
#include <string>
#include <allegro5/allegro.h>
#include "MemoryMappedDevice.h"
#include "TimedDevice.h"


using namespace std;

class PIA8255 : public MemoryMappedDevice, public TimedDevice {

private:

	// Registers
	uint8_t mCR = 0x1b;

	// PIA ports
	PortVal pPortA = 0x0, pPortB = 0x0, pPortC = 0x0;

	// PIA port values (separate values for input and output configuration)
	PortVal mPAIn = 0, mPBIn = 0, mPCIn = 0;
	PortVal mPAOut = 0xff, mPBOut = 0xff, mPCOut = 0xff;
	
	// Ports that can be connected to other devices
	int PIA_PORT_A, PIA_PORT_B, PIA_PORT_C;


public:

	PIA8255(string name, double tickRate, uint8_t waitStates, uint16_t adr, DebugTracing  *debugTracing, ConnectionManager *connectionManager, DeviceManager* deviceManager);

	bool read(uint16_t adr, uint8_t& data);
	bool dump(uint16_t adr, uint8_t& data) override;
	bool write(uint16_t adr, uint8_t data);

	// Reset device
	bool reset();

	// Device power on
	bool power() { return reset(); }

	// Advance until time tickTime
	bool advanceUntil(uint64_t tickTime) override;

	// Outputs the internal state of the device
	bool outputState(ostream& sout) override;

	// Serialise the device's state into an array that can
	// be added to an execution trace easily.
	bool serialiseState(SerialisedState& serialisedState) override;

	// Output a single serialised device state
	bool outputSerialisedState(SerialisedState& serialisedState, ostream& sout) override;

};

#endif