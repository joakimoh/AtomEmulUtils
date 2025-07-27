#ifndef DEBUGGER_H
#define DEBUGGER_H

#include <iostream>
#include <string>
#include <sstream>

using namespace std;

class DeviceManager;
class Device;
class DebugManager;
class Engine;

class Debugger {

private:

	DebugManager* mDM = NULL;
	DeviceManager *mDevices = NULL;
	Engine* mEngine = NULL;
	string mOutDir;

	int mAccessMode = -1;
	uint8_t mReadData = 0xff;
	uint8_t mWrittenData = 0xff;
	uint16_t  mOperandAdr = 0xffff;

	bool mTracingEnabled = false;
	int mPretraceLen = 10;
	bool mExtensiveTracing = false;
	bool mRecurringTracing = false;

	bool readString(istream& sin, string& s);
	bool readOptString(istream& sin, string& s);
	bool readHexInt(istream& sin, int& i);
	bool readOptHexInt(istream& sin, int& i);
	bool readOptPosInt(istream& sin, int& i);
	bool readPosInt(istream& sin, int& i);
	bool readChar(istream& sin, char& c);

	bool openOutFile(istream& sin, ofstream* &sout);

	bool readMemCmd(istream& sin, ostream& sout);
	bool disCmd(istream& sin, ostream &sout);

	double getDeviceTime(Device* dev);

public:

	Debugger(Engine *engine, DeviceManager *devices, DebugManager *debugManager, string outDir);

	void run();

	void help();

	bool levelCmd(istream& sin);
	bool bufferCmd(istream& sin, bool recurring);
	
	bool disToFileCmd(istream& sin);
	bool disToScreenCmd(istream& sin);

	bool readMemToFileCmd(istream& sin);
	bool readMemToScreenCmd(istream& sin);

	bool writeMemCmd(istream& sin);
	bool writeMemStrCmd(istream& sin);
	bool dumpDevCmd(istream& sin);
	bool dumpUcCmd(istream& sin);
	bool listDevicesCmd(istream& sin);

	bool haltCmd(istream& sin);
	bool stepCmd(istream &sin, bool stepOver);
	bool contCmd(istream &sin);

	bool breakCmd(istream& sin);

	bool exitCmd(istream& sin);

};



#endif