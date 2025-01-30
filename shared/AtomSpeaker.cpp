#include "AtomSpeaker.h"
#include <cmath>

//
// Model of the CUTS Interface implemented by the Acorn Atom
//
// It connects the PIA 8255 and a Tape Recorder
//
// PIA Port	PIA Direction	CUTS Port	Usage
// PC0		OUT				TAPE_OUT	Tape output - both provides and enables cassette output
// PC1		OUT				ENA_TONE	Enable 2.4 kHz tone generation to the cassette output
// PC4		IN				TONE		2.4 KHz tone input from the CUTS Interface
// PC5		IN				TAPE_IN		Cassette input
// 
// CUTS Port	Usage
// CAS_IN		Cassette input from the Tape Recorder
// CAS_OUT		Cassette output to the Tape Recorder
//

#define SAMPLES_PER_FRAGMENT    312*4
#define N_FRAGMENTS				8

AtomSpeaker::AtomSpeaker(string name, double systemClock, int sampleFreq, DebugInfo debugInfo, ConnectionManager* connectionManager) :
	SoundDevice(name, ATOM_SPEAKER_DEV, debugInfo, connectionManager), mSystemClock(systemClock)
{
	registerPort("OUT", IN_PORT, 0x01, OUT, &mOUT);	// From PIA PC2


	mUpdateFreqCount = (int)round(systemClock *1e6 / sampleFreq); // limit updates to 32 kHz


	al_reserve_samples(0);

	// Create audio stream
	mAudioStream = al_create_audio_stream(
		N_FRAGMENTS,					// #fragments
		SAMPLES_PER_FRAGMENT,			// size of a fragment
		sampleFreq,						// sample frequency
		ALLEGRO_AUDIO_DEPTH_UINT8,
		ALLEGRO_CHANNEL_CONF_1
	);
	if (!mAudioStream) {
		throw runtime_error("Could not create audio stream");
		cout << "Could not create audio stream\n";
	}

	void* buf;
	while ((buf = al_get_audio_stream_fragment(mAudioStream))) {
		al_fill_silence(buf, SAMPLES_PER_FRAGMENT, ALLEGRO_AUDIO_DEPTH_UINT8, ALLEGRO_CHANNEL_CONF_1);
		al_set_audio_stream_fragment(mAudioStream, buf);
	}

	if (!al_attach_audio_stream_to_mixer(mAudioStream, al_get_default_mixer())) {
		throw runtime_error("Could not attach audio stream to audio mixer");
		cout << "Could not attach audio stream to audio mixer\n";
	}
}

AtomSpeaker::~AtomSpeaker()
{
	al_drain_audio_stream(mAudioStream);
	al_destroy_audio_stream(mAudioStream);
}


bool AtomSpeaker::advance(uint64_t stopCycle)
{

	updateAudio(mOUT << 7);
	if (mOUT != pOUT)
		mSoundCnt++;
	pOUT = mOUT;
	mCycleCount = stopCycle;


	return true;
}

bool AtomSpeaker::updateAudio(uint8_t val)
{
	mSamples.push_back(val);
	//mSamples.push_back(val);

	if (mSamples.size() >= SAMPLES_PER_FRAGMENT)
	// Samples corresponding to a complete fragment exists => audio output possible
	{

		if (mSoundCnt > 0) {

			ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
			al_register_event_source(queue, al_get_audio_stream_event_source(mAudioStream));

			ALLEGRO_EVENT event;

			al_wait_for_event(queue, &event);

			if (event.type == ALLEGRO_EVENT_AUDIO_STREAM_FRAGMENT) {

				int8_t* buf = (int8_t*)al_get_audio_stream_fragment(mAudioStream);
				if (!buf) {
					/* Buffer is full */
					return true;
				}

				memcpy(buf, &mSamples[0], SAMPLES_PER_FRAGMENT);
				/*
				for (int i = 0; i < SAMPLES_PER_FRAGMENT; i++) {
					buf[i] = (mSamples[i] != 0 ? 0xff : 0x00);
				}
				*/

				if (!al_set_audio_stream_fragment(mAudioStream, buf)) {
					return false;
				}

				al_destroy_event_queue(queue);
				
			}

			
			
		}
		mSamples.clear();
		mSoundCnt = 0;
		
	}
	
	
	return true;
}