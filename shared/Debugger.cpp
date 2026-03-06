#include "Debugger.h"
#include "DeviceManager.h"
#include "DebugTracing.h"
#include <iostream>
#include "Utility.h"
#include "Engine.h"
#include <iomanip>
#include <sstream>
#include <filesystem>
#include "Engine.h"
#include "P6502.h"
#include "ConnectionManager.h"
#include "Device.h"
#include "Codec6502.h"
#include "TimedDevice.h"

using namespace std;

Debugger::Debugger(P6502* cpu, GUI *gui, Engine *engine, DeviceManager* deviceManager, ConnectionManager* connectionManager, DebugTracing* debugTracing, string outDir) :
	mDM(deviceManager), mEngine(engine), mOutDir(outDir),mGUI(gui), mCPU(cpu), mCM(connectionManager), mDT(debugTracing)
{
	if (gui == nullptr || engine == nullptr || connectionManager == nullptr || deviceManager == nullptr || debugTracing == nullptr)
		throw runtime_error("some arguments are null pointers");

	mCodec = new Codec6502();
}

Debugger::~Debugger()
{
	delete mCodec;
}

bool Debugger::readOptString(istream& sin, string& s)
{
	if (sin.eof())
		return true;

	sin >> s;

	return s.length() != 0;
}

bool Debugger::readString(istream& sin, string &s)
{
	if (sin.eof())
		return false;

	sin >> s;

	return s.length() != 0;
}


bool Debugger::readHexInt(istream& sin, int& i)
{
	if (sin.eof())
		return false;

	i = -1;
	sin >> hex >> i;

	return i >= 0;
}

bool Debugger::readPosInt(istream& sin, int& i)
{
	if (sin.eof())
		return false;

	i = -1;
	sin >> dec >> i;

	return i >= 0;
}

bool Debugger::readChar(istream& sin, char& c)
{
	if (sin.eof())
		return false;

	sin >> skipws >> c >> noskipws;

	return true;
}

bool Debugger::memMapCmd(istream& sin)
{
	mDM->printMemoryMap();
	mDM->printMemoryTree();
	return true;
}


double Debugger::getDeviceTime(Device* dev)
{
	double t_s;

	if (dev == NULL || !dynamic_cast<TimedDevice*>(dev)) {
		Device* up_dev = NULL;
		if (!mDM->getMicroprocessor(up_dev)) {
			return 0.0;
		}
		P6502* up = static_cast<P6502*>(up_dev);
		t_s = up->getTimeSec();
	}
	return t_s;
}

bool Debugger::dumpDevCmd(istream& sin)
{
	string dev_name;
	if (!readString(sin, dev_name))
		return false;

	Device* dev = NULL;
	if (!mDM->getDevice(dev_name, dev)) {
		cout << "Non-existing device '" << dev_name << "!\n";
		return false;
	}
	double t_s = getDeviceTime(dev);
	cout << "Device time (sec ms us): " << Utility::encodeCPUTime(t_s) << "\n";
	dev->outputState(cout);

	return true;
}

bool Debugger::dumpMPCmd(istream& sin)
{
	Device* dev = NULL;
	
	if (!mDM->getMicroprocessor(dev)) {
		return false;
	}
	double t_s = getDeviceTime(dev);
	cout << "Device time (sec ms us): " << Utility::encodeCPUTime(t_s) << "\n";
	dev->outputState(cout);

	return true;
}

bool Debugger::listDevicesCmd(istream& sin)
{
	vector<Device*> *devices;
	devices = mDM->getDevices();
	for (int i = 0; i < devices->size();i++) {
		cout << left << setfill(' ') << setw(20) << (*devices)[i]->name << ": " << setw(25) << _DEVICE_ID((*devices)[i]->devType) <<
			setw(25) << _DEVICE_CATEGORY((*devices)[i]->category) << 
			((*devices)[i]->memoryMapped()?
				Utility::int2HexStr(((MemoryMappedDevice*)(*devices)[i])->getClaimedAddressSpace().getStartOfSpace(),4) + " " +
				Utility::int2HexStr(((MemoryMappedDevice*)(*devices)[i])->getClaimedAddressSpace().getEndOfSpace(), 4):
				"") <<
			"\n";
	}

	return true;
}

