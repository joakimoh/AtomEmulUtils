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

private:



public:

	SoundDevice(string name, DeviceId devId, DebugInfo debugInfo, ConnectionManager* connectionManager);


};





#endif