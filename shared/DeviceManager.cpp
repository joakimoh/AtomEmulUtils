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
#include "DeviceManager.h"
#include "Device.h"
#include "SDCard.h"
#include "ADC7002.h"
#include "MemoryProxyDevice.h"
#include "KeyboardDevice.h"
#include "DeviceMemorySegment.h"
#include "MemorySegmentTree.h"
#include "Display.h"
#include "TimedDevice.h"

using namespace std;

//
// DeviceManager class
//

uint16_t DeviceManager::getHexVal(stringstream& sin)
{
	string v_s;
	sin >> v_s;
	return stoi(v_s, 0, 16);

}

double DeviceManager::getDoubleVal(stringstream& sin)
{
	string v_s;
	sin >> v_s;
	return stod(v_s);
}

int DeviceManager::getIntVal(stringstream& sin)
{
	string v_s;
	sin >> v_s;
	return stoi(v_s, 0, 10);
}

string DeviceManager::getFileName(string& path, stringstream& sin)
{

	string ROM_file_name;
	sin >> ROM_file_name;
	filesystem::path map_file_path = path;
	filesystem::path map_dir_path = map_file_path.parent_path();
	filesystem::path ROM_file_path = ROM_file_name;
	filesystem::path file_path = map_dir_path / ROM_file_path;
	return file_path.string();
}

