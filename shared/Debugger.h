#ifndef DEBUGGER_H
#define DEBUGGER_H

#include <iostream>
#include <string>
#include <sstream>

using namespace std;

class Devices;
class DebugManager;
class Engine;

class Debugger {

private:

	DebugManager* mDM = NULL;
	Devices *mDevices = NULL;
	Engine* mEngine = NULL;

	int mAccessMode = -1;
	uint8_t mReadData = 0xff;
	uint8_t mWrittenData = 0xff;

public:

	Debugger(Engine *engine, Devices *devices, DebugManager *debugManager);

	void debug();

	bool readMemCmd(istream &sin);
	bool writeMemCmd(istream& sin);
	bool writeMemStrCmd(istream& sin);
	bool dumpDevCmd(istream& sin);
	bool dumpUcCmd(istream& sin);
	bool listDevicesCmd(istream& sin);

	bool haltCmd(istream& sin);
	bool stepCmd(istream &sin);
	bool contCmd(istream &sin);

	bool breakCmd(istream& sin);

	bool exitCmd(istream& sin);

};



#endif