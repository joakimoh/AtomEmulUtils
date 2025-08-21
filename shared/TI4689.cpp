#include "TI4689.h"
#include <cmath>
#include "Utility.h"

//
// Model of the TI4689 Sound Chip from the 1980's
// 


TI4689::TI4689(string name, double cpuClock, int sampleFreq, double emulationRate, double subEmulationRate, double speed,
	DebugManager* debugManager, ConnectionManager* connectionManager) :

	SoundDevice(name, TI4689_DEV, cpuClock, sampleFreq, emulationRate, subEmulationRate, speed, debugManager, connectionManager)
{
	registerPort("CLK", IN_PORT, 0x1, CLK,	&mCLK);
	registerPort("D",	IN_PORT, 0xff, D,	&mD);
	registerPort("WE", IN_PORT, 0x1, WE, &mWE);

	// Create a default allegro Mixer and connect it to an allegro Voice
	al_reserve_samples(0);

	// Get a handle to the previously created Mixer
	mMixer = al_get_default_mixer();
	
	setEmulationRate(speed);

}

void TI4689::setEmulationRate(double speed)
{
	SoundDevice::setEmulationRate(speed);

	mSamplesPerFragment = (int)round(0.5 * mSampleRate / mLowEmulationRate); // one emulation base cycle of audio
	mCpuCyclesPerSample = (int)round(1e6 * mCPUClock / mBaseSampleRate);
	mNFragments = 4;
	

	if (DBG_LEVEL_DEV(this,DBG_VERBOSE)) {
		DBG_LOG(this, DBG_VERBOSE, "CPU Clock:                    " + to_string(mCPUClock) + " MHz");
		DBG_LOG(this, DBG_VERBOSE, "Emulation rate:               " + to_string(mLowEmulationRate));
		DBG_LOG(this, DBG_VERBOSE, "Sample rate:                  " + to_string(mSampleRate));
		DBG_LOG(this, DBG_VERBOSE, "Samples per fragment:         " + to_string(mSamplesPerFragment));
		DBG_LOG(this, DBG_VERBOSE, "CPU cycles per sample:        " + to_string(mCpuCyclesPerSample));
		DBG_LOG(this, DBG_VERBOSE, "No of audio stream fragments: " + to_string(mNFragments));
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
			mSampleRate,				// sample frequency - compensated for emulation speed
			ALLEGRO_AUDIO_DEPTH_INT16,		// int16_t samples
			ALLEGRO_CHANNEL_CONF_2			// Stereo => pair of int16_t samples
		);
		if (!mChannelStream[channel]) {
			DBG_LOG(this, DBG_ERROR, "Could not create audio stream " + to_string(channel));
			throw runtime_error("Could not create audio stream");

		}

		// Fill the channel with silence that lasts eight emulation periods (mNFragments / mLowEmulationRate)
		void* buf;
		while ((buf = al_get_audio_stream_fragment(mChannelStream[channel]))) {
			al_fill_silence(buf, mSamplesPerFragment, ALLEGRO_AUDIO_DEPTH_INT16, ALLEGRO_CHANNEL_CONF_2);
			al_set_audio_stream_fragment(mChannelStream[channel], buf);
		}

		// Connect the channel to the default mixer
		if (!al_attach_audio_stream_to_mixer(mChannelStream[channel], mMixer)) {
			DBG_LOG(this, DBG_ERROR, "Could not attach audio stream " + to_string(channel) + " to audio mixer");
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
bool TI4689::advanceUntil(uint64_t stopCycle)
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
					// A Square Wave Tone generator
					//

					if (mSampleCount % mChannelHalfCycleSamples[channel] == 0) {
						mOutput[channel] = 1 - mOutput[channel];
						DBG_LOG_COND(
							mChannelVolume[channel] > 0, this, DBG_AUDIO,
							to_string(1e6 * mCPUClock / (2 * (mCycleCount - mHalfCycleCount[channel]))) +
							" Hz (" + to_string(mCycleCount - mHalfCycleCount[channel]) + " 1/2 CPU cycles) for 1/2 cycle samples of " +
								to_string(mChannelHalfCycleSamples[channel]) + " for channel " + to_string(channel)
						);
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

						if (DBG_LEVEL_DEV(this,DBG_AUDIO) && mChannelVolume[channel] > 0 && mChannelHalfCycleSamples[channel] > 0) {
							double freq = 0.5 * mSampleRate / mChannelHalfCycleSamples[channel];
							DBG_LOG(
								this, DBG_AUDIO, to_string(mSamplesPerFragment / mSamplesSize[channel]) + " samples" +
								" (" + to_string((double)mSamplesPerFragment / (mSampleRate / freq)) + " cycles)" +
								" of frequency " + to_string(freq) +
								" and max volume " + to_string(mChannelVolume[channel]) +
								" generated for " + _TI4689_GENERATOR(channel)
							);
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

// Process input port changes directly (i.e. don't wait until the next call of the advanceUntil() method)
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

			switch (reg_addr) {

			case R0_Tone_1_Freq:
			case R2_Tone_2_Freq:
			case R4_Tone_3_Freq:
				// Tone Generator 1/2/3 Frequency
			{
				if (channel < 3) {
					if (first_byte) {
						mGenSrc[channel].freq = (mGenSrc[channel].freq & 0x3f0) | TI4689_LSB_FREQ(mD);
						DBG_LOG(this, DBG_AUDIO, "Set LSB of " + generator +  " to 0x" + Utility::int2HexStr(TI4689_LSB_FREQ(mD),1));
					}
					else {
						mGenSrc[channel].freq = (mGenSrc[channel].freq & 0x00f) | (TI4689_MSB_FREQ(mD) << 4);
						DBG_LOG(this, DBG_AUDIO, "Set MSB of " + generator+ " to 0x" + Utility::int2HexStr(TI4689_MSB_FREQ(mD),1));
					}
					double freq = 0;

					if (mGenSrc[channel].freq > 0) {
						freq = 1e6 * mCLK / (32 * mGenSrc[channel].freq);
						mChannelHalfCycleSamples[channel] = (int)round(0.5 * mBaseSampleRate / freq);
						mOutput[channel] = 1;
					}
					else {
						mChannelHalfCycleSamples[channel] = 0;
						mOutput[channel] = 0;
					}
					DBG_LOG(
						this, DBG_AUDIO, " <=> Frequency " + to_string(freq) + " Hz (0x" +
						Utility::int2HexStr(mGenSrc[channel].freq,4) + ")" + " and " +
						to_string(mChannelHalfCycleSamples[channel]) + " samples per tone 1/2 cycle "
					);

				}
				else {
					DBG_LOG(this, DBG_ERROR, "Invalid channel " + to_string(channel) + "!");
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
						mChannelVolume[channel] = (int)round(mChannelLevelMax * pow(10, -a / 10.0));
					DBG_LOG(
						this, DBG_AUDIO,
						"Set Attenuation for "s + generator + " to 0x" + Utility::int2HexStr(a,2) + " = > Attenuation " +
						_TI6847_ATTENUATION(a) + " => Volume " + to_string(mChannelVolume[channel])
					);
				}
				else {
					DBG_LOG(this, DBG_ERROR, "Invalid channel " + to_string(channel) + "!");
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
					mNoiseShiftSamples = (int)round(mBaseSampleRate / freq); // shift only once per low to high transition of the freq
				else
					mNoiseShiftSamples = -1;

				DBG_LOG(
					this, DBG_AUDIO,
					"Set Noise Generator Type to "s +_TI6847_NOISE_TYPE(mGenSrc[channel].noiseType) +
						", No of noise shift samples to " + to_string(mNoiseShiftSamples) +
						" and noise rate to " + _TI6847_NOISE_RATE(mGenSrc[channel].shiftRate)
				);

				break;
			}

			default:
				break;
			}
		}
		pWE = mWE;
	}

}

string TI4689::noiseChannelFrequency2Str()
{
	stringstream ss;

	switch (mGenSrc[TI4689_NOISE_GENERATOR].shiftRate) {
	case DIV_512:
		ss << 1e6 * mCLK / 512;
		break;
	case DIV_1024:
		ss << 1e6 * mCLK / 1024;
		break;
	case DIV_2048:
		ss << 1e6 * mCLK / 2048;;
		break;
	default:
		ss << "Given by Tone Generator 3";
		break;

	}

	return ss.str();
}

// Outputs the internal state of the device
bool TI4689::outputState(ostream& sout)
{

	for (int i = 0; i < 4; i++) {
		
		int a = mGenSrc[i].att;
		if (TI4689_ATT_OFF(a)) {
			sout << "Channel " << i << " Attenuation = 0x" << Utility::int2HexStr(a,2) << "   <=> OFF" << "\n";
		}
		else
			sout << "Channel " << i << " Attenuation = 0x" << Utility::int2HexStr(a,2) << "   <=> -" << a << " dB \n";

		if (i == TI4689_NOISE_GENERATOR) {
			sout << "Channel " << i << " Noise Type  = 0x" << Utility::int2HexStr(mGenSrc[i].noiseType,2) << "   <=> " << (mGenSrc[i].noiseType == WHITE_NOISE ? "White Noise" : "Periodic Noise") << "\n";
			sout << "Channel " << i << " Shift Rate  = 0x" << Utility::int2HexStr(mGenSrc[i].shiftRate,2) << "   <=> " << noiseChannelFrequency2Str() << " Hz\n";
		}
		else {
			int f = mGenSrc[i].freq;
			sout << "Channel " << i << " Frequency   = 0x" << Utility::int2HexStr(f,4) << " <=> " << 1e6 * mCLK / (32 * f) << " Hz\n";

		}
	}

	return true;
}