bool Debugger::writeMemCmd(istream& sin)
{
	int a, data;

	if (!readHexInt(sin, a))
		return false;

	int n = 0;
	while (!sin.eof()) {

		if (!readHexInt(sin, data))
			return false;

		if (!mDM->writeMemoryMappedDevice(a++, data)) {
			cout << "Failed to write to memory-mapped device at address 0x" << hex << a << "!\n";
			return false;
		}

		n++;
	}

	return n > 0;
}

bool Debugger::writeMemStrCmd(istream & sin)
{
	int a;
	char c;

	if (!readHexInt(sin, a))
		return false;

	if (!readChar(sin, c))
		return false;

	if (c != '"')
		return false;

	int n = 0;
	while (readChar(sin, c) && c != '"') {
		if (!mDM->writeMemoryMappedDevice(a++, (uint8_t)c)) {
			cout << "Failed to write to memory-mapped device at address 0x" << hex << a << "!\n";
			return false;
		}
		n++;
	}

	return n > 0;
}

bool Debugger::disToFileCmd(istream& sin)
{
	ofstream* fout_p = NULL;
	if (!openOutFile(sin, fout_p)) {
		return false;
	}
	bool success = disCmd(sin, *fout_p);
	fout_p->close();
	return success;
}

bool Debugger::disToScreenCmd(istream& sin)
{
	return disCmd(sin, cout);
}

bool Debugger::disCmd(istream& sin, ostream& sout)
{
	int a1, a2;

	if (!readHexInt(sin, a1))
		return false;

	if (!readHexInt(sin,a2))
		a2 = min(0xffff, a1 + 16);

	if (a1 > a2 || a1 < 0 || a2 < 0 || a1 > 0xffff || a2 > 0xffff) {
		cout << "Only 16-bit address can be specified and the second address parameter cannot be smaller than the first one!\n";
		return true;
	}

	return printInstructions(a1, a2, false, true, sout);
}

bool Debugger::openOutFile(istream& sin, ofstream * &sout)
{
	string s;
	ofstream* fout_p = NULL;
	if (!readString(sin, s)) {
		return false;
	}
	filesystem::path gen_dir_path = mOutDir;
	filesystem::path fout_path = gen_dir_path / s;
	string fn = fout_path.string();
	fout_p = new ofstream(fn, ios::out | ios::binary);
	if (fout_p == NULL || !*fout_p) {
		cout << "Failed to create file '" << fn << "'!\n";
		return false;
	}

	sout = fout_p;

	return true;
}

bool Debugger::readMemToFileCmd(istream& sin)
{
	ofstream* fout_p = NULL;
	if (!openOutFile(sin, fout_p)) {
		return false;
	}
	bool success = readMemCmd(sin, *fout_p);
	fout_p->close();
	delete fout_p;
	return success;
}

bool Debugger::readMemToScreenCmd(istream& sin)
{
	return readMemCmd(sin, cout);
}

bool Debugger::readMemCmd(istream  &sin, ostream &sout)
{
	int a1, a2;

	if (!readHexInt(sin, a1))
		return false;

	if (!readHexInt(sin, a2))
		a2 = a1;

	uint8_t bytes[16] = { 0 };
	int r_sz = 16;
	for (int a = a1; a <= a2; a++) {
		uint8_t data;
		int ofs = (a - a1) % 16;
		if (ofs == 0)
			r_sz = ((a2 - a) < 16 ? a2 - a + 1 : 16);
		if (!mDM->dumpDeviceMemory(a, data)) {
			sout << "Illegal address 0x" << hex << a1 << "\n";
			break;
		}
		bytes[ofs] = data;
		if ((r_sz == 16 && ofs == 15) || a == a2) {
			sout << Utility::int2HexStr(a - ofs, 6);
			for (int i = 0; i < r_sz; i++)
				sout << " " << Utility::int2HexStr(bytes[i], 2);
			for (int i = 0; i < 16 - r_sz; i++)
				sout << "   ";
			sout << " ";
			for (int i = 0; i < r_sz; i++) {
				uint8_t c = bytes[i];
				sout << " " << (c >= 32 && c < 127 ? (char)c : '.');
			}
			sout << "\n";
		}
	}

	return true;
}

