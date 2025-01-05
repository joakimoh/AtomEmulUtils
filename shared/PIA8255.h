#ifndef PIA8255_H
#define PIA8255_H

#include <cstdint>
#include <vector>
#include "Device.h"
#include "AtomKeyBoard.h"

using namespace std;

class PIA8255 : public Device {

private:

	AtomKeyBoard *mKeyBoard = NULL;

public:

	PIA8255(uint16_t adr, AtomKeyBoard *keyboard, DebugInfo debugInfo);

	bool read(uint16_t adr, uint8_t& data);
	bool write(uint16_t adr, uint8_t data);

};

#endif