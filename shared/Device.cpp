#include "Device.h"
#include "RAM.h"
#include "ROM.h"
#include "PIA8255.h"
#include "VDU6847.h"
#include "VIA6522.h"
#include "AtomKeyboardDevice.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <allegro5/allegro.h>
#include <string>
#include "P6502.h"
#include <vector>
#include "AtomCUTSInterface.h"
#include "TapeRecorder.h"
#include "AtomSpeaker.h"

using namespace std;

//
// Device class
//

Device::Device(string n, DeviceId typ, DeviceCategory cat, DebugInfo debugInfo, ConnectionManager *connectionManager) :
	devType(typ), mConnectionManager(connectionManager), mDebugInfo(debugInfo), name(n), category(cat)
{
}

Device::~Device()
{
	for (int i = 0; i < mPorts.size(); i++)
		delete mPorts[i];
}

// Used by a device to make a port available for routing

bool Device::registerPort(string name, PortDirection dir, uint8_t mask, int &index, uint8_t *val)
{
	index = mPortIndex++;
	DevicePort *device_port = new DevicePort();
	device_port->dev = this;
	device_port->name = name;
	device_port->dir = dir;
	device_port->mask = mask;
	device_port->localIndex = index;
	device_port->val = val;
	device_port->globalIndex = -1;
	if (mDebugInfo.dbgLevel & DBG_DEVICE)
		cout << "DEVICE ADDS PORT " << mConnectionManager->printDevicePort(device_port) << "\n";

	mPorts.push_back(device_port);
	
	mConnectionManager->addDevicePort(this, device_port);

	if (mDebugInfo.dbgLevel & DBG_VERBOSE)
	cout << "ADDED " << this->name << " " << _PORT_DIR(dir) << " PORT '" << name << "' #" << dec << index << " (#" << device_port->globalIndex << ")\n";
	

	return true;
}

//
// Update each connected input port (dst) based on the output port (src)
// 
// dst = dst & ~mask | (src & mask) when shifts = 0
// dst = dst & ~mask | ((src >> shifts) & mask) when shifts > 0
// dst = dst & ~mask | ((src << -shifts) & mask) when shifts < 0
//
bool Device::updatePort(int index, uint8_t val)
{
	if (index < 0 && index >= mPorts.size())
		return false;

	if (*(mPorts[index]->val) == val)
		return true;

	*(mPorts[index]->val) = val;

	for (int i = 0; i < mPorts[index]->inputs.size(); i++) {
		InputReference input = mPorts[index]->inputs[i];
		uint8_t pval = *(input.port->val);
		if (input.shifts >= 0)
			*(input.port->val) = ((pval & ~input.mask) | ((val >> input.shifts) & input.mask)) & input.port->mask;
		else
			*(input.port->val) = ((pval & ~input.mask) | ((val << (-input.shifts)) & input.mask)) & input.port->mask;

		if (((mDebugInfo.dbgLevel & DBG_DEVICE) != 0) && *(input.port->val) != pval) {
			string shift_s, c_dir;
			if (input.shifts >= 0) 
				shift_s = "((src >> shifts) & mask)";
			else
				shift_s = "((src << shifts) & mask)";
			cout << input.port->dev->name << ":" << input.port->name << " = " <<
				input.port->name << " &  ~mask | " << shift_s << " = 0x" << hex <<
				(int)pval << " & 0x" << hex << setfill('0') << setw(2) << (int)(uint8_t)(~input.mask) << " | ((0x" << hex << (int)val <<
				(input.shifts >= 0 ? " >> " : " << ") << setfill(' ') << dec << (input.shifts >= 0 ? input.shifts : -input.shifts) <<
				") & 0x" << hex << (int)input.mask << ")" << setfill('0') << setw(2) <<
				" = 0x" << hex << (int)*(input.port->val) << dec << "\n";
		}
	}
	return true;
}

bool Device::getPortIndex(string name, DevicePort * &port) {
	for (int i = 0; i < mPorts.size(); i++) {
		if (mPorts[i]->name == name) {
			port = mPorts[i];
			return true;
		}
	}
	return false;
}


//
// Devices class
//