bool Debugger::stepCmd(istream &sin, bool stepOver, ostream& sout)
{
	int n = 1;

	if (mEngine->isRunning()) {
		sout << "microprocessor is still running - halt it before trying to step through instructions!\n";
		return true;
	}

	// Check for optional number and - if present - use it to set n
	if (!readPosInt(sin, n))
		n = 1;

	if (stepOver && n != 1) {
		sout << "skip command doesn't take any parameter!\n";
		return false;
	}


	// Wait for stepping to complete
	markStartOfWaiting();
	mEngine->step(n, stepOver);
	markEndOfWaiting();

	if (!stepOver)
		mEngine->printInstrLog(sout);

	return printNextInstr(sout);
}

bool Debugger::printNextInstr(ostream& sout)
{
	uint16_t PC = mCPU->getOpcodePC();
	sout << "NEXT INSTRUCTION ";
	return printInstructions(PC, PC + 3, true, false, sout);
}

bool Debugger::printInstructions(uint16_t startAdr, uint16_t endAdr, bool printFirstOnly, bool ASCII, ostream& sout)
{
	vector<uint8_t> bytes;
	uint16_t pc = startAdr;
	uint8_t data;

	int i = 0;
	for (int a = startAdr; a <= endAdr; a++) {
		string s;
		if (!mDM->dumpDeviceMemory(a, data)) {
			sout << "Next instruction is at an illegal address (0x" << hex << pc << ")\n";
			return false;
		}
		bytes.push_back(data);

		// Test if there is enough bytes to decode a complete instruction
		if (bytes.size() >= 2) {
			int old_pc = pc;
			if (mCodec->decodeInstrFromBytes(pc, bytes, s, ASCII)) {
				int consumed_bytes = pc - old_pc;
				bytes.erase(bytes.begin(), bytes.begin() + consumed_bytes);
				sout << s << "\n";
				i++;
				if (printFirstOnly)
					break;
			}
		}
	}

	return true;
}

bool Debugger::contCmd(istream &sin)
{

	// Wait for breakpoint or an (via GUI debugger menu) explicit user request to stop waiting
	markStartOfWaiting();
	mEngine->cont(cout);
	markEndOfWaiting();

	return true;
}

bool Debugger::resetCmd(istream& sin)
{
	return mCPU->reset() && printNextInstr(cout);
}

bool Debugger::setRegCmd(istream& sin)
{
	if (mEngine->isRunning()) {
		cout << "The micoprocessor is running - it needs to be halted before changing any register value!\n";
		return true;
	}

	string reg;
	if (!readString(sin, reg))
		return false;

	int val;
	if (!readHexInt(sin, val))
		return false;

	if (reg == "A" && val >= 0 && val <= 255)
		mCPU->setAcc((uint8_t)val);
	else if (reg == "X" && val >= 0 && val <= 255)
		mCPU->setRegX((uint8_t)val);
	else if (reg == "Y" && val >= 0 && val <= 255)
		mCPU->setRegY((uint8_t)val);
	else if (reg == "SP" && val >= 0 && val <= 255)
		mCPU->setSP((uint8_t)val);
	else if (reg == "SR" && val >= 0 && val <= 255)
		mCPU->setSR((uint8_t)val);
	else if (reg == "PC" && val >= 0 && val <= 65535)
		mCPU->setPC((uint16_t)val);
	else
		return false;

	return true;
}

bool Debugger::readPortSel(istream& sin, PortSelection &portSel)
{
	string port_s;
	if (!readString(sin, port_s))
		return false;

	if (!mCM->extractPort(port_s, portSel)) {
		cout << "Port doesn't exist!\n";
		return false;
	}

	return true;
}

Device* Debugger::readDevice(istream& sin)
{
	string dev_s;
	if (!readString(sin, dev_s))
		return nullptr;

	Device* dev;
	if ((dev = mCM->getDevice(dev_s)) == nullptr) {
		cout << "Device doesn't exist!\n";
		return nullptr;
	}

	return dev;
}

