#include "Debugger.h"
#include "Devices.h"
#include "DebugManager.h"
#include <iostream>
#include "Utility.h"
#include "Engine.h"
#include <iomanip>
#include <sstream>
#include <filesystem>
#include "Engine.h"

using namespace std;

Debugger::Debugger(Engine *engine, Devices  *devices, DebugManager *debugManager, string outDir) :
	mDM(debugManager),mDevices(devices), mEngine(engine), mOutDir(outDir)
{

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



bool Debugger::readOptHexInt(istream& sin, int& i)
{
	if (sin.eof())
		return false;

	i = -1;
	sin >> hex >> i;

	return i > 0;
}

bool Debugger::readHexInt(istream& sin, int& i)
{
	if (sin.eof())
		return false;

	i = -1;
	sin >> hex >> i;

	return i > 0;
}

bool Debugger::readOptPosInt(istream& sin, int& i)
{
	if (sin.eof())
		return false;

	i = -1;
	sin >> dec >> i;

	return i > 0;
}

bool Debugger::readPosInt(istream& sin, int& i)
{
	if (sin.eof())
		return false;

	i = -1;
	sin >> dec >> i;

	return i > 0;
}

bool Debugger::readChar(istream& sin, char& c)
{
	if (sin.eof())
		return false;

	sin >> skipws >> c >> noskipws;

	return true;
}


bool Debugger::levelCmd(istream& sin)
{
	string sub_cmd, level;

	if (!readString(sin, sub_cmd))
		return false;

	if (sub_cmd == "set") {
	
		if (!readString(sin, level))
			return false;

		if (!mDM->setDebugLevel(level)) {
			cout << "Invalid parameter '" << level << "'!\n";
			return false;
		}
	}
	else if (sub_cmd == "clr") {

		if (!readString(sin, level))
			return false;
		;
		if (!mDM->clearDebugLevel(level)) {
			cout << "Invalid parameter '" << level << "'!\n";
			return false;
		}
	}
	else if (sub_cmd == "off") {
		mDM->clearDebugLevel();
	}
	else {
		cout << "Invalid sub command to dbg - a valid command is one of 'set' and 'clr'!\n";
		return true;
	}

	return true;
}

bool Debugger::bufferCmd(istream& sin, bool recurring)
{
	string sub_cmd;

	if (!readString(sin, sub_cmd))
		return false;

	mRecurringTracing = recurring;

	if (sub_cmd == "set") {

		if (!readPosInt(sin, mPretraceLen))
			return false;

		mExtensiveTracing = false;

		if (!mDM->enableBuffering(mPretraceLen, 1, mExtensiveTracing, recurring)) {
			return false;
		}

		mTracingEnabled = true;
	}
	else if (sub_cmd == "xset") {

		if (!readPosInt(sin, mPretraceLen))
			return false;

		mExtensiveTracing = true;

		if (!mDM->enableBuffering(mPretraceLen, 1, mExtensiveTracing, recurring)) {
			return false;
		}

		mTracingEnabled = true;
	}
	else if (sub_cmd == "off") {

		mDM->disableBuffering();

		mTracingEnabled = false;
	}

	else {
		cout << "Invalid sub command to buf - valid sub command is one of set, xset and off!\n";
		return false;
	}

	return true;
}

double Debugger::getDeviceTime(Device* dev)
{
	double t_s;
	if (dev == NULL || !dev->getTimeSec(t_s)) {
		Device* uc = NULL;
		if (!mDevices->getUc(uc)) {
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
	if (!mDevices->getDevice(dev_name, dev)) {
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
	
	if (!mDevices->getUc(dev)) {
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
	mDevices->getPeripherals(devices);
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

		if (!mDevices->writeMemoryMappedDevice(a++, data)) {
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
		if (!mDevices->writeMemoryMappedDevice(a++, (uint8_t)c)) {
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
		a2 = a1 + 16;

	vector<uint8_t> bytes;
	uint16_t pc = a1;
	uint8_t data;

	int i = 0;
	for (int a = a1; a <= a2; a++) {
		string s;
		if (!mDevices->dumpDeviceMemory(a, data)) {
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

	if (!readOptHexInt(sin, a2))
		a2 = a1;

	uint8_t bytes[16] = { 0 };
	int r_sz = 16;
	for (int a = a1; a <= a2; a++) {
		uint8_t data;
		int ofs = (a - a1) % 16;
		if (ofs == 0)
			r_sz = ((a2 - a) < 16 ? a2 - a + 1 : 16);
		if (!mDevices->dumpDeviceMemory(a, data)) {
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

bool Debugger::stepCmd(istream &sin, bool stepOver)
{
	int n = 1;

	// Check for optional number and - if present - use it to set n
	if (!readOptPosInt(sin, n))
		n = 1;

	if (stepOver && n != 1) {
		cout << "skip command doesn't take any parameter!\n";
		return false;
	}

	// Turn on tracing if previously enabled
	if (mTracingEnabled) {
		mPretraceLen = n;
		mDM->enableBuffering(mPretraceLen, 1, mExtensiveTracing, false);
	}

	mEngine->step(n, stepOver);

	if (mTracingEnabled && !mDM->outputBufferWindow(cout)) {
		cout << "Trace not possible to retrieve!\n";
		return false;
	}

	return true;
}


bool Debugger::contCmd(istream &sin)
{
	// Turn on tracing if previously enabled
	if (mTracingEnabled)
		mDM->enableBuffering(mPretraceLen, 1, mExtensiveTracing, false);

	mEngine->cont();
	return true;
}

bool Debugger::exitCmd(istream &sin)
{
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

	// Turn on tracing if previously enabled
	if (mTracingEnabled)
		mDM->enableBuffering(mPretraceLen, 1, mExtensiveTracing, mRecurringTracing);


	if (sub_cmd == "x") {
		mAccessMode = Engine::ENG_X_BRK_WAIT;
		mEngine->setBreakPointAndWait(Engine::RunState(mAccessMode), a, mReadData, mWrittenData, mOperandAdr, mRecurringTracing);
	}
	else if (sub_cmd == "r") {
		mAccessMode = Engine::ENG_R_BRK_WAIT;
		int d;
		if (readHexInt(sin, d)) {
			mReadData = d;
			mAccessMode = Engine::ENG_R_V_BRK_WAIT;
		}
		mEngine->setBreakPointAndWait(Engine::RunState(mAccessMode), a, mReadData, mWrittenData, mOperandAdr, mRecurringTracing);
	}
	else if (sub_cmd == "w") {
		mAccessMode = Engine::ENG_W_BRK_WAIT;
		int d;
		if (readHexInt(sin, d)) {
			mWrittenData = d;
			mAccessMode = Engine::ENG_W_V_BRK_WAIT;
		}
		mEngine->setBreakPointAndWait(Engine::RunState(mAccessMode), a, mReadData, mWrittenData, mOperandAdr, mRecurringTracing);
	}
	else if (sub_cmd == "rw") {
		mAccessMode = Engine::ENG_RW_BRK_WAIT;
		int d; 
		if (readHexInt(sin, d)) {
			mWrittenData = d;
			mAccessMode = Engine::ENG_RW_V_BRK_WAIT;
		}
		mEngine->setBreakPointAndWait(Engine::RunState(mAccessMode), a, mReadData, mWrittenData, mOperandAdr, mRecurringTracing);
	}
	else {
		cout << "Illegal sub command - valid sub command to break is one of x,r,w and rw!\n";
		return false;
	}

	return true;
}

bool Debugger::haltCmd(istream& sin)
{
	mEngine->halt();

	if (mTracingEnabled && !mDM->outputBufferWindow(cout)) {
		cout << "Trace not possible to retrieve!\n";
		return false;
	}

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
	cout << "dbg set|clr <flags>:                                set or clear any of the debug flags ewdupirkvstxacASC\n";
	cout << "dbg off:                                            clear all debug flags\n";
	cout << "(c)buf set <tracing window size>:                   set the size of the tracing window - (quick) instruction tracing only (cbuf <=> recurring)\n";
	cout << "(c)buf xset <tracing window size>:                  set the size of the tracing window - (slow) extended tracing (cbuf <=> recurring)\n";
	cout << "buf off:                                            disable tracing\n";
	cout << "uc:                                                 get the microcontroller's state\n";
	cout << "step [<no of instructions>]:                        execute the specifed no of instructions (default is 1) and then stop (instruction tracing only)\n";
	cout << "xstep <no of instructions>:                         execute the specifed no of instructions (default is 1) and then stop (extended tracing)\n";
	cout << "skip:                                               as 'step 1' but will step over a JSR instruction\n";
	cout << "cont:                                               continue execution (if previusly stopped)\n";
	cout << "break x <hex address>:                              continue execution until the program counter reaches the specified address\n";
	cout << "break r|w|rw <hex address>:                         continue execution until the specified address is accessed in the way specified\n";
	cout << "halt:                                               stop execution\n";
	cout << "exit:                                               exit the debugger\n";
}

void Debugger::run()
{
	while (true) {

		cout << "> ";

		string cmd_line;
		getline(cin, cmd_line);
		istringstream sin(cmd_line);

		string cmd;
		if (readString(sin, cmd)) {

			bool success = true;
			if (cmd == "help")
				help();
			else if (cmd == "read")
				success = readMemToScreenCmd(sin);
			else if (cmd == "fread")
				success = readMemToFileCmd(sin);
			else if (cmd == "dbg")
				success = levelCmd(sin);
			else if (cmd == "buf")
				success = bufferCmd(sin, false);
			else if (cmd == "cbuf")
				success = bufferCmd(sin, true);
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
				success = stepCmd(sin, false);
			else if (cmd == "skip")
				success = stepCmd(sin, true);
			else if (cmd == "cont")
				success = contCmd(sin);
			else if (cmd == "halt")
				success = haltCmd(sin);
			else if (cmd == "break")
				success = breakCmd(sin);
			else if (cmd == "exit")
				success = exitCmd(sin);
			else if (cmd != "")
				cout << "Unknown command '" << cmd << "'!\n";

			if (!success)
				cout << "Invalid parameters for the command!\n";
		}
	}
}