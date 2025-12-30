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
	uint16_t mVideoMemAdr = 0x0;

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

public:

	VideoDisplayUnit(string name, DeviceId devId, Display *display, double cpuClock, uint8_t waitStates, uint16_t adr, uint16_t sz,
		uint16_t videoMemAdr, DebugManager  *debugManager, ConnectionManager* connectionManager, DeviceManager* deviceManager);

	virtual ~VideoDisplayUnit() {};

	bool setVideoRam(RAM* ram);
	uint16_t getVideoMemAdr();
	
	// Read a VDU register (if applicable for the VDU type)
	virtual bool read(uint16_t adr, uint8_t& data);

	// Write to a VDU register (if applicable for the VDU type)
	virtual bool write(uint16_t adr, uint8_t data);

	// Make sure the screen update rate is always the same as the field rate (normally 50 or 60 Hz), irrespectively of the emulation rate
	void setEmulationSpeed(double emulationSpeed) override;

	// Set screen refresh rate
	void setRefreshRate(double rate);

	double getMeasuredRefreshRate();

	// Called by each VDU when it refreshed the PC screen
	void refreshEvent();
};

#endif
