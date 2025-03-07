#include "VideoDisplayUnit.h"


VideoDisplayUnit::VideoDisplayUnit(string name, DeviceId devId, double cpuClock, uint16_t adr, uint16_t sz, ALLEGRO_BITMAP* disp, int dispW, int dispH, uint16_t videoMemAdr, DebugManager  *debugManager, ConnectionManager* connectionManager) :
	MemoryMappedDevice(name, devId, VDU_DEVICE, cpuClock, adr, sz, debugManager, connectionManager), mVideoMemAdr(videoMemAdr), mDisplay(disp), mDisplayWidth(dispW),
	mDisplayHeight(dispH)
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