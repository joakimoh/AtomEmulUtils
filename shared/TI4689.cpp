#include "TI4689.h"
#include <cmath>

//
// Model of the TI4689 Sound Chip from the 1980's
// 


TI4689::TI4689(string name, double cpuClock, double fieldRate, int sampleFreq, DebugManager* debugManager, ConnectionManager* connectionManager) :

	SoundDevice(name, TI4689_DEV, cpuClock, sampleFreq, debugManager, connectionManager)
{
	registerPort("CLK", IN_PORT, 0x1, CLK,	&mCLK);
	registerPort("D",	IN_PORT, 0xff, D,	&mD);
	registerPort("WE", IN_PORT, 0x1, WE, &mWE);

	al_reserve_samples(4);

	mMixer = al_get_default_mixer();
	
	mFieldRate = fieldRate;
	mSamplesPerFragment = (int)round(1.0 * mSampleRate / mFieldRate); // 2 fields of audio
	mNFragments = 8;

	cout << "Field rate: " << dec << mFieldRate << "\n";
	cout << "Sample rate: " << dec << mSampleRate << "\n";

	for (int i = 0; i < 4; i++) {
		mChannelStream[i] = al_create_audio_stream(
			mNFragments,					// #fragments
			mSamplesPerFragment,			// size of a fragment
			mSampleRate,					// sample frequency
			ALLEGRO_AUDIO_DEPTH_INT16,
			ALLEGRO_CHANNEL_CONF_2
		);
		if (!mChannelStream[i]) {
			cout << "Could not create audio stream " << dec << i << "\n";
			throw runtime_error("Could not create audio stream");

		}
		// Create and output silence that lasts one field
		void* buf;
		while ((buf = al_get_audio_stream_fragment(mChannelStream[i]))) {
			al_fill_silence(buf, mSamplesPerFragment, ALLEGRO_AUDIO_DEPTH_INT16, ALLEGRO_CHANNEL_CONF_2);
			al_set_audio_stream_fragment(mChannelStream[i], buf);
		}

		if (!al_attach_audio_stream_to_mixer(mChannelStream[i], mMixer)) {
			cout << "Could not attach audio stream " << dec << i << " to audio mixer\n";
			throw runtime_error("Could not attach audio stream to audio mixer");
		}

		// Add one field of silence
		for (int i = 0; i < mSamplesPerFragment; i++)
			mSamples[i].push_back(0x0);
	}
}

TI4689::~TI4689()
{
	for (int i = 0; i < 4; i++) {
		al_drain_audio_stream(mChannelStream[i]);
		al_destroy_audio_stream(mChannelStream[i]);
	}
}

// Advance until clock cycle stopcycle has been reached
bool TI4689::advance(uint64_t stopCycle)
{
	for (int i = 0; i < 4; i++) {

		int16_t val; // sample to be generated

		if (i == 3) {

			//
			// The Noise Generator
			// 
	
			// The noise generator uses a shift register with a feedback network according to below for white noise:
			//																			 .__
			//																			 |  \  __
			//				+-------------------------------------------------------+	 |   \/  \
			//		+------>|b14|b13|b13|b10|b09|b08|b07|b06|b05|b04|b03|b02|b01|b00|----|NOT|	  |---> OUT
			//		|		+-------------------------------------------------------+	 |   /\__/
			//		|								_______ 				   |   |	 |__/
			//		|______________________________/      //-------------------+   |
			//									  |  XOR ||________________________+
			//									   \______\\ 
			//
			// For periodic noise, the feedback network is just bit b0
			// 
			// Shifting is made either based on the input clock (CLK) with three dividers (512, 1024 & 2048) 
			// ((mChannelCyclePeriod[i] > 0) or
			// based on the output of Tone Generator 3 ((mChannelCyclePeriod[i] == -1).
			//
			if (
				(mChannelCyclePeriod[i] > 0 && mChannelCycle[i] % mChannelCyclePeriod[i] == 0) ||
				(mChannelCyclePeriod[i] == -1 && mChannelCycle[3] % mChannelCyclePeriod[3] == 0)
				) {
				uint8_t feedback_network;
				if (mGenSrc[i].noiseType == WHITE_NOISE)
					feedback_network = ((mNoiseShiftRegister >> 1) & 0x1) ^ (mNoiseShiftRegister & 0x1);
				else // PERIODIC_NOISE
					feedback_network = mNoiseShiftRegister & 0x1;
				mNoiseShiftRegister = ((mNoiseShiftRegister >> 1) | (feedback_network << 14)) & 0x7fff;			
			}
			uint8_t out = (1 - (mNoiseShiftRegister & 0x1));
			if (out == 0)
				val = -mChannelLevel[i];
			else
				val = mChannelLevel[i];
		}

		else if (i < 3 && mChannelCyclePeriod[i] != 0) {

			//
			// A Tone generator
			//

			// Add one sinusoidal sample of the current selected level

			int phase = mChannelCycle[i] % mChannelCyclePeriod[i];
			int16_t val = mChannelLevel[i] * sin(2 * 3.14 * phase / mChannelCyclePeriod[i]);


		}

		if (i < 4) {
			mSamples[i].push_back(val);
			mChannelSampleCnt[i]++;
			{
				if (mSamples[i].size() >= mSamplesPerFragment)

					// Samples corresponding to a complete fragment exists => audio output possible
				{

					if (mChannelSampleCnt[i] > 0) {

						ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
						al_register_event_source(queue, al_get_audio_stream_event_source(mChannelStream[i]));

						ALLEGRO_EVENT event;

						al_wait_for_event(queue, &event);

						if (event.type == ALLEGRO_EVENT_AUDIO_STREAM_FRAGMENT) {

							int8_t* buf = (int8_t*)al_get_audio_stream_fragment(mChannelStream[i]);
							if (!buf) {
								/* Buffer is full */
								return true;
							}

							memcpy(buf, &mSamples[0], mSamplesPerFragment);

							if (!al_set_audio_stream_fragment(mChannelStream[i], buf)) {
								return false;
							}
						}
						al_destroy_event_queue(queue);

					}
					mSamples[i].clear();
					mChannelSampleCnt[i] = 0;

				}
			}
		}
	}

	mCycleCount = stopCycle;
	return true;
}

