#ifndef PIA8255_H
#define PIA8255_H

#include <cstdint>
#include <vector>
#include <string>
#include <allegro5/allegro.h>
#include "Device.h"
#include "AtomKeyboard.h"
#include "VDU6847.h"


using namespace std;

class PIA8255 : public Device {

private:

	AtomKeyboard			*mKeyboard = NULL;
	uint8_t					mSync60HzEvent = 0; // 60 Hz sync signal - high (1) or low (0)
	int						mN60HzCycles = 0;

	// PIA registers
	uint8_t mPortA, mPortB, mPortC, mCR;

	// Ports that can be connected to other devices
	enum PiaPorts {PIA_PORT_A = 0, PIA_PORT_B = 1, PIA_PORT_C = 2};



public:

	PIA8255(string name, uint16_t adr, int n60HzCycles, AtomKeyboard *keyboard, DebugInfo debugInfo, ConnectionManager *connectionManager);

	bool read(uint16_t adr, uint8_t& data);
	bool write(uint16_t adr, uint8_t data);

	// Reset device
	bool reset();

	// Advance until clock cycle stopcycle has been reached
	bool advance(uint64_t stopCycle);

	void updateInput(uint8_t port, uint8_t bit, uint8_t val);
};

#endif