#include "TapeRecorder.h"
#include <cmath>
#include <fstream>
#include <iostream>
#include <iomanip>

TapeRecorder::TapeRecorder(string name, double cpuClock, DebugTracing  *debugTracing, ConnectionManager* connectionManager) :
	Device(name, TAPE_RECORDER_DEV, OTHER_DEVICE, cpuClock, debugTracing, connectionManager)
{
	registerPort("CAS_IN", OUT_PORT, 0x01, CAS_IN, &mCAS_IN);
	registerPort("CAS_OUT", IN_PORT, 0x01, CAS_OUT, &mCAS_OUT);

	mCodec = new CSWCodec(44100, mDM);

}

TapeRecorder::~TapeRecorder()
{
	delete mCodec;
}

bool TapeRecorder::advanceUntil(uint64_t stopCycle)
{
	if (!mLoadFromTape && !mSaveToTape) {
		mCycleCount = stopCycle;
		return true;
	}

	while (mCycleCount < stopCycle) {

		if (mCAS_OUT != pCAS_OUT) {

			if (mSaveToTape) {

				// One 1/2 cycle has passed => update tape file
				if (mRecord) {
					unsigned int pulse_len = (int)round((double)mHalfCycleDuration / (mCPUClock * 1e6) * mSampleRate);
					if (!mCodec->writePulse(pulse_len)) {
						return false;
					}
				}
			}
			mHalfCycleDuration = 0;

		}

		if (mLoadFromTape && mPlay) {

			if (mStartPlaying || mCycleCount - mCasInPulseStartCount == mCasInPulseLen) {
				mStartPlaying = false;
				mCasInPulseLevel = 1 - mCasInPulseLevel;
				updatePort(CAS_IN, mCasInPulseLevel);
				unsigned int pulse_len;
				if (!mCodec->readPulse(mCasInPulses, mCasInPulseIndex, pulse_len)) {
					mLoadFromTape = false;
					updatePort(CAS_IN, 0);
					DBG_LOG(this,DBG_VERBOSE, "End of Tape reached!");
					stop();
				}
				else {
				
					mCasInPulseLen = (int)round(mCPUClock * 1e6 * pulse_len / mSampleRate);
					mCasInPulseStartCount = mCycleCount;
				}
			}
		}

		pCAS_OUT = mCAS_OUT;

		mHalfCycleDuration++;

		mCycleCount++;


	}

	return true;
}

bool TapeRecorder::startLoadFile(string tapeFile)
{
	mLoadFromTape = true;
	mCasInPulseIndex = 0;
	mPlay = false;

	if (!mCodec->decode(tapeFile, mCasInPulses, mCasInPulseLevel, mSampleRate)) {
		return false;
	}
	DBG_LOG(this,DBG_VERBOSE, "CSW File with  " + to_string(mCasInPulses.size()) + " pulse bytes...");
	updatePort(CAS_IN, mCasInPulseLevel);

	return true;
}

bool TapeRecorder::startSaveFile(string tapeFile)
{
	mSaveToTape = true;
	mHalfCycleDuration = 0;
	return mCodec->openTapeFileW(tapeFile);

	return true;
}

void TapeRecorder::play()
{
	mPlay = true;
	mStartPlaying = true;
	DBG_LOG(this,DBG_VERBOSE, "PLAY");
}

void TapeRecorder::rewind()
{
	if (mLoadFromTape) {
		mCasInPulseIndex = 0;
		mPlay = false;
		DBG_LOG(this, DBG_VERBOSE, "REWIND");
	}
}

void TapeRecorder::pause()
{
	mPlay = false;
	mRecord = false;
	DBG_LOG(this, DBG_VERBOSE, "PAUSE");
}

void TapeRecorder::stop()
{
	if (mLoadFromTape) {
		mLoadFromTape = false;
		mPlay = false;
		mCasInPulses.clear();
		updatePort(CAS_IN, 0);
		DBG_LOG(this, DBG_VERBOSE, "STOP PLAYING");
	}
	else if (mSaveToTape) {

		// Add a last long pulse
		if (mRecord) {
			unsigned int pulse_len = (int)round((double)mHalfCycleDuration / (mCPUClock * 1e6) * mSampleRate);
			if (!mCodec->writePulse(pulse_len)) {
				DBG_LOG(this, DBG_ERROR, "Failed to write last pulse!");
			}
		}

		mSaveToTape = false;
		mRecord = false;
		DBG_LOG(this, DBG_VERBOSE, "STOP RECORDING");

		mCodec->closeTapeFileW();
	}
	
}

void TapeRecorder::record()
{
	mRecord = true;
	DBG_LOG(this, DBG_VERBOSE, "RECORD");
}

// Outputs the internal state of the device
bool TapeRecorder::outputState(ostream& sout)
{
	sout << "Playing =    " << (mPlay ? "Yes" : "No") << "\n";
	sout << "Recording =  " << (mRecord ? "Yes" : "No") << "\n";
	return true;
}