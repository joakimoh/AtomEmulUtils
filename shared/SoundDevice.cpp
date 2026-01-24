#include "SoundDevice.h"


SoundDevice::SoundDevice(
	string name, DeviceId devId, double cpuClock, int SampleFreq, double baseEmulationRate, double highEmulationRate,
	DebugTracing* debugTracing, ConnectionManager* connectionManager
) :
	Device(name, devId, SOUND_DEVICE, cpuClock, debugTracing, connectionManager), mBaseSampleRate(SampleFreq)
{
	setInitialEmulationSpeed(baseEmulationRate, highEmulationRate, mEmulationSpeed);

	if (round(highEmulationRate / SampleFreq) != 1)
		throw runtime_error("The audio device needs to be scheduled with the high rate!");
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