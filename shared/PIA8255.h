#ifndef PIA8255_H
#define PIA8255_H

#include <cstdint>
#include <vector>
#include "Device.h"
#include "AtomKeyboard.h"
#include <allegro5/allegro.h>

using namespace std;

class PIA8255 : public Device {

private:

	AtomKeyboard *mKeyboard = NULL;
	uint8_t mSync60HzState = 0; // 60 Hz sync signal - high (1) or low (0)

public:

	PIA8255(uint16_t adr, AtomKeyboard *keyboard, DebugInfo debugInfo);

	bool read(uint16_t adr, uint8_t& data);
	bool write(uint16_t adr, uint8_t data);

	void toggle60Hz();

};

#endif