#include "DebugManager.h"
#include <iostream>
#include <iomanip>
#include "P6502.h"
#include "Device.h"
#include "Utility.h"
#include <bitset>


using namespace std;

bool DebugManager::debug(DebugLevel level)
{
	return (mDbgLevel & level) != 0;
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

void DebugManager::enableTracing(uint16_t adr, int preTraceLen, int postTraceLen, bool recurring, bool extensive)
{
	mDbgLevel = DBG_6502;
	mTraceAdr = adr;
	mPreTraceLen = preTraceLen;
	mPostTraceLen = postTraceLen;
	mRecurringTracing = recurring;
	mExtensiveLog = extensive;
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
	mPortDevice = portDevice;
	mPort = port;
}

bool DebugManager::matchPort(DevicePort* port)
{
	return (mPortDevice == "" || (port != NULL && port->dev != NULL && port->dev->name == mPortDevice && port->name == mPort));
}

void DebugManager::clearDebugLevel(DebugLevel level)
{
	mDbgLevel &= ~level;
}

void DebugManager::startLogging()
{
	mLogging = true;
}

void DebugManager::stopLogging()
{
	mLogging = false;
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

	if (mTraceAdr > 0) {

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
					printInstrLogData(mBufferedInstrLog[i]);
					read_pos = (read_pos + 1) % mPreTraceLen;
				}
				mBufferInstrSize = 0;
				mBufferInstrReadIndex = 0;				
			}
			mEndofPrebufferingReached = true;
		}

		if (mEndofPrebufferingReached && mTraceCount > mPostTraceLen) {
			mDbgLevel &= ~DBG_6502;
			if (mRecurringTracing) {
				mEndofPrebufferingReached = false;
			}
			else {
				mEndOfTracingReached = true;
				mEndofPrebufferingReached = true;
			}
		}

	}

	

}

void DebugManager::log(Device * dev, DebugLevel level, string line)
{
	if ((mDbgLevel & level) == 0)
		return;

	double t = dev->getCycleCount() / (dev->mCPUClock * 1e6);
	string t_s = Utility::encodeCPUTime(t);
	string prefix = " ";
	if (level != DBG_6502)
		prefix = " *** ";
	if (mExtensiveLog && mTraceAdr > 0 && (level == DBG_6502 || (mDbgLevel & level) != 0)) {
		if (!mEndOfTracingReached && !mEndofPrebufferingReached)
			mSout << t_s << prefix << line;
	}

	if (mFetchAdr == mCyclicLogAdr || ((mDbgLevel & level) != 0 && (mTraceAdr <= 0 || (mEndofPrebufferingReached && !mEndOfTracingReached)))) {
		cout << t_s << prefix << line;
		if (mTraceAdr > 0 && mEndofPrebufferingReached && !mEndOfTracingReached)
			mTraceCount++;
	}

}

void DebugManager::log(Device* dev, DebugLevel level, InstrLogData instrLogData)
{
	if ((mDbgLevel & level) == 0)
		return;

	// Update circular buffer
	mBufferedInstrLog[mBufferInstrWriteIndex] = instrLogData;
	mBufferInstrWriteIndex = (mBufferInstrWriteIndex + 1) % mPreTraceLen;
	if (mBufferInstrWriteIndex == mBufferInstrReadIndex)
		mBufferInstrReadIndex = (mBufferInstrReadIndex + 1) % mPreTraceLen;
	if (mBufferInstrSize < mPreTraceLen)
		mBufferInstrSize++;

	if ((mDbgLevel & level) != 0 && mEndofPrebufferingReached && !mEndOfTracingReached) {
		printInstrLogData(instrLogData);
		mTraceCount++;
	}

}

void DebugManager::setDevices(Devices * devices)
{
	mDevices = devices;
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
	sout << "mem[" << hex << setw(3) << setfill('0') << stack_adr << "]=" << setw(2) << setfill('0') << hex << instrLogData.stack;

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
	sout << "\n";

	cout << sout.str();
}