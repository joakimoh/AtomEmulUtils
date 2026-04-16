#ifndef CRTC_6845_H
#define CRTC_6845_H

#include <cstdint>
#include "MemoryMappedDevice.h"
#include "RAM.h"
#include <cmath>
#include <vector>
#include "ClockedDevice.h"


class CRTC6845 : public MemoryMappedDevice, public ClockedDevice {

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
	double mCLK = 1;		// INPUT - Clock rate [MHz] (1 or 2 MHz for a BBC Micro Model B e.g.)
	double pCLK = 1;
	PortVal mNEXT_CHAR;		// INPUT  - Advance one character
	PortVal mDISPTMG = 0x0;	// OUTPUT - DISPlay TiMinG: When high, the display is in the active area (delay specified by R9 skew bits)
	PortVal mRA = 0x0;		// OUTPUT - Raster Address for row of a character (5 bits)
	PortVal mCUDISP = 0x0;	// OUTPUT - CUrsor DISPlay: High when cursor shall be displayed (delay specified by R9 skew bits)
	PortVal mHS = 0x0;		// OUTPUT -	Horizontal Sync
	PortVal mVS = 0x0;		// OUTPUT -	Vertical Sync


	// M6845 Registers
	uint8_t mReg[18];
	int mRegWriteCnt[16] = { 0 };
	int mRegUpdates = 0;
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
		R13_StartAddressL = 13,		// StartAddressL		-""-
		R14_CursorH = 14,			// CursorH				Cursor location (14 bits <=> 16K positions) -  display address of the char cell holding the cursor
		R15_CursorL = 15,			// CursorL				-"--
		R16_LightPenL = 16,			// LightPenH			Value of StartAddress when light pen is detected
		R17_LightPenH = 17			// LightPenL			-""-
		// In the Interlace Sync & Video Mode, the register instead holds scan lines/char row -2
	};

	typedef struct RegInfo {
		uint8_t	mask;
		bool	writable;
		bool	readable;
		string	descShort;
		string	descLong;
	} RegInfo;

	const RegInfo mRegInfo[18] = {
		{0xff, true, false, "R0  HorizontalTotal    ", "b7:b0       Total horizontal chars per line - 1 (must be even for interlaced modes)"},
		{0xff, true, false, "R1  HorizontalDisplayed", "b7:b0       Visible chars per line"},
		{0xff, true, false, "R2  HSYncPosition      ", "b7:b0       Horizontal sync pos - 1"},
		{0xff, true, false, "R3  SyncWidth          ", "b7:b0       Width of horizontal sync pulse(b3:b0 1-15) and vertical sync pulse(b7:b4 1-16; 0 <=> 16)"},
		{0x7f, true, false, "R4  VerticalTotal      ", "b6:b0       Integer part of no of character lines - 1"},
		{0x1f, true, false, "R5  VerticalTotalAdjust", "b4:b0       Fraction part in unit scan lines"},
		{0x7f, true, false, "R6  VerticalDisplayed  ", "b6:b0       No of visible char rows"},
		{0x7f, true, false, "R7  VSyncPosition      ", "b6:b0       Vertical sync pos in char row"},
		{0xf3, true, false, "R8  InterlaceAndSkew   ", "b7:b4,b1:b0 b1b0 : mode(*0:non-interlaced,01:sync,11:video), b54 : char skew, b7b6 : cursor skew"},
		{0x1f, true, false, "R9  MaxScanLineAddress ", "b4:b0       Scan lines / char row - 1"},
		{0x7f, true, false, "R10 CursorStartRaster  ", "b6:b0       b4:b0: address(0:31), b6b5 : blinking"},
		{0x1f, true, false, "R11 CursorEndRaster    ", "b4:b0       b4:b0: address(0 - 31)"},
		{0x3f, true, false, "R12 StartAddressH      ", "b5:b0       Start refresh address after vertical blanking - MSB"},	
		{0xff, true, false, "R13 StartAddressL      ", "b7:b0       Start refresh address after vertical blanking - LSB"},
		{0x3f, true, true,  "R14 CursorH            ", "b5:b0       Cursor location(14 bits <=> 16K positions) - MSB"},
		{0xff, true, true,  "R15 CursorL            ", "b7:b0       Cursor location(14 bits <=> 16K positions) - LSB"},
		{0x3f, false, true, "R16 LightPenH          ", "b5:b0       Value of StartAddress when light pen is detected - MSB"},	
		{0xff, false, true, "R17 LightPenL          ", "b7:b0       Value of StartAddress when light pen is detected - LSB"}		
	};

	int mCharRow = 0;
	int mCharCol = 0;	

	int mStartAdr_R12_R13 = 0x0;
	int mCursorLocation_R14_R15 = 0x0;
	
	int mCharLines_R9 = 1;
	int mScreenRasterLines = 1;


	// Vertical scan lines: top border, active lines, sync pulse, bottom border (all in unit 'line')
	int mActiveLines_R6xR9 = 1; // A multiple of mCharLines_R9
	int mScreenActiveLines = 1;
	int mActiveRows_R6 = 1;
	int mVSyncRow_R7 = 1;
	int mVSyncLine_R7xR9 = 1;
	int mVSyncPulseH_R3 = 1;
	int mCharRows_R4 = 1;

	int mScreenVSyncLine = 1;
	int mScreenVSyncPulseH = 1;

	int mScreenScanLines = 1; // the no of scan lines that are unique per pair of field (ie. one frame) (should be ~625 for PAL and ~525 for NTSC)

	int mActiveRowChars_R1 = 1;
	int mHzSyncPos_R2 = 1;
	int mHzSyncPulseW_R3 = 1;
	int mCharCols_R0 = 4;
	int mCharSkew_R8 = 0;
	int mCursSkew_R8 = 0;
#define _INTERLACE_MODE(x) ((x&1)==0?"Non - interlaced":((x&3)==3?"Interlaced & Video":"Interlaced"))
	inline bool non_interlaced(int m) { return ((m & 0x1) == 0x0); }
	inline bool interlaced(int m) { return ((m & 0x3) == 0x1); }
	inline bool interlaced_video(int m) { return ((m & 0x3) == 0x3); }
#define INTERLACED			2
#define INTERLACED_VIDEO	3
	int mInterlaceMode_R8 = 0;

	int mInitialised = false;


	int mField = 0;
	int mScanLine = 0;

public:

	void updateSettings(uint8_t reg);
	void printSettings();

	ALLEGRO_COLOR green, black;

	CRTC6845(string name, uint16_t adr, double tickRate, uint8_t accessSpeed,
		DebugTracing  *debugTracing, ConnectionManager* connectionManager, DeviceManager* deviceManager);
	~CRTC6845() {}

	bool readByte(BusAddress adr, BusByte& data);
	bool dump(BusAddress adr, uint8_t& data) override;
	bool writeByte(BusAddress adr, BusByte data);

	// Reset device
	bool reset();


	// Device power on
	bool power() { return reset(); }

	// Advance until time tickTime
	bool advanceUntil(uint64_t tickTime) override;
	bool advanceChar();
	bool updateOutputs();

	// Called by other device to get next memory address to fetch char/graphics data from
	bool getMemFetchAdr(uint16_t& adr);

	// Outputs the internal state of the device
	bool outputState(ostream& sout) override;

	void processAnaloguePortUpdate(int index) override;

};

#endif


