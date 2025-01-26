#include "TapeRecorder.h"

TapeRecorder::TapeRecorder(string name, double systemClock, DebugInfo debugInfo, ConnectionManager* connectionManager) :
	Device(name, TAPE_RECORDER_DEV, OTHER_DEVICE, debugInfo, connectionManager), mSystemClock(systemClock * 1e6)
{
	registerPort("CAS_IN", OUT_PORT, 0x01, CAS_IN, &mCAS_IN);
	registerPort("CAS_OUT", IN_PORT, 0x01, CAS_IN, &mCAS_OUT);

	mCodec = new CSWCodec(44100, mDebugInfo);

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
					unsigned int pulse_len = (int)round((double)mHalfCycleDuration / mSystemClock * mSampleRate);
					if (!mCodec->writePulse(pulse_len)) {
						return false;
					}
				}
			}
			mHalfCycleDuration = 0;

		}

		else if (mLoadFromTape && mPlay) {

			if (mStartPlaying || mCycleCount - mCasInPulseStartCount == mCasInPulseLen) {
				mStartPlaying = false;
				mCasInPulseLevel = 1 - mCasInPulseLevel;
				updatePort(CAS_IN, mCasInPulseLevel);
				unsigned int pulse_len;
				if (!mCodec->readPulse(mCasInPulses, mCasInPulseIndex, pulse_len)) {
					mLoadFromTape = false;
					updatePort(CAS_IN, 0);
				}
				mCasInPulseLen = (int)round(mSystemClock * pulse_len / mSampleRate);
				mCasInPulseStartCount = mCycleCount;
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
	cout << "Open CSW File '" << tapeFile << "'\n";

	if (!mCodec->decode(tapeFile, mCasInPulses, mCasInPulseLevel, mSampleRate)) {
		return false;
	}
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
}

void TapeRecorder::rewind()
{
	if (mLoadFromTape) {
		mCasInPulseIndex = 0;
		mPlay = false;
	}
}

void TapeRecorder::pause()
{
	mPlay = false;
	mRecord = false;
}

void TapeRecorder::stop()
{
	if (mLoadFromTape) {
		cout << "STOP - clear file...\n";
		mLoadFromTape = false;
		mPlay = false;
		mCasInPulses.clear();
	}
	else if (mSaveToTape) {
		cout << "STOP - saving to file...\n";
		mSaveToTape = false;
		mRecord = false;
		mCodec->closeTapeFileW();
	}
}

void TapeRecorder::record()
{
	mRecord = true;
}