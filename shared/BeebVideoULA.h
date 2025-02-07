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
	int DISEN, CURSOR, CRTC_DIN, TCG_DIN, CRTC_DREQ, TCG_DREQ, RIN, GIN, BIN, N_CHARS, SL_DUR, INV, VS;
	int FR, SL_L, SL_H;
	uint8_t mDISEN = 0x1;	// INPUT -	DISEN = ~(~DEN | RA3)
	uint8_t mCURSOR = 0x1;	// INPUT -	CURSOR from M6845
	uint8_t mCRTC_DIN = 0xff;	// INPUT -	Data in from the CRTC (M6845)
	uint8_t mTCG_DIN = 0xff;	// INPUT -	Data in from the Teletext Character Generator (TGC - SA5050)
	uint8_t mCRTC_DREQ = 0x0;	// OUTPUT - Request data from the CRTC
	uint8_t mTCG_DREQ = 0x0;	// OUTPUT - Request data from the TCG
	uint8_t mRIN = 0x1;		// INPUT -	R from SA5050
	uint8_t mGIN = 0x1;		// INPUT -	G from SA5050
	uint8_t mBIN = 0x1;		// INPUT -	B from SA5050
	uint8_t mN_CHARS = 0x0;	// INPUT - no of chars/scan line as provided by the CRTC
	uint8_t mSL_DUR = 64;	// INPUT - scan line duration in us as provied by the CRTC
	uint8_t mINV = 0x0;		// INPUT - invert video
	uint8_t mVS = 0x0;		// INPUT - vertical sync
	uint8_t mFR = 0x0;		// INPUT - frame rate [Hz]
	uint8_t mSL_L = 0x0;	// INPUT - No scan lines
	uint8_t mSL_H = 0x0;	// INPUT - 

	// Video ULA Registers
	uint8_t mControlRegister = 0x00;	// Video ULA base address + 0
	uint8_t mPaletteMem[16] = { 0 };	// 16 x 4 bit palette memory

	//
	// SA5050 Teletext Character Generator
	// 

	ALLEGRO_BITMAP* mDisplayBitmap = NULL;
	ALLEGRO_LOCKED_REGION* mLockedDisplayBitMap;
	ALLEGRO_STATE mAllegroState;

	int mScanLine = 0;			//  [field scan lines]
	int mFieldCount = 0;
	int mNCols = 0;				// No of visible columns
	int mCursorSegment = -1;	// The current cursor segment being drawn (0-2 when active)

	double mCPUClock = 2.0; // [MHz]
	double CRTCClock = 1.0; // [MHz]
	uint8_t mPixelW = 1;

	bool mNewFrame = false;

	uint32_t mColours[8] = {
		0xff000000,	// 0	Black
		0xffff0000,	// 1	Red
		0xff00ff00, // 2	Green
		0xffffff0,	// 3	Yellow
		0xff0000ff,	// 4	Blue
		0xffff00ff,	// 5	Magenta
		0xff00ffff,	// 6	Cyan
		0xffffffff	// 7	White
		// 8	Flashing Black/White
		// 9	Flashing Red/Cyan
		// 10	Flashing Green/Magenta
		// 11	Flashing Yellow/Blue
		// 12	Flashing Blue/Yellow
		// 13	Flashing Magenta/Green
		// 14	Flashing Cyan/Red
		// 15	Flashing White/Black
	};

	bool readGraphicsMem(uint16_t adr, uint8_t& data);

	void lockDisplay();
	void unlockDisplay();

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

};

#endif