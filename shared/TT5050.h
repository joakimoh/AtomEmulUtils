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

	bool readGraphicsMem(uint16_t adr, uint8_t& data);

public:

	bool getVisibleArea(int& w, int& h);

	ALLEGRO_COLOR green, black;

	TT5050(string name, uint16_t adr, double clockSpeed, ALLEGRO_BITMAP* disp, uint16_t videoMemAdr, DebugInfo debugInfo, ConnectionManager* connectionManager);
	~TT5050();

	bool read(uint16_t adr, uint8_t& data);
	bool write(uint16_t adr, uint8_t data);

	inline double getScanLineDuration();
	inline int getScanLinesPerFrame();
	inline int getFrameRate();


	// Reset device
	bool reset();

	// Advance until clock cycle stopcycle has been reached
	bool advance(uint64_t stopCycle);

	bool advanceLine(uint64_t& endCycle);



	void lockDisplay();
	void unlockDisplay();

};

#endif