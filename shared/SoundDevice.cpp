#include "SoundDevice.h"


SoundDevice::SoundDevice(string name, DeviceId devId, DebugInfo debugInfo, ConnectionManager* connectionManager) :
	Device(name, devId, SOUND_DEVICE, debugInfo, connectionManager)
{

}