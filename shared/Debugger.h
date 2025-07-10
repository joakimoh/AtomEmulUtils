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

public:

	Debugger(Engine *engine, Devices *devices, DebugManager *debugManager);

	void debug();

	bool dumpCmd(istream &sin);

	bool haltCmd(istream& sin);
	bool stepCmd(istream &sin);
	bool contCmd(istream &sin);

	bool breakCmd(istream& sin);

	bool exitCmd(istream& sin);

};



#endif