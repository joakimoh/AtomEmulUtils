#ifndef CLOCKED_DEVICE_H
#define CLOCKED_DEVICE_H

#include <cstdint>
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

	uint64_t deviceTick(int deviceCycles);

	uint64_t timeTick(int ticks);


};

#endif
