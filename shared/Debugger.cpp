#include "Debugger.h"
#include "Devices.h"
#include "DebugManager.h"
#include <iostream>
#include "Utility.h"
#include "Engine.h"
#include <iomanip>
#include <sstream>

using namespace std;

Debugger::Debugger(Engine *engine, Devices  *devices, DebugManager *debugManager) : mDM(debugManager),mDevices(devices), mEngine(engine)
{

}

bool Debugger::bufferCmd(istream& sin)
{
	string sub_cmd;
	sin >> sub_cmd;
	if (sub_cmd == "out") {
		if (!mDM->emptyBuffer(cout)) {
			cout << "Buffering needs to be enabled first!\n";
			return false;
		}
	}
	else if (sub_cmd == "set") {
		int len;
		sin >> len;
		if (!mDM->enableBuffering(len)) {
			cout << "Buffering size needs to be in the interval [1,1000]!\n";
			return false;
		}
	}
	else if (sub_cmd == "dis") {
		mDM->disableBuffering();
	}
	else {
		return false;
	}

	return true;
}
bool Debugger::dumpDevCmd(istream& sin)
{
	string dev_name;
	sin >> dev_name;
	Device* dev = NULL;
	if (!mDevices->getDevice(dev_name, dev)) {
		cout << "Non-existing device '" << dev_name << "!\n";
		return false;
	}
	dev->outputState(cout);

	return true;
}

bool Debugger::dumpUcCmd(istream& sin)
{
	Device* dev = NULL;
	
	if (!mDevices->getUc(dev)) {
		return false;
	}

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
	sin >> hex >> a;
	while (!sin.eof()) {
		sin >> hex >> data;
		if (!mDevices->writeMemoryMappedDevice(a++, data)) {
			cout << "Failed to write to memory-mapped device at address 0x" << hex << a << "!\n";
			return false;
		}
	}
	return true;
}

bool Debugger::writeMemStrCmd(istream & sin)
{
	int a;
	char c;
	sin >> hex >> a >> skipws >> c;
	if (c != '"')
		return false;
	while (sin.get(c) && c != '"') {
		if (!mDevices->writeMemoryMappedDevice(a++, (uint8_t)c)) {
			cout << "Failed to write to memory-mapped device at address 0x" << hex << a << "!\n";
			return false;
		}
	}
	return true;
}

bool Debugger::disCmd(istream& sin)
{
	Codec6502 mCodec;
	int a1, a2;
	sin >> hex >> a1;
	sin >> hex >> a2;
	vector<uint8_t> bytes;
	uint16_t pc = a1;
	uint8_t data;

	for (int a = a1; a <= a2; a++) {
		string s;
		if (!mDevices->dumpDeviceMemory(a, data)) {
			cout << "Illegal address 0x" << hex << a1 << "\n";
			break;
		}
		bytes.push_back(data);
		if (bytes.size() >= 4 || a == a2) {
			int old_pc = pc;
			if (!mCodec.decodeInstrFromBytes(pc, bytes, s)) {
				cout << "Failed to decode instruction at address 0x" << hex << pc << "\n";
				break;
			}
			int consumed_bytes = pc - old_pc;
			bytes.erase(bytes.begin(), bytes.begin() + consumed_bytes);
			cout << s << "\n";
		}
	}
	
	return true;
}

bool Debugger::readMemCmd(istream  &sin)
{
	int a1, a2;
	sin >> hex >> a1;
	sin >> hex >> a2;
	uint8_t bytes[16] = { 0 };
	int r_sz = 16;
	for (int a = a1; a <= a2; a++) {
		uint8_t data;
		int ofs = (a - a1) % 16;
		if (ofs == 0)
			r_sz = ((a2 - a) < 16 ? a2 - a + 1 : 16);
		if (!mDevices->dumpDeviceMemory(a, data)) {
			cout << "Illegal address 0x" << hex << a1 << "\n";
			break;
		}
		bytes[ofs] = data;
		if ((r_sz == 16 && ofs == 15) || a == a2) {
			cout << "\n" << Utility::int2hexStr(a - ofs, 6);
			for (int i = 0; i < r_sz; i++)
				cout << " " << Utility::int2hexStr(bytes[i], 2);
			for (int i = 0; i < 16 - r_sz; i++)
				cout << "   ";
			cout << " ";
			for (int i = 0; i < r_sz; i++) {
				uint8_t c = bytes[i];
				cout << " " << (c >= 32 && c < 127 ? (char)c : '.');
			}
			cout << "\n";
		}
	}

	return true;
}

bool Debugger::stepCmd(istream &sin)
{
	int n = 1;
	if (sin.tellg() > 0)
		sin >> n;
	mEngine->step(n);
	return true;
}

bool Debugger::contCmd(istream &sin)
{
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
	sin >> sub_cmd;
	int a;
	sin >> hex >> a;
	if (sub_cmd == "x") {
		mAccessMode = 0;
		mEngine->setBreakPointAndWait(0, a, mReadData, mWrittenData);		
	}
	else if (sub_cmd == "r") {
		mAccessMode = 1;
		mEngine->setBreakPointAndWait(1, a, mReadData, mWrittenData);
	}
	else if (sub_cmd == "w") {
		mAccessMode = 2;
		mEngine->setBreakPointAndWait(2, a, mReadData, mWrittenData);
	}
	else if (sub_cmd == "rw") {
		mAccessMode = 3;
		mEngine->setBreakPointAndWait(3, a, mReadData, mWrittenData);
	}
	else
		return false;

	return true;
}

bool Debugger::haltCmd(istream& sin)
{
	mEngine->halt();
	return true;
}

void Debugger::debug()
{
	while (true) {

		cout << "> ";
		string cmd_line;
		getline(cin, cmd_line);
		istringstream sin(cmd_line);
		string cmd;
		sin >> cmd;

		if (cmd == "read")
			readMemCmd(sin);
		else if (cmd == "buf")
			bufferCmd(sin);
		else if (cmd == "dis")
			disCmd(sin);
		else if (cmd == "write")
			writeMemCmd(sin);
		else if (cmd == "swrite")
			writeMemStrCmd(sin);
		else if (cmd == "state")
			dumpDevCmd(sin);
		else if (cmd == "uc")
			dumpUcCmd(sin);
		else if (cmd == "devices")
			listDevicesCmd(sin);
		else if (cmd == "step")
			stepCmd(sin);
		else if (cmd == "cont")
			contCmd(sin);
		else if (cmd == "halt")
			haltCmd(sin);
		else if (cmd == "break")
			breakCmd(sin);
		else if (cmd == "exit")
			exitCmd(sin);
		else if (cmd != "")
			cout << "Unknown command '" << cmd << "'!\n";
	}
}