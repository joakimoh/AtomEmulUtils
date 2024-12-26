#ifndef VDU6847_H
#define VDU6847_H

#include <cstdint>
#include "Device.h"

class VDU6847 : Device {

private:
	

public:

	VDU6847(uint16_t adr);

	bool read(uint16_t adr, uint8_t& data);
	bool write(uint16_t adr, uint8_t data);

};

#endif