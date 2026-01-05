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
class GUI;
class P6502;

class Debugger {

private:

	DeviceManager *mDevices = nullptr;
	Engine* mEngine = nullptr;
	P6502* mCPU = nullptr;
	string mOutDir;
	GUI* mGUI = nullptr;

	int mAccessMode = -1;
	uint8_t mReadData = 0xff;
	uint8_t mWrittenData = 0xff;
	uint16_t  mOperandAdr = 0xffff;

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

	bool mQuit = false;

	enum DebuggerState { DBG_WAIT_TO_RUN, DBG_RUNNING, DBG_COMPLETED };

	bool mDebuggerWaiting = false;

	DebuggerState mState = DBG_WAIT_TO_RUN;

	bool mWaitingEnabled = true;

	void markStartOfWaiting();
	void markEndOfWaiting();

	void help();

	bool disToFileCmd(istream& sin);
	bool disToScreenCmd(istream& sin);

	bool readMemToFileCmd(istream& sin);
	bool readMemToScreenCmd(istream& sin);

	bool writeMemCmd(istream& sin);
	bool writeMemStrCmd(istream& sin);
	bool dumpDevCmd(istream& sin);
	bool dumpUcCmd(istream& sin);
	bool listDevicesCmd(istream& sin);

	bool memLogCmd(istream& sin);

	bool haltCmd(istream& sin);
	bool stepCmd(istream& sin, bool stepOver, ostream& sout);
	bool contCmd(istream& sin);
	bool breakCmd(istream& sin);
	bool clrBreakpointCmd(istream& sin);

	bool resetCmd(istream& sin);

	bool setRegCmd(istream& sin);

public:


	Debugger(P6502 *cpu, GUI *gui, Engine *engine, DeviceManager *devices, string outDir);

	void run();

	bool stopWaiting();

	bool exit();

	bool running();

	bool waiting();

	bool waitingEnabled();



};



#endif