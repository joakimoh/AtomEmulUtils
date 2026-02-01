#include "SoundDevice.h"


SoundDevice::SoundDevice(
	string name, DeviceId devId, int SampleFreq, double baseEmulationRate, double highEmulationRate,
	DebugTracing* debugTracing, ConnectionManager* connectionManager
) :
	Device(name, devId, SOUND_DEVICE, debugTracing, connectionManager), mBaseSampleRate(SampleFreq)
{
	setInitialEmulationSpeed(baseEmulationRate, highEmulationRate, mEmulationSpeed);

}

void SoundDevice::setInitialEmulationSpeed(double baseEmulationRate, double highEmulationRate, double speed)
{
	mBaseLowEmulationRate = baseEmulationRate;
	mBaseHighEmulationRate = highEmulationRate;

	setEmulationSpeed(speed);
}

void SoundDevice::setEmulationSpeed(double speed)
{
	Device::setEmulationSpeed(speed);
	mLowEmulationRate = mBaseLowEmulationRate * speed;
	mHighEmulationRate = mBaseHighEmulationRate * speed;
	mSampleRate = (int) round(mBaseSampleRate * speed);

}