#include "Debugger.h"
#include "DeviceManager.h"
#include "DebugManager.h"
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

using namespace std;

Debugger::Debugger(P6502* cpu, GUI *gui, Engine *engine, DeviceManager* deviceManager, ConnectionManager* connectionManager, string outDir) :
	mDM(deviceManager), mEngine(engine), mOutDir(outDir),mGUI(gui), mCPU(cpu), mCM(connectionManager)
{
	if (gui == nullptr || engine == nullptr || connectionManager == nullptr || deviceManager == nullptr)
		throw runtime_error("some arguments are null pointers");
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




double Debugger::getDeviceTime(Device* dev)
{
	double t_s;
	if (dev == NULL || !dev->getTimeSec(t_s)) {
		Device* uc = NULL;
		if (!mDM->getUc(uc)) {
			return 0.0;
		}
		uc->getTimeSec(t_s);
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

bool Debugger::dumpUcCmd(istream& sin)
{
	Device* dev = NULL;
	
	if (!mDM->getUc(dev)) {
		return false;
	}
	double t_s = getDeviceTime(dev);
	cout << "Device time (sec ms us): " << Utility::encodeCPUTime(t_s) << "\n";
	dev->outputState(cout);

	return true;
}

bool Debugger::listDevicesCmd(istream& sin)
{
	vector<Device*> devices;
	mDM->getPeripherals(devices);
	for (int i = 0; i < devices.size();i++) {
		cout << left << setw(20) << devices[i]->name << ": " << setw(25) << _DEVICE_ID(devices[i]->devType) << "\n";
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
	Codec6502 mCodec;
	int a1, a2;

	if (!readHexInt(sin, a1))
		return false;

	if (!readHexInt(sin,a2))
		a2 = min(0xffff, a1 + 16);

	if (a1 > a2 || a1 < 0 || a2 < 0 || a1 > 0xffff || a2 > 0xffff) {
		cout << "Only 16-bit address can be specified and the second address parameter cannot be smaller than the first one!\n";
		return true;
	}

	vector<uint8_t> bytes;
	uint16_t pc = a1;
	uint8_t data;

	int i = 0;
	for (int a = a1; a <= a2; a++) {
		string s;
		if (!mDM->dumpDeviceMemory(a, data)) {
			sout << "Illegal address 0x" << hex << a1 << "\n";
			break;
		}
		bytes.push_back(data);

		// Test if there is enough bytes to decode a complete instruction
		if (bytes.size() >= 2) {
			int old_pc = pc;
			if (mCodec.decodeInstrFromBytes(pc, bytes, s, true)) {
				int consumed_bytes = pc - old_pc;
				bytes.erase(bytes.begin(), bytes.begin() + consumed_bytes);
				sout << s << "\n";
				i++;
			}
		}
	}
	
	return true;
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

	return true;
}

bool Debugger::contCmd(istream &sin)
{

	// Wait for breakpoint or an (via GUI debugger menu) explicit user request to stop waiting
	markStartOfWaiting();
	mEngine->cont();
	markEndOfWaiting();

	return true;
}

bool Debugger::resetCmd(istream& sin)
{
	return mCPU->reset();
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

bool Debugger::setPortCmd(istream& sin)
{
	if (mEngine->isRunning()) {
		cout << "The micoprocessor is running - it needs to be halted before changing any port value!\n";
		return true;
	}

	string port_s;
	if (!readString(sin, port_s))
		return false;

	PortSelection port_sel;
	if (!mCM->extractPort(port_s, port_sel)) {
		cout << "Port doesn't exist!\n";
		return false;
	}

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
		uint8_t *in_val = p->valIn;
		uint8_t *out_val = p->valOut;
		int sz = 0;
		for (int mask = p->mask; mask != 0; sz++) mask = mask >> 1;
		uint8_t sz_mask = ((1 << sz) - 1);
		uint8_t io_mask = (p->ioDirMask) & sz_mask;
		uint8_t val;
		uint8_t cur_dir;
		if (p->dir == IN_PORT) {
			cur_dir = 0;
			val = *in_val;
		}
		else if (p->dir == OUT_PORT) {
			cur_dir = sz_mask;
			val = *out_val;
		}
		else { //p->dir == IO_PORT
			cur_dir = io_mask;
			val = (io_mask & (*out_val)) | (~io_mask & (*in_val) & 0xff);
		}
		string dir = (p->dir == IN_PORT ? "IN" : (p->dir == OUT_PORT ? "OUT" : "INOUT"));
		string cur_dir_s = Utility::mask2DirStr(cur_dir, sz);
		string val_s = Utility::int2BinStr(val, sz);
		cout << setw(10) <<  p->name << setw(7) << dir << setw(9) << cur_dir_s << " " << sz << setw(sz+1) << val_s << "\n";
	}
	return true;
}

bool Debugger::breakCmd(istream& sin)
{
	string sub_cmd;
	if (!readString(sin, sub_cmd))
		return false;

	int a;
	if (!readHexInt(sin, a))
		return false;

	// Indicate to the engine that the debugger will start waiting
	markStartOfWaiting();

	if (sub_cmd == "x") {
		mAccessMode = Engine::ENG_X_BRK_WAIT;
		mEngine->setBreakPointAndWait(Engine::RunState(mAccessMode), a, mReadData, mWrittenData, mOperandAdr, false, mLogWinEnabled);
	}
	else if (sub_cmd == "r") {
		mAccessMode = Engine::ENG_R_BRK_WAIT;
		int d;
		if (readHexInt(sin, d)) {
			mReadData = d;
			mAccessMode = Engine::ENG_R_V_BRK_WAIT;
		}
		mEngine->setBreakPointAndWait(Engine::RunState(mAccessMode), a, mReadData, mWrittenData, mOperandAdr, false, mLogWinEnabled);
	}
	else if (sub_cmd == "w") {
		mAccessMode = Engine::ENG_W_BRK_WAIT;
		int d;
		if (readHexInt(sin, d)) {
			mWrittenData = d;
			mAccessMode = Engine::ENG_W_V_BRK_WAIT;
		}
		mEngine->setBreakPointAndWait(Engine::RunState(mAccessMode), a, mReadData, mWrittenData, mOperandAdr, false, mLogWinEnabled);
	}
	else if (sub_cmd == "rw") {
		mAccessMode = Engine::ENG_RW_BRK_WAIT;
		int d; 
		if (readHexInt(sin, d)) {
			mWrittenData = d;
			mAccessMode = Engine::ENG_RW_V_BRK_WAIT;
		}
		mEngine->setBreakPointAndWait(Engine::RunState(mAccessMode), a, mReadData, mWrittenData, mOperandAdr, false, mLogWinEnabled);
	}
	else {
		cout << "Illegal sub command - valid sub command to break is one of x,r,w and rw!\n";
		return false;
	}

	mEngine->printInstrWindow(cout);

	markEndOfWaiting();

	return true;
}

bool Debugger::haltCmd(istream& sin)
{
	mEngine->halt();

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

bool Debugger::logWindCmd(istream& sin)
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

void Debugger::help()
{
	cout << "Commands are:\n";
	cout << "read <hex start address> [<hex end address>]:       read memory content - default is end address = start address\n";
	cout << "dis <hex start address> [<hex end address>]         disassemble memory content - default is 10 instructions\n";
	cout << "write <hex start address <hex byte> { <hex byte> }: write bytes to memory\n";
	cout << "swrite <hex start address> \"<string>\":              write ASCII string to memory\n";
	cout << "devices:                                            lists the devices\n";
	cout << "state <name of device>:                             get a device's state\n";
	cout << "uc:                                                 get the microcontroller's state\n";
	cout << "step [<no of instructions>]:                        execute the specifed no of instructions (default is 1) and then stop (instruction tracing only)\n";
	cout << "xstep <no of instructions>:                         execute the specifed no of instructions (default is 1) and then stop (extended tracing)\n";
	cout << "skip:                                               as 'step 1' but will step over a JSR instruction\n";
	cout << "cont:                                               continue execution (if previusly stopped)\n";
	cout << "break x <hex address>:                              continue execution until the program counter reaches the specified address\n";
	cout << "break r|w|rw <hex address>:                         continue execution until the specified address is accessed in the way specified\n";
	cout << "cbreak:                                             clear any previously set breakpoint\n";
	cout << "halt:                                               stop execution\n";
	cout << "mlog (set <adr> | clr):                             add logging of a specific memory address to instruction log\n";
	cout << "rset (A|X|Y|SP|SR|PC) <hex val>:                    set a register value\n";
	cout << "pset <dev name>:<port name>[<qualifier>] <hex val>: set a device's input port's value. <qualifier> ::= <bit no> | [<high bit no>;<low bit no>\n";
	cout << "reset:                                              reset the microprocessor\n";
	cout << "twin (set <sz> | clr):                              enable trace window of a certain size or disable it\n";
	cout << "exit:                                               exit the debugger\n";
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
			else if (cmd == "twin")
				success = logWindCmd(sin);
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
			else if (cmd == "uc")
				success = dumpUcCmd(sin);
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
				success = breakCmd(sin);
			else if (cmd == "cbreak")
				success = clrBreakpointCmd(sin);
			else if (cmd == "exit")
				success = exit();
			else if (cmd == "mlog")
				success = memLogCmd(sin);
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
	mEngine->cont();

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

bool Debugger::clrBreakpointCmd(istream& sin)
{
	return mEngine->clrBreakPoint();
}