#include "VideoDisplayUnit.h"


VideoDisplayUnit::VideoDisplayUnit(string name, DeviceId devId, uint16_t adr, uint16_t sz, ALLEGRO_BITMAP* disp, int dispW, int dispH, uint16_t videoMemAdr, DebugInfo debugInfo, ConnectionManager* connectionManager) :
	MemoryMappedDevice(name, devId, VDU_DEVICE, adr, sz, debugInfo, connectionManager), mVideoMemAdr(videoMemAdr), mDisplay(disp), mDisplayWidth(dispW),
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
	// Call parent class to trigger scheduling of other devices when applicable
	return MemoryMappedDevice::read(adr, data);
}

bool VideoDisplayUnit::write(uint16_t adr, uint8_t data)
{
	// Call parent class to trigger scheduling of other devices when applicable
	return  MemoryMappedDevice::write(adr, data);
}