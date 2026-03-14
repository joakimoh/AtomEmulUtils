#ifndef DEVICES_H
#define DEVICES_H

#include <cstdint>
#include <vector>
#include <string>
#include <map>
#include "DebugTracing.h"
#include <allegro5/allegro_primitives.h>
#include <iostream>
#include "VideoSettings.h"
#include "Device.h"
#include "MemorySegmentTree.h"
#include "TimedDevice.h"


using namespace std;

// Forward references to classed defined later on to allow them being referenced before they are declared
class ConnectionManager;
class MemoryMappedDevice;
class MemoryProxyDevice;
class VideoDisplayUnit;
class SoundDevice;
class Program;
class P6502;
class RAM;
class KeyboardDevice;
class Display;


class DeviceManager {

private:

	vector<Device*> mDevices;
	vector<MemoryMappedDevice*> mMemoryMappedDevices;
	vector<MemoryMappedDevice*> mNonOverlappingMemoryMappedDevices;
	vector<MemoryProxyDevice*> mMemoryProxyDevices;
	DebugTracing* mDM = NULL;
	ConnectionManager* mCM = NULL;
	Device *mMicroprocessor = NULL;

	string getFileName(string& path, stringstream& sin);
	uint16_t getHexVal(stringstream& sin);
	double getDoubleVal(stringstream& sin);
	int getIntVal(stringstream& sin);

	bool createMemoryMap();

	MemorySegmentTree mMemoryTree;

	int mAudioSampleRate = 0;
	Display* mDisplay = nullptr;

public:

	DeviceManager(
		string memMapFile, double& tickRate, Display* display, DebugTracing* debugTracing,
		ConnectionManager* connectionManager, P6502*& microprocessor, VideoDisplayUnit*& vdu,
		SoundDevice * &sound_device, vector<Device*>& allDevices,
		vector<TimedDevice*>& baseRateScheduledDevices, vector<TimedDevice*>& subRateScheduledDevices, vector<TimedDevice*>& instructionRateScheduledDevices,
		double speed,
		double &baseSchedulingRate, double&highSchedulingRate
	);

	~DeviceManager();

	vector<Device*> *getDevices() { return &mDevices;}
	bool getPeripherals(vector<Device*>& devices);
	bool getOtherDevices(vector<Device*>& devices);
	bool getMemoryMappedDevices(vector<MemoryMappedDevice*>& devices);
	bool getMemoryDevices(vector<MemoryMappedDevice*>& devices);
	bool getRAMs(vector<RAM*>& devices);

	bool loadData(Program data);

	bool getDevice(string name, Device*& device);
	bool getDevice(DeviceId id, Device*& device);
	bool getMicroprocessor(Device* &device) { device = mMicroprocessor; return true; }

	

	int size() { return (int)mDevices.size(); }

	// Non-intrusive reading of the memory location of a device.
	// If no memory-mapped device exists at the specified address,
	// the method will return false.
	bool dumpDeviceMemory(uint16_t adr, uint8_t& data);

	// Write to a memory-mapped device (for debugger use only)
	bool writeMemoryMappedDevice(uint16_t adr, uint8_t data);

	MemoryMappedDevice* getSelectedMemoryMappedDevice(uint16_t adr);
	void printMemoryMap();
	void printMemoryTree();

};

#endif
