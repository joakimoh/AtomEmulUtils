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

	VideoDisplayUnit(string name, DeviceId devId, uint16_t adr, uint16_t sz, ALLEGRO_BITMAP* disp, int dispW, int dispH, uint16_t videoMemAdr, DebugInfo debugInfo, ConnectionManager* connectionManager);

	virtual ~VideoDisplayUnit() {};

	virtual bool getVisibleArea(int& w, int& h) = 0;

	bool setVideoRam(RAM* ram);
	uint16_t getVideoMemAdr();

	virtual double getScanLineDuration() = 0;
	virtual double getScanLinesPerFrame() = 0;
	virtual double getFrameRate() = 0;
	virtual int getCharScanLines() = 0;
	virtual int getVerticalSyncPos() = 0;
	virtual int getHorizontalSyncPos() = 0;
	virtual int getCharsPerLine() = 0;
	virtual int getVisibleCharsPerLine() = 0;

	virtual bool advanceLine(uint64_t& endCycle) = 0;

	virtual bool read(uint16_t adr, uint8_t& data);
	virtual bool write(uint16_t adr, uint8_t data);
};

#endif