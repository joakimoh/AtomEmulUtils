#ifndef DEBUG_MANAGER_H
#define DEBUG_MANAGER_H

#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <cstdint>
#include "Codec6502.h"

class P6502;
class Device;
class DevicePort;
class Devices; 

using namespace std;

typedef int DebugLevel;
#define	DBG_NONE			0x00000
#define	DBG_ERROR			0x00001
#define	DBG_VERBOSE			0x00002
#define	DBG_WARNING			0x00004
#define	DBG_6502			0x00008
#define DBG_PORT			0x00010
#define DBG_INTERRUPTS		0x00020	// Interrupts & reset
#define DBG_KEYBOARD		0x00040
#define DBG_VDU				0x00080
#define DBG_IO_PERIPHERAL	0x00100
#define	DBG_DEVICE			0x00200
#define DBG_TRGGERING		0x00400	// TRIGGERING
#define DBG_TIME			0x00800
#define DBG_AUDIO			0x01000
#define DBG_TAPE			0x02000
#define DBG_RESET			0x04000	// Only reset
#define DBG_SPI				0x08000
#define DBG_ADC				0x10000
#define	DBG_ALL				0xfffff

#define DBG_ON
//#define TIME_DEBUG
//#define DBG_UC_TIME
//#define VDU_TIME_DEBUG

#ifdef DBG_ON

#define DBG_TRACING()				mDM->tracingEnabled()
#define DBG_LOG(...)				mDM->log(__VA_ARGS__)
#define DBG_LOG_COND(cond,...)		if(cond) mDM->log(__VA_ARGS__)
#define DBG_PBUF(...)				mDM->preBuffer(__VA_ARGS__)
#define DBG_LEVEL(...)				mDM->debugLevelIs(__VA_ARGS__)
#define DBG_LEVEL_DEV(...)			mDM->debugLevelIs(__VA_ARGS__)
#define DBG_TRACING_OR_LEVEL(level)	(DBG_LEVEL_DEV(level) || DBG_TRACING())
#define DBG_COND_TRACING(cond, ...)	DBG_LOG_COND(cond || mDM->tracingEnabled(), __VA_ARGS__)
#define DBG_ADR_INT_TRIGGER(...)	mDM->triggerInterruptLogging(__VA_ARGS__)
#define DBG_ADR_TRIGGER(...)		mDM->triggerLogging(__VA_ARGS__)
#define DBG_STOP(...)				mDM->triggerExecutionStop(__VA_ARGS__)
#define DBG_MATCH_PORT(x)			mDM->matchPort(x)


#else

#define DBG_TRACING_OR_LEVEL(...)	false
#define DBG_TRACING()				false
#define DBG_LOG(...)
#define DBG_LOG_COND(...)
#define DBG_PBUF(...)
#define DBG_LEVEL(x)				false
#define DBG_LEVEL_DEV(x)			false
#define DBG_COND_TRACING(...)		false
#define DBG_ADR_INT_TRIGGER(...)
#define DBG_ADR_TRIGGER(...)
#define DBG_STOP(...)				false
#define DBG_MATCH_PORT(x)			false

#endif

typedef struct LogPort_struct {
	string device;
	string port;
} LogPort;

typedef struct InstrLogData_struct {
	double logTime = 0;
	Codec6502::InstructionInfo instr;
	uint8_t A = 0x0;
	uint8_t X = 0x0;
	uint8_t Y = 0x0;
	uint8_t SP = 0x0;
	uint8_t SR = 0x0;
	uint16_t PC = 0x0;
	uint16_t opcodePC = 0x0;
	uint16_t opcode = 0x0;
	uint16_t operand = 0x0;
	int accessAdr = -1;
	bool decodeFailure = false;
	bool rwFailure = false;
	bool execFailure = false;
	bool activeIRQ = false;
	bool activeNMI = false;
	uint16_t stack = 0x0;
	uint8_t readVal = 0;
	uint8_t writtenVal = 0;
	int memContent = -1;
} InstrLogData;


