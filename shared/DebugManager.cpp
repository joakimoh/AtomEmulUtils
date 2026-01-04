#include "DebugManager.h"
#include <iostream>
#include <iomanip>
#include "P6502.h"
#include "Device.h"
#include "DeviceManager.h"
#include "Utility.h"
#include <bitset>


using namespace std;

string DebugManager::levels2str(DebugLevel debugLevel)
{
	string s;
	if ((debugLevel &  DBG_ERROR) != 0)
		s += " ERROR";	if ((debugLevel & DBG_VERBOSE) != 0)		s += " VERBOSE";	if ((debugLevel & DBG_WARNING) != 0)		s += " WARNING";	if ((debugLevel & DBG_6502) != 0)		s += " 6502";	if ((debugLevel & DBG_PORT) != 0)		s += " PORT";	if ((debugLevel & DBG_INTERRUPTS) != 0)
		s += " INTERRUPTS";	if ((debugLevel & DBG_KEYBOARD) != 0)		s += " KEYBOARD";	if ((debugLevel & DBG_VDU) != 0)		s += " VDU";	if ((debugLevel & DBG_IO_PERIPHERAL) != 0)		s += " IO_PERIPHERAL";	if ((debugLevel & DBG_DEVICE) != 0)		s += " DEVICE";	if ((debugLevel & DBG_TRGGERING) != 0)		s += " TRGGERING";	if ((debugLevel & DBG_TIME) != 0)		s += " TIME";	if ((debugLevel & DBG_AUDIO) != 0)		s += " AUDIO";	if ((debugLevel & DBG_TAPE) != 0)		s += " TAPE";	if ((debugLevel & DBG_RESET) != 0) 		s += " RESET";	if ((debugLevel & DBG_SPI) != 0)		s += " SPI";	if ((debugLevel & DBG_ADC) != 0)		s += " ADC";
	return s;
}

//
// ewdupirkvstxacASC
bool DebugManager::string2debugLevel(string level, DebugLevel& debugLevel)
{
	debugLevel = DBG_NONE;
	if (level.find("e") != string::npos)
		debugLevel  |= DBG_ERROR;
	if (level.find("w") != string::npos)
		debugLevel  |= DBG_WARNING;
	if (level.find("d") != string::npos)
		debugLevel  |= DBG_DEVICE;
	if (level.find("u") != string::npos)
		debugLevel  |= DBG_6502;
	if (level.find("p") != string::npos)
		debugLevel  |= DBG_PORT;
	if (level.find("i") != string::npos)
		debugLevel  |= DBG_INTERRUPTS | DBG_RESET;
	if (level.find("r") != string::npos)
		debugLevel  |= DBG_RESET;
	if (level.find("k") != string::npos)
		debugLevel  |= DBG_KEYBOARD;
	if (level.find("v") != string::npos)
		debugLevel  |= DBG_VDU;
	if (level.find("s") != string::npos)
		debugLevel  |= DBG_IO_PERIPHERAL;
	if (level.find("t") != string::npos)
		debugLevel  |= DBG_TRGGERING;
	if (level.find("x") != string::npos)
		debugLevel  |= DBG_TIME;
	if (level.find("a") != string::npos)
		debugLevel  |= DBG_AUDIO;
	if (level.find("c") != string::npos)
		debugLevel  |= DBG_TAPE;
	if (level.find("A") != string::npos)
		debugLevel  |= DBG_ALL;
	if (level.find("S") != string::npos)
		debugLevel  |= DBG_SPI;
	if (level.find("C") != string::npos)
		debugLevel  |= DBG_ADC;

	if (debugLevel == DBG_NONE && level != "")
		return false;

	return true;
}


bool DebugManager::setDebugLevel(DebugLevel level)
{
	mDbgLevel |= level;
	return true;
}

