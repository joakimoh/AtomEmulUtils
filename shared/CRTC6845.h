#ifndef CRTC_6845_H
#define CRTC_6845_H

#include <cstdint>
#include "VideoDisplayUnit.h"
#include "RAM.h"
#include <cmath>


class CRTC6845 : public VideoDisplayUnit {

	//
	// This emulates the Hitachi HD6845SP used in e.g. the BBC Micro.
	// It is slightly different than the Motorola M6845 (R9 valuehas different meaning)
	// so be sure that it is the Hitachi one that you use in your system.
	//

public:

	//
	// M6845
	// 

	// M6845 Ports
	int CLK, DISPTMG, RA, CUDISP, HS, VS;
	uint8_t mCLK = 1;		// INPUT - Clock rate [MHz] (1 or 2 MHz for a BBC Micro Model B e.g.)
	uint8_t pCLK = 1;
	uint8_t mNEXT_CHAR;		// INPUT  - Advance one character
	uint8_t mDISPTMG = 0x0;	// OUTPUT - DISPlay TiMinG: When high, the display is in the active area (delay specified by R9 skew bits)
	uint8_t mRA = 0x0;		// OUTPUT - Raster Address for row of a character (5 bits)
	uint8_t mCUDISP = 0x0;	// OUTPUT - CUrsor DISPlay: High when cursor shall be displayed (delay specified by R9 skew bits)
	uint8_t mHS = 0x0;		// OUTPUT -	Horizontal Sync
	uint8_t mVS = 0x0;		// OUTPUT -	Vertical Sync


	// M6845 Registers
	uint8_t mReg[18];
	int mRegWriteCnt[16] = { 0 };
	int mRegUpdates = 0;
	typedef struct RegInfo {
		uint8_t	mask;
		bool	writable;
		bool	readable;
	} RegInfo;

	const RegInfo mRegInfo[18] = {
		{0xff, true, false},	// R0 -		HorizontalTotal (8 bits)
		{0xff, true, false},	// R1 -		HorizontalDisplayed (8 bits)
		{0xff, true, false},	// R2 -		HSYncPosition (8 bits)
		{0xff, true, false},	// R3 -		SyncWidth (8 bits)
		{0x7f, true, false},	// R4 -		VerticalTotal
		{0x1f, true, false},	// R5 -		VerticalTotalAdjust
		{0x7f, true, false},	// R6 -		VerticalDisplayed
		{0x7f, true, false},	// R7 -		VSyncPosition
		{0xf3, true, false},	// R8 -		InterlaceAndSkew
		{0x1f, true, false},	// R9 -		MaxScanLineAddress
		{0x7f, true, false},	// R10 -	CursorStartRaster
		{0x1f, true, false},	// R11 -	CursorEndraster
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
		R2_HSYncPosition = 2,		// HSYncPosition		Horizontal sync pos - 1
		R3_SyncWidth = 3,			// SyncWidth			Width of horizontal sync pulse (b3:b0 1-15) and vertical syn pulse (b7:b4 1-16; 0<=>16)
		R4_VerticalTotal = 4,		// VerticalTotal		Integer part of no of character lines - 1
		R5_VerticalTotalAdjust = 5,	// VerticalTotalAdjust	Fraction part in unit scan lines
		R6_VerticalDisplayed = 6,	// VerticalDisplayed	No of visible char rows	
		R7_VSyncPosition = 7,		// VSyncPosition		Vertical sync pos in char row
		R8_InterlaceMode = 8,		// InterlaceAndSkew		b1b0: Raster scan mode
									//						b1b0:	VS
									//								*0: non-interlaced
									//								01: interlace sync
									//								11: interlace sync  & video
									//						b5b4:	d1d0 (DISPTMG)
									//								00	No Skew
									//								01	One char skew
									//								10	Two char skew
									//								11	No output
									//						b7b6:	c1c0 (CUDSIP)
									//								00	No Skew
									//								01	One char skew
									//								10	Two char skew
									//								11	No output

		R9_MaxScanLineAddress = 9,	// MaxScanLineAddress	Scan lines/char row - 1	(Scan line/char row must be an even no)*
		R10_CursorStart = 10,		// CursorStartRaster	
									// b4:b0	cursor start raster address (0-31)
									//	b6b5:	BP 
									//			00		No blink
									//			01		Curssor non-display
									//			10		Blink 16 feild period
									//			11		Blink 32 field period
		R11_CursorEnd = 11,			// CursorEndRaster
									// b4:b0 cursor end raster line (0-31)
		R12_StartAddressH = 12,		// StartAddressH		Start refresh address after vertical blanking
		R13_StartAddressL = 13,		// StartAddress		-""-
		R14_CursorH = 14,			// CursorH				Cursor location (14 bits <=> 16K positions) -  display address of the char cell holding the cursor
		R15_CursorL = 15,			// CursorL				-"--
		R16_LightPenL = 16,			// LightPenH			Value of StartAddress when light pen is detected
		R17_LightPenH = 17			// LightPenL			-""-
		// In the Interlace Sync & Video Mode, the register instead holds scan lines/char row -2
	};

	int mCharRow = 0;
	int mCharCol = 0;	

	int mStartAdr = 0x0;
	int mCursorLocation = 0x0;
	
