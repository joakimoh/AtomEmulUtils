#include "SoundDevice.h"


SoundDevice::SoundDevice(string name, DeviceId devId, double cpuClock, int SampleFreq, DebugManager  *debugManager, ConnectionManager* connectionManager) :
	Device(name, devId, SOUND_DEVICE, cpuClock, debugManager, connectionManager), mSampleRate(SampleFreq)
{

}

void SoundDevice::setFieldRate(int fieldRate)
{
	mFieldRate = fieldRate;
}