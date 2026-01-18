#include "ClockedDevice.h"
#include <cmath>
#include <algorithm>
#include <iostream>

ClockedDevice::ClockedDevice(double cpuClock, double deviceClock) : mDeviceClock(deviceClock)
{
	if (deviceClock > 0)
		mCpuCyclesPerDeviceCycle = max(1, (int)round(cpuClock / deviceClock));
}