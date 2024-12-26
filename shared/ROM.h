#ifndef ROM_H
#define ROM_H

#include <cstdint>
#include "Device.h"
#include <string>
#include <vector>


using namespace std;

class ROM : Device {

private:

public:


	ROM(uint16_t adr, uint16_t sz, string binaryContent);

	bool read(uint16_t adr, uint8_t& data);
	bool write(uint16_t adr, uint8_t data);
};

#endif