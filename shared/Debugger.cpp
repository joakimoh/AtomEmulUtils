#include "Debugger.h"
#include "Devices.h"
#include "DebugManager.h"
#include <iostream>
#include "Utility.h"
#include "Engine.h"

using namespace std;

Debugger::Debugger(Engine *engine, Devices  *devices, DebugManager *debugManager) : mDM(debugManager),mDevices(devices), mEngine(engine)
{

}

bool Debugger::dumpCmd(istream  &sin)
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
	return true;
}

bool Debugger::contCmd(istream &sin)
{
	return true;
}

bool Debugger::exitCmd(istream &sin)
{
	return true;
}

bool Debugger::breakCmd(istream& sin)
{
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

		if (cmd == "dump")
			dumpCmd(sin);
		else if (cmd == "step")
			stepCmd(sin);
		else if (cmd == "cont")
			contCmd(sin);
		else if (cmd == "exit")
			exitCmd(sin);
		else if (cmd == "exit")
			exitCmd(sin);
		else
			cout << "Unknown command '" << cmd << "'!\n";
	}
}