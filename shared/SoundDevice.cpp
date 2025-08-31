#include "SoundDevice.h"


SoundDevice::SoundDevice(
	string name, DeviceId devId, double cpuClock, int SampleFreq, double baseEmulationRate, double baseSubEmulationRate,
	DebugManager* debugManager, ConnectionManager* connectionManager
) :
	Device(name, devId, SOUND_DEVICE, cpuClock, debugManager, connectionManager), mBaseSampleRate(SampleFreq)
{
	setEmulationSpeed(baseEmulationRate, baseSubEmulationRate, mEmulationSpeed);
}

void SoundDevice::setEmulationSpeed(double baseEmulationRate, double subEmulationRate, double speed)
{
	mBaseLowEmulationRate = baseEmulationRate;
	mBaseHighEmulationRate = subEmulationRate;
	setEmulationSpeed(speed);
}

void SoundDevice::setEmulationSpeed(double speed)
{
	mLowEmulationRate = mBaseLowEmulationRate * speed;
	mHighEmulationRate = mBaseHighEmulationRate * speed;
	mSampleRate = (int) round(mBaseSampleRate * speed);
	mEmulationSpeed = speed;
}