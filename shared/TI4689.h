#ifndef TI4689_H
#define TI4689_H

#include "SoundDevice.h"
#include <fstream>
#include <iostream>
#include "CSWCodec.h"
#include "allegro5/allegro_audio.h"
#include <cmath>

class TI4689 : public SoundDevice {

private:

	ALLEGRO_VOICE* mVoice = NULL;
	ALLEGRO_MIXER* mMixer = NULL;
	ALLEGRO_AUDIO_STREAM* mAudioStream = NULL;
	ALLEGRO_AUDIO_STREAM* mChannel1Stream = NULL;
	ALLEGRO_AUDIO_STREAM* mChannel2Stream = NULL;
	ALLEGRO_AUDIO_STREAM* mChannel3Stream = NULL;
	ALLEGRO_AUDIO_STREAM* mNoiseChannelStream = NULL;

	uint8_t mCLK = 4;
	uint8_t mD = 0x0;	// Data in
	uint8_t mWE = 0x1;	// Write Enable (active low)
	uint8_t pWE = 0x0;
	int CLK, D, WE;

	enum Attenuation {
		ATT_2dB = 0x1,
		ATT_4dB = 0x2,
		ATT_8dB = 0x4,
		ATT_16dB = 0x8,
		ATT_OFF = 0xf
	};
#define TI4689_INVALID_ATT(x) (x != ATT_2dB && x != ATT_4dB && x != ATT_8dB && x != ATT_16dB && x != ATT_OFF)
#define _TI6847_ATTENUATION(x) (x==ATT_2dB?"2dB":(x==ATT_4dB?"4dB":(x==ATT_8dB?"8dB":(x==ATT_16dB?"16dB":"Off"))))

	enum NoiseType { PERIODIC_NOISE = 0, WHITE_NOISE = 1, NO_NOISE = 3};
#define _TI6847_NOISE_TYPE(x) (x==PERIODIC_NOISE?"Periodic":(x==WHITE_NOISE?"White noise":"Undefined"))
	enum NoiseRate { DIV_512 = 0, DIV_1024 = 1, DIV_2048 = 2, GEN_3_OUT = 3, NO_RATE = 7};
#define _TI6847_NOISE_RATE(x) (x==DIV_512?"N/512":(x==DIV_1024?"N/1024":(x==DIV_2048?"N/2048":(x==GEN_3_OUT?"Generator 3 Out":"Off"))))

	typedef struct SndGenSrc_struct {
		uint16_t		freq = 0;				// 10-bit frequency - from bits b3:b0 (LSB) of first data byte written to register and b7:b0 (MSB) of second data byte
		NoiseType		noiseType = NO_NOISE;	// Only used for the noise generator source: Noise type - from bit b2 of data byte written to register
		NoiseRate		shiftRate = NO_RATE;	//  Only used for for the noise generator source: Shift rate for noise - from bits b1:b0 of data byte written to register
		Attenuation		att;					// 4-bit attenuation - from bits b3:b0 of data byte written to register
	} SndGenSrc;

	enum GenSrc { GEN_1 = 0, GEN_2 = 1, GEN_3 = 2, NOISE_GEN = 3};
	SndGenSrc mGenSrc[4] = { 0 };

	//
	// Registers
	// 
	// The register no is given by bits b6b5b4 of the byte written to the device
	//
	enum RegEnum {
		R0_Tone_1_Freq = 0x0,	// Tone Generator 1 Frequency
		R1_Tone_1_Att = 0x1,	// Tone Generator 1 Attenuation
		R2_Tone_2_Freq = 0x2,	// Tone Generator 2 Frequency
		R3_Tone_2_Att = 0x3,	// Tone Generator 2 Attenuation
		R4_Tone_3_Freq = 0x4,	// Tone Generator 3 Frequency
		R5_Tone_3_Att = 0x5,	// Tone Generator 3 Attenuation
		R6_Noise_Control = 0x6,	// Noise Control
		R7_Noise_Att = 0x7		// Noise Attenuation
	};

#define TI4689_GEN_SRC(x)		((x >> 5) & 0x3)
#define TI4689_REG_NO(x)		((x >> 4) & 0x7)
#define TI4689_FIRST_BYTE(x)	((x & 0x80) != 0)
#define TI4689_SECOND_BYTE(x)	((x & 0x80) == 0)
#define TI4689_LSB_FREQ(x)		(x & 0xf)
#define TI4689_MSB_FREQ(x)		(x & 0x3f)
#define TI4689_ATTENUATION(x)	(x & 0x7)
#define TI4689_NOISE_RATE(x)	NoiseRate(x & 0x3)
#define TI4689_NOISE_TYPE(x)	NoiseType((x >> 2) & 0x1)

public:

	TI4689(string name, int sampleFreq, double cpuClock, DebugManager* debugManager, ConnectionManager* connectionManager);
	~TI4689();

	// Advance until clock cycle stopcycle has been reached
	bool advance(uint64_t stopCycle);

	// Process input port changes directly (i.e. don't wait until the next call of the advance() method)
	void processPortUpdate(int index);
};





#endif