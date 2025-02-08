#ifndef CRTC_6845_H
#define CRTC_6845_H

#include <cstdint>
#include "VideoDisplayUnit.h"
#include "RAM.h"



class CRTC6845 : public VideoDisplayUnit {

public:

	//
	// M6845
	// 

	// M6845 Ports
	int CLK, DEN, RA, CURS, HS, VS, RESET;
	uint8_t mCLK = 1.0;			// INPUT - Clock rate [MHz] (1 or 2 MHz for a BBC Micro Model B e.g.)
	uint8_t mNEXT_CHAR;		// INPUT  - Advance one character
	uint8_t mRESET = 0x1;	// INPUT
	uint8_t mDEN = 0x0;		// OUTPUT - Display ENable: When high, the display is in the active area
	uint8_t mRA = 0x1f;		// OUTPUT - Raster Address for row of a character (5 bits)
	uint8_t mCURS = 0x0;	// OUTPUT - Cursor Display Indication
	uint8_t mHS = 0x0;		// OUTPUT -	Horizontal Sync
	uint8_t mVS = 0x0;		// OUTPUT -	Vertical Sync


	// M6845 Registers
	uint8_t mReg[18];
	typedef struct RegInfo {
		uint8_t	mask;
		bool	writable;
		bool	readable;
	} RegInfo;

	const RegInfo mRegInfo[18] = {
		{0xff, true, false},	// R0 -		HorizontalTotal
		{0xff, true, false},	// R1 -		HorizontalDisplayed
		{0xff, true, false},	// R2 -		HSYncPosition
		{0x0f, true, false},	// R3 -		HSyncWidth
		{0x7f, true, false},	// R4 -		VerticalTotal
		{0x1f, true, false},	// R5 -		VerticalTotalAdjust
		{0x1f, true, false},	// R6 -		VerticalDisplayed
		{0x7f, true, false},	// R7 -		VSyncPosition
		{0x03, true, false},	// R8 -		InterlaceMode
		{0x1f, true, false},	// R9 -		MaxScanLineAddress
		{0x7f, true, false},	// R10 -	CursorStart
		{0x1f, true, false},	// R11 -	CursorEnd
		{0x3f, true, false},	// R12 -	StartAddressH	
		{0xff, true, false},	// R13 -	StartAddress
		{0x3f, true, true},		// R14 -	CursorH
		{0xff, true, true},		// R15 -	CursorL
		{0x3f, false, true},	// R16 -	LightPenH	
		{0xff, false, true}		// R17 -	LightPenL
	};
	uint8_t mAddressRegister = 0x0; // M6845 base address + 0	- Selects one of the 18 registers below (5 bits)
	enum M6845RegEnum {
		R0_HorizontalTotal = 0,		// HorizontalTotal		Horizontal frequency HS; Displayed + non-displayed chars per line - 1 (chars/line must be even)
		R1_HorizontalDisplayed = 1,	// HorizontalDisplayed	Visible chars per line
		R2_HSYncPosition = 2,		// HSYncPosition		Horizontal sync pos
		R3_HSyncWidth = 3,			// HSyncWidth			Width of horizontal sync pulse
		R4_VerticalTotal = 4,		// VerticalTotal		Integer part of no of scan lines - 1
		R5_VerticalTotalAdjust = 5,	// VerticalTotalAdjust	Fraction part of no of scan lines
		R6_VerticalDisplayed = 6,	// VerticalDisplayed	No of visible char rows	
		mR7_VSyncPosition = 7,		// VSyncPosition		Vertical sync pos in char row
		R8_InterlaceMode = 8,		// InterlaceMode		Raster scan mode (*0: non-interlaces, 01: interlaced, 11: interlaced & video)
		R9_MaxScanLineAddress = 9,	// MaxScanLineAddress	Scan lines/char row - 1	(Scan line/char row must be an even no)
		R10_CursorStart = 10,		// CursorStart			b6: enable blink, b5: blink rate (0:1/16 FR,1:1/32 FR), b4:0: cursor start line - FR = Field Rate
		R11_CursorEnd = 11,			// CursorEnd			Cursor end scan line - relative position (0-31 start & send both odd or even for mode 01 & 11)
		R12_StartAddressH = 12,		// StartAddressH		Start refresh address after vertical blanking
		R13_StartAddressL = 13,		// StartAddress		-""-
		R14_CursorH = 14,			// CursorH				Cursor location (14 bits <=> 16K positions) -  display address of the char cell holding the cursor
		R15_CursorL = 15,			// CursorL				-"--
		R16_LightPenL = 16,			// LightPenH			Value of StartAddress when light pen is detected
		R17_LightPenH = 17			// LightPenL			-""-
	};

	int mCharRow = 0;
	int mCharCol = 0;
	int mVisibleLines = 0;
	double mScanLines = 0;

	double mCPUClock = 2.0;

public:

	bool getVisibleCharArea(int& w, int& h);
	bool getVisibleArea(int& w, int& h) {
		return getVisibleCharArea(w, h);
	}

	ALLEGRO_COLOR green, black;

	CRTC6845(string name, uint16_t adr, double clockSpeed, ALLEGRO_BITMAP* disp, uint16_t videoMemAdr, DebugInfo debugInfo, ConnectionManager* connectionManager);
	~CRTC6845() {}

	bool read(uint16_t adr, uint8_t& data);
	bool write(uint16_t adr, uint8_t data);

	inline double getScanLineDuration();
	inline double getScanLinesPerFrame();
	inline double getFrameRate();
	inline int getCharScanLines();


	// Reset device
	bool reset();

	// Advance until clock cycle stopcycle has been reached
	bool advance(uint64_t stopCycle);

	// Advance line is not applicable
	bool advanceLine(uint64_t& endCycle) { return true; }

	// Called by other device to trigger the output of new data
	bool updateDataOutput(uint8_t& data);

};

#endif