DeviceManager::DeviceManager(
	string memMapFile, double& tickRate, Display* display, DebugTracing* debugTracing,
	ConnectionManager* mCM, P6502*& microprocessor, VideoDisplayUnit*& mainVDU, SoundDevice* &sound_device, vector<Device*>& allDevices,
	vector<TimedDevice*>& baseRateScheduledDevices, vector<TimedDevice*>& subRateScheduledDevices, vector<TimedDevice*>& instructionRateScheduledDevices,
	double speed, double& baseSchedulingRate, double& highSchedulingRate
) : mDM(debugTracing), mCM(mCM), mDisplay(display)
{

	tickRate = 1.0; // Default 1 Mhz simulation clock


	mainVDU = NULL;
	sound_device = NULL;

	mCM->setDeviceManager(this);

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

			else if (cmd == "TICK_RATE") {
				sin >> tickRate;
			}

			else if (cmd == "ADD") {

				//
				// Add a device
				//

				string dev_name, dev_type;
				sin >> dev_type;
				sin >> dev_name;

				//
				// Keyboard DeviceManager
				//

				if (dev_type == "ATOMKB") {

					AtomKeyboardDevice* kb = new AtomKeyboardDevice(dev_name, tickRate, mDM, mCM);
					mDevices.push_back(kb);

				}

				else if (dev_type == "BEEBKB") {

					uint8_t startup_options = getHexVal(sin) & 0xff;
					BeebKeyboard* kb = new BeebKeyboard(dev_name, tickRate, 1.0, startup_options, mDM, mCM);
					mDevices.push_back(kb);

				}

				//
				// Audio DeviceManager
				//

				else if (dev_type == "ATOMSP") {

					mAudioSampleRate = highSchedulingRate;
					AtomSpeaker* sp = new AtomSpeaker(
						dev_name, tickRate, mAudioSampleRate, baseSchedulingRate, highSchedulingRate, mDM, mCM
					);
					mDevices.push_back(sp);
					sound_device = sp;

				}

				else if (dev_type == "TI4689") {

					mAudioSampleRate = highSchedulingRate;
					TI4689* sd = new TI4689(
						dev_name, tickRate, mAudioSampleRate, baseSchedulingRate, highSchedulingRate, mDM, mCM
					);
					mDevices.push_back(sd);
					sound_device = sd;

				}

				//
				// Tape DeviceManager
				//

				else if (dev_type == "TAPREC") {

					TapeRecorder* tr = new TapeRecorder(dev_name, tickRate, mDM, mCM);
					mDevices.push_back(tr);

				}

				else if (dev_type == "ATOMCAS") {

					AtomCUTSInterface* cuts = new AtomCUTSInterface(dev_name, tickRate, mDM, mCM);
					mDevices.push_back(cuts);

				}

				//
				// Microprocessors
				//

				else if (dev_type == "CPU_6502") {
					double cpuClock;
					sin >> cpuClock;
					microprocessor = new P6502(dev_name, cpuClock, tickRate, mDM, mCM, this);
					mDevices.push_back(microprocessor);
					mMicroprocessor = microprocessor;

				}

				//
				// Memory DeviceManager
				//

				else if (dev_type == "SRAM") {

					uint16_t dev_adr = getHexVal(sin);
					uint16_t dev_sz = getHexVal(sin);
					uint8_t wait_states = (uint8_t)(getIntVal(sin) & 0xff);
					RAM* ram = new RAM(dev_name, wait_states, false, dev_adr, dev_sz, mDM, mCM, this);
					mDevices.push_back(ram);
				}

				else if (dev_type == "DRAM") {

					uint16_t dev_adr = getHexVal(sin);
					uint16_t dev_sz = getHexVal(sin);
					uint8_t wait_states = (uint8_t)(getIntVal(sin) & 0xff);
					RAM* ram = new RAM(dev_name, wait_states, true, dev_adr, dev_sz, mDM, mCM, this);
					mDevices.push_back(ram);

				}

				else if (dev_type == "ROM") {

					uint16_t dev_adr = getHexVal(sin);
					uint16_t dev_sz = getHexVal(sin);
					uint8_t wait_states = (uint8_t)(getIntVal(sin) & 0xff);
					string ROM_file_path = getFileName(memMapFile, sin);
					ROM* rom = new ROM(dev_name, wait_states, dev_adr, dev_sz, ROM_file_path, mDM, mCM, this);
					mDevices.push_back(rom);

				}

				else if (dev_type == "BeebRomSel") {

					uint16_t dev_adr = getHexVal(sin);
					uint16_t dev_sz = getHexVal(sin);
					uint8_t wait_states = (uint8_t)(getIntVal(sin) & 0xff);
					BeebROMSel *paged_rom_sel = new BeebROMSel(dev_name, tickRate, wait_states, dev_adr, mDM, mCM, this);
					mDevices.push_back(paged_rom_sel);

				}

				//
				// Serial & Parallel I/O DeviceManager
				//

				else if (dev_type == "BeebSerULA") {

					uint16_t dev_adr = getHexVal(sin);
					uint16_t dev_sz = getHexVal(sin);
					uint8_t wait_states = (uint8_t)(getIntVal(sin) & 0xff);
					BeebSerialULA* serial_ULA = new BeebSerialULA(dev_name, dev_adr, tickRate, wait_states, mDM, mCM, this);
					mDevices.push_back(serial_ULA);

				}

				else if (dev_type == "PIA8255") {

					uint16_t dev_adr = getHexVal(sin);
					uint16_t dev_sz = getHexVal(sin);
					uint8_t wait_states = (uint8_t)(getIntVal(sin) & 0xff);
					PIA8255* pia = new PIA8255(dev_name, tickRate, wait_states, dev_adr, mDM, mCM, this);
					mDevices.push_back(pia);

				}

				else if (dev_type == "VIA6522") {

					uint16_t dev_adr = getHexVal(sin);
					uint16_t dev_sz = getHexVal(sin);
					uint8_t wait_states = (uint8_t)(getIntVal(sin) & 0xff);
					double clk = getDoubleVal(sin);
					VIA6522* via = new VIA6522(dev_name, dev_adr, clk, tickRate, wait_states, mDM, mCM, this);
					mDevices.push_back(via);

				}

				else if (dev_type == "ACIA6850") {

					uint16_t dev_adr = getHexVal(sin);
					uint16_t dev_sz = getHexVal(sin);
					uint8_t wait_states = (uint8_t)(getIntVal(sin) & 0xff);
					double clk = getDoubleVal(sin);
					ACIA6850* acia = new ACIA6850(dev_name, dev_adr, clk, tickRate, wait_states, mDM, mCM, this);
					mDevices.push_back(acia);

				}

				//
				// Video DeviceManager
				//

				else if (dev_type == "VDU6847") {

					uint16_t dev_adr = getHexVal(sin);
					uint16_t dev_sz = getHexVal(sin);
					uint8_t wait_states = (uint8_t)(getIntVal(sin) & 0xff);
					uint16_t video_mem_adr = getHexVal(sin);
					if (!mDisplay->initialised()) mDisplay->init();
					mainVDU = new VDU6847(dev_name, dev_adr, mDisplay, tickRate, wait_states, video_mem_adr, mDM, mCM, this);
					mDevices.push_back(mainVDU);
				}

				else if (dev_type == "CRTC6845") {

					uint16_t dev_adr = getHexVal(sin);
					uint16_t dev_sz = getHexVal(sin);
					uint8_t wait_states = (uint8_t)(getIntVal(sin) & 0xff);
					if (!mDisplay->initialised()) mDisplay->init();
					CRTC6845* crtc = new CRTC6845(dev_name, dev_adr, tickRate, wait_states, mDM, mCM, this);
					mDevices.push_back(crtc);
				}

				else if (dev_type == "TT5050") {

					if (!mDisplay->initialised()) mDisplay->init();
					TT5050* tcg = new TT5050(dev_name, 0x0, tickRate, 0x0, mDM, mCM);
					mDevices.push_back(tcg);
				}

				else if (dev_type == "BeebVideoULA") {

					uint16_t dev_adr = getHexVal(sin);
					uint16_t dev_sz = getHexVal(sin);
					uint8_t wait_states = (uint8_t)(getIntVal(sin) & 0xff);
					if (!mDisplay->initialised()) mDisplay->init();
					mainVDU = new BeebVideoULA(dev_name, dev_adr, mDisplay, tickRate, wait_states, mDM, mCM, this);
					mDevices.push_back(mainVDU);
				}

				//
				// Misc. devices
				//

				// Emulation of BBC Micro IC31 & IC32
				else if (dev_type == "BEEBVIALATCH") {
					BeebViaLatch* latch = new BeebViaLatch(dev_name, tickRate, mDM, mCM);
					mDevices.push_back(latch);
				}

				else if (dev_type == "SD_CARD") {
					string card_file_path = getFileName(memMapFile, sin);
					SDCard* card = new SDCard(dev_name, card_file_path, mDM, mCM);
					mDevices.push_back(card);
				}

				else if (dev_type == "ADC7002") {
					uint16_t dev_adr = getHexVal(sin);
					uint16_t dev_sz = getHexVal(sin);
					uint8_t wait_states = (uint8_t)(getIntVal(sin) & 0xff);
					double clk = getDoubleVal(sin);
					ADC7002* adc = new ADC7002(dev_name, tickRate, clk, dev_adr, dev_sz, wait_states, mDM, mCM, this);
					mDevices.push_back(adc);
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
				mem_dev->registerMemoryGap(gap_adr, gap_sz);

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
				if (!mCM->connect(src_port, dst_port, invert, process)) {
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
					if (VERBOSE_OUTPUT)
						cout << "Device '" << triggered_device->name << "' triggered by memory access to device '" << accessed_device->name << "'\n";
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
					if (VERBOSE_OUTPUT)
						cout << "Device '" << triggered_device->name << "' triggered by call from device '" << caller->name << "'\n";
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
				if (sch_s == "LOW_RATE") {
					sch_dev->scheduling = LOW_RATE;
				}
				else if (sch_s == "HIGH_RATE") {
					sch_dev->scheduling = HIGH_RATE;
				}
				else if (sch_s == "INSTR_RATE") {
					sch_dev->scheduling = INSTR_RATE;
				}
				else if (sch_s == "NONE")
					sch_dev->scheduling = NONE;
				else {
					cout << "Invalid scheduling '" << sch_s << "' policy at line " << dec << line_no << ":\n\t" << line << "\n";
					throw runtime_error("Syntax error");
				}
			}
			else if (cmd == "EMU_LOW_RATE") {
				sin >> baseSchedulingRate;
				if (mDevices.size() > 0)
					throw runtime_error("EMU_LOW_RATE statement must come before adding any devices!");
			}
			else if (cmd == "EMU_HIGH_RATE") {
				sin >> highSchedulingRate;
				if (mDevices.size() > 0)
					throw runtime_error("EMU_HIGH_RATE statement must come before adding any devices!");
			}
			else if (cmd == "VIDEO") {
				string video_fmt_s;
				sin >> video_fmt_s;
				if (mDisplay->initialised())
					throw runtime_error("VIDEO statement must come before adding any video display unit!");
				if (video_fmt_s == "PAL")
					mDisplay->init(VideoFormat::PAL_FMT);
				else if (video_fmt_s == "NTSC")
					mDisplay->init(VideoFormat::NTSC_FMT);
				else
					throw runtime_error("Illegal video format '" + video_fmt_s + "'");
			}
			else if (cmd == "INIT") {
				string dst_port_s;
				sin >> dst_port_s;
				uint8_t port_val = (uint8_t)(getIntVal(sin) & 0xff);
				PortSelection dst_port_sel;
				if (
					!mCM->extractPort(dst_port_s, dst_port_sel) &&
					!(dst_port_sel.port->dir == IN_PORT || dst_port_sel.port->dir == IO_PORT)) {
					cout << "Invalid port '" << dst_port_s << "' in INIT statement!\n";
					throw runtime_error("Syntax error");
				}
				DevicePort* dst_port = dst_port_sel.port;
				Device* dev = dst_port->dev;
				uint8_t pval = *(dst_port->valIn);
				uint8_t mask = dst_port_sel.bits.mask;
				uint8_t shifts = dst_port_sel.bits.lowBit;
				uint8_t nval = ((pval & ~mask) | ((port_val << shifts) & mask)) & mask;
				*(dst_port->valIn) = nval;
				//cout << "INIT " << dst_port_s << " " << hex << (int)port_val << " => value change from 0x" << (int)pval << " to 0x" << (int)nval << "\n";
			}

			else {
				throw runtime_error("Syntax error");
			}
		}
		catch (...) {
			cout << "Error found at line " << dec << line_no << " of the memory map file:\n\t" << line << "\n";
			throw runtime_error("Syntax error");
		}

		line_no++;
	}

	if (mainVDU == NULL) {
		cout << "Warning: No video display unit specifed - will run headless!\n";
		//throw runtime_error("No video data unit device specifed");
	}

	if (microprocessor == NULL) {
		cout << "No microprocessor device specifed!\n";
		throw runtime_error("No microprocessor device specifed");
	}

	// Update the video data unit with graphics memory data
	if (mainVDU != nullptr) {


		// Get RAM address that matches the VDU's video memory address
		// Get RAM device that matches the program load address
		RAM* ram = NULL;
		uint16_t video_mem_start_adr = mainVDU->getVideoMemAdr();
		if (VERBOSE_EXT_OUTPUT)
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
		if (VERBOSE_EXT_OUTPUT)
			cout << "Found RAM that matches video memory range\n";
		mainVDU->setVideoRam(ram);
		if (VERBOSE_EXT_OUTPUT)
			cout << "Video RAM set for '" << mainVDU->name << "' (" << _DEVICE_ID(mainVDU->devType) << ")\n";

	}


	// Create a memory map based on all registered memory-mapped devices
	createMemoryMap();

	// Sort the devices according to their specified scheduling
	// Also power on (reset) each device and propagate its ports' values to connected devices
	for (int i = 0; i < mDevices.size(); i++) {
		Device* d = mDevices[i];
		allDevices.push_back(d);
		d->power();
		d->updatePorts();
		if (d->category == MICROROCESSOR_DEVICE || d->category == MEMORY_DEVICE) {
			continue;
		}
		else if (dynamic_cast<TimedDevice*>(d) != nullptr) {
			TimedDevice* td = dynamic_cast<TimedDevice*>(d);
			if (d->scheduling == INSTR_RATE)
				instructionRateScheduledDevices.push_back(td);
			else if (d->scheduling == HIGH_RATE)
				subRateScheduledDevices.push_back(td);
			else if (d->scheduling == LOW_RATE)
				baseRateScheduledDevices.push_back(td);
		}
		if (VERBOSE_OUTPUT)
			cout << d->name << " scheduled on " << _SCHEDULING(d->scheduling) << " basis\n";
	}
	if (VERBOSE_EXT_OUTPUT)
		cout << "Each device now powered on (reset) and each of its port's output have been shared with the connected devices...\n";

	if (!getPageMemDevice(microprocessor->mZPMemDev, 0)) {
		cout << "Failed to get zero-page memory device!\n";
		throw runtime_error("Failed to get zero-page memory device");
	}

	if (!getPageMemDevice(microprocessor->mStackMemDev, 1)) {
		cout << "Failed to get page one memory device!\n";
		throw runtime_error("Failed to get page one memory device");
	}

	if (VERBOSE_EXT_OUTPUT)
		mCM->printRouting();

	if (VERBOSE_OUTPUT)
		printMemoryMap();

}

