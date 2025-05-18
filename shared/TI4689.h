#ifndef TI4689_H
#define TI4689_H

#include "SoundDevice.h"
#include <fstream>
#include <iostream>
#include "CSWCodec.h"
#include "allegro5/allegro_audio.h"
#include <cmath>

class TI4689 : public SoundDevice {

private:

	uint8_t mOUT = 0x0;
	int OUT;
	uint8_t pOUT = 0x0;

	int mUpdateFreqCount = 0;
	int mSamplesPerFragment = 512;
	int mNFragments = 8;


	bool updateAudio(uint8_t val);

	ALLEGRO_AUDIO_STREAM* mAudioStream = NULL;

	vector<uint8_t> mSamples;
	int mSoundCnt = 0;

	uint8_t mRegister[8] = { 0 };

public:

	TI4689(string name, double cpuClock, int sampleFreq, DebugManager* debugManager, ConnectionManager* connectionManager);
	~TI4689();

	bool advance(uint64_t stopCycle);

	void setFieldRate(int fieldRate);

};





#endif