#ifndef TAPE_RECORDER_H
#define TAPE_RECORDER_H

#include "Device.h"
#include <fstream>
#include <iostream>
#include "CSWCodec.h"

class TapeRecorder : public Device {

private:

	uint8_t mCAS_IN = 0x0;
	uint8_t mCAS_OUT = 0x0;
	int CAS_IN, CAS_OUT;
	uint8_t pCAS_OUT = 0x0;

	vector<uint8_t> mCasInPulses;

	int mHalfCycleDuration = 0x0;

	double mSystemClock;

	int mSampleRate = 44100;

	CSWCodec* mCodec = NULL;

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
	TapeRecorder(string name, double systemClock, DebugInfo debugInfo, ConnectionManager* connectionManager);
	~TapeRecorder();

	bool advance(uint64_t stopCycle);

	bool startLoadFile(string tapeFile);
	bool startSaveFile(string tapeFile);

	void play();
	void rewind();
	void pause();
	void stop();
	void record();

};


#endif