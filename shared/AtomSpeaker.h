#ifndef ATOM_SPEAKER_INTERFACE
#define ATOM_SPEAKER_INTERFACE

#include "Device.h"
#include <fstream>
#include <iostream>
#include "CSWCodec.h"
#include "allegro5/allegro_audio.h"

class AtomSpeaker : public Device {

private:

	uint8_t mOUT = 0x0;
	int OUT;
	uint8_t pOUT = 0x0;

	double mSystemClock;

	int mUpdateFreqCount = 0;


	bool updateAudio(uint8_t val);

	ALLEGRO_AUDIO_STREAM* mAudioStream = NULL;

	vector<uint8_t> mSamples;
	bool mSound = false;

public:

	AtomSpeaker(string name, double systemClock, DebugInfo debugInfo, ConnectionManager* connectionManager);
	~AtomSpeaker();

	bool advance(uint64_t stopCycle);

};





#endif