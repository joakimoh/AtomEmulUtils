#ifndef DEBUG_MANAGER_H
#define DEBUG_MANAGER_H

#include <vector>
#include <string>
#include <iostream>
#include <sstream>

class P6502;
class Device;
class DevicePort;

using namespace std;

typedef int DebugLevel;
#define	DBG_NONE			0x000
#define	DBG_ERROR			0x001
#define	DBG_VERBOSE			0x002
#define	DBG_WARNING			0x004
#define	DBG_6502			0x008
#define DBG_PORT			0x010
#define DBG_INTERRUPTS		0x020
#define DBG_KEYBOARD		0x040
#define DBG_VDU				0x080
#define DBG_IO_PERIPHERAL	0x100
#define	DBG_DEVICE			0x200
#define DBG_TRGGERING		0x400	// TRIGGERING
#define DBG_TIME			0x800
#define	DBG_ALL				0xfff


class DebugManager {

private:

	int mTraceCount = 0;
	vector<string> mBufferedTraceLines;
	bool mEndOfTracingReached = false;
	bool mEndofPrebufferingReached = false;
	uint16_t mFetchAdr = 0x0;
	stringstream mSout;

	int mInterruptLogAdr = -1;
	int mCyclicLogAdr = -1;
	int mLogAdr = -1;

	int mTraceAdr = -1;
	int mPostTraceLen = 0;
	int mPreTraceLen = 10;

	int mStopAdr = -1;
	int mDumpAdr = -1;
	int mDumpSz = -1;


	int mX = -1;
	int mY = -1;
	int mA = -1;
	int mMatchX = -1;
	int mMatchY = -1;
	int mMatchA = -1;
	
	string mPortDevice = "";
	string mPort = "";


	DebugLevel mDbgLevel = DBG_NONE;
	bool mLogging = false;

	bool mRecurringTracing = false;
	bool mStopped = false;

	bool matchFetchAddress(uint16_t fetchAdr);

public:

	bool debug(DebugLevel level);

	void enableLogging(uint16_t adr);
	void enableCyclicLogging(uint16_t adr);
	void enableInterruptLogging(uint16_t adr);
	void enableTracing(uint16_t adr, int preTraceLen, int postTraceLen, bool recurring);
	void enableMemDump(uint16_t adr, int sz);
	void enableExecStop(uint16_t adr);
	bool tracing();

	void setDebugLevel(DebugLevel level);
	void setDebugPort(string portDevice, string port);
	void clearDebugLevel(DebugLevel level);

	void triggerInterruptLogging(uint16_t adr, bool condition);
	void triggerLogging(uint16_t adr);
	bool triggerExecutionStop(P6502 * cpu, uint16_t adr);

	void startLogging();
	void stopLogging();

	void preBuffer(uint16_t adr, uint8_t X, uint8_t Y, uint8_t A);

	void log(Device * dev, DebugLevel level, string line);

	bool matchPort(DevicePort* port);
};




#endif