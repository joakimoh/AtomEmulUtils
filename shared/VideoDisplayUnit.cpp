#include "VideoDisplayUnit.h"


VideoDisplayUnit::VideoDisplayUnit(string name, DeviceId devId, uint16_t adr, uint16_t sz, ALLEGRO_BITMAP* disp, uint16_t videoMemAdr, DebugInfo debugInfo, ConnectionManager* connectionManager) :
	MemoryMappedDevice(name, devId, VDU_DEVICE, adr, sz, debugInfo, connectionManager), mVideoMemAdr(videoMemAdr), mDisplay(disp)
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