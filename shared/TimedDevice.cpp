#include "TimedDevice.h"

TimedDevice::TimedDevice(double tickRate) : mTickRate(tickRate)
{
	mTicksPerMs = (uint64_t)round(mTickRate * 1e6 / 1000);
}