bool Debugger::setPortCmd(istream& sin)
{
	/*
	if (mEngine->isRunning()) {
		cout << "The micoprocessor is running - it needs to be halted before changing any port value!\n";
		return true;
	}
	*/

	PortSelection port_sel;
	if (!readPortSel(sin, port_sel))
		return false;

	DevicePort* dev_port = port_sel.port;
	Device* dev = dev_port->dev;
	BitsSelection bits = port_sel.bits;

	uint8_t mask = bits.mask; // change mask
	int low_bit = bits.lowBit; // first set bit in change mask
	uint8_t& port_in_val = *(dev_port->valIn);
	uint8_t& port_out_val = *(dev_port->valOut);

	int val;
	if (!readHexInt(sin, val) || val < 0 || (val << low_bit) > mask) {
		cout << "no value compatible with the select port range was provided!\n";
		cout << "mask = " << hex << (int)mask << ", low bit = " << low_bit << " => shifted val = 0x" << (int)(val << low_bit) << "\n";
		return true;
	}

	// Assign new port value
	uint8_t keep_mask = ~mask;
	uint8_t shifted_val = val << low_bit;
	port_in_val = (port_in_val & keep_mask) | (shifted_val & mask);
	

	return true;
}


bool Debugger::listPortsCmd(istream& sin)
{
	string dev_name;
	if (!readString(sin, dev_name))
		return false;

	Device* dev = NULL;
	if (!mDM->getDevice(dev_name, dev)) {
		cout << "Non-existing device '" << dev_name << "!\n";
		return true;
	}
	vector<DevicePort*>* ports = nullptr;
	if (!dev->getPorts(ports) || ports == nullptr) {
		cout << "Failed to get port info for device " << dev->name << "!\n";
		return true;
	}
	for (int i = 0; i < ports->size(); i++) {
		DevicePort* p = (*ports)[i];
		uint8_t cur_dir;
		int sz;
		uint8_t val = Device::getPortVal(p, sz, cur_dir);
		string dir = (p->dir == IN_PORT ? "IN" : (p->dir == OUT_PORT ? "OUT" : "INOUT"));
		string cur_dir_s = Utility::mask2DirStr(cur_dir, sz);
		string val_s = Utility::int2BinStr(val, sz);
		cout << setfill(' ') << setw(10) << p->name << setw(7) << dir << setw(9) << "DIR " << cur_dir_s << setw(2) << " SZ " <<
			setw(2) << sz << setw(sz + 1) << " VAL 0b" << val_s << "\n";
	}
	return true;
}

bool Debugger::breakCmd(istream& sin, bool repetition)
{
	string sub_cmd;
	if (!readString(sin, sub_cmd))
		return false;

	if (sub_cmd == "clr") {
		mEngine->clrBreakPoint();
		return true;
	}

	int a;
	if (!readHexInt(sin, a))
		return false;

	// Indicate to the engine that the debugger will start waiting
	markStartOfWaiting();


	if (sub_cmd == "x") {
		mAccessMode = Engine::ENG_X_BRK_WAIT;
		mEngine->setBreakPointAndWait(cout, Engine::RunState(mAccessMode), a, mReadData, mWrittenData, mOperandAdr, repetition, mLogWinEnabled);
	}
	else if (sub_cmd == "r") {
		mAccessMode = Engine::ENG_R_BRK_WAIT;
		int d;
		if (readHexInt(sin, d)) {
			mReadData = d;
			mAccessMode = Engine::ENG_R_V_BRK_WAIT;
		}
		mEngine->setBreakPointAndWait(cout, Engine::RunState(mAccessMode), a, mReadData, mWrittenData, mOperandAdr, repetition, mLogWinEnabled);
	}
	else if (sub_cmd == "w") {
		mAccessMode = Engine::ENG_W_BRK_WAIT;
		int d;
		if (readHexInt(sin, d)) {
			mWrittenData = d;
			mAccessMode = Engine::ENG_W_V_BRK_WAIT;
		}
		mEngine->setBreakPointAndWait(cout, Engine::RunState(mAccessMode), a, mReadData, mWrittenData, mOperandAdr, repetition, mLogWinEnabled);
	}
	else if (sub_cmd == "rw") {
		mAccessMode = Engine::ENG_RW_BRK_WAIT;
		int d; 
		if (readHexInt(sin, d)) {
			mWrittenData = d;
			mAccessMode = Engine::ENG_RW_V_BRK_WAIT;
		}
		mEngine->setBreakPointAndWait(cout, Engine::RunState(mAccessMode), a, mReadData, mWrittenData, mOperandAdr, false, mLogWinEnabled);
	}
	else {
		cout << "Illegal sub command - valid sub command to break is one of clr,x,r,w and rw!\n";
		return false;
	}

	mEngine->printInstrWindow(cout);
	printNextInstr(cout);

	markEndOfWaiting();

	return true;
}

