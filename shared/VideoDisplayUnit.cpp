#include "VideoDisplayUnit.h"


VideoDisplayUnit::VideoDisplayUnit(string name, DeviceId devId, VideoSettings videoSettings, double cpuClock, uint8_t waitStates, uint16_t adr, uint16_t sz, ALLEGRO_BITMAP* disp, uint16_t videoMemAdr, DebugManager  *debugManager, ConnectionManager* connectionManager) :
	MemoryMappedDevice(name, devId, VDU_DEVICE, cpuClock, waitStates, adr, sz, debugManager, connectionManager),
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