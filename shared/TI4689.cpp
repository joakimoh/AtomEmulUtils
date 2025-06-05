#include "TI4689.h"
#include <cmath>

//
// Model of the TI4689 Sound Chip from the 1980's
// 


TI4689::TI4689(string name, double cpuClock, double fieldRate, int sampleFreq, DebugManager* debugManager, ConnectionManager* connectionManager, double speed) :

	SoundDevice(name, TI4689_DEV, cpuClock, sampleFreq, debugManager, connectionManager)
{
	registerPort("CLK", IN_PORT, 0x1, CLK,	&mCLK);
	registerPort("D",	IN_PORT, 0xff, D,	&mD);
	registerPort("WE", IN_PORT, 0x1, WE, &mWE);

	// Create a default allegro Mixer and connect it to an allegro Voice
	al_reserve_samples(0);

	// Get a handle to the previously created Mixer
	mMixer = al_get_default_mixer();
	
	setFieldRate(fieldRate, speed);

}

void TI4689::setFieldRate(int fieldRate, double speed)
{
	SoundDevice::setFieldRate(fieldRate, speed);

	mSamplesPerFragment = (int)round(0.5 * mSampleRate / mFieldRate); // one field of audio
	mCpuCyclesPerSample = (int)round(1e6 * mCPUClock / mSampleRate);
	mNFragments = 8;
	

	if (DBG_LEVEL(DBG_VERBOSE)) {
		cout << "CPU Clock:                    " << dec << mCPUClock << " MHz\n";
		cout << "Field rate:                   " << dec << mFieldRate << "\n";
		cout << "Sample rate:                  " << dec << mSampleRate << "\n";
		cout << "Samples per fragment:         " << dec << mSamplesPerFragment << "\n";
		cout << "CPU cycles per sample:        " << dec << mCpuCyclesPerSample << "\n";
		cout << "No of audio stream fragments: " << dec << mNFragments << "\n";
	}

	mSampleCount = 0;

	for (int channel = 0; channel < 4; channel++) {
		if (mChannelStream[channel] != NULL) {
			al_drain_audio_stream(mChannelStream[channel]);
			al_destroy_audio_stream(mChannelStream[channel]);
		}
		mSamplesSize[channel] = 0;
	}

	for (int channel = 0; channel < 4; channel++) {

		mSamples[channel].resize(mSamplesPerFragment);

		mChannelStream[channel] = al_create_audio_stream(
			mNFragments,					// #fragments
			mSamplesPerFragment,			// size of a fragment
			mRealSampleRate,					// sample frequency
			ALLEGRO_AUDIO_DEPTH_INT16,		// int16_t samples
			ALLEGRO_CHANNEL_CONF_2			// Stereo => pair of int16_t samples
		);
		if (!mChannelStream[channel]) {
			cout << "Could not create audio stream " << dec << channel << "\n";
			throw runtime_error("Could not create audio stream");

		}

		// Fill the channel with silence that lasts eight CRTC fields (mNFragments / mFieldRate)
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

			for (int channel = 0; channel < 4; channel++) {

				//
				// Generate one sample
				//

				int16_t val = 0; // sample to be generated

				if (channel == TI4689_NOISE_GENERATOR) {

					//
					// The Noise Generator
					// 

					// The noise generator uses a shift register with a feedback network according to below for white noise:
					//
					//				 set to '1' when R6 is written to
					//				  |															 .__
					//				  V															 |  \  __
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
					// When the Noise Control Register (R6) is written to, all bits in the shift regsiter will be cleared except
					// b14 that will be set.
					// 
					// Shifting is made either based on the input clock (CLK) with three dividers (512, 1024 & 2048) 
					// ((mChannelHalfCycleSamples[channel] > 0) or
					// based on the output of Tone Generator 3 ((mChannelHalfCycleSamples[channel] == -1).
					//


					// Shift register based on each '0'->'1' transition of either
					// - the reference clock (CLK) divided by 512/1204/2048 (mNoiseShiftSamples > 0) OR
					// - the Tone Generator 3 (mNoiseShiftSamples == -1)
					if (
						(mNoiseShiftSamples > 0 && mSampleCount % mNoiseShiftSamples == 0) ||
						(mNoiseShiftSamples == -1 && mToneGen3LHTransition)
						)
					{
						mToneGen3LHTransition = false;
						uint8_t feedback_network;
						if (mGenSrc[channel].noiseType == WHITE_NOISE)
							feedback_network = ((mNoiseShiftRegister >> 1) & 0x1) ^ (mNoiseShiftRegister & 0x1);
						else // PERIODIC_NOISE
							feedback_network = mNoiseShiftRegister & 0x1;
						mNoiseShiftRegister = ((mNoiseShiftRegister >> 1) | (feedback_network << 14)) & 0x7fff;
					}

					mOutput[channel] = (1 - (mNoiseShiftRegister & 0x1)); // output should be inverted

				}

				else if (channel <= TI4689_TONE_GENERATOR_3 && mChannelHalfCycleSamples[channel] > 0) {

					//
					// A Square Wave Tone generator<
					//

					if (mSampleCount % mChannelHalfCycleSamples[channel] == 0) {
						mOutput[channel] = 1 - mOutput[channel];
						if (DBG_LEVEL(DBG_AUDIO) && mChannelVolume[channel] > 0) {
							int diff = mCycleCount - mHalfCycleCount[channel];
							cout << (1e6 * mCPUClock) / (2 * diff) << " Hz (" << dec << diff << " 1/2 CPU cycles) for 1/2 cycle samples of " <<
								mChannelHalfCycleSamples[channel] << " for channel " << channel << "\n";
						}
						mHalfCycleCount[channel] = mCycleCount;
					}

				}

				// Change value range [0,1] to range [-1,+1] x channel volume
				val = ((mOutput[channel] << 1) - 1) * mChannelVolume[channel];;

				// Add stereo sample
				if (mSamplesSize[channel] < mSamplesPerFragment) {
					mSamples[channel][mSamplesSize[channel]++] = { val, val };
				}

				//
				// Output samples when samples corresponding to a complete fragment exist
				//

				if (mSamplesSize[channel] >= mSamplesPerFragment)
				{
					// Check if the stream is ready for new data
					int16_t* buf = (int16_t*)al_get_audio_stream_fragment(mChannelStream[channel]);

					if (buf) {

						// Stream was ready for new data

						//  Add the samples to the stream
						memcpy(buf, (char*)&mSamples[channel][0], sizeof(SoundSample) * mSamplesPerFragment);
						if (!al_set_audio_stream_fragment(mChannelStream[channel], buf)) {
							return false;
						}

						if (DBG_LEVEL(DBG_AUDIO) && mChannelVolume[channel] > 0 && mChannelHalfCycleSamples[channel] > 0) {
							double freq = 0.5 * mSampleRate / mChannelHalfCycleSamples[channel];
							cout << dec << mSamplesPerFragment << "/" << mSamplesSize[channel] << " samples" <<
								" (" << ((double)mSamplesPerFragment / (mSampleRate / freq)) << " cycles)" <<
								" of frequency " << dec << freq <<
								" and max volume " << mChannelVolume[channel] <<
								" generated for " << _TI4689_GENERATOR(channel) << "\n";
						}

						// Clear samples
						mSamplesSize[channel] = 0;
					}

				}

				if (channel == TI4689_TONE_GENERATOR_3 && pOutput[channel] == 0 && mOutput[channel] == 1)
					mToneGen3LHTransition = true;

				// Save output value for transition detection in upcoming rounds
				pOutput[channel] = mOutput[channel];
			}

			mSampleCount++;
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


			int reg_addr = TI4689_REG_NO(mD);
			uint8_t channel = TI4689_GEN_SRC(mD);
			bool first_byte = TI4689_FIRST_BYTE(mD);

			if (!first_byte) {
				reg_addr = TI4689_REG_NO(mFirstByte);
				channel = TI4689_GEN_SRC(mFirstByte);
			}
			else
				mFirstByte = mD;

			string generator = _TI4689_GENERATOR(channel);


			//cout << "Write 0x" << hex << (int) mD << dec << " to Sound Chip\n";


			switch (reg_addr) {

			case R0_Tone_1_Freq:
			case R2_Tone_2_Freq:
			case R4_Tone_3_Freq:
				// Tone Generator 1/2/3 Frequency
			{
				if (channel < 3) {
					if (first_byte) {
						mGenSrc[channel].freq = (mGenSrc[channel].freq & 0x3f0) | TI4689_LSB_FREQ(mD);
						if (DBG_LEVEL(DBG_AUDIO)) {
							cout << "Set LSB of " << generator << " to 0x" << hex << TI4689_LSB_FREQ(mD);
						}
					}
					else {
						mGenSrc[channel].freq = (mGenSrc[channel].freq & 0x00f) | (TI4689_MSB_FREQ(mD) << 4);
						if (DBG_LEVEL(DBG_AUDIO)) {
							cout << "Set MSB of " << generator << " to 0x" << hex << TI4689_MSB_FREQ(mD);
						}
					}
					double freq = 0;
					
					if (mGenSrc[channel].freq > 0) {
						freq = 1e6 * mCLK / (32 * mGenSrc[channel].freq);
						mChannelHalfCycleSamples[channel] = (int)round(0.5 * mSampleRate / freq);
						mOutput[channel] = 1;
					}
					else {
						mChannelHalfCycleSamples[channel] = 0;		
						mOutput[channel] = 0;
					}
					if (DBG_LEVEL(DBG_AUDIO)) {
						cout << " <=> Frequency " <<
							dec << freq << " Hz (0x" <<
							hex << mGenSrc[channel].freq << ")" << " and " << mChannelHalfCycleSamples[channel] << " samples per tone 1/2 cycle " << 
							"\n";

					}

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
					if (TI4689_ATT_OFF(a)) {
						mChannelVolume[channel] = 0;
						mOutput[channel] = 0;
					}
					else
						mChannelVolume[channel] = (int) round (mChannelLevelMax * pow(10, - a / 10.0));
					if (DBG_LEVEL(DBG_AUDIO)) {
						cout << "Set Attenuation for " << generator <<  " to 0x" << hex << a << " = > Attenuation " <<
							_TI6847_ATTENUATION(a) << " => Volume " << dec << mChannelVolume[channel] << "\n";
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

				double freq = 0;
				mOutput[channel] = 1;
				switch (mGenSrc[channel].shiftRate) {
				case DIV_512:
					freq = 1e6 * mCLK / 512;
					break;
				case DIV_1024:
					freq = 1e6 * mCLK / 1024;
					break;
				case DIV_2048:
					freq = 1e6 * mCLK / 2048;;
					break;
				case GEN_3_OUT:
					break;
				default:
					mOutput[channel] = 0;
					break;
				}
				if (freq > 0)
					mNoiseShiftSamples = (int)round(mSampleRate / freq); // shift only once per low to high transition of the freq
				else
					mNoiseShiftSamples = -1;

				if (DBG_LEVEL(DBG_AUDIO)) {
					cout << "Set Noise Generator Type to " << _TI6847_NOISE_TYPE(mGenSrc[channel].noiseType) <<
						", No of noise shift samples to " << dec << mNoiseShiftSamples << 
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