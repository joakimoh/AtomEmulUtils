#include "TapeRecorder.h"
#include <cmath>

TapeRecorder::TapeRecorder(string name, double cpuClock, DebugManager  *debugManager, ConnectionManager* connectionManager) :
	Device(name, TAPE_RECORDER_DEV, OTHER_DEVICE, cpuClock, debugManager, connectionManager)
{
	registerPort("CAS_IN", OUT_PORT, 0x01, CAS_IN, &mCAS_IN);
	registerPort("CAS_OUT", IN_PORT, 0x01, CAS_OUT, &mCAS_OUT);

	mCodec = new CSWCodec(44100, mDM);

}

TapeRecorder::~TapeRecorder()
{
	delete mCodec;
}

bool TapeRecorder::advance(uint64_t stopCycle)
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
					if (DBG_LEVEL_DEV(this,DBG_VERBOSE))
						cout << "End of Tape reached!\n";
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
	if (DBG_LEVEL_DEV(this,DBG_VERBOSE))
		cout << "CSW File with  " << mCasInPulses.size() << " pulse bytes...\n";
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
	if (DBG_LEVEL_DEV(this,DBG_VERBOSE))
		cout << "PLAY\n";
}

void TapeRecorder::rewind()
{
	if (mLoadFromTape) {
		mCasInPulseIndex = 0;
		mPlay = false;
		if (DBG_LEVEL_DEV(this,DBG_VERBOSE))
			cout << "REWIND\n";
	}
}

void TapeRecorder::pause()
{
	mPlay = false;
	mRecord = false;
	if (DBG_LEVEL_DEV(this,DBG_VERBOSE))
		cout << "PAUSE\n";
}

void TapeRecorder::stop()
{
	if (mLoadFromTape) {
		mLoadFromTape = false;
		mPlay = false;
		mCasInPulses.clear();
		updatePort(CAS_IN, 0);
		if (DBG_LEVEL_DEV(this,DBG_VERBOSE))
			cout << "STOP PLAYING\n";
	}
	else if (mSaveToTape) {

		// Add a last long pulse
		if (mRecord) {
			unsigned int pulse_len = (int)round((double)mHalfCycleDuration / (mCPUClock * 1e6) * mSampleRate);
			if (!mCodec->writePulse(pulse_len)) {
				cout << "Failed to write last pulse!\n";
			}
		}

		mSaveToTape = false;
		mRecord = false;
		if (DBG_LEVEL_DEV(this,DBG_VERBOSE))
			cout << "STOP RECORDING\n";

		mCodec->closeTapeFileW();
	}
	
}

void TapeRecorder::record()
{
	mRecord = true;
	if (DBG_LEVEL_DEV(this,DBG_VERBOSE))
		cout << "RECORD\n";
}