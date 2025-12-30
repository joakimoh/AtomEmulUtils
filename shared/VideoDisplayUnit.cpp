#include "Display.h"
#include "VideoDisplayUnit.h"


VideoDisplayUnit::VideoDisplayUnit(
	string name, DeviceId devId, Display *display, double cpuClock, uint8_t waitStates, 
	uint16_t adr, uint16_t sz, uint16_t videoMemAdr, DebugManager  *debugManager,
	ConnectionManager* connectionManager, DeviceManager* deviceManager
) :
	MemoryMappedDevice(name, devId, VDU_DEVICE, cpuClock, waitStates, adr, sz, debugManager, connectionManager, deviceManager),
	mVideoMemAdr(videoMemAdr), mDisplay(display)
{
	mDisplayBitmap = display->getDisplayBitmap();
	mVideoSettings = display->getVideoSettings();
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

// Set PC screen refresh rate
void VideoDisplayUnit::setRefreshRate(double rate)
{
	mFieldsPerRefreshPeriod = (int) round(mVideoSettings.getFieldRate() / rate);
}

double VideoDisplayUnit::getMeasuredRefreshRate() {
	return mVideoSettings.getFieldRate() / mFieldsPerRefreshPeriod;
}

// Called by each VDU when it refreshed the PC screen
void VideoDisplayUnit::refreshEvent()
{
	auto prev_refresh_time_point_ms = mRefreshTimePoint;
	mRefreshTimePoint = chrono::high_resolution_clock::now();
	auto refresh_time_ms = chrono::duration_cast<std::chrono::milliseconds>(mRefreshTimePoint - prev_refresh_time_point_ms).count();
	mDisplay->updateMeasuredFrameRate(1000 / refresh_time_ms);
}