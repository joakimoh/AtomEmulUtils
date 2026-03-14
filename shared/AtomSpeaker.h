#ifndef ATOM_SPEAKER_INTERFACE
#define ATOM_SPEAKER_INTERFACE

#include "SoundDevice.h"
#include <fstream>
#include <iostream>
#include "CSWCodec.h"
#include "allegro5/allegro_audio.h"
#include <cmath>
#include "TimedDevice.h"

class AtomSpeaker : public SoundDevice, public TimedDevice {

private:

	PortVal mOUT = 0x0;
	int OUT;
	PortVal pOUT = 0x0;

	int mSamplesPerFragment = 512;
	int mNFragments = 8;


	bool updateAudio(uint8_t val);

	ALLEGRO_AUDIO_STREAM* mAudioStream = NULL;

	vector<uint8_t> mSamples;
	int mSoundCnt = 0;
public:

	AtomSpeaker(
		string name, double tickRate, int sampleFreq, double emulationRate, double highEmulationRate,
		DebugTracing  *debugTracing, ConnectionManager* connectionManager
	);
	~AtomSpeaker();

	bool advanceUntil(uint64_t stopTick) override;

	void setEmulationSpeed(double speed) override;

	// Outputs the internal state of the device
	bool outputState(ostream& sout) override;

};





#endif