DeviceManager::~DeviceManager()
{
	// Delete all devices
	for (int i = 0; i < mDevices.size(); i++) {
		delete mDevices[i];
	}
	
	// Also delete paged memory proxy devices (if they exist)
	for (int i = 0; i < mMemoryProxyDevices.size(); i++) {

		delete mMemoryProxyDevices[i];
	}
}

bool DeviceManager::loadData(Program data)
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

		if (VERBOSE_OUTPUT)
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

bool DeviceManager::getPeripherals(vector<Device*>& devices)
{
	for (int i = 0; i < mDevices.size(); i++) {
		if (mDevices[i]->category != MEMORY_DEVICE && mDevices[i]->category != MICROROCESSOR_DEVICE) {
			devices.push_back(mDevices[i]);
			if (VERBOSE_EXT_OUTPUT)
				cout << "Adding peripheral '" << mDevices[i]->name << "' of type " << _DEVICE_ID(mDevices[i]->devType) << "\n";
		}
	}
	return true;
}

bool DeviceManager::getMemoryMappedDevices(vector<MemoryMappedDevice*>& devices)
{
	for (int i = 0; i < mDevices.size(); i++) {
		if (mDevices[i]->memoryMapped()) {
			devices.push_back((MemoryMappedDevice*)mDevices[i]);
			if (VERBOSE_EXT_OUTPUT)
				cout << "Adding memory-mapped device '" << mDevices[i]->name << "' of type " << _DEVICE_ID(mDevices[i]->devType) << "\n";
		}
	}
	return true;
}

