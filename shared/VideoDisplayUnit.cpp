#include "Display.h"
#include "VideoDisplayUnit.h"


VideoDisplayUnit::VideoDisplayUnit(
	string name, DeviceId devId, Display *display, uint8_t waitStates, 
	BusAddress adr, BusAddress sz, BusAddress videoMemAdr, DebugTracing  *debugTracing,
	ConnectionManager* connectionManager, DeviceManager* deviceManager
) :
	MemoryMappedDevice(name, devId, VDU_DEVICE, waitStates, adr, sz, debugTracing, connectionManager, deviceManager),
	mVideoMemAdr(videoMemAdr), mDisplay(display)
{
	mDisplayBitmap = display->getDisplayBitmap();
	mVideoSettings = display->getVideoSettings();

	mRefreshedFieldsPerSecond = mVideoSettings.getFieldRate() / mFieldsPerRefreshPeriod;
}

bool VideoDisplayUnit::setVideoRam(RAM* ram)
{
	mVideoMem = ram;
	return true;
}

BusAddress VideoDisplayUnit::getVideoMemAdr() {
	return mVideoMemAdr;
}

bool VideoDisplayUnit::readByte(BusAddress adr, BusByte& data)
{
	data = 0xff;
	return selected(adr);
}

bool VideoDisplayUnit::writeByte(BusAddress adr, BusByte data)
{
	return selected(adr);
}

// Make sure the screen update rate is never higher than the field rate (normally 50 or 60 Hz), irrespectively of the emulation rate
void VideoDisplayUnit::setEmulationSpeed(double emulationSpeed)
{
	Device::setEmulationSpeed(emulationSpeed);

	mFieldsPerRefreshPeriod = max(1,(int)round(emulationSpeed));
	if (emulationSpeed > 1)
		mRefreshedFieldsPerSecond = (int) round(mVideoSettings.getFieldRate() * emulationSpeed / mFieldsPerRefreshPeriod);
	else
		mRefreshedFieldsPerSecond = (int)round(mVideoSettings.getFieldRate() * emulationSpeed);
}

// Set PC screen refresh rate
void VideoDisplayUnit::setRefreshRate(double rate)
{
	double field_rate = mVideoSettings.getFieldRate() * mEmulationSpeed;
	mFieldsPerRefreshPeriod = max(1,(int) round(field_rate / rate));
	mRefreshedFieldsPerSecond = field_rate / mFieldsPerRefreshPeriod;
}

// Called by each VDU when it refreshed the PC screen
void VideoDisplayUnit::refreshEvent()
{
	mAccRefreshCount++;
	if (mAccRefreshCount >= mRefreshedFieldsPerSecond) {
		auto prev_refresh_time_point_ms = mRefreshTimePoint;
		mRefreshTimePoint = chrono::high_resolution_clock::now();
		int refresh_time_ms = chrono::duration_cast<std::chrono::milliseconds>(mRefreshTimePoint - prev_refresh_time_point_ms).count();
		double measured_field_rate = mAccRefreshCount * 1000.0 / refresh_time_ms;
		mDisplay->updateMeasuredFrameRate(measured_field_rate);
		mAccRefreshCount = 0;				
	}
	
}