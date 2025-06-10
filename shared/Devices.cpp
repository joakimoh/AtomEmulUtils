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
#include "ACIA6850.h"
#include "BeebSerialULA.h"
#include "TI4689.h"
#include "BeebViaLatch.h"
#include "Device.h"
#include "ConnectionManager.h"
#include "Devices.h"
#include "SDCard.h"

using namespace std;

//
// Devices class
//

uint16_t Devices::getHexVal(stringstream& sin)
{
	string v_s;
	sin >> v_s;
	return stoi(v_s, 0, 16);

}

double Devices::getDoubleVal(stringstream& sin)
{
	string v_s;
	sin >> v_s;
	return stod(v_s);
}

int Devices::getIntVal(stringstream& sin)
{
	string v_s;
	sin >> v_s;
	return stoi(v_s, 0, 10);
}

string Devices::getFileName(string& path, stringstream& sin)
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
	VideoSettings videoSettings,
	string memMapFile, double& cpuClock, int audioSampleFreq, ALLEGRO_DISPLAY* disp, ALLEGRO_BITMAP* dispBitmap, Resolution disRes, DebugManager* debugManager,
	Program program, Program data, ConnectionManager& connection_manager, P6502*& microprocessor, VideoDisplayUnit*& mainVDU, SoundDevice* &sound_device,
	vector<Device*>& fieldScheduledDevices, vector<Device*>& halflineScheduledDevices, vector<Device*>& instrScheduledDevices,
	double speed) :mDM(debugManager)
{
	vector<VideoDisplayUnit*> vdus;

	cpuClock = 1.0; // Default 1 Mhz CPU clock

	// BBC Micro Page ROM support

	mainVDU = NULL;
	sound_device = NULL;

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

			if (cmd.substr(0, 2) == "//" || cmd == "") {
				// Commment
			}

			else if (cmd == "CLOCK") {
				sin >> cpuClock;
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

					AtomKeyboardDevice* kb = new AtomKeyboardDevice(dev_name, cpuClock, mDM, &connection_manager);
					mDevices.push_back(kb);

				}

				else if (dev_type == "BEEBKB") {

					uint8_t startup_options = getHexVal(sin) & 0xff;
					BeebKeyboard* kb = new BeebKeyboard(dev_name, cpuClock, startup_options, mDM, &connection_manager);
					mDevices.push_back(kb);

				}

				//
				// Audio Devices
				//

				else if (dev_type == "ATOMSP") {

					AtomSpeaker* sp = new AtomSpeaker(dev_name, cpuClock, videoSettings.getFieldRate(), audioSampleFreq, mDM, &connection_manager, speed);
					mDevices.push_back(sp);
					sound_device = sp;

				}

				else if (dev_type == "TI4689") {

					TI4689* sd = new TI4689(dev_name, cpuClock, videoSettings.getFieldRate(), audioSampleFreq, mDM, &connection_manager, speed);
					mDevices.push_back(sd);
					sound_device = sd;

				}

				//
				// Tape Devices
				//

				else if (dev_type == "TAPREC") {

					TapeRecorder* tr = new TapeRecorder(dev_name, cpuClock, mDM, &connection_manager);
					mDevices.push_back(tr);

				}

				else if (dev_type == "ATOMCAS") {

					AtomCUTSInterface* cuts = new AtomCUTSInterface(dev_name, cpuClock, mDM, &connection_manager);
					mDevices.push_back(cuts);

				}

				//
				// Microprocessors
				//

				else if (dev_type == "CPU_6502") {

					microprocessor = new P6502(dev_name, cpuClock, mDM, &connection_manager);
					mDevices.push_back(microprocessor);

				}

				//
				// Memory Devices
				//

				else if (dev_type == "SRAM") {

					uint16_t dev_adr = getHexVal(sin);
					uint16_t dev_sz = getHexVal(sin);
					uint8_t wait_states = (uint8_t)(getIntVal(sin) & 0xff);
					RAM* ram = new RAM(dev_name, cpuClock, wait_states, false, dev_adr, dev_sz, mDM, &connection_manager);
					mDevices.push_back(ram);

				}

				else if (dev_type == "DRAM") {

					uint16_t dev_adr = getHexVal(sin);
					uint16_t dev_sz = getHexVal(sin);
					uint8_t wait_states = (uint8_t)(getIntVal(sin) & 0xff);
					RAM* ram = new RAM(dev_name, cpuClock, wait_states, true, dev_adr, dev_sz, mDM, &connection_manager);
					mDevices.push_back(ram);

				}

				else if (dev_type == "ROM") {

					uint16_t dev_adr = getHexVal(sin);
					uint16_t dev_sz = getHexVal(sin);
					uint8_t wait_states = (uint8_t)(getIntVal(sin) & 0xff);
					string ROM_file_path = getFileName(memMapFile, sin);
					ROM* rom = new ROM(dev_name, cpuClock, wait_states, dev_adr, dev_sz, ROM_file_path, mDM, &connection_manager);
					mDevices.push_back(rom);

				}

				else if (dev_type == "BeebRomSel") {

					uint16_t dev_adr = getHexVal(sin);
					uint16_t dev_sz = getHexVal(sin);
					uint8_t wait_states = (uint8_t)(getIntVal(sin) & 0xff);
					BeebROMSel *paged_rom_sel = new BeebROMSel(dev_name, cpuClock, wait_states, dev_adr, mDM, &connection_manager);
					mDevices.push_back(paged_rom_sel);

				}

				//
				// Serial & Parallel I/O Devices
				//

				else if (dev_type == "BeebSerULA") {

					uint16_t dev_adr = getHexVal(sin);
					uint16_t dev_sz = getHexVal(sin);
					uint8_t wait_states = (uint8_t)(getIntVal(sin) & 0xff);
					BeebSerialULA* serial_ULA = new BeebSerialULA(dev_name, dev_adr, cpuClock, wait_states, mDM, &connection_manager);
					mDevices.push_back(serial_ULA);

				}

				else if (dev_type == "PIA8255") {

					uint16_t dev_adr = getHexVal(sin);
					uint16_t dev_sz = getHexVal(sin);
					uint8_t wait_states = (uint8_t)(getIntVal(sin) & 0xff);
					PIA8255* pia = new PIA8255(dev_name, cpuClock, wait_states, dev_adr, mDM, &connection_manager);
					mDevices.push_back(pia);

				}

				else if (dev_type == "VIA6522") {

					uint16_t dev_adr = getHexVal(sin);
					uint16_t dev_sz = getHexVal(sin);
					uint8_t wait_states = (uint8_t)(getIntVal(sin) & 0xff);
					double clk = getDoubleVal(sin);
					VIA6522* via = new VIA6522(dev_name, dev_adr, clk, cpuClock, wait_states, mDM, &connection_manager);
					mDevices.push_back(via);

				}

				else if (dev_type == "ACIA6850") {

					uint16_t dev_adr = getHexVal(sin);
					uint16_t dev_sz = getHexVal(sin);
					uint8_t wait_states = (uint8_t)(getIntVal(sin) & 0xff);
					double clk = getDoubleVal(sin);
					ACIA6850* acia = new ACIA6850(dev_name, dev_adr, clk, cpuClock, wait_states, mDM, &connection_manager);
					mDevices.push_back(acia);

				}

				//
				// Video Devices
				//

				else if (dev_type == "VDU6847") {

					uint16_t dev_adr = getHexVal(sin);
					uint16_t dev_sz = getHexVal(sin);
					uint8_t wait_states = (uint8_t)(getIntVal(sin) & 0xff);
					uint16_t video_mem_adr = getHexVal(sin);
					mainVDU = new VDU6847(dev_name, dev_adr, videoSettings, cpuClock, wait_states, disp, dispBitmap, video_mem_adr, mDM, &connection_manager);
					mDevices.push_back(mainVDU);
					vdus.push_back(mainVDU);

				}

				else if (dev_type == "CRTC6845") {

					uint16_t dev_adr = getHexVal(sin);
					uint16_t dev_sz = getHexVal(sin);
					uint8_t wait_states = (uint8_t)(getIntVal(sin) & 0xff);
					CRTC6845* crtc = new CRTC6845(dev_name, dev_adr, videoSettings, cpuClock, wait_states, dispBitmap, mDM, &connection_manager);
					mDevices.push_back(crtc);
					vdus.push_back(crtc);
				}

				else if (dev_type == "TT5050") {

					TT5050* tcg = new TT5050(dev_name, 0x0, cpuClock, dispBitmap, 0x0, mDM, &connection_manager);
					mDevices.push_back(tcg);
				}

				else if (dev_type == "BeebVideoULA") {

					uint16_t dev_adr = getHexVal(sin);
					uint16_t dev_sz = getHexVal(sin);
					uint8_t wait_states = (uint8_t)(getIntVal(sin) & 0xff);
					mainVDU = new BeebVideoULA(dev_name, dev_adr, videoSettings, cpuClock, wait_states, disp, dispBitmap, mDM, &connection_manager);
					mDevices.push_back(mainVDU);
					vdus.push_back(mainVDU);
				}

				//
				// Misc. devices
				//

				// Emulation of BBC Micro IC31 & IC32
				else if (dev_type == "BEEBVIALATCH") {
					BeebViaLatch* latch = new BeebViaLatch(dev_name, cpuClock, mDM, &connection_manager);
					mDevices.push_back(latch);
				}

				else if (dev_type == "SD_CARD") {
					string card_file_path = getFileName(memMapFile, sin);
					SDCard* card = new SDCard(dev_name, cpuClock, card_file_path, mDM, &connection_manager);
					mDevices.push_back(card);
				}

				else {
					cout << "Syntax error at line " << dec << line_no << ":\n\t" << line << "\n";
					throw runtime_error("Syntax error");
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

			else if (cmd.length() >= 7 && cmd.substr(0, 7) == "CONNECT") {

				//
				// Connect Device ports
				// 
				// Syntax;
				//	CONNECT[:{<qualifer>}+] <src device>:<src port>[;<high bit>[;<low bit>]]	<dst device>:<dst port>[;<high bit>[;<low bit>]]
				// 
				// qualifer ::= 'I' | 'P'
				//

				bool invert = false;
				bool process = false;
				if (cmd.length() == 7) {
				}
				else if (cmd.length() >= 9 && cmd.length() <= 10 && cmd.substr(7, 1) == ":") {
					for (int i = 0; i < cmd.length() - 8; i++) {
						if (cmd.substr(8 + i) == "I")
							invert = true;
						else if (cmd.substr(8 + i) == "P")
							process = true;
					}
				}
				else {
					throw runtime_error("Syntax error");
				}


				string src_port, dst_port;
				sin >> src_port;
				sin >> dst_port;
				if (!connection_manager.connect(src_port, dst_port, invert, process)) {
					throw runtime_error("Syntax error");
				}



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
				if (sch_s == "FIELD") {
					sch_dev->scheduling = FIELD;
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
			else if (cmd == "INIT") {
				string port_s;
				sin >> port_s;
				uint8_t port_val = (uint8_t)(getIntVal(sin) & 0xff);
				PortSelection port_sel;
				if (!connection_manager.extractPort(port_s, port_sel) || port_s.find(";") != string::npos) {
					cout << "Invalid port '" << port_s << "' in INIT statement!\n";
					throw runtime_error("Syntax error");
				}
				DevicePort* dev_port = port_sel.port;
				Device* dev = dev_port->dev;
				*(dev_port->val) = port_val;
			}

			else {
				throw runtime_error("Syntax error");
			}
		}
		catch (...) {
			cout << "Error found at line " << dec << line_no << "of the memory map file:\n\t" << line << "\n";
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

	// Update the video data unit(s) with graphics memory data
	{
		for (int v = 0; v < vdus.size(); v++) {

			VideoDisplayUnit* vdu = vdus[v];

			// Get RAM address that matches the VDU's video memory address
			// Get RAM device that matches the program load address
			RAM* ram = NULL;
			uint16_t video_mem_start_adr = vdu->getVideoMemAdr();
			if (DBG_LEVEL(DBG_VERBOSE))
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
			if (DBG_LEVEL(DBG_VERBOSE))
				cout << "Found RAM that matches video memory range\n";
			vdu->setVideoRam(ram);
			if (DBG_LEVEL(DBG_VERBOSE))
				cout << "Video RAM set for '" << vdu->name << "' (" << _DEVICE_ID(vdu->devType) << ")\n";

		}
	}

	// Update the microprocessor with memory-mapped devices that it shall be able to access
	if (!getMemoryMappedDevices(microprocessor->mDevices) || !getMemoryDevices(microprocessor->mMemories)) {
		cout << "Failed to get memory-mapped devices!\n";
		throw runtime_error("Failed to get memory-mapped devices");
	}

	// Also maintain a separate list of memory-mapped devices for the Devices class itself
	getMemoryMappedDevices(mMemoryMappedDevices);

	// Sort the devices according to their specified scheduling
	// Also power on (reset) each device and propagate its ports' values to connected devices
	for (int i = 0; i < mDevices.size(); i++) {
		Device* d = mDevices[i];
		d->power();
		d->updatePorts();
		if (d->category == MICROROCESSOR_DEVICE || d->category == VDU_DEVICE || d->category == MEMORY_DEVICE)
			continue;
		else if (d->scheduling == INSTR)
			instrScheduledDevices.push_back(d);
		else if (d->scheduling == HLINE)
			halflineScheduledDevices.push_back(d);
		else if (d->scheduling == FIELD)
			fieldScheduledDevices.push_back(d);
		if (DBG_LEVEL(DBG_VERBOSE))
			cout << d->name << " scheduled on " << _SCHEDULING(d->scheduling) << " basis\n";
	}
	if (debugManager->debug(DBG_VERBOSE))
		cout << "Each device now powered on (reset) and each of its port's output have been shared with the connected devices...\n";

	if (!getPageMemDevice(microprocessor->mZPMemDev, 0)) {
		cout << "Failed to get zero-page memory device!\n";
		throw runtime_error("Failed to get zero-page memory device");
	}

	if (!getPageMemDevice(microprocessor->mStackMemDev, 1)) {
		cout << "Failed to get page one memory device!\n";
		throw runtime_error("Failed to get page one memory device");
	}

	if (!loadData(program))
		throw runtime_error("");

	if (!loadData(data))
		throw runtime_error("");

	if (DBG_LEVEL(DBG_VERBOSE))
		connection_manager.printRouting();

	if (sound_device != NULL)
		sound_device->setFieldRate(mainVDU->getFieldRate(), speed);

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
			if (dev->devType == RAM_DEV && ((MemoryMappedDevice*)dev)->selected(data.loadAdr)) {
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

		if (DBG_LEVEL(DBG_VERBOSE))
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
			if (DBG_LEVEL(DBG_VERBOSE))
				cout << "Adding peripheral '" << mDevices[i]->name << "' of type " << _DEVICE_ID(mDevices[i]->devType) << "\n";
		}
	}
	return true;
}

bool Devices::getMemoryMappedDevices(vector<MemoryMappedDevice*>& devices)
{
	for (int i = 0; i < mDevices.size(); i++) {
		if (mDevices[i]->memoryMapped()) {
			devices.push_back((MemoryMappedDevice*)mDevices[i]);
			if (DBG_LEVEL(DBG_VERBOSE))
				cout << "Adding memory-mapped device '" << mDevices[i]->name << "' of type " << _DEVICE_ID(mDevices[i]->devType) << "\n";
		}
	}
	return true;
}

bool Devices::getOtherDevices(vector<Device*>& devices)
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
			if (DBG_LEVEL(DBG_VERBOSE))
				cout << "Adding other device '" << mDevices[i]->name << "' of type " << _DEVICE_ID(mDevices[i]->devType) << "\n";
		}
	}
	return true;
}

