#include "AtomSpeaker.h"
#include <cmath>

//
// Model of the Speaker Interface implemented by the Acorn Atom
//
//


AtomSpeaker::AtomSpeaker(string name, double cpuClock, double fieldRate, int sampleFreq, DebugManager  *debugManager, ConnectionManager* connectionManager,
	double speed) :
	SoundDevice(name, ATOM_SPEAKER_DEV, cpuClock, sampleFreq, debugManager, connectionManager)
{
	registerPort("OUT", IN_PORT, 0x01, OUT, &mOUT);	// From PIA PC2

	mUpdateFreqCount = (int)round(mCPUClock *1e6 / sampleFreq); // limit updates to sampleFreq

	al_reserve_samples(0);

	setFieldRate(fieldRate, speed);

	
}

AtomSpeaker::~AtomSpeaker()
{
	al_drain_audio_stream(mAudioStream);
	al_destroy_audio_stream(mAudioStream);
}

void AtomSpeaker::setFieldRate(int fieldRate, double speed)
{
	SoundDevice::setFieldRate(fieldRate, speed);

	mSamples.clear();
	mSoundCnt = 0;

	if (mAudioStream != NULL) {
			al_drain_audio_stream(mAudioStream);
			al_destroy_audio_stream(mAudioStream);
	}

	mSamplesPerFragment = (int) round(1.0 * mSampleRate / mFieldRate * speed); // 2 frames of audio
	mNFragments = 8;

	// Create audio stream
	mAudioStream = al_create_audio_stream(
		mNFragments,					// #fragments
		mSamplesPerFragment,			// size of a fragment
		mRealSampleRate,					// sample frequency
		ALLEGRO_AUDIO_DEPTH_UINT8,
		ALLEGRO_CHANNEL_CONF_1
	);
	if (!mAudioStream) {
		DBG_LOG(this, DBG_ERROR, "Could not create audio stream");
		throw runtime_error("Could not create audio stream");
	}

	// Create and output silence that lasts one field
	void* buf;	
	while ((buf = al_get_audio_stream_fragment(mAudioStream))) {
		al_fill_silence(buf, mSamplesPerFragment, ALLEGRO_AUDIO_DEPTH_UINT8, ALLEGRO_CHANNEL_CONF_1);
		al_set_audio_stream_fragment(mAudioStream, buf);
	}

	if (!al_attach_audio_stream_to_mixer(mAudioStream, al_get_default_mixer())) {
		DBG_LOG(this, DBG_ERROR, "Could not attach audio stream to audio mixer");
		throw runtime_error("Could not attach audio stream to audio mixer");
	}

	// Add one field of silence
	for (int i = 0; i < mSamplesPerFragment; i++)
		mSamples.push_back(0x0);
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

	if (mSamples.size() >= mSamplesPerFragment)

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

				memcpy(buf, &mSamples[0], mSamplesPerFragment);

				if (!al_set_audio_stream_fragment(mAudioStream, buf)) {
					return false;
				}							
			}
			al_destroy_event_queue(queue);			
			
		}
		mSamples.clear();
		mSoundCnt = 0;
		
	}
	
	
	return true;
}

// Outputs the internal state of the device
bool AtomSpeaker::outputState(ostream& sout)
{
	sout << "The Acorn Atom Sound Device is stateless!\n";

	return true;
}