bool DebugManager::setDebugLevel(string levelS)
{
	DebugLevel level;
	if (!string2debugLevel(levelS, level))
		return false;
	mDbgLevel |= level;
	return true;
}
bool DebugManager::clearDebugLevel(DebugLevel level)
{
	mDbgLevel &= ~level;
	return true;
}

bool DebugManager::clearDebugLevel(string levelS)
{
	DebugLevel level;
	if (!string2debugLevel(levelS, level))
		return false;
	mDbgLevel &= ~level;
	return true;
}

void DebugManager::clearDebugLevel()
{
	mDbgLevel = DBG_NONE;
}

DebugLevel DebugManager::getDebugLevel()
{
	return mDbgLevel;
}

bool DebugManager::debugLevelIs(DebugLevel level)
{
	return  (mDbgLevel & level) != 0;
}

bool DebugManager::debugLevelIs(Device *dev, DebugLevel level)
{
	return mInitialised && (
		mTracingState != TRACING_OFF && ((mLogDevice == NULL || dev == mLogDevice) || (mDbgLevel & DBG_6502) != 0 )
		) && 
		(mDbgLevel & level) != 0;
}

void DebugManager::enableExecStop(uint16_t adr)
{
	mStopAdr = adr;
}

void DebugManager::enableMemDump(uint16_t adr, int sz)
{
	mDumpAdr = adr;
	mDumpSz = sz;
}

// 
// Delayed tracing (mTriggeringArmed = true) is triggered by calling this
// method.
//
void DebugManager::armTriggering()
{
	mTriggeringArmed = true;
}

void DebugManager::disarmTriggering()
{
	mTriggeringArmed = false;
	if (mTracingState == POST_TRACING)
		mTracingState = PREBUF_TRACING;
}

void DebugManager::setMemLogAdr(uint16_t adr)
{
	mMemLogAdr = adr;
}

void DebugManager::enableLogging(uint16_t adr)
{
	mLogAdr = adr;
}

void DebugManager::enableCyclicLogging(uint16_t adr)
{
	mCyclicLogAdr = adr;
}

void DebugManager::enableInterruptLogging(uint16_t adr)
{
	mInterruptLogAdr = adr;
}

bool DebugManager::enableBuffering(int preTraceLen, int postTraceLen, bool extensive, bool recurring)
{

	if (preTraceLen <= 0 || preTraceLen > INSTR_BUFFER_SIZE) {
		cout << "Pre-buffering size must be in the range [1," << INSTR_BUFFER_SIZE << "]!n";
		return false;
	}

	// Check that no other tracing than microcontroller debugging is enabled for quick tracing
	if (!extensive) {
		if (mDbgLevel == DBG_NONE)
			mDbgLevel = DBG_6502;
		else if (mDbgLevel != DBG_6502)
			return false;
	}

	mRecurringTracing = recurring;

	// Clear buffer
	mBufWindowReadIndex = mBufWindowWriteIndex = mBufferInstrSize = 0;
	
	mExtensiveLog = extensive;
	mPreTraceLen = preTraceLen;
	mPostTraceLen = postTraceLen;
	mTriggeringArmed = true;
	mTracingState = PREBUF_TRACING;

	return true;
}

void DebugManager::disableBuffering()
{
	// Reset buffering
	mBufferInstrSize = 0;
	mBufWindowReadIndex = 0;
	mBufWindowReadIndex = mBufWindowWriteIndex = mBufferInstrSize = 0;

	mTracingState = TRACING_OFF;
}

bool DebugManager::outputBufferWindow(ostream& sout)
{
	if (mBufferInstrSize == 0 && mExtBufferWindow.size() == 0)
		return false;

	int read_pos = mBufWindowReadIndex;
	if (mExtensiveLog) {
		for (int i = 0; i < mBufferInstrSize; i++) {
			sout << mExtBufferWindow[read_pos];
			read_pos = (read_pos + 1) % mPreTraceLen;
		}
	}
	else {
		for (int i = 0; i < mBufferInstrSize; i++) {
			printInstrLogData(sout, mInstrBufferWindow[read_pos]);
			read_pos = (read_pos + 1) % mPreTraceLen;
		}
	}

	return true;
}