bool Debugger::haltCmd(istream& sin)
{
	mEngine->halt();
	printNextInstr(cout);

	return true;
}

bool Debugger::memLogCmd(istream& sin)
{
	string sub_cmd;
	if (!readString(sin, sub_cmd))
		return false;

	if (sub_cmd == "clr") {
		mCPU->setMemLogging(-1);
		return true;
	}

	if (sub_cmd != "set")
		return false;

	int a;
	if (!readHexInt(sin, a))
		return false;

	mCPU->setMemLogging(a);
	return true;
}

bool Debugger::logWinCmd(istream& sin)
{
	string sub_cmd;
	if (!readString(sin, sub_cmd))
		return false;

	if (sub_cmd == "clr") {
		mEngine->disableLogWindow();
		mLogWinEnabled = false;
		return true;
	}

	if (sub_cmd != "set")
		return false;

	int sz;
	if (!readPosInt(sin, sz))
		return false;

	if (!mEngine->enableLogWindow(sz))
		return false;

	mLogWinEnabled = true;
	return true;
}

bool Debugger::logPortsCmd(istream& sin)
{
	string sub_cmd;
	if (!readString(sin, sub_cmd))
		return false;

	if (sub_cmd == "clr") {
		mEngine->disableLogWindow();
		mLogWinEnabled = false;
		return true;
	}

	if (sub_cmd != "set")
		return false;

	PortSelection port_sel;
	vector< DevicePort*> logged_ports;
	while (readPortSel(sin, port_sel)) {
		DevicePort* dev_port = port_sel.port;
		logged_ports.push_back(dev_port);
	}

	if (logged_ports.size() == 0) {
		cout << "At least one valid device port needs to be selected!\n";
		return false;
	}

	if (!mEngine->setLoggedPorts(logged_ports))
		return false;

	return true;
}

bool Debugger::logDevicesCmd(istream& sin)
{
	string sub_cmd;
	if (!readString(sin, sub_cmd))
		return false;

	if (sub_cmd == "clr") {
		mEngine->disableLogWindow();
		mLogWinEnabled = false;
		return true;
	}

	if (sub_cmd != "set")
		return false;

	vector< Device*> logged_devices;
	Device* dev;
	while ((dev = readDevice(sin)) != nullptr)
		logged_devices.push_back(dev);

	if (logged_devices.size() == 0) {
		cout << "At least one valid device needs to be selected!\n";
		return false;
	}

	if (!mEngine->setLoggedDevices(logged_devices))
		return false;

	return true;
}

