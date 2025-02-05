#ifndef BEEB_VDU_H
#define BEEB_VDU_H

#include <cstdint>
#include "VideoDisplayUnit.h"
#include "RAM.h"



class BeebVDU : public VideoDisplayUnit {

public:

	//
	// M6845
	// 

	// M6845 Ports
	int DEN, RA, CURS, HS, VS, RESET;
	uint8_t mRESET = 0x1;	// INPUT
	uint8_t mDEN = 0x0;		// OUTPUT
	uint8_t mRA = 0x1f;		// OUTPUT - Raster Address for row of a character (5 bits)
	uint8_t mCURS = 0x0;	// OUTPUT - Cursor Display Indication
	uint8_t mHS = 0x0;		// OUTPUT -	Horizontal Sync
	uint8_t mVS = 0x0;		// OUTPUT -	Vertical Sync


	// M6845 Registers
	uint8_t mReg[18];
	uint8_t mAddressRegister = 0x0; // M6845 base address + 0	- Selects one of the 18 registers below (5 bits)
	enum M6845RegEnum {
		R0_HorizontalTotal = 0,		// M6845 base address + 1; AddressRegister = 0
		R1_HorizontalDisplayed = 1,	// M6845 base address + 1; AddressRegister = 1
		R2_HSYncPosition = 2,		// M6845 base address + 1; AddressRegister = 2
		R3_HSyncWidth = 3,			// M6845 base address + 1; AddressRegister = 3
		R4_VerticalTotal = 4,		// M6845 base address + 1; AddressRegister = 4
		R5_VerticalTotalAdjust = 5,	// M6845 base address + 1; AddressRegister = 5
		R6_VerticalDisplayed = 6,	// M6845 base address + 1; AddressRegister = 6
		mR7_VSyncPosition = 7,		// M6845 base address + 1; AddressRegister = 7
		R8_InterlaceMode = 8,		// M6845 base address + 1; AddressRegister = 8
		R9_MaxScanLineAddress = 9,	// M6845 base address + 1; AddressRegister = 9
		R10_CursorStart = 10,		// M6845 base address + 1; AddressRegister = 10
		R11_CursorEnd = 11,			// M6845 base address + 1; AddressRegister = 11
		R12_StartAddressH = 12,		// M6845 base address + 1; AddressRegister = 12
		R13_StartAddressL = 13,		// M6845 base address + 1; AddressRegister = 13
		R14_CursorH = 14,			// M6845 base address + 1; AddressRegister = 14
		R15_CursorL = 15,			// M6845 base address + 1; AddressRegister = 15
		R16_LightPenL = 16,			// M6845 base address + 1; AddressRegister = 16
		R17_LightPenH = 17			// M6845 base address + 1; AddressRegister = 17
	};

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

	// SA5050 Ports
	int R, G, B;
	uint8_t mR = 0x0; // RED out
	uint8_t mG = 0x0; // GREEN out
	uint8_t mB = 0x0; // BLUE out





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

	BeebVDU(string name, uint16_t adr, double clockSpeed, ALLEGRO_BITMAP* disp, uint16_t videoMemAdr, DebugInfo debugInfo, ConnectionManager* connectionManager);
	~BeebVDU();

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