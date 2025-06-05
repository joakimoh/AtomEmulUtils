#ifndef DEVICES_H
#define DEVICES_H

#include <cstdint>
#include <vector>
#include <string>
#include <map>
#include "DebugManager.h"
#include <allegro5/allegro_primitives.h>
#include <iostream>
#include "VideoSettings.h"


using namespace std;

// Forward references to classed defined later on to allow them being referenced before they are declared
class ConnectionManager;
class Device;
class MemoryMappedDevice;
class VideoDisplayUnit;
class SoundDevice;


class Devices {

private:

	vector<Device*> mDevices;
	vector<MemoryMappedDevice*> mMemoryMappedDevices;
	DebugManager* mDM = NULL;

	string getFileName(string& path, stringstream& sin);
	uint16_t getHexVal(stringstream& sin);
	double getDoubleVal(stringstream& sin);
	int getIntVal(stringstream& sin);

public:

	Devices(
		VideoSettings videoSettings,
		string memMapFile, double& cpuClock, int audioSampleFreq, ALLEGRO_DISPLAY* disp, ALLEGRO_BITMAP* dispBitmap, Resolution disRes, DebugManager* debugManager,
		Program program, Program data, ConnectionManager& connectionManager, P6502*& microprocessor, VideoDisplayUnit*& vdu,
		SoundDevice * &sound_device,
		vector<Device*>& fieldScheduledDevices, vector<Device*>& halfLineScheduledDevices, vector<Device*>& instructionScheduledDevices, double speed
	);

	~Devices();

	bool getPeripherals(vector<Device*>& devices);
	bool getOtherDevices(vector<Device*>& devices);
	bool getMemoryMappedDevices(vector<MemoryMappedDevice*>& devices);
	bool getMemoryDevices(vector<MemoryMappedDevice*>& devices);
	bool getRAMs(vector<RAM*>& devices);
	bool getPageMemDevice(MemoryMappedDevice*& zpMem, uint8_t page);

	bool loadData(Program data);


	bool getDevice(string name, Device*& device) {
		device = NULL;
		for (int i = 0; i < mDevices.size(); i++) {
			if (mDevices[i]->name == name) {
				device = mDevices[i];
				return true;
			}
		}
		return false;
	}

	//
	// Find a device of a certain type.
	// If more than one device of this type is found,
	// the first one is returned but an error is also indicated.
	//
	bool getDevice(DeviceId id, Device*& device) {
		bool found = false;
		device = NULL;
		for (int i = 0; i < mDevices.size(); i++) {
			if (mDevices[i]->devType == id) {
				if (!found) {
					device = mDevices[i];
					found = true;
				}
				else
					return false;
			}
		}
		return found;
	}

	int size() { return (int)mDevices.size(); }

	// Non-intrusive reading of the memory location of a device.
	// If no memory-mapped device exists at the specified address,
	// the method will return false.
	bool dumpDeviceMemory(uint16_t adr, uint8_t& data);


};

#endif
