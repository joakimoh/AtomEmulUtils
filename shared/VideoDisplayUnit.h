#ifndef VIDEO_DISPLAY_UNIT_H
#define VIDEO_DISPLAY_UNIT_H


#include "MemoryMappedDevice.h"
#include "RAM.h"
#include <cstdint>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include "Device.h"
#include "VideoSettings.h"
#include <chrono>

using namespace std;

class Display;

class VideoDisplayUnit : public MemoryMappedDevice {

protected:

	RAM* mVideoMem = nullptr;
	BusAddress mVideoMemAdr = 0x0;

	ALLEGRO_BITMAP* mDisplayBitmap = nullptr;
	ALLEGRO_DISPLAY* mAllegroDisplay = nullptr;
	Display* mDisplay = nullptr;

	int mScanLine = 0;
	int pScanLine = 1;

	int mField = 0;
	int pField = 1;

	VideoSettings mVideoSettings;

	int mFieldsPerRefreshPeriod = 1;

	chrono::time_point<std::chrono::high_resolution_clock> mRefreshTimePoint = chrono::high_resolution_clock::now();
	int mAccRefreshCount = 0;
	int mRefreshedFieldsPerSecond = 50;

	virtual bool readGraphicsMem(BusAddress adr, uint8_t& data)  = 0;

public:

	VideoDisplayUnit(string name, DeviceId devId, Display *display, uint8_t accessSpeed, BusAddress adr, BusAddress sz,
		BusAddress videoMemAdr, DebugTracing  *debugTracing, ConnectionManager* connectionManager, DeviceManager* deviceManager);

	virtual ~VideoDisplayUnit() {};

	bool setVideoRam(RAM* ram);
	BusAddress getVideoMemAdr();
	
	// Read a VDU register (if applicable for the VDU type)
	virtual bool readByte(BusAddress adr, BusByte& data);

	// Write to a VDU register (if applicable for the VDU type)
	virtual bool writeByte(BusAddress adr, BusByte data);

	//  Make sure the screen update rate is never higher than the field rate (normally 50 or 60 Hz), irrespectively of the emulation rate
	void setEmulationSpeed(double emulationSpeed) override;

	// Set PC screen refresh rate
	void setRefreshRate(double rate);

	// Get PC screen refresh rate
	int getRefreshRate() { return mRefreshedFieldsPerSecond;  }

	// Called by each VDU when it refreshed the PC screen
	void refreshEvent();
};

#endif
