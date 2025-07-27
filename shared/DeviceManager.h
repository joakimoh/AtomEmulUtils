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
#include "Device.h"


using namespace std;

// Forward references to classed defined later on to allow them being referenced before they are declared
class ConnectionManager;
class MemoryMappedDevice;
class VideoDisplayUnit;
class SoundDevice;
class Program;
class P6502;
class RAM;


class DeviceManager {

private:

	vector<Device*> mDevices;
	vector<MemoryMappedDevice*> mMemoryMappedDevices;
	DebugManager* mDM = NULL;
	Device *mMicroprocessor = NULL;

	string getFileName(string& path, stringstream& sin);
	uint16_t getHexVal(stringstream& sin);
	double getDoubleVal(stringstream& sin);
	int getIntVal(stringstream& sin);

	vector<MemoryMappedDevice*>* mDevicesByAddress[64 * 1024] = { NULL };

public:

	DeviceManager(
		VideoSettings videoSettings,
		string memMapFile, double& cpuClock, int audioSampleFreq, ALLEGRO_DISPLAY* disp, ALLEGRO_BITMAP* dispBitmap, Resolution disRes, DebugManager* debugManager,
		Program program, Program data, ConnectionManager& connectionManager, P6502*& microprocessor, VideoDisplayUnit*& vdu,
		SoundDevice * &sound_device,
		vector<Device*>& fieldScheduledDevices, vector<Device*>& halfLineScheduledDevices, vector<Device*>& instructionScheduledDevices, double speed
	);

	~DeviceManager();

	void registerMemorySpace(MemoryMappedDevice* device, uint16_t adr, uint16_t sz);
	void registerMemoryGap(MemoryMappedDevice* device, uint16_t adr, uint16_t sz);

	bool getPeripherals(vector<Device*>& devices);
	bool getOtherDevices(vector<Device*>& devices);
	bool getMemoryMappedDevices(vector<MemoryMappedDevice*>& devices);
	bool getMemoryDevices(vector<MemoryMappedDevice*>& devices);
	bool getRAMs(vector<RAM*>& devices);
	bool getPageMemDevice(MemoryMappedDevice*& zpMem, uint8_t page);

	bool loadData(Program data);

	bool getDevice(string name, Device*& device);
	bool getDevice(DeviceId id, Device*& device);
	bool getUc(Device* &device) { device = mMicroprocessor; return true; }

	

	int size() { return (int)mDevices.size(); }

	// Non-intrusive reading of the memory location of a device.
	// If no memory-mapped device exists at the specified address,
	// the method will return false.
	bool dumpDeviceMemory(uint16_t adr, uint8_t& data);

	// Write to a memory-mapped device (for debugger use only)
	bool writeMemoryMappedDevice(uint16_t adr, uint8_t data);

	MemoryMappedDevice* getMemoryMappedDevicebyAddress(uint16_t adr);
	void printMemoryMap();

};

#endif
