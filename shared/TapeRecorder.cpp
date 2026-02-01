#include "TapeRecorder.h"
#include <cmath>
#include <fstream>
#include <iostream>
#include <iomanip>

TapeRecorder::TapeRecorder(string name, double tickRate, DebugTracing  *debugTracing, ConnectionManager* connectionManager) :
	Device(name, TAPE_RECORDER_DEV, OTHER_DEVICE, debugTracing, connectionManager), TimedDevice(tickRate)
{
	registerPort("CAS_IN", OUT_PORT, 0x01, CAS_IN, &mCAS_IN);
	registerPort("CAS_OUT", IN_PORT, 0x01, CAS_OUT, &mCAS_OUT);

	mCodec = new CSWCodec(44100, mDM);

}

TapeRecorder::~TapeRecorder()
{
	delete mCodec;
}

bool TapeRecorder::advanceUntil(uint64_t stopTick)
{
	if (!mLoadFromTape && !mSaveToTape) {
		mTicks = stopTick;
		return true;
	}

	while (mTicks < stopTick) {

		if (mCAS_OUT != pCAS_OUT) {

			if (mSaveToTape) {

				// One 1/2 cycle has passed => update tape file
				if (mRecord) {
					unsigned int pulse_len = (int)round((double)mHalfCycleDuration / (mTickRate * 1e6) * mSampleRate);
					if (!mCodec->writePulse(pulse_len)) {
						return false;
					}
				}
			}
			mHalfCycleDuration = 0;

		}

		if (mLoadFromTape && mPlay) {

			if (mStartPlaying || mTicks - mCasInPulseStartCount == mCasInPulseLen) {
				mStartPlaying = false;
				mCasInPulseLevel = 1 - mCasInPulseLevel;
				updatePort(CAS_IN, mCasInPulseLevel);
				unsigned int pulse_len;
				if (!mCodec->readPulse(mCasInPulses, mCasInPulseIndex, pulse_len)) {
					mLoadFromTape = false;
					updatePort(CAS_IN, 0);
					DBG_LOG(this,DBG_VERB_DEV, "End of Tape reached!");
					stop();
				}
				else {
				
					mCasInPulseLen = (int)round(mTickRate * 1e6 * pulse_len / mSampleRate);
					mCasInPulseStartCount = mTicks;
				}
			}
		}

		pCAS_OUT = mCAS_OUT;

		mHalfCycleDuration++;

		mTicks++;


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
	DBG_LOG(this,DBG_VERB_DEV, "CSW File with  " + to_string(mCasInPulses.size()) + " pulse bytes...");
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
	DBG_LOG(this,DBG_VERB_DEV, "PLAY");
}

void TapeRecorder::rewind()
{
	if (mLoadFromTape) {
		mCasInPulseIndex = 0;
		mPlay = false;
		DBG_LOG(this, DBG_VERB_DEV, "REWIND");
	}
}

void TapeRecorder::pause()
{
	mPlay = false;
	mRecord = false;
	DBG_LOG(this, DBG_VERB_DEV, "PAUSE");
}

void TapeRecorder::stop()
{
	if (mLoadFromTape) {
		mLoadFromTape = false;
		mPlay = false;
		mCasInPulses.clear();
		updatePort(CAS_IN, 0);
		DBG_LOG(this, DBG_VERB_DEV, "STOP PLAYING");
	}
	else if (mSaveToTape) {

		// Add a last long pulse
		if (mRecord) {
			unsigned int pulse_len = (int)round((double)mHalfCycleDuration / (mTickRate * 1e6) * mSampleRate);
			if (!mCodec->writePulse(pulse_len)) {
				DBG_LOG(this, DBG_ERROR, "Failed to write last pulse!");
			}
		}

		mSaveToTape = false;
		mRecord = false;
		DBG_LOG(this, DBG_VERB_DEV, "STOP RECORDING");

		mCodec->closeTapeFileW();
	}
	
}

void TapeRecorder::record()
{
	mRecord = true;
	DBG_LOG(this, DBG_VERB_DEV, "RECORD");
}

// Outputs the internal state of the device
bool TapeRecorder::outputState(ostream& sout)
{
	sout << "Playing =    " << (mPlay ? "Yes" : "No") << "\n";
	sout << "Recording =  " << (mRecord ? "Yes" : "No") << "\n";
	return true;
}