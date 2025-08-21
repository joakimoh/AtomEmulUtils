#include "SoundDevice.h"


SoundDevice::SoundDevice(
	string name, DeviceId devId, double cpuClock, int SampleFreq, double baseEmulationRate, double baseSubEmulationRate, double emulationSpeed,
	DebugManager* debugManager, ConnectionManager* connectionManager
) :
	Device(name, devId, SOUND_DEVICE, cpuClock, debugManager, connectionManager), mSampleRate(SampleFreq)
{
	setEmulationRate(baseEmulationRate, baseSubEmulationRate, emulationSpeed);
}

void SoundDevice::setEmulationRate(double baseEmulationRate, double subEmulationRate, double speed)
{
	mBaseLowEmulationRate = baseEmulationRate;
	mBaseHighEmulationRate = subEmulationRate;
	setEmulationRate(speed);
}

void SoundDevice::setEmulationRate(double speed)
{
	mLowEmulationRate = mBaseLowEmulationRate * speed;
	mHighEmulationRate = mBaseHighEmulationRate * speed;
	mSampleRate = mBaseSampleRate * speed;
}