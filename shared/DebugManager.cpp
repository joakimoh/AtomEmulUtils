#include "DebugManager.h"
#include <iostream>
#include <iomanip>
#include "P6502.h"
#include "Device.h"
#include "Utility.h"

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

void DebugManager::enableCyclicLogging(uint16_t adr)
{
	mCyclicLogAdr = adr;
}
void DebugManager::enableInterruptLogging(uint16_t adr)
{
	mInterruptLogAdr = adr;
}
void DebugManager::enableTracing(uint16_t adr, int preTraceLen, int postTraceLen, bool recurring)
{
	mTraceAdr = adr;
	mPreTraceLen = preTraceLen;
	mPostTraceLen = postTraceLen;
	mRecurringTracing = recurring;
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
	mX = X;
	mY = Y;
	mA = A;

	if (mTraceAdr > 0) {

		if (!mEndofPrebufferingReached && !mEndOfTracingReached) {
			string s = mSout.str();
			if (s != "") {
				mBufferedTraceLines.push_back(s);
				if (mBufferedTraceLines.size() > mPreTraceLen)
					mBufferedTraceLines.erase(mBufferedTraceLines.begin(), mBufferedTraceLines.end() - mPreTraceLen);
				mSout.str(""); // flush string stream
			}
		}

		if (matchFetchAddress(mTraceAdr) && !mEndofPrebufferingReached) {
			mDbgLevel |= DBG_6502;
			mTraceCount = 0;
			if (mRecurringTracing)
				std::cout << "-----------------------------------------------------------------------------------------------------------------------------------\n";
			for (int i = 0; i < mBufferedTraceLines.size(); i++)
				std::cout << mBufferedTraceLines[i];
			mBufferedTraceLines.clear();
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

	mFetchAdr = fetchAdr;

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
	if (mTraceAdr > 0 && (level == DBG_6502 || (mDbgLevel & level) != 0)) {
		if (!mEndOfTracingReached && !mEndofPrebufferingReached)
			mSout << t_s << prefix << line;
	}

	if (mFetchAdr == mCyclicLogAdr || ((mDbgLevel & level) != 0 && (mTraceAdr <= 0 || (mEndofPrebufferingReached && !mEndOfTracingReached)))) {
		cout << t_s << prefix << line;
		if (mTraceAdr > 0 && mEndofPrebufferingReached && !mEndOfTracingReached)
			mTraceCount++;
	}

}