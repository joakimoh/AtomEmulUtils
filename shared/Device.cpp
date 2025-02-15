#include "Device.h"
#include "RAM.h"
#include "ROM.h"
#include "PIA8255.h"
#include "VDU6847.h"
#include "VIA6522.h"
#include "CRTC6845.h"
#include "BeebVideoULA.h"
#include "BeebKeyboard.h"
#include "TT5050.h"
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
#include "MemoryMappedDevice.h"
#include "BeebRomSel.h"

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

// Get pointer to other device to be able to call its methods
bool Device::connectDevice(Device* dev)
{
	if (mDebugInfo.dbgLevel & DBG_VERBOSE)
		cout << "DEVICE '" << dev->name << "' can be invoked by device '" << this->name << "'!\n";
	mConnectedDevices.push_back(dev);
	return true;
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

		if ((mDebugInfo.dbgLevel & DBG_DEVICE) != 0 &&  *(input.port->val) != pval) {
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
		
		if (input.port->triggerDevice)
			input.port->dev->trigger(input.port->localIndex);

		
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

uint16_t Devices::getHexVal(stringstream& sin)
{
	string v_s;
	sin >> v_s;
	return stoi(v_s, 0, 16);

}

string Devices::getFileName(string &path, stringstream& sin)
{

	string ROM_file_name;
	sin >> ROM_file_name;
	filesystem::path map_file_path = path;
	filesystem::path map_dir_path = map_file_path.parent_path();
	filesystem::path ROM_file_path = ROM_file_name;
	filesystem::path file_path = map_dir_path / ROM_file_path;
	return file_path.string();
}

Devices::Devices(
	string memMapFile, double clockSpeed, int audioSampleFreq, ALLEGRO_BITMAP* disp, int dispW, int dispH, DebugInfo debugInfo,
	Program program, Program data, ConnectionManager& connection_manager, P6502* &microprocessor, VideoDisplayUnit * &mainVDU,
	vector<Device*>& frameScheduledDevices, vector<Device*>& halflineScheduledDevices, vector<Device*>& instrScheduledDevices) :mDebugInfo(debugInfo)
{
	vector<VideoDisplayUnit*> vdus;

	// BBC Micro Page ROM support
	vector<ROM*> paged_ROMs;
	BeebROMSel* paged_rom_sel = NULL;

	mainVDU = NULL;
	SoundDevice * sound_device = NULL;

	connection_manager.setDevices(this);

	ifstream fin(memMapFile, ios::in | ios::ate);

	if (!fin) {
		cout << "couldn't open memory map file " << memMapFile << "\n";
		throw runtime_error("couldn't open memory map file");
	}

	fin.seekg(0);

	string line;
	int line_no = 1;
	
	while (getline(fin, line)) {

		try {
			string cmd;
			stringstream sin(line);		

			sin >> cmd;

			if (cmd.substr(0,2) == "//" || cmd == "") {
				// Commment
			}

			else if (cmd == "ADD") {

				//
				// Add a device
				//

				string dev_name, dev_type;
				sin >> dev_type;
				sin >> dev_name;

				//
				// Keyboard Devices
				//

				if (dev_type == "ATOMKB") {			

					AtomKeyboardDevice* kb = new AtomKeyboardDevice(dev_name, mDebugInfo, &connection_manager);
					mDevices.push_back(kb);

				}

				else if (dev_type == "BEEBKB") {

					uint8_t startup_options = getHexVal(sin) & 0xff;
					BeebKeyboard* kb = new BeebKeyboard(dev_name, startup_options, mDebugInfo, &connection_manager);
					mDevices.push_back(kb);

				}

				//
				// Audio Devices
				//

				else if (dev_type == "ATOMSP") {

					AtomSpeaker* sp = new AtomSpeaker(dev_name, clockSpeed, audioSampleFreq, mDebugInfo, &connection_manager);
					mDevices.push_back(sp);
					sound_device = sp;

				}

				//
				// Tape Devices
				//

				else if (dev_type == "TAPREC") {

					TapeRecorder* tr = new TapeRecorder(dev_name, clockSpeed, mDebugInfo, &connection_manager);
					mDevices.push_back(tr);

				}

				else if (dev_type == "ATOMCAS") {

					AtomCUTSInterface* cuts = new AtomCUTSInterface(dev_name, clockSpeed, mDebugInfo, &connection_manager);
					mDevices.push_back(cuts);

				}

				//
				// Microcontrollers
				//

				else if (dev_type == "UC6502") {

					microprocessor = new P6502(dev_name, clockSpeed, mDebugInfo, &connection_manager);
					mDevices.push_back(microprocessor);

				}

				//
				// Memory Devices
				//

				else if (dev_type == "SRAM") {		

					uint16_t dev_adr = getHexVal(sin);
					uint16_t dev_sz = getHexVal(sin);
					RAM* ram = new RAM(dev_name, false, dev_adr, dev_sz, mDebugInfo, &connection_manager);
					mDevices.push_back(ram);

				}

				else if (dev_type == "DRAM") {

					uint16_t dev_adr = getHexVal(sin);
					uint16_t dev_sz = getHexVal(sin);
					RAM* ram = new RAM(dev_name, true, dev_adr, dev_sz, mDebugInfo, &connection_manager);
					mDevices.push_back(ram);

				}

				else if (dev_type == "ROM") {

					uint16_t dev_adr = getHexVal(sin);
					uint16_t dev_sz = getHexVal(sin);
					string ROM_file_path = getFileName(memMapFile, sin);
					ROM* rom = new ROM(dev_name, dev_adr, dev_sz, ROM_file_path, mDebugInfo, &connection_manager);
					mDevices.push_back(rom);

				}

				else if (dev_type == "BeebRomSel") {

					uint16_t dev_adr = getHexVal(sin);
					uint16_t dev_sz = getHexVal(sin);
					paged_rom_sel = new BeebROMSel(dev_name, dev_adr, mDebugInfo, &connection_manager);
					mDevices.push_back(paged_rom_sel);

				}

				//
				// Serial & Parallel I/O Devices
				//

				else if (dev_type == "PIA8255") {

					uint16_t dev_adr = getHexVal(sin);
					uint16_t dev_sz = getHexVal(sin);
					PIA8255* pia = new PIA8255(dev_name, dev_adr, mDebugInfo, &connection_manager);
					mDevices.push_back(pia);

				}

				else if (dev_type == "VIA6522") {

					uint16_t dev_adr = getHexVal(sin);
					uint16_t dev_sz = getHexVal(sin);
					VIA6522* via = new VIA6522(dev_name, dev_adr, mDebugInfo, &connection_manager);
					mDevices.push_back(via);

				}

				//
				// Video Devices
				//

				else if (dev_type == "VDU6847") {

					uint16_t dev_adr = getHexVal(sin);
					uint16_t dev_sz = getHexVal(sin);
					uint16_t video_mem_adr = getHexVal(sin);
					mainVDU = new VDU6847(dev_name, dev_adr, clockSpeed, disp, dispW, dispH, video_mem_adr, mDebugInfo, &connection_manager);
					mDevices.push_back(mainVDU);
					vdus.push_back(mainVDU);

				}

				else if (dev_type == "CRTC6845") {

					uint16_t dev_adr = getHexVal(sin);
					uint16_t dev_sz = getHexVal(sin);
					CRTC6845* crtc = new CRTC6845(dev_name, dev_adr, clockSpeed, disp, dispW, dispH, mDebugInfo, &connection_manager);
					mDevices.push_back(crtc);
					vdus.push_back(crtc);
				}

				else if (dev_type == "TT5050") {

					TT5050* tcg = new TT5050(dev_name, 0x0, clockSpeed, disp, 0x0, mDebugInfo, &connection_manager);
					mDevices.push_back(tcg);
				}

				else if (dev_type == "BeebVideoULA") {

					uint16_t dev_adr = getHexVal(sin);
					uint16_t dev_sz = getHexVal(sin);
					mainVDU = new BeebVideoULA(dev_name, dev_adr, clockSpeed, disp, dispW, dispH, mDebugInfo, &connection_manager);
					mDevices.push_back(mainVDU);
					vdus.push_back(mainVDU);
				}

			}

			else if (cmd == "GAP") {
				string mem_dev_name;
				sin >> mem_dev_name;
				Device* dev = NULL;
				if (!getDevice(mem_dev_name, dev) || !dev->memoryMapped()) {
					cout << "Can't find any memory-mapped device '" << mem_dev_name << "' at line " << dec << line_no << ":\n\t" << line << "\n";
					throw runtime_error("Syntax error");
				}
				uint16_t gap_adr = getHexVal(sin);
				uint16_t gap_sz = getHexVal(sin);
				MemoryMappedDevice* mem_dev = (MemoryMappedDevice*)dev;
				mem_dev->addMemoryGap(gap_adr, gap_sz);

			}

			else if (cmd == "CONNECT") {

				//
				// Connect Device ports
				// 
				// Syntax;
				//	CONNECT <src device>:<src port>[;<high bit>[;<low bit>]]	<dst device>:<dst port>[;<high bit>[;<low bit>]]
				//

				string src_port, dst_port;
				sin >> src_port;
				sin >> dst_port;
				connection_manager.connect(src_port, dst_port);

			}

			else if (cmd == "TRIGGER") {

				//
				// Define triggering of device execution based on memory access (MEM), port changes (PORT) or  explicit call by another device (CALL)
				//

				string triggered_device_name, access, accessed_device_name, accessed_adr_s;
				bool write = false;
				sin >> triggered_device_name;
				Device* triggered_device = NULL;
				if (!getDevice(triggered_device_name, triggered_device)) {
					cout << "Unknown device '" << triggered_device_name << "' at line " << dec << line_no << ":\n\t" << line << "\n";
					throw runtime_error("Syntax error");
				}
				string trigger_type;
				sin >> trigger_type;

				if (trigger_type == "MEM") {
					sin >> accessed_device_name;
					Device* accessed_device = NULL;
					if (!getDevice(accessed_device_name, accessed_device)) {
						cout << "Syntax error at line " << dec << line_no << ":\n\t" << line << "\n";
						throw runtime_error("Syntax error");
					}
					sin >> access;
					if (access == "READ")
						write = false;
					else if (access == "WRITE")
						write = true;
					else {
						cout << "Syntax error at line " << dec << line_no << ":\n\t" << line << "\n";
						throw runtime_error("Syntax error");
					}
					sin >> accessed_adr_s;
					uint16_t accessed_adr = stoi(accessed_adr_s, 0, 16);
					((MemoryMappedDevice*)accessed_device)->registerAccess(triggered_device, accessed_adr, write);
				}
				else if (trigger_type == "PORT") {
					DevicePort* device_port;
					string port_name;
					sin >> port_name;
					if (!triggered_device->getPortIndex(port_name, device_port)) {
						cout << "Unknown port '" << port_name << "' at line " << dec << line_no << ":\n\t" << line << "\n";
						throw runtime_error("Syntax error");
					}
					device_port->triggerDevice = true;
				}
				else if (trigger_type == "CALL") {
					string caller_name;
					sin >> caller_name;
					Device* caller;
					if (!getDevice(caller_name, caller)) {
						cout << "Unknown device '" << caller_name << "' at line " << dec << line_no << ":\n\t" << line << "\n";
						throw runtime_error("Syntax error");
					}
					if (!caller->connectDevice(triggered_device)) {
						cout << "Failed to connect device '" << triggered_device_name << " and '" << caller_name << "' at line " << dec << line_no << ":\n\t" << line << "\n";
						throw runtime_error("Syntax error");
					}
				}
			}
			else if (cmd == "SCHED") {

				//
				// Define timely scheduling of device execution
				//

				string dev_s, sch_s;
				sin >> dev_s;
				Device* sch_dev = NULL;
				if (!getDevice(dev_s, sch_dev)) {
					cout << "Specified device '" << dev_s << "' cannot be found at line " << dec << line_no << ":\n\t" << line << "\n";
					throw runtime_error("Syntax error");
				}
				sin >> sch_s;
				if (sch_s == "FRAME") {
					sch_dev->scheduling = FRAME;
				}
				else if (sch_s == "HLINE") {
					sch_dev->scheduling = HLINE;
				}
				else if (sch_s == "INSTR") {
					sch_dev->scheduling = INSTR;
				}
				else if (sch_s == "NONE")
					sch_dev->scheduling = NONE;
				else {
					cout << "Invalid scheduling '" << sch_s << "' policy at line " << dec << line_no << ":\n\t" << line << "\n";
					throw runtime_error("Syntax error");
				}
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

	if (mainVDU == NULL) {
		cout << "No video data unit device specifed!\n";
		throw runtime_error("No video data unit device specifed");
	}

	if (microprocessor == NULL) {
		cout << "No microprocessor device specifed!\n";
		throw runtime_error("No microprocessor device specifed");
	}

	
	if (paged_ROMs.size() > 0) {
		if (paged_rom_sel == NULL) {
			cout << "No paged ROM register specifed!\n";
			throw runtime_error("No paged ROM register specifed");
		}
		paged_rom_sel->addROMs(paged_ROMs);
	}

	// Update the video data unit(s) with graphics memory data
	{
		for (int v = 0; v < vdus.size(); v++) {

			VideoDisplayUnit* vdu = vdus[v];

			// Get RAM address that matches the VDU's video memory address
			// Get RAM device that matches the program load address
			RAM* ram = NULL;
			uint16_t video_mem_start_adr = vdu->getVideoMemAdr();
			if (mDebugInfo.dbgLevel & DBG_VERBOSE)
				cout << "Video Memory starts at address 0x" << hex << video_mem_start_adr << "\n";
			for (int i = 0; i < mDevices.size(); i++) {
				Device* dev = mDevices[i];
				if (dev->category == PERIPHERAL || dev->category == MEMORY_DEVICE) {
					MemoryMappedDevice* mem_dev = (MemoryMappedDevice*)dev;
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
			vdu->setVideoRam(ram);
			if (mDebugInfo.dbgLevel & DBG_VERBOSE)
				cout << "Video RAM set for '" << vdu->name << "' (" << _DEVICE_ID(vdu->devType) << ")\n";

		}
	}

	// Update the microprocessor with memory-mapped devices that it shall be able to access
	if (!getMemoryMappedDevices(microprocessor->mDevices) || !getMemoryDevices(microprocessor->mMemories)) {
		cout << "Failed to get memory-mapped devices!\n";
		throw runtime_error("Failed to get memory-mapped devices");
	}

	// Sort the devices according to their specified scheduling
	for (int i = 0; i < mDevices.size(); i++) {
		Device* d = mDevices[i];
		if (d->category == MICROROCESSOR_DEVICE || d->category == VDU_DEVICE || d->category==MEMORY_DEVICE)
			continue;
		else if (d->scheduling == INSTR)
			instrScheduledDevices.push_back(d);
		else if (d->scheduling == HLINE)
			halflineScheduledDevices.push_back(d);
		else if (d->scheduling == FRAME)
			frameScheduledDevices.push_back(d);
		if (mDebugInfo.dbgLevel & DBG_VERBOSE)
			cout << d->name << " scheduled on " << _SCHEDULING(d->scheduling) << " basis\n";
	}

	if (!getZPMemDevice(microprocessor->mZPMemDev)) {
		cout << "Failed to zero-page memory device!\n";
		throw runtime_error("Failed to zero-page memory device");
	}

	if (!loadData(program))
		throw runtime_error("");

	if (!loadData(data))
		throw runtime_error("");

	if (mDebugInfo.dbgLevel & DBG_VERBOSE)
		connection_manager.printRouting();

	if (sound_device != NULL)
		sound_device->setFrameRate(mainVDU->getFrameRate());

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
		if (mDevices[i]->memoryMapped()) {
			devices.push_back((MemoryMappedDevice *) mDevices[i]);
			if (mDebugInfo.dbgLevel && DBG_VERBOSE)
				cout << "Adding memory-mapped device '" << mDevices[i]->name << "' of type " << _DEVICE_ID(mDevices[i]->devType) << "\n";
		}
	}
	return true;
}

bool Devices::getOtherDevices(vector<Device *> &devices)
{
	for (int i = 0; i < mDevices.size(); i++) {
		if (
			mDevices[i]->category != MICROROCESSOR_DEVICE &&
			mDevices[i]->category != MEMORY_DEVICE &&
			mDevices[i]->category != VDU_DEVICE &&
			mDevices[i]->category != SOUND_DEVICE &&
			mDevices[i]->category != KEYBOARD_DEVICE
			) {
			devices.push_back(mDevices[i]);
			if (mDebugInfo.dbgLevel && DBG_VERBOSE)
				cout << "Adding other device '" << mDevices[i]->name << "' of type " << _DEVICE_ID(mDevices[i]->devType) << "\n";
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
bool Devices::getZPMemDevice(MemoryMappedDevice * &zpMem)
{
	for (int i = 0; i < mDevices.size(); i++) {
		Device* dev = mDevices[i];
		if (dev->category == MEMORY_DEVICE) {
			MemoryMappedDevice* mdev = (MemoryMappedDevice*)dev;
			if (mdev->selected(0x0) && mdev->selected(0xff)) {
				if (mDebugInfo.dbgLevel & DBG_VERBOSE)
					cout << "Adding zero page memory device '" << mDevices[i]->name << "' of type " << _DEVICE_ID(mDevices[i]->devType) << "\n";
				zpMem = mdev;
			}
		return true;
		}
	}
	return false;
}