void Debugger::help()
{
	cout << "Commands are:\n";
	cout << "read <hex start address> [<hex end address>]:       read memory content - default is end address = start address\n";
	cout << "dis <hex start address> [<hex end address>]         disassemble memory content - default is 10 instructions\n";
	cout << "write <hex start address <hex byte> { <hex byte> }: write bytes to memory\n";
	cout << "swrite <hex start address> \"<string>\":              write ASCII string to memory\n";
	cout << "devices:                                            lists the devices\n";
	cout << "state <name of device>:                             get a device's state\n";
	cout << "up:                                                 get the microprocessor's state\n";
	cout << "step [<no of instructions>]:                        execute the specified no of instructions (default is 1) and then stop (instruction tracing only)\n";
	cout << "skip:                                               as 'step 1' but will step over a JSR instruction\n";
	cout << "cont:                                               continue execution (if previusly stopped)\n";
	cout << "break x <hex address>:                              continue execution until the program counter reaches the specified address\n";
	cout << "break r|w|rw <hex address>:                         continue execution until the specified address is accessed in the way specified\n";
	cout << "break clr:                                          clear any previously set breakpoint\n";
	cout << "halt:                                               stop execution\n";
	cout << "mlog (set <adr> | clr):                             add logging of a specific memory address to instruction log\n";
	cout << "rset (A|X|Y|SP|SR|PC) <hex val>:                    set a register value\n";
	cout << "pset <dev name>:<port name>[<qualifier>] <hex val>: set a device's input port's value. <qualifier> ::= <bit no> | [<high bit no>;<low bit no>\n";
	cout << "reset:                                              reset the microprocessor\n";
	cout << "twin (set <sz> | clr):                              enable trace window of a certain size or disable it\n";
	cout << "plog (set <port> {,...<port>} | clr):               add logging of specific device ports to the trace\n";
	cout << "dlog (set <device> {,...<device>} | clr):           add logging of specific devices' states to the trace\n";
	cout << "exit:                                               exit the debugger\n";
	cout << "dsel (set <device> {,...<device>} | clr):           select the devices to be part of the extensive tracing\n";
	cout << "settings:                                           output the current settings (breakpoints, selected devices for logging etc.)\n";
	cout << "notrace:                                            turn off extensive tracing\n";
	cout << "map:                                                print memory map\n";
	cout << "trace <string with from letters below> [<n>]:   turn on extensive tracing for the devices selected by the dsel command. Stop after <n> instructions if specified.\n";
	cout << "\t'V' verbose output\n"; 
	cout << "\t'e' errors\n";
	cout << "\t'w' warnings\n";
	cout << "\t'u' microprocessor execution\n";
	cout << "\t'p' device port updates\n";
	cout << "\t'i' interrupts & reset\n";
	cout << "\t'r' only reset\n";
	cout << "\t'k' keyboard\n";
	cout << "\t'g' graphics <=> video display units\n";
	cout << "\t's' serial/parallel I/O peripherals\n";
	cout << "\t'a' audio\n";
	cout << "\t'd' device execution in general\n";
	cout << "\t't' triggering on R/W accesses\n";
	cout << "\t'c' cassette tape I/O\n";
	cout << "\t'x' measuring execution time of the different components\n";
	cout << "\t'S' SPI devices\n";
	cout << "\t'C' ADC devices\n";
	cout << "\t'X' extensive debugging for the selected debug scope\n";
	cout << "\t'A' all the above\n\n";
}

void Debugger::run()
{
	mState = DBG_RUNNING;
	mQuit = false;

	mGUI->updateDebuggerOptions();

	while (!mQuit) {

		mWaitingEnabled = true;

		if (mEngine->isRunning())
			cout << "\n(running)> ";
		else
			cout << "\n(halted)> ";

		string cmd_line;
		getline(cin, cmd_line);
		istringstream sin(cmd_line);

		string cmd;
		if (readString(sin, cmd)) {

			bool success = true;
			if (cmd == "help")
				help();
			else if(cmd == "map")
				success = memMapCmd(sin);
			else if (cmd == "dsel")
				success = selectTracedDevices(sin);
			else if (cmd == "trace")
				success = enableTracingCmd(sin);
			else if (cmd == "notrace")
				success = disableTracingCmd();
			else if (cmd == "twin")
				success = logWinCmd(sin);
			else if (cmd == "ports")
				success = listPortsCmd(sin);
			else if (cmd == "rset")
				success = setRegCmd(sin);
			else if (cmd == "pset")
				success = setPortCmd(sin);
			else if (cmd == "reset")
				success = resetCmd(sin);
			else if (cmd == "read")
				success = readMemToScreenCmd(sin);
			else if (cmd == "fread")
				success = readMemToFileCmd(sin);
			else if (cmd == "dis")
				success = disToScreenCmd(sin);
			else if (cmd == "fdis")
				success = disToFileCmd(sin);
			else if (cmd == "write")
				success = writeMemCmd(sin);
			else if (cmd == "swrite")
				success = writeMemStrCmd(sin);
			else if (cmd == "state")
				success = dumpDevCmd(sin);
			else if (cmd == "up")
				success = dumpMPCmd(sin);
			else if (cmd == "devices")
				success = listDevicesCmd(sin);
			else if (cmd == "step")
				success = stepCmd(sin, false, cout);
			else if (cmd == "skip")
				success = stepCmd(sin, true, cout);
			else if (cmd == "cont")
				success = contCmd(sin);
			else if (cmd == "halt")
				success = haltCmd(sin);
			else if (cmd == "break")
				success = breakCmd(sin, false);
			else if (cmd == "cbreak")
				success = breakCmd(sin, true);
			else if (cmd == "plog")
				success = logPortsCmd(sin);
			else if (cmd == "dlog")
				success = logDevicesCmd(sin);
			else if (cmd == "exit")
				success = exit();
			else if (cmd == "mlog")
				success = memLogCmd(sin);
			else if (cmd == "settings")
				success = settingsCmd();
			else if (cmd != "")
				cout << "Unknown command '" << cmd << "'!\n";

			if (!success)
				cout << "Invalid parameters for the command!\n";
		}
	}

	mState = DBG_COMPLETED;
	mGUI->updateDebuggerOptions();
}

