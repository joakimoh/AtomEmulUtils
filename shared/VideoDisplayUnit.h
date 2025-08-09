#ifndef VIDEO_DISPLAY_UNIT_H
#define VIDEO_DISPLAY_UNIT_H


#include "MemoryMappedDevice.h"
#include "RAM.h"
#include <cstdint>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include "Device.h"
#include "VideoSettings.h"

using namespace std;

class VideoDisplayUnit : public MemoryMappedDevice {

protected:

	RAM* mVideoMem = NULL;
	uint16_t mVideoMemAdr = 0x0;

	ALLEGRO_BITMAP* mDisplay = NULL;

	int mScanLine = 0;
	int pScanLine = 1;

	int mField = 0;
	int pField = 1;

	VideoSettings mVideoSettings;

public:

	VideoDisplayUnit(string name, DeviceId devId, VideoSettings videoSettings, double cpuClock, uint8_t waitStates, uint16_t adr, uint16_t sz,
		ALLEGRO_BITMAP* disp, uint16_t videoMemAdr, DebugManager  *debugManager, ConnectionManager* connectionManager, DeviceManager* deviceManager);

	virtual ~VideoDisplayUnit() {};

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
	virtual int getActiveCharsPerLine() = 0;
	virtual int getScreenScanLine() = 0;
	virtual int getActiveLines() = 0;
	virtual int getActiveCharRows() = 0;
	virtual int getScreenScanLines() = 0;
	virtual int fieldScanLineOffset() = 0;

	int mSkipFields = 1;
	bool setSkipFields(int n) { if (n > 0 && n < 10) { mSkipFields = n; return true; } return false; }

	//
	// Interlace-related methods
	//

	// Check if interlace is enabled (On)
	virtual bool interlaceOn() = 0;
	
	// Read a VDU register (if applicable for the VDU type)
	virtual bool read(uint16_t adr, uint8_t& data);

	// Write to a VDU register (if applicable for the VDU type)
	virtual bool write(uint16_t adr, uint8_t data);
};

#endif
