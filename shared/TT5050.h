#ifndef TT_5050_H
#define TT_5050_H

#include <cstdint>
#include "Device.h"
#include "RAM.h"



class TT5050 : public Device {

public:

	//
	// SA5050 Teletext Character Generator
	// 


	// SA5050 Ports
	int R, G, B;
	uint8_t mR = 0x0; // RED out
	uint8_t mG = 0x0; // GREEN out
	uint8_t mB = 0x0; // BLUE out

public:

	ALLEGRO_COLOR green, black;

	TT5050(string name, uint16_t adr, double clockSpeed, ALLEGRO_BITMAP* disp, uint16_t videoMemAdr, DebugInfo debugInfo, ConnectionManager* connectionManager);

	// Reset device
	bool reset();

	// Advance until clock cycle stopcycle has been reached
	bool advance(uint64_t stopCycle);

	bool trigger(int port);

};

#endif