bool Debugger::exit()
{
	mQuit = true;

	// Stop any potentially ongoing waiting (for breakpoint)
	stopWaiting();

	// Make sure the engine is back into 'run' state (and not e.g. halted) before shutting down the debugger
	mEngine->clrBreakPoint();
	mEngine->cont(cout);

	return true;
}

bool Debugger::running()
{
	return mState == DBG_RUNNING;
}

bool Debugger::waiting()
{
	return mDebuggerWaiting;
}

bool Debugger::waitingEnabled()
{
	return mWaitingEnabled;
}

bool Debugger::stopWaiting()
{
	mWaitingEnabled = false;
	return true;
}

bool Debugger::stopTracing()
{
	disableTracingCmd();
	return true;
}

void Debugger::markStartOfWaiting()
{
	mDebuggerWaiting = true;
	mGUI->updateDebuggerOptions();
}

void Debugger::markEndOfWaiting()
{
	mDebuggerWaiting = false;
	mGUI->updateDebuggerOptions();
}

bool Debugger::enableTracingCmd(istream& sin)
{
	string levels;
	if (!readString(sin, levels))
		return false;
	
	if (!mDT->setDebugLevel(levels)) {
		return false;
	}

	int n_instr;
	if (!readPosInt(sin, n_instr))
		return true;

	if (n_instr <= 0)
		return false;

	mEngine->limitTracing(n_instr);

	return true;
}

bool Debugger::disableTracingCmd()
{
	mDT->clearDebugLevel();
	return true;
}


bool Debugger::selectTracedDevices(istream& sin)
{
	string sub_cmd;
	if (!readString(sin, sub_cmd))
		return false;

	if (sub_cmd == "clr") {
		vector<Device*>* devices = mDM->getDevices();
		if (devices == nullptr)
			return false;
		for (int i = 0; i < devices->size(); i++)
			(*devices)[i]->disableTracing();
		return true;
	}

	if (sub_cmd != "set")
		return false;

	Device* dev;
	int n = 0;
	while ((dev = readDevice(sin)) != nullptr && ++n) {
		dev->enableTracing();
	}

	if (n == 0) {
		cout << "At least one valid device needs to be selected!\n";
		return false;
	}

	return true;

}

bool Debugger::settingsCmd()
{
	// Output traced devices
	vector<Device*>* traced_devices = mDM->getDevices();
	if (traced_devices != nullptr && traced_devices->size() > 0) {
		cout << "Devices selected for tracing: ";
		for (int i = 0; i < traced_devices->size(); i++) {
			Device* dev = (*traced_devices)[i];
			if (dev != nullptr && dev->tracingEnabled()) {
				cout << dev->name << " ";
			}
		}
	}

	// Output logged devices
	vector<Device*>* logged_devices = nullptr;
	mEngine->getLoggedDevices(logged_devices);
	if (logged_devices != nullptr && logged_devices->size() > 0) {
		cout << "Devices selected for logging: ";
		for (int i = 0; i < logged_devices->size(); i++) {
			Device* dev = (*logged_devices)[i];
			if (dev != nullptr) {
				cout << dev->name << " ";
			}
		}
		cout << "\n";
	}

	// Output logged ports
	vector<DevicePort*>* logged_ports = nullptr;
	mEngine->getLoggedPorts(logged_ports);
	if (logged_ports != nullptr && logged_ports->size() > 0) {
		cout << "Ports selected for logging: ";
		for (int i = 0; i < logged_ports->size(); i++) {
			DevicePort* port = (*logged_ports)[i];
			if (port != nullptr) {
				cout << port->dev->name << ":" << port->name << " ";
			}
		}
		cout << "\n";
	}

	// Output breakpoint
	string breakpoint_info = mEngine->getBreakPointInfo();
	if (breakpoint_info != "") {
		cout << "Breakpoint set: " << breakpoint_info << "\n";
	}

	return true;
}