bool DebugManager::enableTracing()
{
	mTracingState = TRACING_ON;
	if ((mDbgLevel & DBG_6502) == DBG_6502)
		mExtensiveLog = false;
	else
		mExtensiveLog = true;
	return true;
}

bool DebugManager::disableTracing()
{
	mTracingState = TRACING_OFF;
	return true;
}

bool DebugManager::enableTracing(uint16_t adr, int preTraceLen, int postTraceLen, bool recurring, bool extensive, bool delayed)
{
	// Check that no other tracing than microcontroller debugging is enabled for quick tracing
	if (!extensive) {
		if (mDbgLevel == DBG_NONE)
			mDbgLevel = DBG_6502;
		else if ((mDbgLevel & DBG_6502) == 0)
			return false;
	}

	// Reset buffering
	mBufferInstrSize = 0;
	mBufWindowReadIndex = 0;
	mBufWindowReadIndex = mBufWindowWriteIndex = mBufferInstrSize = 0;

	mTraceAdr = adr;
	mPreTraceLen = preTraceLen;
	mPostTraceLen = postTraceLen;
	mRecurringTracing = recurring;
	mExtensiveLog = extensive;
	mTriggeringArmed = !delayed;
	if (!mExtensiveLog)
		mPreTraceLen = min(INSTR_BUFFER_SIZE, preTraceLen);

	mTracingState = PREBUF_TRACING;

	return true;
}

bool DebugManager::tracingEnabled()
{
	return mTracingState != TRACING_OFF;
}

bool DebugManager::tracingActive()
{
	return mTracingState != TRACING_OFF && mTracingState != PREBUF_TRACING;
}


//
// Set debug port
//
// Only a temporary setting as the existance of the port cannot be checked
// at the time - will be checked when the setDeviceManager() method is called later on
//
void DebugManager::setDebugPort(string portDevice, string port)
{
	mDbgLevel |= DBG_PORT;
	LogPort log_port = { portDevice, port };
	mTmpLogPorts.push_back(log_port);

}

bool DebugManager::matchPort(DevicePort* port)
{
	if (!mInitialised || (mDbgLevel & DBG_PORT) == 0)
		return false;

	// If no ports were specifed (and for port debug enabled), a match will always be indicated
	if (mLogPorts.size() == 0)
		return true;
	
	if (port == NULL || port->dev == NULL) {
		cout << "INTERNAL ERROR in matchPort()!\n";
		return true;
	}

	for (int i = 0; i < mLogPorts.size();i++) {
		if (port == mLogPorts[i])
			return true;
	}
	return false;
}

void DebugManager::triggerInterruptLogging(uint16_t fetchAdr, bool condition)
{
	if (condition && fetchAdr == mInterruptLogAdr)
		mTracingState = POST_TRACING;
}

void DebugManager::triggerLogging(uint16_t fetchAdr)
{
	if (fetchAdr == mLogAdr)
		mTracingState = POST_TRACING;
}

bool DebugManager::triggerExecutionStop(P6502 *cpu, uint16_t fetchAdr)
{
	if (mStopAdr > 0 && fetchAdr == mStopAdr && !mStopped) {
		std::cout << "Execution stop triggered!\n";
		if (mDumpAdr > 0) {
			// Output pre post tracing
			for (int a = mDumpAdr; a < mDumpAdr + mDumpSz; a++) {
				uint8_t d;
				cpu->readDevice(a, d);
				std::cout << "0x" << hex << setw(4) << setfill('0') << a <<
					" 0x" << setw(2) << (int)d << "\n";
			}
		}
		mStopped = true;
		return true;
	}
	return false;
}

bool DebugManager::matchFetchAddress(uint16_t fetchAdr)
{
	return fetchAdr == mFetchAdr;
}

