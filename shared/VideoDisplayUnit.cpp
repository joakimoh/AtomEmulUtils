#include "VideoDisplayUnit.h"


VideoDisplayUnit::VideoDisplayUnit(string name, DeviceId devId, VideoSettings videoSettings, double cpuClock, uint8_t waitStates, uint16_t adr, uint16_t sz,
	 ALLEGRO_BITMAP* disp, uint16_t videoMemAdr, DebugManager  *debugManager, ConnectionManager* connectionManager, DeviceManager* deviceManager) :
	MemoryMappedDevice(name, devId, VDU_DEVICE, cpuClock, waitStates, adr, sz, debugManager, connectionManager, deviceManager),
	mVideoMemAdr(videoMemAdr), mDisplay(disp), mVideoSettings(videoSettings)
{

}

bool VideoDisplayUnit::setVideoRam(RAM* ram)
{
	mVideoMem = ram;
	return true;
}

uint16_t VideoDisplayUnit::getVideoMemAdr() {
	return mVideoMemAdr;
}

bool VideoDisplayUnit::read(uint16_t adr, uint8_t& data)
{
	data = 0xff;
	return selected(adr);
}

bool VideoDisplayUnit::write(uint16_t adr, uint8_t data)
{
	return selected(adr);
}

// Make sure the screen update rate is always the same as the field rate (normally 50 or 60 Hz), irrespectively of the emulation rate
void VideoDisplayUnit::setEmulationSpeed(double emulationSpeed)
{
	// int field_rate = mVideoSettings.getFieldRate();
	mFieldsPerRefreshPeriod = max(1,(int)round(emulationSpeed));
}