// Process input port changes directly (i.e. don't wait until the next call of the advance() method)
void TI4689::processPortUpdate(int index)
{

	if (index == CLK) {

	}

	else if (index == D) {

	}
		
	else if (index == WE) {

		if (mWE == 1 && mWE != pWE) // Sample data on WE low to high transition
		{
			// Write configuration for a channel 


			int reg_no = TI4689_REG_NO(mD);
			uint8_t src = TI4689_GEN_SRC(mD);
			bool first_byte = TI4689_FIRST_BYTE(mD);

			//cout << "Write 0x" << hex << (int) mD << dec << " to Sound Chip\n";


			switch (reg_no) {

			case R0_Tone_1_Freq:
			case R2_Tone_2_Freq:
			case R4_Tone_3_Freq:
				// Tone Generator 1/2/3 Frequency
			{
				if (src < 3) {
					if (first_byte) {
						mGenSrc[src].freq = (mGenSrc[src].freq & 0x3f0) | TI4689_LSB_FREQ(mD);
						if (mDM->debug(DBG_AUDIO)) {
							cout << "Set LSB of Tone Generator " << dec << src << " to 0x" << hex << TI4689_LSB_FREQ(mD) << " => Frequency " <<
								dec << (2.0e6 * mCLK / (32 * mGenSrc[src].freq)) << " Hz (0x" << hex << mGenSrc[src].freq << ")\n";
						}
					}
					else {
						mGenSrc[src].freq = (mGenSrc[src].freq & 0x00f) | (TI4689_MSB_FREQ(mD) << 4);
						if (mDM->debug(DBG_AUDIO)) {
							cout << "Set MSB of Tone Generator " << dec << src << " to 0x" << hex << TI4689_MSB_FREQ(mD) << " => Frequency " <<
								dec << (2.0e6 * mCLK / (32 * mGenSrc[src].freq)) << " Hz (0x" << hex << mGenSrc[src].freq << ")\n";
						}
					}
					mChannelCyclePeriod[src] = (int)round(mCPUClock * 2e6 / mGenSrc[src].freq);
					mSamples[src].clear();
					mChannelSampleCnt[src] = 0;
				}
				else {
					cout << "Invalid channel " << dec << src << "!\n";
				}
				break;
			}

			case R1_Tone_1_Att:
			case R3_Tone_2_Att:
			case R5_Tone_3_Att:
			case R7_Noise_Att:
				// Tone Generator 1/2/3 Attenuation
			{
				if (src < 4) {
					int a = TI4689_ATTENUATION(mD);
					if (TI4689_INVALID_ATT(a))
						mGenSrc[src].att = ATT_OFF;
					else
						mGenSrc[src].att = Attenuation(a);
					if (mDM->debug(DBG_AUDIO)) {
						if (reg_no == R7_Noise_Att)
							cout << "Set Attenuation for Noise Generator to 0x" << hex << a << " => Attenuation " <<
							_TI6847_ATTENUATION(mGenSrc[src].att) << "\n";
						else
							cout << "Set Attenuation for Tone Generator " << dec << src << " to 0x" << hex << a << " => Attenuation " <<
							_TI6847_ATTENUATION(mGenSrc[src].att) << "\n";
					}
					if (mGenSrc[src].att == ATT_OFF)
						mChannelLevel[src] = 0;
					else {
						// Attenuation = 2 x value [dB]
						int att_dB = mGenSrc[src].att;
						double att = pow(10, att_dB);
						double att_scaling = mChannelLevelMax / pow(10,16);
						mChannelLevel[src] = (int) round(att_scaling * att);
					}
				}
				else {
					cout << "Invalid channel " << dec << src << "!\n";
				}

				break;
			}

			case R6_Noise_Control:
				// Noise Control
			{
				mGenSrc[src].noiseType = TI4689_NOISE_TYPE(mD);
				mGenSrc[src].shiftRate = TI4689_NOISE_RATE(mD);

				switch (mGenSrc[src].shiftRate) {
				case DIV_512:
					mChannelCyclePeriod[src] = (int) round(512.0 * mCPUClock / mCLK);
					break;
				case DIV_1024:
					mChannelCyclePeriod[src] = (int)round(1024.0 * mCPUClock / mCLK);
					break;
				case DIV_2048:
					mChannelCyclePeriod[src] = (int)round(2048.0 * mCPUClock / mCLK);
					break;
				case GEN_3_OUT:
					mChannelCyclePeriod[src] = -1;
					break;
				default:
					mChannelCyclePeriod[src] = 0;
					break;
				}
				 

				if (mDM->debug(DBG_AUDIO)) {
					cout << "Set Noise Generator Type to " << _TI6847_NOISE_TYPE(mGenSrc[src].noiseType) <<
						" and noise rate to " << _TI6847_NOISE_RATE(mGenSrc[src].shiftRate) << "\n";
				}
				break;
			}

			default:
				break;
			}
		}
		pWE = mWE;
	}

}