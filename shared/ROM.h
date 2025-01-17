#ifndef ROM_H
#define ROM_H

#include <cstdint>
#include "Device.h"
#include <string>
#include <vector>


using namespace std;

class ROM : public Device {

private:

public:


	ROM(string name, uint16_t adr, uint16_t sz, string binaryContent, DebugInfo debugInfo);

	bool read(uint16_t adr, uint8_t& data);
	bool write(uint16_t adr, uint8_t data);
};

#endif