bool DeviceManager::getOtherDevices(vector<Device*>& devices)
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
			if (VERBOSE_EXT_OUTPUT)
				cout << "Adding other device '" << mDevices[i]->name << "' of type " << _DEVICE_ID(mDevices[i]->devType) << "\n";
		}
	}
	return true;
}

bool DeviceManager::getRAMs(vector<RAM*>& RAMs)
{
	for (int i = 0; i < mDevices.size(); i++) {
		if (mDevices[i]->devType == RAM_DEV) {
			RAMs.push_back((RAM*)mDevices[i]);
			if (VERBOSE_EXT_OUTPUT)
				cout << "Adding RAM '" << mDevices[i]->name << "\n";
		}
	}
	return true;
}

bool DeviceManager::getMemoryDevices(vector<MemoryMappedDevice*>& devices)
{
	for (int i = 0; i < mDevices.size(); i++) {
		if (mDevices[i]->category == MEMORY_DEVICE) {
			devices.push_back((MemoryMappedDevice*)mDevices[i]);
			if (VERBOSE_EXT_OUTPUT)
				cout << "Adding memory '" << mDevices[i]->name << "' of type " << _DEVICE_ID(mDevices[i]->devType) << "\n";
		}
	}
	return true;
}

bool DeviceManager::getPageMemDevice(MemoryMappedDevice*& zpMem, uint8_t page)
{
	uint16_t low_adr = page << 8;
	uint16_t high_adr = low_adr | 0xff;
	for (int i = 0; i < mDevices.size(); i++) {
		Device* dev = mDevices[i];
		if (dev->category == MEMORY_DEVICE) {
			MemoryMappedDevice* mdev = (MemoryMappedDevice*)dev;
			if (mdev->selected(low_adr) && mdev->selected(high_adr)) {
				if (VERBOSE_EXT_OUTPUT)
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
bool DeviceManager::dumpDeviceMemory(uint16_t adr, uint8_t& data)
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

bool DeviceManager::getDevice(string name, Device*& device) {
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
bool DeviceManager::getDevice(DeviceId id, Device*& device) {
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

// Write to a memory-mapped device (for debugger use only)
bool DeviceManager::writeMemoryMappedDevice(uint16_t adr, uint8_t data)
{
	for (int i = 0; i < mMemoryMappedDevices.size(); i++) {
		MemoryMappedDevice* dev = mMemoryMappedDevices[i];
		if (dev->selected(adr)) {
			return dev->write(adr, data);
		}
	}
	return false;
}

bool DeviceManager::createMemoryMap()
{
	// Get a list of all memory-mapped devices
	getMemoryMappedDevices(mMemoryMappedDevices);

	MemoryProxyDevice *proxy = NULL;

	for (int i = 0; i < mMemoryMappedDevices.size(); i++) {

		MemoryMappedDevice* dev = mMemoryMappedDevices[i];
		AddressSpaceInfo claimed_dev_space = dev->getClaimedAddressSpace();

		// First check existing memory proxy devices
		bool proxy_exists = false;
		for (int j = 0; j < mMemoryProxyDevices.size(); j++) {			
			AddressSpaceInfo proxy_space = mMemoryProxyDevices[j]->getClaimedAddressSpace();
			if (claimed_dev_space == proxy_space) {
				proxy = mMemoryProxyDevices[j];
				proxy->addDevice(dev);
				proxy_exists = true;
				break;
			}
		}

		// Then check already registered devices
		if (!proxy_exists) {

			for (int i = 0; i < mNonOverlappingMemoryMappedDevices.size(); i++) {
				MemoryMappedDevice* existing_dev = mNonOverlappingMemoryMappedDevices[i];
				AddressSpaceInfo existing_dev_space = existing_dev->getClaimedAddressSpace();
				if (claimed_dev_space == existing_dev_space) {
					mNonOverlappingMemoryMappedDevices.erase(mNonOverlappingMemoryMappedDevices.begin()+i); // remove from the non-overlapping list as now identified as overlapping
					int index = (int) mMemoryProxyDevices.size();
					AddressSpaceInfo space = dev->getClaimedAddressSpace();

					// Create memory proxy with the existing device as the first entry
					proxy = new MemoryProxyDevice("Proxy_" + to_string(index), space.getStartOfSpace(), space.getSizeOfSpace(), existing_dev, mDM,  mCM, this);

					// Add the analysed device as the proxy's second entry
					proxy -> addDevice(dev);

					// Add the proxy to the list of meory proxies
					mMemoryProxyDevices.push_back(proxy);
					
					proxy_exists = true;
					break;
				}
				else if (existing_dev_space.intersects(claimed_dev_space)) {
					cout << "Device " << dev->name << "'s address space " << claimed_dev_space << " intersects with device " <<
						existing_dev->name << "'s address space " << existing_dev_space << "!\n";
						return false;
				}
			}

			if (!proxy_exists)  {
				mNonOverlappingMemoryMappedDevices.push_back(dev);
			}
		}

	}
	
	// From memory proxes
	for (int i = 0; i < mMemoryProxyDevices.size(); i++) {

		MemoryProxyDevice* dev = mMemoryProxyDevices[i];
		vector<AddressSpace> dev_spaces = dev->getClaimedAddressSpace().getAddressSpaces();
		for (int j = 0; j < dev_spaces.size(); j++) {
			AddressSpace space = dev_spaces[j];
			uint16_t a1 = space.getStartOfSpace();
			uint16_t a2 = space.getEndOfSpace();
			DeviceMemorySegment segment(a1, a2, dev);
			mMemoryTree.insert(segment);
		}

		
	}

	// And from non-overlapping memory-mapped devices
	for(int i = 0; i < mNonOverlappingMemoryMappedDevices.size();i++) {
		MemoryMappedDevice* dev = mNonOverlappingMemoryMappedDevices[i];
		vector<AddressSpace> dev_spaces = dev->getClaimedAddressSpace().getAddressSpaces();
		for (int j = 0; j < dev_spaces.size(); j++) {
			AddressSpace space = dev_spaces[j];
			uint16_t a1 = space.getStartOfSpace();
			uint16_t a2 = space.getEndOfSpace();
			DeviceMemorySegment segment(a1, a2, dev);
			mMemoryTree.insert(segment);
		}
	}
	if (VERBOSE_EXT_OUTPUT)
		mMemoryTree.print();

	return true;
}



MemoryMappedDevice* DeviceManager::getSelectedMemoryMappedDevice(uint16_t adr)
{
	return mMemoryTree.find(adr);
}

void DeviceManager::printMemoryMap()
{
	cout << "\nMEMORY MAP\n\n";
	for (int i = 0; i < mMemoryMappedDevices.size(); i++) {
		MemoryMappedDevice* mm_dev = mMemoryMappedDevices[i];
		AddressSpaceInfo space = mm_dev->getClaimedAddressSpace();
		cout << setw(15) << setfill(' ')  << mm_dev->name << " " << space << "\n";
	}
	cout << "\n";

}