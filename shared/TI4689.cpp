#include "TI4689.h"
#include <cmath>

//
// Model of the TI4689 Sound Chip from the 1980's
// 
//


TI4689::TI4689(string name, double cpuClock, int sampleFreq, DebugManager* debugManager, ConnectionManager* connectionManager) :
	SoundDevice(name, TI4689_DEV, cpuClock, sampleFreq, debugManager, connectionManager)
{
	registerPort("OUT", IN_PORT, 0x01, OUT, &mOUT);	// From PIA PC2

	mUpdateFreqCount = (int)round(mCPUClock * 1e6 / sampleFreq); // limit updates to sampleFreq

	al_reserve_samples(0);


}

TI4689::~TI4689()
{
	al_drain_audio_stream(mAudioStream);
	al_destroy_audio_stream(mAudioStream);
}

void TI4689::setFieldRate(int fieldRate)
{
	SoundDevice::setFieldRate(fieldRate);

	int mSamplesPerFragment = (int)round(1.0 * mSampleRate / mFieldRate); // 2 frames of audio
	int mNFragments = 8;

	// Create audio stream
	mAudioStream = al_create_audio_stream(
		mNFragments,					// #fragments
		mSamplesPerFragment,			// size of a fragment
		mSampleRate,					// sample frequency
		ALLEGRO_AUDIO_DEPTH_UINT8,
		ALLEGRO_CHANNEL_CONF_1
	);
	if (!mAudioStream) {
		throw runtime_error("Could not create audio stream");
		cout << "Could not create audio stream\n";
	}

	// Create and output silence that lasts one field
	void* buf;
	while ((buf = al_get_audio_stream_fragment(mAudioStream))) {
		al_fill_silence(buf, mSamplesPerFragment, ALLEGRO_AUDIO_DEPTH_UINT8, ALLEGRO_CHANNEL_CONF_1);
		al_set_audio_stream_fragment(mAudioStream, buf);
	}

	if (!al_attach_audio_stream_to_mixer(mAudioStream, al_get_default_mixer())) {
		throw runtime_error("Could not attach audio stream to audio mixer");
		cout << "Could not attach audio stream to audio mixer\n";
	}

	// Add one field of silence
	for (int i = 0; i < mSamplesPerFragment; i++)
		mSamples.push_back(0x0);
}


bool TI4689::advance(uint64_t stopCycle)
{

	updateAudio(mOUT << 7);
	if (mOUT != pOUT)
		mSoundCnt++;
	pOUT = mOUT;
	mCycleCount = stopCycle;


	return true;
}

bool TI4689::updateAudio(uint8_t val)
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