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

	// Create a default allegro Mixer and connect it to an allegro Voice
	al_reserve_samples(0);

	// Create a queue for audio stream events
	mQueue = al_create_event_queue();

	// Get a handle to the previously created Mixer
	mMixer = al_get_default_mixer();
	
	mFieldRate = fieldRate;
	mSamplesPerFragment = (int)round(1.0 * mSampleRate / mFieldRate); // 2 fields of audio
	mCpuCyclesPerSample = (int) round(1e6 * cpuClock / mSampleRate);
	mNFragments = 8;

	if (mDM->debug(DBG_AUDIO)) {
		cout << "Sample rate: " << dec << mSampleRate << "\n";
	}

	for (int channel = 0; channel < 4; channel++) {

		mChannelStream[channel] = al_create_audio_stream(
			mNFragments,					// #fragments
			mSamplesPerFragment,			// size of a fragment
			mSampleRate,					// sample frequency
			ALLEGRO_AUDIO_DEPTH_INT16,
			ALLEGRO_CHANNEL_CONF_2
		);
		if (!mChannelStream[channel]) {
			cout << "Could not create audio stream " << dec << channel << "\n";
			throw runtime_error("Could not create audio stream");

		}

		// Fill the channel with silence that lasts one CRTC field
		void* buf;
		while ((buf = al_get_audio_stream_fragment(mChannelStream[channel]))) {
			al_fill_silence(buf, mSamplesPerFragment, ALLEGRO_AUDIO_DEPTH_INT16, ALLEGRO_CHANNEL_CONF_2);
			al_set_audio_stream_fragment(mChannelStream[channel], buf);
		}

		// Connect the channel to the default mixer
		if (!al_attach_audio_stream_to_mixer(mChannelStream[channel], mMixer)) {
			cout << "Could not attach audio stream " << dec << channel << " to audio mixer\n";
			throw runtime_error("Could not attach audio stream to audio mixer");
		}

		// Add one field of silence to the initial samples
		for (int i = 0; i < mSamplesPerFragment; i++)
			mSamples[channel].push_back(0x0);

		al_register_event_source(mQueue, al_get_audio_stream_event_source(mChannelStream[channel]));

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
	while (mCycleCount < stopCycle) {

		if (mCycleCount % mCpuCyclesPerSample == 0) {

			mSampleCount++;

			for (int channel = 0; channel < 4; channel++) {

				//
				// Generate one sample
				//

				int16_t val; // sample to be generated

				if (channel == 3) {

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
					// ((mChannelHalfCycleSamples[channel] > 0) or
					// based on the output of Tone Generator 3 ((mChannelHalfCycleSamples[channel] == -1).
					//
					if (
						(mChannelHalfCycleSamples[channel] > 0 && mSampleCount % mChannelHalfCycleSamples[channel] == 0) ||
						(mChannelHalfCycleSamples[channel] == -1 && mSampleCount % mChannelHalfCycleSamples[3] == 0)
						) {
						uint8_t feedback_network;
						if (mGenSrc[channel].noiseType == WHITE_NOISE)
							feedback_network = ((mNoiseShiftRegister >> 1) & 0x1) ^ (mNoiseShiftRegister & 0x1);
						else // PERIODIC_NOISE
							feedback_network = mNoiseShiftRegister & 0x1;
						mNoiseShiftRegister = ((mNoiseShiftRegister >> 1) | (feedback_network << 14)) & 0x7fff;
					}
					uint8_t out = (1 - (mNoiseShiftRegister & 0x1));
					if (out == 0)
						val = -mChannelLevel[channel];
					else
						val = mChannelLevel[channel];
				}

				else if (channel < 3 && mChannelHalfCycleSamples[channel] != 0) {

					//
					// A Square Wave Tone generator
					//

					if (mSampleCount % mChannelHalfCycleSamples[channel] == 0)
						mOutput[channel] = -mOutput[channel];

					val = mOutput[channel];

				}

				// Add sample
				mSamples[channel].push_back(val);

				//
				// Output samples when samples corresponding to a complete fragment exist
				//

				if (mSamples[channel].size() >= mSamplesPerFragment)
				{
					// Check if the stream is ready for new data
					int8_t* buf = (int8_t*)al_get_audio_stream_fragment(mChannelStream[channel]);

					if (buf) {
						// Stream was ready for new data - add the current samples to it!
						memcpy(buf, &mSamples[channel][0], mSamplesPerFragment);
						if (!al_set_audio_stream_fragment(mChannelStream[channel], buf)) {
							return false;
						}

						if (mChannelLevel[channel] > 0 && mChannelHalfCycleSamples[channel] > 0 && mDM->debug(DBG_AUDIO)) {
							// mChannelHalfCycleSamples[channel] = (int)round(0.5 * mSampleRate / mGenSrc[channel].freq);
							double freq = 0.5 * mSampleRate / mChannelHalfCycleSamples[channel];
							string generator = (channel < 3 ? "Tone Generator " + to_string(channel + 1) : "Noise Generator");
							cout << dec << mSamplesPerFragment << " samples (" << mSampleRate/freq << " cycles)" << 
								" of frequency " << dec << freq <<
								" and max volume " << mChannelLevel[channel] <<
								" generated for " << generator << "\n";
						}

						// Clear samples
						mSamples[channel].clear();
					}

				}


			}
		}

		mCycleCount++;
	}
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
			uint8_t channel = TI4689_GEN_SRC(mD);
			bool first_byte = TI4689_FIRST_BYTE(mD);

			if (!first_byte) {
				reg_no = TI4689_REG_NO(mFirstByte);
				channel = TI4689_GEN_SRC(mFirstByte);
			}
			else
				mFirstByte = mD;

			string generator = (channel < 3 ? "Tone Generator " + to_string(channel+1) : "Noise Generator");


			//cout << "Write 0x" << hex << (int) mD << dec << " to Sound Chip\n";


			switch (reg_no) {

			case R0_Tone_1_Freq:
			case R2_Tone_2_Freq:
			case R4_Tone_3_Freq:
				// Tone Generator 1/2/3 Frequency
			{
				if (channel < 3) {
					if (first_byte) {
						mGenSrc[channel].freq = (mGenSrc[channel].freq & 0x3f0) | TI4689_LSB_FREQ(mD);
						if (mDM->debug(DBG_AUDIO)) {
							cout << "Set LSB of " << generator << " to 0x" << hex << TI4689_LSB_FREQ(mD);
						}
					}
					else {
						mGenSrc[channel].freq = (mGenSrc[channel].freq & 0x00f) | (TI4689_MSB_FREQ(mD) << 4);
						if (mDM->debug(DBG_AUDIO)) {
							cout << "Set MSB of " << generator << " to 0x" << hex << TI4689_MSB_FREQ(mD);
						}
					}
					if (mGenSrc[channel].freq > 0)
						mChannelHalfCycleSamples[channel] = (int)round(0.5 * mSampleRate / mGenSrc[channel].freq);
					else
						mChannelHalfCycleSamples[channel] = 0;
					if (mDM->debug(DBG_AUDIO)) {
						cout << " <=> Frequency " <<
							dec << (mGenSrc[channel].freq > 0 ? (1e6 * mCLK / (32 * mGenSrc[channel].freq)) : 0) << " Hz (0x" <<
							hex << mGenSrc[channel].freq << ")" << " and " << mChannelHalfCycleSamples[channel] << " samples per tone 1/2 cycle " << 
							"\n";

					}
					mSamples[channel].clear();
				}
				else {
					cout << "Invalid channel " << dec << channel << "!\n";
				}
				break;
			}

			case R1_Tone_1_Att:
			case R3_Tone_2_Att:
			case R5_Tone_3_Att:
			case R7_Noise_Att:
				// Tone Generator 1/2/3 Attenuation
				// Attenuation is - 1dB x value unless value is 0xf (then volume is off completely)
			{
				if (channel < 4) {
					int a = TI4689_ATTENUATION(mD);
					mGenSrc[channel].att = a;
					if (TI4689_ATT_OFF(a))
						mChannelLevel[channel] = 0;
					else
						mChannelLevel[channel] = (int) round (mChannelLevelMax * pow(10, - a / 10.0));
					if (mDM->debug(DBG_AUDIO)) {
						cout << "Set Attenuation for " << generator <<  " to 0x" << hex << a << " = > Attenuation " <<
							_TI6847_ATTENUATION(a) << " => Volume " << dec << mChannelLevel[channel] << "\n";
					}
				}
				else {
					cout << "Invalid channel " << dec << (int)channel << "!\n";
				}

				break;
			}

			case R6_Noise_Control:
				// Noise Control
			{
				mGenSrc[channel].noiseType = TI4689_NOISE_TYPE(mD);
				mGenSrc[channel].shiftRate = TI4689_NOISE_RATE(mD);

				switch (mGenSrc[channel].shiftRate) {
				case DIV_512:
					mChannelHalfCycleSamples[channel] = (int) round(0.5 * 512 * mSampleRate / mCLK);
					break;
				case DIV_1024:
					mChannelHalfCycleSamples[channel] = (int)round(0.5 * 1024 * mSampleRate / mCLK);
					break;
				case DIV_2048:
					mChannelHalfCycleSamples[channel] = (int)round(0.5 * 2048 * mSampleRate / mCLK);
					break;
				case GEN_3_OUT:
					mChannelHalfCycleSamples[channel] = -1;
					break;
				default:
					mChannelHalfCycleSamples[channel] = 0;
					break;
				}
				 

				if (mDM->debug(DBG_AUDIO)) {
					cout << "Set Noise Generator Type to " << _TI6847_NOISE_TYPE(mGenSrc[channel].noiseType) <<
						" and noise rate to " << _TI6847_NOISE_RATE(mGenSrc[channel].shiftRate) << "\n";
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