#include "Device.h"
#include "RAM.h"
#include "ROM.h"
#include "PIA8255.h"
#include "VDU6847.h"
#include "VIA6522.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <allegro5/allegro.h>

using namespace std;

Device::Device(DeviceEnum typ, uint16_t adr, uint16_t sz, DebugInfo debugInfo) : devType(typ), mDevAdr(adr), mDevSz(sz),
mDebugInfo(debugInfo)
{
}

bool Device::selected(uint16_t adr)
{
	return (adr >= mDevAdr && adr < mDevAdr + mDevSz);
}

bool Device::validAdr(uint16_t adr)
{
	return selected(adr);
}

Devices::Devices(
	std::string memMapFile, int n60HzCycles, ALLEGRO_BITMAP* disp, Keyboard *keyboard, DebugInfo debugInfo, Program program, Program data) :
	mDebugInfo(debugInfo)
{

	ifstream fin(memMapFile, ios::in | ios::ate);

	if (!fin) {
		cout << "couldn't open memory map file " << memMapFile << "\n";
		throw runtime_error("couldn't open memory map file");
	}

	fin.seekg(0);

	string line;
	PIA8255* pia = NULL;
	VDU6847* vdu = NULL;
	while (getline(fin, line)) {

		string dev_typ_s, dev_adr_s, dev_sz_s;
		stringstream sin(line);
		int dev_adr, dev_sz;

		sin >> dev_typ_s;
		sin >> dev_adr_s;
		dev_adr = stoi(dev_adr_s, 0, 16);
		sin >> dev_sz_s;
		dev_sz = stoi(dev_sz_s, 0, 16);

		DeviceAllocation a;
		a.startAdr = dev_adr;
		a.size = dev_sz;


		if (dev_typ_s == "RAM") {
			a.deviceType = DeviceEnum::RAM_DEV;
			RAM* ram = new RAM(a.startAdr, a.size, mDebugInfo);
			mDevices.push_back(ram);
		}
		else if (dev_typ_s == "VDU6847") {
			a.deviceType = DeviceEnum::VDU6847_DEV;
			string video_mem_adr_s;
			sin >> video_mem_adr_s;
			a.videoMemAdr = stoi(video_mem_adr_s, 0, 16);
			vdu = new VDU6847(a.startAdr, n60HzCycles, disp, a.videoMemAdr, mDebugInfo);
			mDevices.push_back(vdu);
		}
		else if (dev_typ_s == "ROM") {
			a.deviceType = DeviceEnum::ROM_DEV;
			string ROM_file_name;
			sin >> ROM_file_name;
			a.ROMFileName = ROM_file_name;
			filesystem::path map_file_path = memMapFile;
			filesystem::path map_dir_path = map_file_path.parent_path();
			filesystem::path ROM_file_path = a.ROMFileName;
			filesystem::path file_path = map_dir_path / ROM_file_path;
			ROM* rom = new ROM(a.startAdr, a.size, file_path.string(), mDebugInfo);
			mDevices.push_back(rom);
		}
		else if (dev_typ_s == "PIA8255") {
			a.deviceType = DeviceEnum::PIA8255_DEV;
			pia = new PIA8255(a.startAdr, n60HzCycles, (AtomKeyboard*)keyboard, mDebugInfo);
			mDevices.push_back(pia);
		}
		else if (dev_typ_s == "VIA6522") {
			a.deviceType = DeviceEnum::VIA6522_DEV;
			VIA6522* via = new VIA6522(a.startAdr, mDebugInfo);
			mDevices.push_back(via);
		}
		else {
			a.deviceType = DeviceEnum::UNDEFINED_DEV;
			cout << "Unsupported device type " << a.deviceType << "!\n";
			throw runtime_error("Attempt to add unsupported device (should never happen)");
		}
	}

	
	if (pia != NULL && vdu != NULL) {
			
			pia->setVdu(vdu);
			if (mDebugInfo.dbgLevel & DBG_VERBOSE)
				cout << "PIA8255 got pointer to VDU6847\n";
			
			// Get RAM address that matches VDU6847 video memory address
			// Get RAM device that matches the program load address
			RAM* ram = NULL;
			uint16_t video_mem_start_adr = vdu -> getVideoMemAdr();
			if (mDebugInfo.dbgLevel & DBG_VERBOSE)
				cout << "Video Memory starts at address 0x" << hex << video_mem_start_adr << "\n";
			for (int i = 0; i < mDevices.size(); i++) {
				Device* dev = mDevices[i];
				if (dev->selected(video_mem_start_adr) && dev->devType == RAM_DEV) {
					ram = (RAM*)dev;
					break;
				}
			}
			
			if (ram == NULL || !ram->selected(video_mem_start_adr + 6 * 1024 - 1)) {
				cout << "couldn't find a RAM device large enough to become video memory\n";
				throw runtime_error("couldn't find a RAM device large enough to become video memory");
			}
			if (mDebugInfo.dbgLevel & DBG_VERBOSE)
				cout << "Found RAM that matches video memory range\n";
			vdu-> setVideoRam(ram);
			if (mDebugInfo.dbgLevel & DBG_VERBOSE)
				cout << "Video RAM set for VDU6847\n";

	}

	if (!loadData(program))
		throw runtime_error("");

	if (!loadData(data))
		throw runtime_error("");

}


