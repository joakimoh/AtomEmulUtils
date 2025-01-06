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

void Devices::toggle60Hz()
{
	for (int i = 0; i < mDevices.size(); i++) {
		Device* dev = mDevices[i];
		if (dev->devType == DeviceEnum::PIA8255_DEV) {
			((PIA8255*)dev)->toggle60Hz();
			break;
		}
	}
}

Devices::Devices(std::string memMapFile, Keyboard *keyboard, DebugInfo debugInfo, Program program) : mDebugInfo(debugInfo)
{

	ifstream fin(memMapFile, ios::in | ios::ate);

	if (!fin) {
		cout << "couldn't open memory map file " << memMapFile << "\n";
		throw runtime_error("couldn't open memory map file");
	}

	fin.seekg(0);

	string line;
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

		if (dev_typ_s == "RAM")
			a.deviceType = DeviceEnum::RAM_DEV; 
		else if (dev_typ_s == "VDU6847") {
			a.deviceType = DeviceEnum::VDU6847_DEV;
		} 
		else if (dev_typ_s == "ROM") {
			a.deviceType = DeviceEnum::ROM_DEV;
			string ROM_file_name;
			sin >> ROM_file_name;
			a.ROMFileName = ROM_file_name;
		} 
		else if (dev_typ_s == "PIA8255")
			a.deviceType = DeviceEnum::PIA8255_DEV; 
		else if (dev_typ_s == "VIA6522")
			a.deviceType = DeviceEnum::VIA6522_DEV;
		else
			a.deviceType = DeviceEnum::UNDEFINED_DEV;

		switch (a.deviceType) {
		case DeviceEnum::RAM_DEV:
		{
			RAM* ram = new RAM(a.startAdr, a.size, mDebugInfo);
			mDevices.push_back(ram);
			break;
		}
		case DeviceEnum::ROM_DEV:
		{
			filesystem::path map_file_path = memMapFile;
			filesystem::path map_dir_path = map_file_path.parent_path();
			filesystem::path ROM_file_path = a.ROMFileName;
			filesystem::path file_path = map_dir_path / ROM_file_path;
			ROM* rom = new ROM(a.startAdr, a.size, file_path.string(), mDebugInfo);
			mDevices.push_back(rom);
			break;
		}
		case DeviceEnum::PIA8255_DEV:
		{
			PIA8255* pia = new PIA8255(a.startAdr, (AtomKeyboard *) keyboard, mDebugInfo);
			mDevices.push_back(pia);
			break;
		}
		case DeviceEnum::VDU6847_DEV:
		{
			VDU6847* vdu = new VDU6847(a.startAdr, mDebugInfo);
			mDevices.push_back(vdu);
			break;
		}
		case DeviceEnum::VIA6522_DEV:
		{
			VIA6522* via = new VIA6522(a.startAdr, mDebugInfo);
			mDevices.push_back(via);
			break;
		}
		default:
			cout << "Unsupported device type " << a.deviceType << "!\n";
			throw runtime_error("Attempt to add unsupported device (should never happen)");
			break;
		}


	}

	if (program.loadAdr > 0) {

		ifstream pin(program.fileName, ios::in | ios::binary | ios::ate);

		if (!pin) {
			cout << "couldn't open program file " << program.fileName << "\n";
			throw runtime_error("couldn't open program file");
		}

		// Get program size
		pin.seekg(0, ios::end);
		int program_size = (int) pin.tellg();
		pin.seekg(0);

		// Get RAM device that matches the program load address
		RAM* ram = NULL;
		for (int i = 0; i < mDevices.size(); i++) {
			Device* dev = mDevices[i];
			if (dev->selected(program.loadAdr) && dev->devType == RAM_DEV) {
				ram = (RAM*)dev;
				break;
			}
		}
		if (ram == NULL || !ram->selected(program.loadAdr + program_size - 1)) {
			throw runtime_error("couldn't find a RAM device large enough to hold the program '" + program.fileName + "'");
		}



		vector<uint8_t> data(program_size);
		pin.read((char*) &data[0], (streamsize) program_size);

		if ( pin.fail() || (int) pin.gcount() < program_size) {
			throw runtime_error("couldn't read all bytes from the program file '" + program.fileName + "'");
		}

		if (!ram->write(program.loadAdr, data, program_size)) {
			throw runtime_error("couldn't update a RAM with the program file '" + program.fileName + "'");
		}

	}

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
			if (mDebugInfo.verbose)
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
			if (mDebugInfo.verbose)
				cout << "WRITE 0x" << hex << (int)data << " to 0x" << adr << "\n";
			return dev->write(adr, data);
		}
	}
	return false;
}