bool Devices::getRAMs(vector<RAM*>& RAMs)
{
	for (int i = 0; i < mDevices.size(); i++) {
		if (mDevices[i]->devType == RAM_DEV) {
			RAMs.push_back((RAM*)mDevices[i]);
			if (DBG_LEVEL(DBG_VERBOSE))
				cout << "Adding RAM '" << mDevices[i]->name << "\n";
		}
	}
	return true;
}

bool Devices::getMemoryDevices(vector<MemoryMappedDevice*>& devices)
{
	for (int i = 0; i < mDevices.size(); i++) {
		if (mDevices[i]->category == MEMORY_DEVICE) {
			devices.push_back((MemoryMappedDevice*)mDevices[i]);
			if (DBG_LEVEL(DBG_VERBOSE))
				cout << "Adding memory device '" << mDevices[i]->name << "' of type " << _DEVICE_ID(mDevices[i]->devType) << "\n";
		}
	}
	return true;
}

bool Devices::getPageMemDevice(MemoryMappedDevice*& zpMem, uint8_t page)
{
	uint16_t low_adr = page << 8;
	uint16_t high_adr = low_adr | 0xff;
	for (int i = 0; i < mDevices.size(); i++) {
		Device* dev = mDevices[i];
		if (dev->category == MEMORY_DEVICE) {
			MemoryMappedDevice* mdev = (MemoryMappedDevice*)dev;
			if (mdev->selected(low_adr) && mdev->selected(high_adr)) {
				if (DBG_LEVEL(DBG_VERBOSE))
					cout << "Adding page " << dec << (int) page << " memory device '" << mDevices[i]->name << "' of type " << _DEVICE_ID(mDevices[i]->devType) << "\n";
				zpMem = mdev;
			}
			return true;
		}
	}
	return false;
}

// Non-intrusive reading of the memory location of a device.
// If no memory-mapped device exists at the specified address,
// the method will return false.
bool Devices::dumpDeviceMemory(uint16_t adr, uint8_t& data)
{
	for (int i = 0; i < mMemoryMappedDevices.size(); i++) {
		MemoryMappedDevice* dev = mMemoryMappedDevices[i];
		if (dev->selected(adr)) {
			return dev->dump(adr, data);
		}
	}

	data = 0x0;
	return false;
}
