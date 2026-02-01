#ifndef TIMED_DEVICE_H
#define TIMED_DEVICE_H

#include <cstdint>
#include <iostream>



using namespace std;


class TimedDevice {

public:

protected:

	double mTickRate = 2.0;

	uint64_t mTicks = 0;

public:

	TimedDevice(double tickRate);

	uint64_t getCycleCount() { return mTicks; }

	//  Advance until time tickTime
	virtual bool advanceUntil(uint64_t tickTime) {  mTicks = tickTime; return true; }

	// Get current device time
	double getTimeSec() { return mTicks * 1e-6 / mTickRate;}

	void timeTick(int ticks) { mTicks += ticks; }


};

#endif
