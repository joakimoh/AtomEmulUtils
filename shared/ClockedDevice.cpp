#include "ClockedDevice.h"
#include <cmath>
#include <algorithm>
#include <iostream>

ClockedDevice::ClockedDevice(double tickRate, double deviceClockRate) : mDeviceClockRate(deviceClockRate), TimedDevice(tickRate)
{
	if (deviceClockRate > 0)
		mTicksPerDeviceCycle = max(1, (int)round(tickRate / deviceClockRate));
	else
		throw runtime_error("illegal device rate");

	mTicksPerHalfDeviceCycle = (int) round(mTicksPerDeviceCycle / 2);
}

// Advance the device time by the specified number of device cycles and return the current device cycle count after the advance
uint64_t ClockedDevice::deviceTick(int deviceCycles)
{
	if (deviceCycles < 0)
		throw runtime_error("negative tick");

	mTicks += mTicksPerDeviceCycle * deviceCycles;
	mCycle += deviceCycles;
	if (mTicks % mTicksPerDeviceCycle < mTicksPerHalfDeviceCycle)
		mClockPhase = LOW_PHASE;
	else
		mClockPhase = HIGH_PHASE;

	return mCycle;
}

// Advance the device time by the specified number of device cycles without changing the device cycle count and return the current device cycle count after the advance
uint64_t ClockedDevice::advanceTimeOnly(int deviceCycles)
{
	if (deviceCycles < 0)
		throw runtime_error("negative tick");

	mTicks += mTicksPerDeviceCycle * deviceCycles;

	return mCycle;
}

uint64_t ClockedDevice::timeTick(int ticks)
{
	TimedDevice::timeTick(ticks);

	mCycle += ticks / mTicksPerDeviceCycle;
	
	if (mTicks % mTicksPerDeviceCycle < mTicksPerHalfDeviceCycle)
		mClockPhase = LOW_PHASE;
	else
		mClockPhase = HIGH_PHASE;

	return mTicks;
}