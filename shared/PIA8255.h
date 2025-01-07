#ifndef PIA8255_H
#define PIA8255_H

#include <cstdint>
#include <vector>
#include "Device.h"
#include "AtomKeyboard.h"
#include <allegro5/allegro.h>
#include "VDU6847.h"

using namespace std;

class PIA8255 : public Device {

private:

	AtomKeyboard *mKeyboard = NULL;
	VDU6847 *mVdu = NULL;
	uint8_t mSync60HzEvent = 0; // 60 Hz sync signal - high (1) or low (0)
	int mN60HzCycles = 0;

public:

	PIA8255(uint16_t adr, int n60HzCycles, AtomKeyboard *keyboard, DebugInfo debugInfo);

	bool read(uint16_t adr, uint8_t& data);
	bool write(uint16_t adr, uint8_t data);

	// Reset device
	bool reset();

	// Advance until clock cycle stopcycle has been reached
	bool advance(uint64_t stopCycle);

	void setVdu(VDU6847 *vdu);
};

#endif