Devices::Devices(
	string memMapFile, int n60HzCycles, double clockSpeed, ALLEGRO_BITMAP* disp, DebugInfo debugInfo,
	Program program, Program data, ConnectionManager& connection_manager, Device * &vdu,
	Device * &soundDevice, vector<Device*>& otherDevices) :mDebugInfo(debugInfo)
{
	soundDevice = NULL;
	vdu = NULL;
	connection_manager.setDevices(this);

	ifstream fin(memMapFile, ios::in | ios::ate);

	if (!fin) {
		cout << "couldn't open memory map file " << memMapFile << "\n";
		throw runtime_error("couldn't open memory map file");
	}

	fin.seekg(0);

	string line;
	int line_no = 1;
	P6502 * microprocessor = NULL;
	
	while (getline(fin, line)) {

		try {
			string dev_typ_s, dev_adr_s, dev_sz_s, dev_name;
			stringstream sin(line);
			int dev_adr, dev_sz;

			sin >> dev_typ_s;

			if (
				dev_typ_s != "CONNECT" && dev_typ_s != "ATOMKB" && dev_typ_s != "UC6502" && dev_typ_s != "ATOMCAS" && dev_typ_s != "//"
				&& dev_typ_s != "TAPREC" && dev_typ_s != "" && dev_typ_s != "ATOMSP"
				) {
				sin >> dev_name;
				sin >> dev_adr_s;
				dev_adr = stoi(dev_adr_s, 0, 16);
				sin >> dev_sz_s;
				dev_sz = stoi(dev_sz_s, 0, 16);
			}

			if (dev_typ_s == "//" || dev_typ_s == "") {
				// Commment
			}
			
			else if (dev_typ_s == "ATOMKB") {
				sin >> dev_name;
				AtomKeyboardDevice * kb = new AtomKeyboardDevice(dev_name, mDebugInfo, &connection_manager);
				mDevices.push_back(kb);
			}

			else if (dev_typ_s == "ATOMSP") {
				sin >> dev_name;
				AtomSpeaker* sp = new AtomSpeaker(dev_name, clockSpeed, mDebugInfo, &connection_manager);
				mDevices.push_back(sp);
				soundDevice = (Device*)sp;
			}

			else if (dev_typ_s == "TAPREC") {
				sin >> dev_name;
				TapeRecorder* tr = new TapeRecorder(dev_name, clockSpeed, mDebugInfo, &connection_manager);
				mDevices.push_back(tr);
			}

			else if (dev_typ_s == "ATOMCAS") {
				sin >> dev_name;
				AtomCUTSInterface * cuts = new AtomCUTSInterface(dev_name, clockSpeed,  mDebugInfo, &connection_manager);
				mDevices.push_back(cuts);
			}

			else if (dev_typ_s == "UC6502") {
				sin >> dev_name;
				microprocessor = new P6502(dev_name, clockSpeed, mDebugInfo, &connection_manager);
				mDevices.push_back(microprocessor);
			}

			else if (dev_typ_s == "RAM") {
				RAM* ram = new RAM(dev_name, dev_adr, dev_sz, mDebugInfo);
				mDevices.push_back(ram);
			}

			else if (dev_typ_s == "VDU6847") {
				string video_mem_adr_s;
				sin >> video_mem_adr_s;
				uint16_t video_mem_adr = stoi(video_mem_adr_s, 0, 16);
				vdu = new VDU6847(dev_name, dev_adr, n60HzCycles, disp, video_mem_adr, mDebugInfo, &connection_manager);
				mDevices.push_back(vdu);
			}
			else if (dev_typ_s == "ROM") {
				string ROM_file_name;
				sin >> ROM_file_name;
				filesystem::path map_file_path = memMapFile;
				filesystem::path map_dir_path = map_file_path.parent_path();
				filesystem::path ROM_file_path = ROM_file_name;
				filesystem::path file_path = map_dir_path / ROM_file_path;
				ROM* rom = new ROM(dev_name, dev_adr, dev_sz, file_path.string(), mDebugInfo);
				mDevices.push_back(rom);
			}
			else if (dev_typ_s == "PIA8255") {
				PIA8255* pia = new PIA8255(dev_name, dev_adr, n60HzCycles, mDebugInfo, &connection_manager);
				mDevices.push_back(pia);
			}
			else if (dev_typ_s == "VIA6522") {
				VIA6522* via = new VIA6522(dev_name, dev_adr, mDebugInfo, &connection_manager);
				mDevices.push_back(via);
			}
			else if (dev_typ_s == "CONNECT") {
				// CONNECT	VDU:FS				PIA:PortC;7			- Connect VDU FS output to PIA PortSelection C b7			
				// CONNECT	PIA:PortA;7			VDU:A/G				- Connect PIA PortSelection A b7:4 to VDU A/G input and GM0:2 inputs
				// CONNECT	PIA:PortA;6;4		VDU:GM
				// CONNECT	VDU:DIN;7			VDU:INV			- Connect data bus b7:6 to VDU inputs INV,A/S & INT/EXT
				// CONNECT	VDU:DIN;6			VDU:A/S
				// CONNECT	VDU:DIN;6			VUD:INT/EXT
				string src_port, dst_port;
				sin >> src_port;
				sin >> dst_port;
				connection_manager.connect(src_port, dst_port);
				
			}
			else {
				cout << "Syntax error at line " << dec << line_no << ":\n\t" << line << "\n";
				throw runtime_error("Syntax error");
			}
		}
		catch (...) {
			cout << "Syntax error at line " << dec << line_no << ":\n\t" << line <<  "\n";
			throw runtime_error("Syntax error");
		}

		line_no++;
	}

	if (vdu == NULL) {
		cout << "No video data unit device specifed!\n";
		throw runtime_error("No video data unit device specifed");
	}

	if (microprocessor == NULL) {
		cout << "No microprocessor device specifed!\n";
		throw runtime_error("No microprocessor device specifed");
	}

	// Update the video data unit with graphics memory data
	{
			
			if (mDebugInfo.dbgLevel & DBG_VERBOSE)
				cout << "PIA8255 got pointer to VDU6847\n";
			
			// Get RAM address that matches VDU6847 video memory address
			// Get RAM device that matches the program load address
			RAM* ram = NULL;
			uint16_t video_mem_start_adr = ((VDU6847 *) vdu) -> getVideoMemAdr();
			if (mDebugInfo.dbgLevel & DBG_VERBOSE)
				cout << "Video Memory starts at address 0x" << hex << video_mem_start_adr << "\n";
			for (int i = 0; i < mDevices.size(); i++) {
				Device* dev = mDevices[i];
				if (dev->category == PERIPHERAL || dev->category == MEMORY_DEVICE) {
					MemoryMappedDevice * mem_dev = (MemoryMappedDevice *) dev;
					if (mem_dev->selected(video_mem_start_adr) && mem_dev->devType == RAM_DEV) {
						ram = (RAM*)mem_dev;
						break;
					}
				}
			}
			
			if (ram == NULL || !ram->selected(video_mem_start_adr + 6 * 1024 - 1)) {
				cout << "couldn't find a RAM device large enough to become video memory\n";
				throw runtime_error("couldn't find a RAM device large enough to become video memory");
			}
			if (mDebugInfo.dbgLevel & DBG_VERBOSE)
				cout << "Found RAM that matches video memory range\n";
			((VDU6847*)vdu)-> setVideoRam(ram);
			if (mDebugInfo.dbgLevel & DBG_VERBOSE)
				cout << "Video RAM set for VDU6847\n";

	}

	// Update the microprocessor with memory-mapped devices that it shall be able to access
	if (!getMemoryMappedDevices(microprocessor->mDevices) || !getMemoryDevices(microprocessor->mMemories)) {
		cout << "Failed to get memory-mapped devices!\n";
		throw runtime_error("Failed to get memory-mapped devices");
	}

	// Update the microprocessor with memory-mapped devices that it shall be able to access
	if (!getNonVduNonSoundTimeAwareDevices(otherDevices)) {
		cout << "Failed to get non-VDU & non-sound time aware devices!\n";
		throw runtime_error("Failed to get non-VDU & non-sound time aware devices");
	}


	if (!loadData(program))
		throw runtime_error("");

	if (!loadData(data))
		throw runtime_error("");

	if (mDebugInfo.dbgLevel & DBG_VERBOSE)
		connection_manager.printRouting();

}

