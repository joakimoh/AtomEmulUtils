#include "DebugManager.h"
#include <iostream>
#include <iomanip>
#include "P6502.h"
#include "Device.h"
#include "Devices.h"
#include "Utility.h"
#include <bitset>


using namespace std;

bool DebugManager::debug(DebugLevel level)
{
	return (mDbgLevel & level) != 0;
}

bool DebugManager::debug(Device *dev, DebugLevel level)
{
	return mInitialised && (mLogDevice == NULL || dev==mLogDevice) && (mDbgLevel & level) != 0;
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

void DebugManager::toggleCondition()
{
	mDelayed = false;
	mDbgLevel = DBG_6502;
	mEndofPrebufferingReached = false;
	mEndOfTracingReached = false;
	mBufferInstrReadIndex = mBufferInstrWriteIndex = mBufferInstrSize = 0;
	mTraceCount = 0;
	mBufferedTraceLines.clear();
}

void DebugManager::toggleLogging()
{
	mTraceAdr = -1;
	mLogAdr = -1;
	mCyclicLogAdr = -1;
	mInterruptLogAdr = -1;
	mDbgLevel = (mDbgLevel ^ DBG_6502) & DBG_6502;
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

void DebugManager::enableTracing(uint16_t adr, int preTraceLen, int postTraceLen, bool recurring, bool extensive, bool delayed)
{
	mDbgLevel = DBG_6502;
	mTraceAdr = adr;
	mPreTraceLen = preTraceLen;
	mPostTraceLen = postTraceLen;
	mRecurringTracing = recurring;
	mExtensiveLog = extensive;
	mDelayed = delayed;
	if (!mExtensiveLog)
		preTraceLen = min(INSTR_BUFFER_SIZE, preTraceLen);
}

bool DebugManager::tracing()
{
	return ((mDbgLevel & DBG_6502) != 0 || mFetchAdr == mCyclicLogAdr || (mTraceAdr > 0 && !mEndOfTracingReached));
}

void DebugManager::setDebugLevel(DebugLevel level)
{
	mDbgLevel |= level;
}

void DebugManager::setDebugPort(string portDevice, string port)
{
	mDbgLevel |= DBG_PORT;
	LogPort log_port = { portDevice, port };
	mTmpLogPorts.push_back(log_port);
	//cout << "Logging added for device '" << portDevice << "' and port '" << port << "'\n";
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

void DebugManager::clearDebugLevel(DebugLevel level)
{
	mDbgLevel &= ~level;
}



void DebugManager::triggerInterruptLogging(uint16_t fetchAdr, bool condition)
{
	if (condition && fetchAdr == mInterruptLogAdr)
		mDbgLevel = DBG_6502;
}

void DebugManager::triggerLogging(uint16_t fetchAdr)
{
	if (fetchAdr == mLogAdr)
		mDbgLevel = DBG_6502;
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
	return (fetchAdr == mFetchAdr && (mMatchX == -1 || mX == mMatchX) && (mMatchY == -1 || mY == mMatchY) && (mMatchA == -1 || mA == mMatchA));
}

void DebugManager::preBuffer(uint16_t fetchAdr, uint8_t X, uint8_t Y, uint8_t A)
{
	mFetchAdr = fetchAdr;
	mX = X;
	mY = Y;
	mA = A;

	if (mTraceAdr > 0 && !mDelayed) {

		if (!mEndofPrebufferingReached && !mEndOfTracingReached) {
			if (mExtensiveLog) {
				string s = mSout.str();
				if (s != "") {
					mBufferedTraceLines.push_back(s);
					if (mBufferedTraceLines.size() > mPreTraceLen)
						mBufferedTraceLines.erase(mBufferedTraceLines.begin(), mBufferedTraceLines.end() - mPreTraceLen);
					mSout.str(""); // flush string stream
				}
			}
		}

		if (matchFetchAddress(mTraceAdr) && !mEndofPrebufferingReached) {
			mDbgLevel |= DBG_6502;
			mTraceCount = 0;
			if (mRecurringTracing)
				std::cout << "-----------------------------------------------------------------------------------------------------------------------------------\n";
			if (mExtensiveLog) {
				for (int i = 0; i < mBufferedTraceLines.size(); i++)
					std::cout << mBufferedTraceLines[i];
				mBufferedTraceLines.clear();
			}
			else {
				int read_pos = mBufferInstrReadIndex;
				for (int i = 0; i < mBufferInstrSize; i++) {
					printInstrLogData(mBufferedInstrLog[read_pos]);
					read_pos = (read_pos + 1) % mPreTraceLen;
				}
				mBufferInstrSize = 0;
				mBufferInstrReadIndex = 0;				
			}
			mEndofPrebufferingReached = true;
			cout << "****\n";
		}

		if (mEndofPrebufferingReached && mTraceCount > mPostTraceLen) {

			if (mRecurringTracing) {
				//mEndOfTracingReached = true;
				mEndofPrebufferingReached = false;
				//mDbgLevel |= DBG_6502;
			} else
			{
				mDbgLevel &= ~DBG_6502;
				mDelayed = false;
				mEndOfTracingReached = true;
				mEndofPrebufferingReached = true;
			}
		}

	}



}

void DebugManager::log(Device * dev, DebugLevel level, string line)
{
	if (!mInitialised && level != DBG_VERBOSE || mLogDevice != NULL && dev != mLogDevice)
		return;

	if (mFetchAdr != mCyclicLogAdr && ((mDbgLevel & level) == 0 || mDelayed))
		return;

	double t = dev->getCycleCount() / (dev->mCPUClock * 1e6);
	string t_s = Utility::encodeCPUTime(t);
	string prefix = t_s + " " + dev->name + " ";
	if (mExtensiveLog && mTraceAdr > 0 && (level == DBG_6502 || (mDbgLevel & level) != 0)) {
		if (!mEndOfTracingReached && !mEndofPrebufferingReached)
			mSout << prefix << line;
	}

	if (mFetchAdr == mCyclicLogAdr || ((mDbgLevel & level) != 0 && (mTraceAdr <= 0 || (mEndofPrebufferingReached && !mEndOfTracingReached)))) {
		cout << prefix << line;
		if (mTraceAdr > 0 && mEndofPrebufferingReached && !mEndOfTracingReached)
			mTraceCount++;
	}

}

void DebugManager::log(Device* dev, DebugLevel level, InstrLogData instrLogData)
{
	if (!mInitialised && level != DBG_VERBOSE || mLogDevice != NULL && dev != mLogDevice)
		return;

	if (mFetchAdr != mCyclicLogAdr && ((mDbgLevel & level) == 0 || mDelayed))
		return;

	if (mFetchAdr == mCyclicLogAdr) {
		printInstrLogData(instrLogData);
		return;
	}

	if (mMemLogAdr > 0 && mDevices != NULL) {
		uint8_t data;
		mDevices->dumpDeviceMemory(mMemLogAdr, data);
		instrLogData.memContent = (int)data;
	}

	if (mTraceAdr > 0) {
		// Update circular buffer
		mBufferedInstrLog[mBufferInstrWriteIndex] = instrLogData;
		if (mBufferInstrWriteIndex == mBufferInstrReadIndex)
			mBufferInstrReadIndex = (mBufferInstrReadIndex + 1) % mPreTraceLen;
		mBufferInstrWriteIndex = (mBufferInstrWriteIndex + 1) % mPreTraceLen;
		if (mBufferInstrSize < mPreTraceLen)
			mBufferInstrSize++;
	}

	if ((mDbgLevel & level) != 0 && (mTraceAdr <= 0 || (mEndofPrebufferingReached && !mEndOfTracingReached ))) {
		printInstrLogData(instrLogData);
		mTraceCount++;
	}
	

}

bool DebugManager::setDevices(Devices * devices)
{
	mDevices = devices;
	mInitialised = true;

	if (devices == NULL)
		return false;

	Device* dev;
	for (int i = 0; i < mTmpLogPorts.size(); i++) {
		string dev_name = mTmpLogPorts[i].device;
		string port_name = mTmpLogPorts[i].port;		
		DevicePort* device_port;
		if (!mDevices->getDevice(dev_name, dev) || !dev->getPortIndex(port_name, device_port)) {
			cout << "Port " << dev_name << ":" << port_name << " doesn't exist!\n";
			return false;
		}
		mLogPorts.push_back(device_port);
	}

	if (mTmpLogDeviceName != "") {
		if (!mDevices->getDevice(mTmpLogDeviceName, dev)) {
			cout << "Device " << mTmpLogDeviceName << " doesn't exist!\n";
			return false;
		}
		mLogDevice = dev;
	}

	return true;
}



void DebugManager::printInstrLogData(InstrLogData instrLogData)
{
	string instr_s = mCodec.decode(instrLogData.opcodePC, instrLogData.opcode, instrLogData.operand);
	Codec6502::InstructionInfo instr = instrLogData.instr;
	stringstream sout;

	string t_s = Utility::encodeCPUTime(instrLogData.logTime);

	sout << t_s << " " << setfill(' ') << setw(30) << left << instr_s << right <<
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

	if (instrLogData.decodeFailure)
		sout << "UNKNOWN INSTR";
	if (instrLogData.rwFailure)
		sout << " R/W FAILURE";
	if (instrLogData.execFailure)
		sout << " EXEC FAILURE";
	if (instrLogData.activeIRQ)
		sout << " *IRQ";
	if (instrLogData.activeNMI)
		sout << " *NMI";
	if (instrLogData.memContent != -1)
		sout << " Mem[0x" << hex << mMemLogAdr << "]=0x" << instrLogData.memContent;
	sout << "\n";

	cout << sout.str();
}