#ifndef CLOCKED_DEVICE_H
#define CLOCKED_DEVICE_H



using namespace std;


class ClockedDevice {

protected:

	double mDeviceClock = 1.0;
	int mCpuCyclesPerDeviceCycle = 1;

public:

	ClockedDevice(double cpuClock, double deviceClock);


};

#endif
