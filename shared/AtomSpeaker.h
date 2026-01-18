#ifndef ATOM_SPEAKER_INTERFACE
#define ATOM_SPEAKER_INTERFACE

#include "SoundDevice.h"
#include <fstream>
#include <iostream>
#include "CSWCodec.h"
#include "allegro5/allegro_audio.h"
#include <cmath>

class AtomSpeaker : public SoundDevice {

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
public:

	AtomSpeaker(
		string name, double cpuClock, int sampleFreq, double emulationRate, double subEmulationRate,
		DebugManager  *debugManager, ConnectionManager* connectionManager
	);
	~AtomSpeaker();

	bool advanceUntil(uint64_t stopCycle);

	void setEmulationSpeed(double speed) override;

	// Outputs the internal state of the device
	bool outputState(ostream& sout) override;

};





#endif