#include "Device.h"
#include "RAM.h"
#include "ROM.h"
#include "PIA8255.h"
#include "VDU6847.h"
#include "VIA6522.h"
#include <iostream>
#include <filesystem>

using namespace std;

Device::Device(DeviceEnum typ, uint16_t adr, uint16_t sz) : devType(typ), mDevAdr(adr), mDevSz(sz)
{

}

bool Device::selected(uint16_t adr)
{
	return (adr >= mDevAdr && adr < mDevAdr + mDevSz);
}

bool Device::validAdr(uint16_t adr)
{
	if (!selected(adr)) {
		cout << "Attempt to read an address outside the device's valid address range [0x" << hex <<
			mDevAdr << ", 0x" << mDevAdr + mDevSz - 1 << "]\n";
		return false;
	}
	return true;
}

bool Device::crMemMap(vector< DeviceAllocation> devAllocation, vector<Device*>& devices, string ROMDirPath)
{
	devices.resize(devAllocation.size());

	for (int i = 0; i < devAllocation.size(); i++) {
		DeviceAllocation& a = devAllocation[i];
		switch (a.deviceType) {
		case DeviceEnum::RAM_DEV:
		{
			RAM *ram = new RAM(a.startAdr, a.size);
			devices.push_back((Device *) ram);
			break;
		}
		case DeviceEnum::ROM_DEV:
		{
			filesystem::path dir_path = ROMDirPath;
			filesystem::path file = a.ROMFileName;
			filesystem::path file_path = dir_path / file;
			ROM* rom = new ROM(a.startAdr, a.size, file_path.string());
			devices.push_back((Device*)rom);
			break;
		}
		case DeviceEnum::PIA8255_DEV:
		{
			PIA8255* pia = new PIA8255(a.startAdr);
			devices.push_back((Device*)pia);
			break;
		}
		case DeviceEnum::VDU6847_DEV:
		{
			VDU6847* vdu = new VDU6847(a.startAdr);
			devices.push_back((Device*)vdu);
			break;
		}
		case DeviceEnum::VIA6522_DEV:
		{
			VIA6522* via = new VIA6522(a.startAdr);
			devices.push_back((Device*)via);
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