bool Devices::loadData(Program data)
{
	if (data.loadAdr > 0) {

		ifstream pin(data.fileName, ios::in | ios::binary | ios::ate);

		if (!pin) {
			cout << "couldn't open data file " << data.fileName << "\n";
			return false;
		}

		// Get data size
		pin.seekg(0, ios::end);
		int program_size = (int)pin.tellg();
		pin.seekg(0);

		// Get RAM device that matches the data load address
		RAM* ram = NULL;
		for (int i = 0; i < mDevices.size(); i++) {
			Device* dev = mDevices[i];
			if (dev->selected(data.loadAdr) && dev->devType == RAM_DEV) {
				ram = (RAM*)dev;
				break;
			}
		}
		if (ram == NULL || !ram->selected(data.loadAdr + program_size - 1)) {
			cout << "couldn't find a RAM device large enough to hold the data '" + data.fileName + "'\n";
			return false;
		}

		vector<uint8_t> d(program_size);
		pin.read((char*)&d[0], (streamsize)program_size);

		if (true || mDebugInfo.dbgLevel & DBG_VERBOSE)
			cout << "Data from file '" << data.fileName << "' of size " << dec << program_size << " bytes (0x" << hex << program_size << ")" <<
			" will be written to memory at address 0x" << data.loadAdr << " to 0x" << data.loadAdr + program_size - 1 << "\n";

		if (pin.fail() || (int)pin.gcount() < program_size) {
			cout << "couldn't read all bytes from the data file '" + data.fileName + "'\n";
			return false;
		}
		if (!ram->write(data.loadAdr, d, program_size)) {
			cout << "couldn't update a RAM with the data file '" + data.fileName + "\n'";
			return false;
		}

	}

	return true;
}

Devices::~Devices()
{
	for (int i = 0; i < mDevices.size(); i++)
		delete mDevices[i];
}

//
//
bool Devices::read(uint16_t adr, uint8_t & data)
{

	for (int i = 0; i < mDevices.size(); i++) {
		Device* dev = mDevices[i];
		if (dev->selected(adr)) {
			bool success = dev->read(adr, data);
			if (mDebugInfo.dbgLevel & DBG_DEVICE)
				cout << "READ 0x" << hex << adr << " => " << (int)data << "\n";
			return success;
		}
	}

	data = 0xff;
	return true;
}

//
//
//
bool Devices::write(uint16_t adr, uint8_t data)
{

	for (int i = 0; i < mDevices.size(); i++) {
		Device* dev = mDevices[i];
		if (dev->selected(adr)) {
			if (mDebugInfo.dbgLevel & DBG_DEVICE)
				cout << "WRITE 0x" << hex << (int)data << " to 0x" << adr << "\n";
			return dev->write(adr, data);
		}
	}
	return false;
}