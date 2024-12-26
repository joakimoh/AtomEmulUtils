#ifndef PIA8255_H
#define PIA8255_H

#include <cstdint>
#include <vector>
#include "Device.h"

using namespace std;

class PIA8255 : Device {

private:

public:

	PIA8255(uint16_t adr);

	bool read(uint16_t adr, uint8_t& data);
	bool write(uint16_t adr, uint8_t data);

};

#endif