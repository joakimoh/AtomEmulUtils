#ifndef VIDEO_DISPLAY_UNIT_H
#define VIDEO_DISPLAY_UNIT_H


#include "MemoryMappedDevice.h"
#include "RAM.h"
#include <cstdint>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include "Device.h"

using namespace std;

class VideoDisplayUnit : public MemoryMappedDevice {

protected:

	RAM* mVideoMem = NULL;
	uint16_t mVideoMemAdr = 0x0;

	ALLEGRO_BITMAP* mDisplay = NULL;

	int mScanLine = 0;

	int mDisplayWidth = 0;
	int mDisplayHeight = 0;

public:

	VideoDisplayUnit(string name, DeviceId devId, double cpuClock, uint8_t waitStates, uint16_t adr, uint16_t sz, ALLEGRO_BITMAP* disp, int dispW, int dispH, uint16_t videoMemAdr, DebugManager  *debugManager, ConnectionManager* connectionManager);

	virtual ~VideoDisplayUnit() {};

	virtual bool getVisibleArea(int& w, int& h) = 0;

	bool setVideoRam(RAM* ram);
	uint16_t getVideoMemAdr();

	virtual double getScanLineDuration() = 0;

	//
	// Get the no of scan lines per field
	// 
	// For non-interlaced modes, this will be the same as the
	// no of scan lines per frame (as field is then the same as a frame).
	// 
	// For interlaced modes, this will be the no of scan lines per one odd/even
	// field where two such fields will correspond to a frame.
	//s
	virtual double getScanLinesPerField() = 0;

	//
	// Get the field rate
	//
	// For  non-interlaced modes, this will be the same as the
	// frame rate (as field is then the same as a frame).
	// 
	// For interlaced modes, this will be the  rate of completing a
	// single field (irrespetively if it was an even or odd one).
	// The frame rate will be 1/2 the field rate in this case as
	// it takes two consecutive fields (one even + one odd)
	// to "paint"" a complete frame.
	//
	virtual double getFieldRate() = 0;


	virtual int getCharScanLines() = 0;
	virtual int getVerticalSyncLine() = 0;
	virtual int getHorizontalSyncPos() = 0;
	virtual int getCharsPerLine() = 0;
	virtual int getVisibleCharsPerLine() = 0;
	virtual int getScreenScanLine() = 0;
	virtual inline int getLeftBorderChars() = 0;
	virtual inline int getRightBorderChars() = 0;
	virtual inline int getTopBorderLines() = 0;
	virtual inline int getBottomBorderLines() = 0;
	virtual inline int getActiveChars() = 0;
	virtual inline int getActiveLines() = 0;
	virtual inline int getRetraceLines() = 0;
	virtual inline int getRetraceChars() = 0;
	virtual inline int getScreenScanLines() = 0;
	virtual inline int fieldScanLineOffset() = 0;

	//
	// Interlace-related methods
	//

	// Check if interlace is enabled (On)
	virtual inline bool interlaceOn() = 0;

	// Advance 1/2 scan line - required for interlace modes as
	// each field is usally 312 1/2 (PAL) or 262 1/2 (NTSC) scan lines
	// to get 625 (PAL) or 525 (NTSC) scan lines per frame (i.e., a pair of even and odd fields)
	// at 50 Hz (PAL) or 60 Hz (NTSC).
	virtual bool advanceHalfLine(uint64_t& endCycle) = 0;


	// Advance a complete scan line
	virtual bool advanceLine(uint64_t& endCycle) = 0;
	
	// Read a VDU register (if applicable for the VDU type)
	virtual bool read(uint16_t adr, uint8_t& data);

	// Write to a VDU register (if applicable for the VDU type)
	virtual bool write(uint16_t adr, uint8_t data);
};

#endif