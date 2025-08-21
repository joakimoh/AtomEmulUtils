#ifndef SOUND_DEVICE_H
#define SOUND_DEVICE_H

#include "Device.h"
#include <fstream>
#include <iostream>
#include "CSWCodec.h"
#include "allegro5/allegro_audio.h"
#include "Device.h"

using namespace std;

class SoundDevice : public Device {

protected:

	double mBaseLowEmulationRate = 60;
	double mBaseHighEmulationRate = mBaseLowEmulationRate * 522;
	double mLowEmulationRate = mBaseLowEmulationRate;
	double mHighEmulationRate = mBaseHighEmulationRate;
	int mSampleRate = 32000;			// sample rate for real-time emulation
	int mBaseSampleRate = 32000;



public:

	SoundDevice(
		string name, DeviceId devId, double cpuClock, int SampleFreq, double baseEmulationRate, double baseSubEmulationRate, double emulationSpeed,
		DebugManager  *debugManager, ConnectionManager* connectionManager
	);

	virtual void setEmulationRate(double baseEmulationRate, double subEmulationRate, double speed);
	virtual void setEmulationRate(double speed);

};





#endif