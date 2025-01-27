#include "CUTSInterface.h"

CUTSInterface::CUTSInterface(string name, double systemClock, DebugInfo debugInfo, ConnectionManager* connectionManager) :
	Device(name, CUTS_DEV, OTHER_DEVICE, debugInfo, connectionManager), mSystemClock(systemClock * 1e6)
{
	registerPort("TONE",		OUT_PORT,	0x01, TONE,		&mTONE);
	registerPort("CAS_IN",		OUT_PORT,	0x01, CAS_IN,	&mCAS_IN);
	registerPort("ENA_TONE",	IN_PORT,	0x01, ENA_TONE, &mENA_TONE);
	registerPort("TAPE_OUT",	IN_PORT,	0x01, TAPE_OUT, &mTAPE_OUT);
	//registerPort("CAS_OUT", OUT_PORT, 0x01, CAS_OUT, &mCAS_OUT);

	mToneHalfcycle = (int)round(systemClock * 1e6 / 2400 / 2);

	mCodec = new CSWCodec(44100, mDebugInfo);

}

CUTSInterface::~CUTSInterface()
{
	delete mCodec;
}

bool CUTSInterface::advance(uint64_t stopCycle)
{
	while (mCycleCount < stopCycle) {

		// Toggle 2.4 kHz tone when one 1/2 cycle of 2.4 Khz has elapsed
		if (mCycleCount % mToneHalfcycle == 0)
			updatePort(TONE, 1 - mTONE);

		// Generate CAS OUT
		uint8_t cas_out = (1 - (((1 - mENA_TONE) | mTONE) & mTAPE_OUT));

		if (cas_out != mCAS_OUT) {

			//cout << "PULSE!\n";

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

		if (mLoadFromTape && mPlay) {

			if (mStartPlaying || mCycleCount - mCasInPulseStartCount == mCasInPulseLen) {
				mStartPlaying = false;
				mCasInPulseLevel = 1 - mCasInPulseLevel;
				updatePort(CAS_IN, mCasInPulseLevel);
				unsigned int pulse_len;
				if (!mCodec->readPulse(mCasInPulses, mCasInPulseIndex, pulse_len)) {
					mLoadFromTape = false;
					updatePort(CAS_IN, 0);
				}
				mCasInPulseLen = (int) round(mSystemClock  * pulse_len / mSampleRate );
				mCasInPulseStartCount = mCycleCount;
			}
		}

		mCAS_OUT = cas_out;

		mHalfCycleDuration++;

		mCycleCount++;

		
	}

	pTONE = mTONE;
	pTAPE_OUT = mTAPE_OUT;

	return true;
}

bool CUTSInterface::startLoadFile(string tapeFile)
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

bool CUTSInterface::startSaveFile(string tapeFile)
{
	mSaveToTape = true;
	mHalfCycleDuration = 0;
	return mCodec->openTapeFileW(tapeFile);

	return true;
}

void CUTSInterface::play()
{
	mPlay = true;
	mStartPlaying = true;
}

void CUTSInterface::rewind()
{
	if (mLoadFromTape) {
		mCasInPulseIndex = 0;
		mPlay = false;
	}
}

void CUTSInterface::pause()
{
	mPlay = false;
	mRecord = false;
}

void CUTSInterface::stop()
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

void CUTSInterface::record()
{
	mRecord = true;
}