class DebugManager {

private:

	Codec6502 mCodec;

	int mTraceCount = 0;
	vector<string> mBufferedTraceLines;

	bool mExtensiveLog = false;
	bool mDelayed = false;

	bool mInitialised = false;

#define	INSTR_BUFFER_SIZE	100

	vector<InstrLogData> mBufferedInstrLog = vector<InstrLogData>(INSTR_BUFFER_SIZE);

	int mBufferInstrReadIndex = 0;
	int mBufferInstrWriteIndex = 0;
	int mBufferInstrSize = 0;

	enum TracingState {PREBUF_TRACING, POST_TRACING, TRACING_ON, TRACING_OFF};
	TracingState mTracingState = TRACING_OFF;
;
	uint16_t mFetchAdr = 0x0;
	stringstream mSout;

	Device* mLogDevice = NULL;
	string mTmpLogDeviceName;


	int mInterruptLogAdr = -1;
	int mCyclicLogAdr = -1;
	int mLogAdr = -1;
	int mMemLogAdr = -1;

	int mTraceAdr = -1;
	int mPostTraceLen = 0;
	int mPreTraceLen = 10;

	int mStopAdr = -1;
	int mDumpAdr = -1;
	int mDumpSz = -1;

	int mPC = -1;
	int mX = -1;
	int mY = -1;
	int mA = -1;

	vector <LogPort> mTmpLogPorts;
	vector <DevicePort*> mLogPorts;

	DebugLevel mDbgLevel = DBG_NONE;

	bool mRecurringTracing = false;
	bool mStopped = false;

	bool matchFetchAddress(uint16_t fetchAdr);

	Devices* mDevices = NULL;

	void printInstrLogData(ostream &sout, InstrLogData instrLogData);
	bool string2debugLevel(string debugLevelS, DebugLevel &debugLevel);

public:

	bool debugLevelIs(Device* dev, DebugLevel level);
	bool debugLevelIs(DebugLevel level);

	bool enableBuffering(int len, bool extensive);
	void disableBuffering();
	bool emptyBuffer(ostream& sout);

	void setUcDebug();
	void enableLogging(uint16_t adr);
	void enableCyclicLogging(uint16_t adr);
	void enableInterruptLogging(uint16_t adr);
	bool enableTracing(uint16_t adr, int preTraceLen, int postTraceLen, bool recurring, bool extensive, bool delayed);
	void enableMemDump(uint16_t adr, int sz);
	void enableExecStop(uint16_t adr);


	bool tracingEnabled();
	bool tracingActive();

	void setMemLogAdr(uint16_t adr);

	bool setDebugLevel(DebugLevel level);
	bool setDebugLevel(string level);
	bool clearDebugLevel(DebugLevel level);
	bool clearDebugLevel(string level);
	void clearDebugLevel();
	DebugLevel getDebugLevel();

	void setDebugPort(string portDevice, string port);

	void triggerInterruptLogging(uint16_t adr, bool condition);
	void triggerLogging(uint16_t adr);
	bool triggerExecutionStop(P6502 * cpu, uint16_t adr);

	void toggleUCdebug();

	// Buffer tracing before a break point is triggered
	void preBuffer(uint16_t adr, uint8_t X, uint8_t Y, uint8_t A);

	//
	// Slower (but more extensive) logging of all the types of device data that are currently enabled
	//
	void log(Device * dev, DebugLevel level, string line);

	//
	// Quick logging of instruction data only
	//
	void log(Device* dev, DebugLevel level, InstrLogData instrLogData);

	bool matchPort(DevicePort* port);

	bool setDevices(Devices *devices);

	bool quickTracing() { return !mExtensiveLog; }

	bool setLogDevice(string devName) { mTmpLogDeviceName = devName; return true; }

};




#endif