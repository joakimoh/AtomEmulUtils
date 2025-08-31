#include "SoundDevice.h"


SoundDevice::SoundDevice(
	string name, DeviceId devId, double cpuClock, int SampleFreq, double baseEmulationRate, double baseSubEmulationRate,
	DebugManager* debugManager, ConnectionManager* connectionManager
) :
	Device(name, devId, SOUND_DEVICE, cpuClock, debugManager, connectionManager), mSampleRate(SampleFreq)
{
	setEmulationSpeed(baseEmulationRate, baseSubEmulationRate, 1.0);
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
	mSampleRate = mBaseSampleRate * speed;
	mEmulationSpeed = speed;
}