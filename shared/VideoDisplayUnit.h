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

	
	// Read a VDU register (if applicable for the VDU type)
	virtual bool read(uint16_t adr, uint8_t& data);

	// Write to a VDU register (if applicable for the VDU type)
	virtual bool write(uint16_t adr, uint8_t data);
};

#endif