void DebugManager::preBuffer(uint16_t fetchAdr, uint8_t X, uint8_t Y, uint8_t A)
{
	mFetchAdr = fetchAdr;
	mX = X;
	mY = Y;
	mA = A;

	if (mTracingState == PREBUF_TRACING) {

		// Save the extensive buffering that has been recorded since the last instruction (if extensive recording is enabled)
		if (mExtensiveLog) {

			// Update circular buffer
			if (mExtBufWindowEntry.size() != 0) {
				mExtBufferWindow[mBufWindowWriteIndex] = mExtBufWindowEntry;
				if (mBufWindowWriteIndex == mBufWindowReadIndex)
					mBufWindowReadIndex = (mBufWindowReadIndex + 1) % mPreTraceLen;
				mBufWindowWriteIndex = (mBufWindowWriteIndex + 1) % mPreTraceLen;
				if (mBufferInstrSize < mPreTraceLen)
					mBufferInstrSize++;
				mExtBufWindowEntry.clear();
			}
		}

		if (mTriggeringArmed && matchFetchAddress(mTraceAdr)) {

			// Output pre-buffered log data (which could be of either the instruction only or extensive type) when the triggering point is detected
			mPostTraceInstrCount = 0;
			if (mRecurringTracing)
				std::cout << "-----------------------------------------------------------------------------------------------------------------------------------\n";
			outputBufferWindow(cout);
			cout << "***\n";
			mTracingState = POST_TRACING;
			mPostTraceInstrCount = 0;
		}
	}


	else if (mTracingState == POST_TRACING) {

		mPostTraceInstrCount++;

		// Stop post-triggering point logging
		if (mPostTraceInstrCount > mPostTraceLen) {

			mPostTraceInstrCount = 0;

			if (mRecurringTracing) {
				mTracingState = PREBUF_TRACING;
				mTriggeringArmed = true;
			}
			else {
				mTracingState = TRACING_OFF;				
			}
		}

	}



}

bool DebugManager::logCurrentInstruction(ostream& sout)
{
	return mMicrocontroller != NULL  && mMicrocontroller->outputState(sout);
}

//
// Slower (but more extensive) logging of all the types of device data that are currently enabled
//
void DebugManager::log(Device * dev, DebugLevel level, string line)
{
	if (mTracingState == TRACING_OFF || !debugLevelIs(dev, level))
		return;

	double t = dev->getCycleCount() / (dev->mCPUClock * 1e6);
	if (t == 0 && mMicrocontroller != NULL) // For device's not keeping time, use the microcontroller's time as reference instead!
		t = mMicrocontroller->getCycleCount() / (dev->mCPUClock * 1e6);
	string t_s = Utility::encodeCPUTime(t);
	string prefix = t_s + " " + dev->name + " ";

	// Buffer the data (instead of outputting it directly) when extensive buffering is ongoing
	if (mExtensiveLog && mTracingState == PREBUF_TRACING) {
		mExtBufWindowEntry += prefix + line;
	}

	// Log the data if the debugging is enabled (armed) and buffering is not ongoing
	if (mTriggeringArmed && (mTracingState == POST_TRACING || mTracingState == TRACING_ON)) {
		cout << prefix << line + "\n";
	}

}

//
// Quick logging of instruction data only
//
void DebugManager::log(Device* dev, DebugLevel level, InstrLogData instrLogData)
{
	if (mTracingState == TRACING_OFF || !debugLevelIs(dev, level))
		return;

	if (mTriggeringArmed && mFetchAdr == mCyclicLogAdr) {
		mTracingState = POST_TRACING;
		printInstrLogData(cout, instrLogData);
		return;
	}

	if (mMemLogAdr > 0 && mDeviceManager != NULL) {
		uint8_t data;
		mDeviceManager->dumpDeviceMemory(mMemLogAdr, data);
		instrLogData.memContent = (int)data;
	}

	// Buffer the instruction data (instead of outputting it directly) when buffering is ongoing
	if (mTracingState == PREBUF_TRACING) {
		// Update circular buffer
		mInstrBufferWindow[mBufWindowWriteIndex] = instrLogData;
		if (mBufWindowWriteIndex == mBufWindowReadIndex)
			mBufWindowReadIndex = (mBufWindowReadIndex + 1) % mPreTraceLen;
		mBufWindowWriteIndex = (mBufWindowWriteIndex + 1) % mPreTraceLen;
		if (mBufferInstrSize < mPreTraceLen)
			mBufferInstrSize++;
	}

	// Log the data if the debugging is enabled (armed) and buffering is not ongoing
	if (mTriggeringArmed && (mTracingState == POST_TRACING || mTracingState == TRACING_ON)) {
		printInstrLogData(cout, instrLogData);
	}
	

}

