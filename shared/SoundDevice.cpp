#include "SoundDevice.h"


SoundDevice::SoundDevice(
	string name, DeviceId devId, double cpuClock, int SampleFreq, double baseEmulationRate, double baseSubEmulationRate,
	DebugTracing* debugTracing, ConnectionManager* connectionManager
) :
	Device(name, devId, SOUND_DEVICE, cpuClock, debugTracing, connectionManager), mBaseSampleRate(SampleFreq)
{
	setInitialEmulationSpeed(baseEmulationRate, baseSubEmulationRate, mEmulationSpeed);
}

void SoundDevice::setInitialEmulationSpeed(double baseEmulationRate, double subEmulationRate, double speed)
{
	mBaseLowEmulationRate = baseEmulationRate;
	mBaseHighEmulationRate = subEmulationRate;

	setEmulationSpeed(speed);
}

void SoundDevice::setEmulationSpeed(double speed)
{
	Device::setEmulationSpeed(speed);
	mLowEmulationRate = mBaseLowEmulationRate * speed;
	mHighEmulationRate = mBaseHighEmulationRate * speed;
	mSampleRate = (int) round(mBaseSampleRate * speed);

}