#ifndef VIA6522_H
#define VIA6522_H

#include <cstdint>
#include "Device.h"

class VIA6522 : public Device {

private:

public:

	VIA6522(uint16_t adr, DebugInfo debugInfo);

	bool read(uint16_t adr, uint8_t& data);
	bool write(uint16_t adr, uint8_t data);

};

#endif