bool DebugManager::setMicrocontroller(Device* microcontrollerDevice)
{
	mMicrocontroller = microcontrollerDevice;
	return true;
}

bool DebugManager::setDeviceManager(DeviceManager * deviceManager)
{
	mDeviceManager = deviceManager;
	mInitialised = true;

	if (deviceManager == nullptr)
		return false;

	Device* dev;
	for (int i = 0; i < mTmpLogPorts.size(); i++) {
		string dev_name = mTmpLogPorts[i].device;
		string port_name = mTmpLogPorts[i].port;		
		DevicePort* device_port;
		if (!mDeviceManager->getDevice(dev_name, dev) || !dev->getPortIndex(port_name, device_port)) {
			cout << "Port " << dev_name << ":" << port_name << " doesn't exist!\n";
			return false;
		}
		mLogPorts.push_back(device_port);
	}

	if (mTmpLogDeviceName != "") {
		if (!mDeviceManager->getDevice(mTmpLogDeviceName, dev)) {
			cout << "Device " << mTmpLogDeviceName << " doesn't exist!\n";
			return false;
		}
		mLogDevice = dev;
	}

	return true;
}



void DebugManager::printInstrLogData(ostream &sout, InstrLogData instrLogData)
{
	string instr_s = mCodec.decode(instrLogData.opcodePC, instrLogData.opcode, instrLogData.operand);
	Codec6502::InstructionInfo instr = instrLogData.instr;

	string t_s = Utility::encodeCPUTime(instrLogData.logTime);

	sout << t_s << " [" << instrLogData.cycles << "] " << setfill(' ') << setw(30) << left << instr_s << right <<
		" " <<  hex << setfill('0') <<
		"A:" << setw(2) << (int)instrLogData.A <<
		" X:" << setw(2) << (int)instrLogData.X <<
		" Y:" << setw(2) << (int)instrLogData.Y <<
		" SP:" << setw(2) << (int)instrLogData.SP <<
		" NV--DIZC:" << setw(8) << bitset<8>(instrLogData.SR & 0xdf) <<
		setw(4) <<
		" PC:" << setw(2) << instrLogData.PC;
	if (instr.readsMem || instr.writesMem)
		sout << " ACCESSED 0x" << hex << setfill('0') << setw(4) << instrLogData.accessAdr;
	if (instr.readsMem)
		sout << " READ 0x" << setw(2) << (int)instrLogData.readVal;
	if (instr.writesMem)
		sout << " WROTE 0x" << setw(2) << (int)instrLogData.writtenVal;

	sout << " ";

	uint16_t stack_adr = (0x100 + instrLogData.SP + 1) & 0x1ff;
	sout << "Mem[" << hex << setw(3) << setfill('0') << stack_adr << "]=" << setw(2) << setfill('0') << hex << instrLogData.stack;

	if (instrLogData.execFailure)
		sout << " EXEC FAILURE";
	if (instrLogData.activeIRQ)
		sout << " *IRQ";
	if (instrLogData.activeNMI)
		sout << " *NMI";
	if (instrLogData.memContent != -1)
		sout << " Mem[0x" << hex << mMemLogAdr << "]=0x" << instrLogData.memContent;


	sout << "\n";

}