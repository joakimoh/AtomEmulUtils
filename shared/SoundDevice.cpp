#include "SoundDevice.h"


SoundDevice::SoundDevice(string name, DeviceId devId, int SampleFreq, DebugInfo  *debugInfo, ConnectionManager* connectionManager) :
	Device(name, devId, SOUND_DEVICE, debugInfo, connectionManager), mSampleRate(SampleFreq)
{

}

void SoundDevice::setFrameRate(int frameRate)
{
	mFrameRate = frameRate;
}