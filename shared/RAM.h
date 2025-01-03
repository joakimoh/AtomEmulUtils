#ifndef RAM_H
#define RAM_H

#include <cstdint>
#include "Device.h"
#include <vector>


using namespace std;

class RAM : public Device {

private:

public:


	RAM(uint16_t adr, uint16_t sz, DebugInfo debugInfo);

	bool read(uint16_t adr, uint8_t& data);
	bool write(uint16_t adr, uint8_t data);
	bool write(uint16_t adr, vector<uint8_t>& data, uint16_t sz);
	bool read(uint16_t adr, vector<uint8_t>& data, uint16_t sz);
};

#endif