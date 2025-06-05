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

	int mFieldRate = 60;
	int mSampleRate = 32000;			// sample rate for real-time emulation
	int mRealSampleRate = mSampleRate; // sample rate compensated for non-real time emulation speed

	double mSpeed = 1.0;


public:

	SoundDevice(string name, DeviceId devId, double cpuClock, int SampleFreq, DebugManager  *debugManager, ConnectionManager* connectionManager);

	virtual void setFieldRate(int fieldRate, double speed);

};





#endif