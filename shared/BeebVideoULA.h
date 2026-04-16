#ifndef BEEB_VDU_H
#define BEEB_VDU_H

#include <cstdint>
#include "VideoDisplayUnit.h"
#include "RAM.h"
#include "SAA5050.h"
#include "CRTC6845.h"
#include <cmath>
#include <chrono>
#include "TimedDevice.h"


#define BITMAP_PTR unsigned int *
#define LOCKED_BITMAP_PTR char *

class BeebVideoULA : public VideoDisplayUnit, public TimedDevice {

private:

	int mDispUsCnt = 0;
	int mPreludeUsCnt = 0;
	int mVideoULACnt = 0;
	int mCRTCnt = 0;
	int mTTCnt = 0;
	int mCharPixelCnt = 0;
	int mBorderCnt = 0;
	int mReadCnt = 0;
	int mLineCnt = 0;

	BITMAP_PTR mMaxDisplayBitmap_p = NULL;

	int mBitMapFlags = 0;

	bool readGraphicsMem(BusAddress adr, uint8_t& data) override;

public:

	//
	// Video ULA
	// 

	// Video ULA Ports
	int DISPTMG, CURSOR, INV, RA, HS, VS, CRTC_CLK, C;// SCROLL_CTRL;
	PortVal mVS = 0x0;
	PortVal mHS = 0x0;
	//uint8_t mSCROLL_CTRL = 0x0;		// INPUT - C0 & C1 selected by VIA PB Port <=> decides hardware scroll address
	PortVal mC = 0x0;		// C1:C0 - set based on SCROLL_CTRL input
	AnaloguePortVal mCRTC_CLK = 1;	// OUTPUT - Clock to CRTC M6845 (1 or 2 MHz)
	PortVal mDISPTMG = 0x1;	// INPUT -	DISPTMG from the CRTC 6845; actual display enable DISEN is calculated as = ~(~DISPTMG | RA3)
	PortVal mCURSOR = 0x0;	// INPUT -	CURSOR from M6845
	PortVal mINV = 0x1;		// INPUT - invert video (HIGH <=> Not inverted)
	PortVal mRA = 0x0;		// INPUT - raster address (4 bits); used for modes 0-6 to select bytes within an 8 row pixel block

	// Video ULA Registers
	uint8_t mControlRegister = 0x00;	// Video ULA base address + 0
	enum CRFieldEnum { CR_CURSOR_SEGMENT = 4, CR_CLOCK_RATE = 3, CR_N_COLS = 2, CR_TELETEXT = 1, CR_FLASH = 0 };
	typedef struct CRField_struct {
		uint8_t mask;
		uint8_t lowBit;
		CRFieldEnum field;
	} CRField;
	
	CRField mCRField[5] = {
		{0x1, 0, CR_FLASH},
		{0x1, 1, CR_TELETEXT},
		{0x3, 2, CR_N_COLS},
		{0x1, 4, CR_CLOCK_RATE},
		{0xe, 5, CR_CURSOR_SEGMENT}
	};
	inline uint8_t getCRField(CRFieldEnum field) {
		return (getCRField(mControlRegister, field));
	}
	inline uint8_t getCRField(uint8_t regVal, CRFieldEnum field) {
		return (regVal >> mCRField[field].lowBit) & mCRField[field].mask;
	}
	uint8_t mPaletteMem[16] = { 0 };	// 16 x 4 bit palette memory

	ALLEGRO_LOCKED_REGION* mLockedDisplayBitMap;
	ALLEGRO_STATE mAllegroState;

	int mScanLine = 0;			// Current scan line
	int mScreenScanLines = 312;	// Scan lines per field
	int mCursorSegment = -1;	// The current cursor segment being drawn (0-2 when active)
	int mScreenW = 640;			// Visible screen area (including borders)
	int mScreenH = 256;			//

	uint8_t mPixelW = 1;
	int mPixelsPerCharacter = 8;	// The no of "big" pixels per byte for modes 0-6 (8 for 2-colour, 4 for 4-colour and 2 for 6-colour)
	int mHzPixelsPerSymbol = 8;		// The no of horizontal screen pixels per symbol (8 for mode 0-6, 16 for mode 7)
	int mHzChars = 0;
	int mHzVisibleChars = 0;
	int mHzVisiblePixels = 0;
	int mHzNonCharVisiblePixels = 0;

	bool mTeletextEnabled = false;

	// Data related to the rendering of a single 'character'
	int mLineRenderedPixels = 0;
	int mRenderedPixels = 0;
	int mCharPos = 0;
	int mCycleCountLineRef = 0;
	int mVisibleCharPos = 0;
	int mActiveCharPos = 0;
	int mHzVisibleCharOffset = 0;


	// Data related to a complete scan line
	int mAdjustedScanLine = 0;
	int mVisibleScanLine = 0;
	bool mAddHalfLine = false;
	bool mNewLine = false;

	// Data related to a complete field
	int mOddField = 0;
	int mDisEna = 0;
	int mFieldOffset = 0;
	bool mNewField = false;

	BITMAP_PTR mLineBitmapDataP = NULL;


	uint8_t mCursorSegments = 0;
	bool mClk2mHz = 0;
	int mPixelRate = 1;

	// Display sizes
	int mVisibleLines;
	int mVisibleLineOffset;

	uint16_t mHwScrollSub = 0x0;

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

	CRTC6845* mCRTC = NULL;
	SAA5050* mTGC = NULL;

	SAA5050::TTColour mFgColour = { 0 };
	SAA5050::TTColour mBgColour = { 0 };
	SAA5050::ScreenDataType* mOldTgcData = nullptr;
	SAA5050::ScreenDataType* mNewTgcData = nullptr;
	bool mValidTgcData = false;

	

	void lockDisplay();
	void unlockDisplay();

	bool advanceChar(uint64_t& endCycle);

	bool updateInternalState(uint8_t newControlRegisterValue);
	void updateHwScrollConstant();
	void calcLineSettings();
	bool addHalfLine(uint64_t &endCycle);


public:

	ALLEGRO_COLOR green, black;

	BeebVideoULA(string name, BusAddress adr, Display *display, double tickRate, uint8_t accessSpeed,
		DebugTracing  *debugTracing, ConnectionManager* connectionManager, DeviceManager* deviceManager);
	~BeebVideoULA();

	bool readByte(BusAddress adr, BusByte& data);
	bool dump(BusAddress adr, uint8_t& data) override;
	bool writeByte(BusAddress adr, BusByte data);

	// Device power on
	bool power();

	// Advance until time tickTime
	bool advanceUntil(uint64_t tickTime) override;

	// Get pointer to other device to be able to call its methods
	bool connectDevice(Device* dev);

	// Process a port update directly (and not just next time the advanceUntil() method is called)
	void processPortUpdate(int port) override;

	// Outputs the internal state of the device
	bool outputState(ostream& sout) override;

};

#endif
