#include "TI4689.h"
#include <cmath>

//
// Model of the TI4689 Sound Chip from the 1980's
// 


TI4689::TI4689(string name, int sampleFreq, double cpuClock, DebugManager* debugManager, ConnectionManager* connectionManager) :

	SoundDevice(name, TI4689_DEV, cpuClock, sampleFreq, debugManager, connectionManager)
{
	registerPort("CLK", IN_PORT, 0x1, CLK,	&mCLK);
	registerPort("D",	IN_PORT, 0xff, D,	&mD);
	registerPort("WE", IN_PORT, 0x1, WE, &mWE);
}

TI4689::~TI4689()
{

}

// Advance until clock cycle stopcycle has been reached
bool TI4689::advance(uint64_t stopCycle)
{
	mCycleCount = stopCycle;
	return true;
}

// Process input port changes directly (i.e. don't wait until the next call of the advance() method)
void TI4689::processPortUpdate(int index)
{
	if (index == D) {
		//cout << "Data = 0x" << hex << (int) mD << "\n";
	}

	else if (index == WE) {
		if (mWE == 0 && pWE == 1)
		{
			// Write to a device register


			int reg_no = TI4689_REG_NO(mD);
			int src = TI4689_GEN_SRC(mD);
			bool first_byte = TI4689_FIRST_BYTE(mD);

			//cout << "Write 0x" << hex << (int) mD << dec << " to Sound Chip\n";


			switch (reg_no) {

			case R0_Tone_1_Freq:
			case R2_Tone_2_Freq:
			case R4_Tone_3_Freq:
				// Tone Generator 1/2/3 Frequency
			{
				if (first_byte) {
					mGenSrc[src].freq = (mGenSrc[src].freq & 0x3f0) | TI4689_LSB_FREQ(mD);
					
					cout << "Set LSB of Tone Generator " << dec << src << " to 0x" << hex << TI4689_LSB_FREQ(mD) << " => Frequency " <<
						dec << (2.0e6 * mCLK / (32 * mGenSrc[src].freq)) << " Hz (0x" << hex << mGenSrc[src].freq << ")\n";
				}
				else {
					mGenSrc[src].freq = (mGenSrc[src].freq & 0x00f) | (TI4689_MSB_FREQ(mD) << 4);
					cout << "Set MSB of Tone Generator " << dec << src << " to 0x" << hex << TI4689_MSB_FREQ(mD) << " => Frequency " <<
						dec << (2.0e6 * mCLK / (32 * mGenSrc[src].freq)) << " Hz (0x" << hex << mGenSrc[src].freq << ")\n";
				}
				break;
			}

			case R1_Tone_1_Att:
			case R3_Tone_2_Att:
			case R5_Tone_3_Att:
			case R7_Noise_Att:
				// Tone Generator 1/2/3 Attenuation
			{
				int a = TI4689_ATTENUATION(mD);
				if (TI4689_INVALID_ATT(a))
					mGenSrc[src].att = ATT_OFF;
				else
					mGenSrc[src].att = Attenuation(a);
				if (reg_no == R7_Noise_Att)
					cout << "Set Attenuation for Noise Generator to 0x" << hex << a << " => Attenuation " <<
					_TI6847_ATTENUATION(mGenSrc[src].att) << "\n";
				else
					cout << "Set Attenuation for Tone Generator " << dec << src << " to 0x" << hex << a << " => Attenuation " <<
					_TI6847_ATTENUATION(mGenSrc[src].att) << "\n";
				break;
			}

			case R6_Noise_Control:
				// Noise Control
			{
				mGenSrc[src].noiseType = TI4689_NOISE_TYPE(mD);
				mGenSrc[src].shiftRate = TI4689_NOISE_RATE(mD);

				cout << "Set Noise Generator Type to " << _TI6847_NOISE_TYPE(mGenSrc[src].noiseType) <<
					" and noise rate to " << _TI6847_NOISE_RATE(mGenSrc[src].shiftRate) << "\n";
				break;
			}

			default:
				break;
			}
	}
		pWE = mWE;
	}

	else if (index == CLK) {

	}
}