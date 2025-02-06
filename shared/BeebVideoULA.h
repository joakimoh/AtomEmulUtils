#ifndef BEEB_VDU_H
#define BEEB_VDU_H

#include <cstdint>
#include "VideoDisplayUnit.h"
#include "RAM.h"



class BeebVideoULA : public VideoDisplayUnit {

public:

	//
	// Video ULA
	// 

	// Video ULA Ports
	int DISEN, CURSOR, DIN, RIN, GIN;
	uint8_t mDISEN = 0x1;	// INPUT -	DISEN = ~(~DEN | RA3)
	uint8_t mCURSOR = 0x1;	// INPUT -	CURSOR from M6845
	uint8_t mDIN = 0xff;	// INPUT -	Data in
	uint8_t mRIN = 0x1;		// INPUT -	R from SA5050
	uint8_t mGIN = 0x1;		// INPUT -	G from SA5050
	uint8_t mBIN = 0x1;		// INPUT -	B from SA5050

	// Video ULA Registers
	uint8_t mControlRegister = 0x00;	// Video ULA base address + 0
	uint8_t mPaletteRegister = 0x00;	// Video ULA base address + 1

	//
	// SA5050 Teletext Character Generator
	// 

	ALLEGRO_BITMAP* mDisplayBitmap = NULL;
	ALLEGRO_LOCKED_REGION* mLockedDisplayBitMap;
	ALLEGRO_STATE mAllegroState;

	int mScanLine = 0; // 0 to 262 [field scan lines]
	int mFieldCount = 0;

	uint32_t mColours[2][4] = {
						{
							0xff00ff00, // opaque green ARGB 8888,
							0xffffff00, // opaque yellow ARGB 8888
							0xff0000ff, // opaque blue ARGB 8888
							0xffff0000 // opaque red ARGB 8888},
						},
							{
							0xfff0dc82, // opaque buff (brown) ARGB 8888
							0xff00ffff, // opaque cyan ARGB 8888,
							0xffff00ff, // opaque magenta ARGB 8888,
							0xffff4500 // opaque orange ARGB 8888
						}
	};

	bool readGraphicsMem(uint16_t adr, uint8_t& data);

public:

	bool getVisibleArea(int& w, int& h);

	ALLEGRO_COLOR green, black;

	BeebVideoULA(string name, uint16_t adr, double clockSpeed, ALLEGRO_BITMAP* disp, uint16_t videoMemAdr, DebugInfo debugInfo, ConnectionManager* connectionManager);
	~BeebVideoULA();

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