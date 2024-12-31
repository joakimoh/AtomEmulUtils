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

using namespace std;

Device::Device(DeviceEnum typ, uint16_t adr, uint16_t sz, bool verbose) : devType(typ), mDevAdr(adr), mDevSz(sz), mVerbose(verbose)
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

bool Device::crMemMap(string memMapFile, vector<Device*>& devices,  bool verbose)
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
			RAM* ram = new RAM(a.startAdr, a.size, verbose);
			devices.push_back(ram);
			break;
		}
		case DeviceEnum::ROM_DEV:
		{
			filesystem::path map_file_path = memMapFile;
			filesystem::path map_dir_path = map_file_path.parent_path();
			filesystem::path ROM_file_path = a.ROMFileName;
			filesystem::path file_path = map_dir_path / ROM_file_path;
			ROM* rom = new ROM(a.startAdr, a.size, file_path.string(), verbose);
			devices.push_back(rom);
			break;
		}
		case DeviceEnum::PIA8255_DEV:
		{
			PIA8255* pia = new PIA8255(a.startAdr, verbose);
			devices.push_back(pia);
			break;
		}
		case DeviceEnum::VDU6847_DEV:
		{
			VDU6847* vdu = new VDU6847(a.startAdr, verbose);
			devices.push_back(vdu);
			break;
		}
		case DeviceEnum::VIA6522_DEV:
		{
			VIA6522* via = new VIA6522(a.startAdr, verbose);
			devices.push_back(via);
			break;
		}
		default:
			cout << "Unsupported device type " << a.deviceType << "!\n";
			return false;
			break;
		}


	}

	return true;
}

bool Device::freeMemMap(vector<Device*>& devices)
{
	for (int i = 0; i < devices.size(); i++)
		delete devices[i];

	return true;
}