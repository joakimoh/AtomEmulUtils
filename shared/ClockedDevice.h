#ifndef CLOCKED_DEVICE_H
#define CLOCKED_DEVICE_H

#include <cstdint>
#include <cmath>
#include "TimedDevice.h"



using namespace std;


class ClockedDevice : public TimedDevice {

public:

	enum ClockPhase {LOW_PHASE = 0, HIGH_PHASE = 1};

protected:


	double mDeviceClockRate = 1.0;
	int mTicksPerDeviceCycle = 1;
	int mTicksPerHalfDeviceCycle = 1;

	uint64_t mCycle = 0;
	ClockPhase mClockPhase = LOW_PHASE;

public:

	ClockedDevice(double tickRate, double deviceClockRate);

	// Advance the device time by the specified number of device cycles and return the current device cycle count after the advance
	uint64_t deviceTick(int deviceCycles);

	// Advance the device time by the specified number of device cycles without changing the device cycle count and return the current device cycle count after the advance
	uint64_t advanceTimeOnly(int deviceCycles);

	uint64_t timeTick(int ticks);

	double getDeviceClockRate() { return mDeviceClockRate;  }

	uint64_t tick2DeviceCycles(uint64_t ticks) {return  (uint64_t) round(ticks / mTicksPerDeviceCycle); }

	uint64_t deviceCycles2Ticks(uint64_t deviceCycles) { return  (uint64_t)round(deviceCycles * mTicksPerDeviceCycle); }


};

#endif
