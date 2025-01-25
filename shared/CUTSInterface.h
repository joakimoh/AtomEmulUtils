#ifndef CUTS_INTERFACE
#define CUTS_INTERFACE

#include "Device.h"
#include <fstream>
#include <iostream>
#include "CSWCodec.h"

class CUTSInterface : public Device {

private:
	uint8_t mTONE = 0x0;
	uint8_t mENA_TONE = 0x0;
	uint8_t mTAPE_OUT = 0x0;
	uint8_t mCAS_IN = 0x0;
	int TONE, CAS_IN, ENA_TONE, TAPE_OUT;

	int mToneHalfcycle;

	uint8_t mCAS_OUT = 0x0;
	int mHalfCycleDuration = 0x0;

	vector<uint8_t> mCasInPulses;

	double mSystemClock;

	vector<uint8_t> mPulses;

	int mSampleRate = 44100;

	CSWCodec * mCodec = NULL;

	bool mSaveToTape = false;
	bool mLoadFromTape = false;
	int mCasInPulseIndex = 0;
	int mCasInPulseStartCount = 0;
	uint8_t mCasInPulseLevel = 0;
	int mCasInPulseLen = 0;
	bool mPlay = false;
	bool mRecord = false;
	bool mStartPlaying = false;


public:
	CUTSInterface(string name, double systemClock, DebugInfo debugInfo, ConnectionManager* connectionManager);
	~CUTSInterface();

	bool advance(uint64_t stopCycle);

	bool startLoadFile(string tapeFile);
	bool startSaveFile(string tapeFile);

	void play();
	void rewind();
	void pause();
	void stop();
	void record();

	bool playing() { return mLoadFromTape;  }
	bool recording() { return mSaveToTape;  }
};





#endif