Devices::~Devices()
{
	for (int i = 0; i < mDevices.size(); i++)
		delete mDevices[i];
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
			if (dev->devType == RAM_DEV && ((MemoryMappedDevice *) dev)->selected(data.loadAdr)) {
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

		if (mDebugInfo.dbgLevel & DBG_VERBOSE)
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

bool Devices::getPeripherals(vector<Device*>& devices)
{
	for (int i = 0; i < mDevices.size(); i++) {
		if (mDevices[i]->category == PERIPHERAL || mDevices[i]->category == VDU_DEVICE) {
			devices.push_back(mDevices[i]);
			if (mDebugInfo.dbgLevel && DBG_VERBOSE)
				cout << "Adding peripheral '" << mDevices[i]->name << "' of type " << _DEVICE_ID(mDevices[i]->devType) << "\n";
		}
	}
	return true;
}

bool Devices::getMemoryMappedDevices(vector<MemoryMappedDevice*> &devices)
{
	for (int i = 0; i < mDevices.size(); i++) {
		if (mDevices[i]->category == PERIPHERAL || mDevices[i]->category == MEMORY_DEVICE) {
			devices.push_back((MemoryMappedDevice *) mDevices[i]);
			if (mDebugInfo.dbgLevel && DBG_VERBOSE)
				cout << "Adding memory-mapped device '" << mDevices[i]->name << "' of type " << _DEVICE_ID(mDevices[i]->devType) << "\n";
		}
	}
	return true;
}

bool Devices::getNonVduNonSoundTimeAwareDevices(vector<Device *> &devices)
{
	for (int i = 0; i < mDevices.size(); i++) {
		if (mDevices[i]->category != MEMORY_DEVICE && mDevices[i]->category != VDU_DEVICE && mDevices[i] -> category != SOUND_DEVICE) {
			devices.push_back(mDevices[i]);
			if (mDebugInfo.dbgLevel && DBG_VERBOSE)
				cout << "Adding non-VDU time-aware device '" << mDevices[i]->name << "' of type " << _DEVICE_ID(mDevices[i]->devType) << "\n";
		}
	}
	return true;
}

bool Devices::getMemoryDevices(vector<MemoryMappedDevice*> &devices)
{
	for (int i = 0; i < mDevices.size(); i++) {
		if (mDevices[i]->category == MEMORY_DEVICE) {
			devices.push_back((MemoryMappedDevice * ) mDevices[i]);
			if (mDebugInfo.dbgLevel && DBG_VERBOSE)
				cout << "Adding memory device '" << mDevices[i]->name << "' of type " << _DEVICE_ID(mDevices[i]->devType) << "\n";
		}
	}
	return true;
}


