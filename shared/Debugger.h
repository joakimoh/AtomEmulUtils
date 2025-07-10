#ifndef DEBUGGER_H
#define DEBUGGER_H

#include <iostream>

class Devices;
class DebugManager;

class Debugger {

private:

	DebugManager* mDM = NULL;
	Devices *mDevices = NULL;

public:

	Debugger(Devices *devices, DebugManager *debugManager);

	void debug();

};



#endif