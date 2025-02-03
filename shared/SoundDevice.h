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

	int mFrameRate = 60;
	int mSampleRate = 32000;


public:

	SoundDevice(string name, DeviceId devId, int SampleFreq, DebugInfo debugInfo, ConnectionManager* connectionManager);

	virtual void setFrameRate(int frameRate);

};





#endif