	int mCharLines = 1;
	int mUniqueCharLines = 1; // the no of raster lines per character row that are unique per pair of fields (i.e one frame)


	// Vertical scan lines: top border, active lines, sync pulse, bottom border (all in unit 'line')
	int mTopBorderRows = 0;
	int mTopBorderLines = 0;
	int mActiveLines = 1; // A multiple of mCharLines
	int mScreenActiveLines = 1;
	int mFieldActiveLines = 1;
	int mActiveRows = 1;
	int mVSyncRow = 1;
	int mVSyncLine = 1;
	int mVSyncPulseH = 1;
	int mBottomBorderLines = 0;
	int mVisibleScanLines = 1; // mTopBorderLines + mActiveLines + mBottomBorderLines
	int mCharRows = 1;
	int mFieldCharRows = 1;

	int mFieldVSyncRow = 1;
	int mFieldVSyncLine = 1;

	int mFieldActiveRows = 1;
	int mVFieldSyncPulseH = 1;
	int mScreenVSyncLine = 1;
	int mScreenVSyncPulseH = 1;

	double mScanLines = 1;
	double mFieldScanLines = 1;
	int mScreenScanLines = 1; // the no of scan lines that are unique per pair of field (ie. one frame) (should be ~625 for PAL and ~525 for NTSC)
	int mRetraceLines = 1;

	// Horizontal line: left border, active chars, sync pulse, right border (all in unit 'char')
	int mLeftBorderChars = 0;
	int mActiveRowChars = 1;
	int mHzSyncPos = 1;
	int mHzSyncPulseW = 1;
	int mRightBorderChars = 0;
	int mVisibleChars = 1; // mLeftBorderChars + mActiveChars + mRightBorderChars
	int mCharCols = 1;
	int mRetraceChars = 1;
	int mCharSkew = 0;
	int mCursSkew = 0;
	int mField = 0;
#define _INTERLACE_MODE(x) ((x&1)==0?"Non - interlaced":((x&3)==3?"Interlaced & Video":"Interlaced"))
	inline bool non_interlaced(int m) { return ((m & 0x1) == 0x0); }
	inline bool interlaced(int m) { return ((m & 0x3) == 0x1); }
	inline bool interlaced_video(int m) { return ((m & 0x3) == 0x3); }
#define INTERLACED			2
#define INTERLACED_VIDEO	3
	int mInterlaceMode = 0;

	int mInitialised = false;

public:

	bool getVisibleCharArea(int& charsPerLine, int& lines);
	bool getVisibleArea(int& w, int& h) {
		// Not supported as cannot be calculated by the M6845 iself
		return false;
	}

	void updateSettings(uint8_t reg);
	void printSettings();

	ALLEGRO_COLOR green, black;

	CRTC6845(string name, uint16_t adr, double cpuclock, uint8_t waitStates, ALLEGRO_BITMAP* disp, int dispW, int dispH,
		DebugManager  *debugManager, ConnectionManager* connectionManager);
	~CRTC6845() {}

	bool read(uint16_t adr, uint8_t& data);
	bool dump(uint16_t adr, uint8_t& data) override;
	bool write(uint16_t adr, uint8_t data);

	inline double getScanLineDuration();
	inline double getScanLinesPerField();
	inline double getScreenScanLines();
	inline double getFieldRate();
	inline int getCharScanLines();
	inline int getUniqueCharScanLines();
	inline int getVerticalSyncLine();
	inline int getHorizontalSyncPos();
	inline int getCharsPerLine();
	inline int getVisibleCharsPerLine();
	inline int getScanLine() { return mScanLine;  }

	inline int getLeftBorderChars();
	inline int getTopBorderLines();
	inline int getActiveChars();
	inline int getActiveLines();
	inline int getRightBorderChars();
	inline int getBottomBorderLines();
	inline int getRetraceLines();
	inline int getRetraceChars();
	inline void getSkew(int& displaySkew, int& cursorSkew) { displaySkew = mCharSkew; cursorSkew = mCursSkew; }

	//
	// Interlace-related methods
	//

	// Check if interlace is enabled (On)
	inline bool interlaceOn();

	// Get scan line offset (0 for even field or non-interlaced mode, 1 for odd field)
	inline int fieldScanLineOffset();

	// Advance 1/2 scan line - required for interlace modes as
	// each field is usally 312 1/2 (PAL) or 262 1/2 (NTSC) scan lines
	// to get 625 (PAL) or 525 (NTSC) scan lines per frame (i.e., a pair of even and odd fields)
	// at 50 Hz (PAL) or 60 Hz (NTSC).
	bool advanceHalfLine(uint64_t& endCycle) { return true; }

	// Reset device
	bool reset();

	// Advance until clock cycle stopcycle has been reached
	bool advance(uint64_t stopCycle);
	bool advanceChar();
	bool updateOutputs();

	// Advance line is not applicable
	bool advanceLine(uint64_t& endCycle) { return true; }

	// Called by other device to get next memory address to fetch char/graphics data from
	bool getMemFetchAdr(uint16_t& adr);

	bool initialised() { return mInitialised; }

	// Process a port update directly (and not just next time the advance() method is called)
	void processPortUpdate(int index) override;

};

#endif


