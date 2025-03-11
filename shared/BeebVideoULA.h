#ifndef BEEB_VDU_H
#define BEEB_VDU_H

#include <cstdint>
#include "VideoDisplayUnit.h"
#include "RAM.h"
#include "TT5050.h"
#include "CRTC6845.h"
#include <cmath>



class BeebVideoULA : public VideoDisplayUnit {

private:

	void updateScreenSz();

public:

	//
	// Video ULA
	// 

	// Video ULA Ports
	int DISPTMG, CURSOR, INV, RA, HS, VS, CRTC_CLK, SCROLL_CTRL;
	uint8_t mSCROLL_CTRL = 0x0;		// INPUT - C0 & C1 selected by VIA PB Port <=> decides hardware scroll address
	uint8_t mC = 0x0;		// C1:C0 - set based on SCROLL_CTRL input
	uint8_t mCRTC_CLK = 1;	// OUTPUT - Clock to CRTC M6845 (1 or 2 MHz)
	uint8_t mDISPTMG = 0x1;	// INPUT -	DISPTMG from the CRTC 6845; actual display enable DISEN is calculated as = ~(~DISPTMG | RA3)
	uint8_t mCURSOR = 0x0;	// INPUT -	CURSOR from M6845
	uint8_t mINV = 0x0;		// INPUT - invert video
	uint8_t mRA = 0x0;		// INPUT - raster address (4 bits); used for modes 0-6 to select bytes within an 8 row pixel block

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
		return (mControlRegister >> mCRField[field].lowBit) & mCRField[field].mask; }
	uint8_t mPaletteMem[16] = { 0 };	// 16 x 4 bit palette memory

	//
	// SA5050 Teletext Character Generator
	// 

	ALLEGRO_BITMAP* mDisplayBitmap = NULL;
	ALLEGRO_LOCKED_REGION* mLockedDisplayBitMap;
	ALLEGRO_STATE mAllegroState;

	int mScanLine = 0;			// Current scan line
	int mScanLines = 312;		// Scan lines per frame
	int mFrame = 0;				// Frame count
	int mNCols = 0;				// No of visible columns
	int mCursorSegment = -1;	// The current cursor segment being drawn (0-2 when active)
	int mVerticalSyncPos = 0;	// Vertical sync pos (in scan lines)
	int mScreenW = 640;			// Visible screen area (including borders)
	int mScreenH = 256;			//

	uint8_t mPixelW = 1;
	int mPixelRate = 1;
	int mPixelsPerByte = 8;	// The no of pixels per byte for modes 0-6 (8 for 2-colour, 4 for 4-colour and 2 for 6-colour)

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

	CRTC6845* mCRTC = NULL;
	TT5050* mTGC = NULL;

public:

	bool getVisibleArea(int& w, int& h);

	ALLEGRO_COLOR green, black;

	BeebVideoULA(string name, uint16_t adr, double cpuclock, ALLEGRO_BITMAP* disp, int dispW, int dispH, DebugManager  *debugManager, ConnectionManager* connectionManager);
	~BeebVideoULA();

	bool read(uint16_t adr, uint8_t& data);
	bool write(uint16_t adr, uint8_t data);

	inline double getScanLineDuration();
	inline double getScanLinesPerFrame();
	inline double getFrameRate();
	inline int getCharScanLines();
	inline int getVerticalSyncLine();
	inline int getHorizontalSyncPos();
	inline int getCharsPerLine();
	inline int getVisibleCharsPerLine();
	inline int getScanLine();
	inline int getLeftBorderChars();
	inline int getTopBorderLines();
	inline int getActiveChars();
	inline int getActiveLines();
	inline int getRightBorderChars();
	inline int getBottomBorderLines();
	inline int getRetraceLines();
	inline int getRetraceChars();


	// Reset device
	bool reset();

	// Advance until clock cycle stopcycle has been reached
	bool advance(uint64_t stopCycle);

	bool advanceLine(uint64_t& endCycle);

	// Get pointer to other device to be able to call its methods
	bool connectDevice(Device* dev);


	bool initialised() {
		return mCRTC != NULL && mTGC != NULL && mCRTC->initialised();
	}

};

#endif