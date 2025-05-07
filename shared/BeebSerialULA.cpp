#include "BeebSerialULA.h"
#include "ACIA6850.h"
#include "Device.h"
#include "Utility.h"
#include <cmath>

BeebSerialULA::BeebSerialULA(
	string name, uint16_t adr, double cpuClock, uint8_t waitStates, DebugManager* debugManager, ConnectionManager* connectionManager) :
	MemoryMappedDevice(name, BEEB_SERIAL_ULA_DEV, PERIPHERAL, cpuClock, waitStates, adr, 1, debugManager, connectionManager)
{
	registerPort("RxD",		OUT_PORT,	0x1,	RxD,		&mRxD);		// Receive Data					to ACIA
	registerPort("CTSO",	OUT_PORT,	0x1,	CTSO,		&mCTSO);	// Clear To Send				to ACIA
	registerPort("DCD",		OUT_PORT,	0x1,	DCD,		&mDCD);		// Data Carrier Detect			to ACIA
	registerPort("TxD",		IN_PORT,	0x1,	TxD,		&mTxD);		// Transmit Data				from ACIA
	registerPort("RTSI",	IN_PORT,	0x1,	RTSI,		&mRTSI);	// Request To Send				from ACIA

	registerPort("CASMO",	OUT_PORT,	0x1,	CASMO,		&mCASMO);	// Cassette Motor control		to relay

	registerPort("CAS_IN",	IN_PORT,	0x1,	CAS_IN,		&mCAS_IN);	// Cassette In					from Tape Recorder
	registerPort("CAS_OUT",	OUT_PORT,	0x1,	CAS_OUT,	&mCAS_OUT);	// Cassette Out					to Tape Recorder

	registerPort("DIn",		IN_PORT,	0x1,	DIn,		&mDIn);		// Serial In					from RS423 Interface
	registerPort("DOut",	OUT_PORT,	0x1,	DOut,		&mDOut);	// Serial Out					to RS423 Interface
	registerPort("RTSO",	OUT_PORT,	0x1,	RTSO,		&mRTSO);	// Request To Send Out			to RS423 Interface
	registerPort("CTSI",	IN_PORT,	0x1,	CTSI,		&mCTSI);	// Clear To Send				from RS423 Interface

	double tolerance = 0.3;
	double t_low = 1 - tolerance;
	double t_high = 1 + tolerance;
	mLowToneHalfCycleDuration = (int)round(cpuClock * 1e6 / 1200 / 2);
	mLowToneHalfCycleDurationMin = (int)round(t_low * cpuClock * 1e6 / 1200 / 2);
	mLowToneHalfCycleDurationMax = (int)round(t_high * cpuClock * 1e6 / 1200 / 2);
	mHighToneHalfCycleDuration = (int)round(cpuClock * 1e6 / 2400 / 2);
	mHighToneHalfCycleDurationMin = (int)round(t_low * cpuClock * 1e6 / 2400 / 2);
	mHighToneHalfCycleDurationMax = (int)round(t_high * cpuClock * 1e6 / 2400 / 2);

	if (mDM->debug(DBG_VERBOSE)) {
		cout << "mLowToneHalfCycleDuration = " << dec << mLowToneHalfCycleDuration << "\n";
		cout << "mLowToneHalfCycleDurationMin = " << dec << mLowToneHalfCycleDurationMin << "\n";
		cout << "mLowToneHalfCycleDurationMax = " << dec << mLowToneHalfCycleDurationMax << "\n";
		cout << "mHighToneHalfCycleDuration = " << dec << mHighToneHalfCycleDuration << "\n";
		cout << "mHighToneHalfCycleDurationMin = " << dec << mHighToneHalfCycleDurationMin << "\n";
		cout << "mHighToneHalfCycleDurationMax = " << dec << mHighToneHalfCycleDurationMax << "\n";
	}

	// Initialise the Tx 1/2 cycle duration to something > 0
	mToneHalfCycleDuration = mHighToneHalfCycleDuration;

	mMinCarrierHalfCycles = (int)round(0.5 * 2400 * 2);
}

bool BeebSerialULA::read(uint16_t adr, uint8_t& data)
{
	data = 0xff; // Control Register is write-only!
	return true;
}

bool  BeebSerialULA::dump(uint16_t adr, uint8_t& data)
{
	data = 0xff;
	return false;
}

/*
*
* https://beebwiki.mdfs.net/Serial_ULA with remarks added in []
* 
* The 19200 baud rate setting is not guaranteed. These are nominal values — actual rates
* are 0.16 percent higher as they are derived from a (16/13) MHz clock.
* 
* In serial mode, the MOS fixes the clock divider of the ACIA at '÷ 64'. The default value
* of the control register is &64 (motor off, serial port enabled, 9600 baud receive and transmit.
* [16/13 Mhz / 64 = 19231]
* 
* In cassette mode the MOS writes the hard-coded value &85 to the control register (motor on,
* cassette enabled, 19200 baud receive, 300 baud transmit) and sets the ACIA clock divider to
* '÷ 16' for the 1200 baud Acorn format, or '÷ 64' for the 300 baud CUTS format.
* ACIA Baud Rate	ACIA Clk Divide		ULA Tx div	ULA Tx clock	ULA Rx div	ULA Rx clock	ACIA Clk periods per
*																				(ignores DIV)	(2400 Hz) tone 1/2 cycle
* ==============	===============		==========	============	==========	============	===================================
* 300				64					64			19200			1*			19200			4
* 1200				16					64			19200			1*			19200			4
* 
* *The ULA itself ignores bits 5 to 3 and generates a receive clock of 19.2 kHz when bit 6 is clear.
* (The SERPROC also disregards bits 5 and 4 but reassigns bit 3 for another purpose, see below.)
* When saving, bits 2 to 0 as well as the ACIA clock divider govern the baud rate, as in serial mode,
* but again the MOS only adjusts the divider.
*

* 
* b5 b4	b3		ULA Divider		Rx clock to ACIA		ACIA Rx baud rate (Rx clock ÷ 64)
* b2 b1	b0		for 16/13 MHz	Tx clock to ACIA		ACIA Tx baud rate (Tx clock÷  64)
*				ULA Clock
* ========		============	==========				=================================
*  0  0	 0		1				1228.8 kHz				19200 baud
*  1  0  0		2				614.4 kHz				9600 baud
*  0  1	 0		4				307.2 kHz				4800 baud
*  1  1	 0		8				153.6 kHz				2400 baud
*  0  0	 1		16				76.8 kHz				1200 baud
*  1  0	 1		64				19.2 kHz				300 baud
*  0  1	 1		128				9.6 kHz					150 baud
*  1  1	 1		256				4.8 kHz					75 baud
* 
*/
bool BeebSerialULA::write(uint16_t adr, uint8_t data)
{
	mCR = data;
	switch (SER_ULA_CR_RxRate) {
	case 0x0:
		mRxClkRate = 1228800; // Gives 19200 with ÷ 64 made by the ACIA
		break;
	case 0x4:
		mRxClkRate = 614400; // Gives 9600 with ÷ 64 made by the ACIA
		break;
	case 0x2:
		mRxClkRate = 307200; // Gives 4800 with ÷ 64 made by the ACIA
		break;
	case 0x6:
		mRxClkRate = 153600; // Gives 2400 with ÷ 64 made by the ACIA
		break;
	case 0x1:
		mRxClkRate = 76800; // Gives 1200 with ÷ 64 made by the ACIA
		break;
	case 0x5:
		mRxClkRate = 19200; // Gives 300 with ÷ 64 made by the ACIA
		break;
	case 0x3:
		mRxClkRate = 9600; // Gives 150 with ÷ 64 made by the ACIA
		break;
	case 0x7:
		mRxClkRate = 4800; // Gives 75 with ÷ 64 made by the ACIA
		break;
	default:
		break;
	}

	switch (SER_ULA_CR_TxRate) {
	case 0x0:
		mTxClkRate = 1228800; // Gives 19200 with ÷ 64 made by the ACIA
		break;
	case 0x1:
		mTxClkRate = 614400; // Gives 9600 with ÷ 64 made by the ACIA
		break;
	case 0x2:
		mTxClkRate = 307200; // Gives 4800 with ÷ 64 made by the ACIA
		break;
	case 0x3:
		mTxClkRate = 153600; // Gives 2400 with ÷ 64 made by the ACIA
		break;
	case 0x4:
		mTxClkRate = 76800; // Gives 1200 with ÷ 64 made by the ACIA
		break;
	case 0x5:
		mTxClkRate = 19200; // Gives 300 with ÷ 64 made by the ACIA
		break;
	case 0x6:
		mTxClkRate = 9600; // Gives 150 with ÷ 64 made by the ACIA
		break;
	case 0x7:
		mTxClkRate = 4800; // Gives 75 with ÷ 64 made by the ACIA
		break;
	default:
		break;

	}

	//
	// In Cassette Mode, RTSO (to the RS423 Interface) is hard-wired as High (i.e., inactive) and CTSO (to the ACIA) as Low (i.e. active)
	// Receive clock rate is also set to 19 200 independently of CR's bits b3 to b5
	//
	if (!SER_ULA_CR_ENA_SER) {
		mRxClkRate = 19200;
		updatePort(RTSO, 1);
		updatePort(CTSO, 0);
	}
	else {
		updatePort(DCD, DCD_ACTIVE);
	}

	// "Tell" the ACIA about the updated Rx & Tx clock rates
	if (mACIA != NULL) {
		mACIA->setRxClkRate(mRxClkRate);
		mACIA->setTxClkRate(mTxClkRate);
	}



	return true;
}


// Reset device
// There is no RESET input pin to the Serial ULA so
// no function to provide here...
bool BeebSerialULA::reset()
{
	return true;
}

bool BeebSerialULA::isNewHalfCycle(int &nCpuCycles)
{
	uint8_t old_level = mLevel;
	mLevel = mCAS_IN;

	if (mCAS_IN != old_level) {
		nCpuCycles = mLevelCnt;
		mLevelCnt = 0;
		return true;
	}
	else {
		mLevelCnt++;
	}
	
	return false;
}


// Advance until clock cycle stopcycle has been reached
bool BeebSerialULA::advance(uint64_t stopCycle)
{
	if (mTapeStartCount < 0) {
		if (mCAS_IN != pCAS_IN) {
			if (!mfirstTapeSample) {
				mTapeStartCount = mCycleCount;
				if (mDM->debug(DBG_IO_PERIPHERAL))
					cout << "*** TAPE STARTS AT " << dec << mTapeStartCount << " cycles (" << mCycleCount / mCPUClock * 1e-6 << "s)\n";
				if (mACIA != NULL)
					mACIA->mDataStart = true;				
			}
			mfirstTapeSample = false;
		}		
		pCAS_IN = mCAS_IN;
	}
	
	
	while (mCycleCount < stopCycle) {

		if (!SER_ULA_CR_ENA_SER) { // Cassette Interface


			mTapeCount = mCycleCount - mTapeStartCount;
			double tape_time = mTapeCount / mCPUClock * 1e-6;
			double time = mCycleCount / mCPUClock * 1e-6;


			// Check for a complete 1/2 Cycle
			int half_cycle_cnt;
			if (isNewHalfCycle(half_cycle_cnt)) {

				//
				// Check for cassette input
				// 

				bool high_tone = (half_cycle_cnt >= mHighToneHalfCycleDurationMin && half_cycle_cnt <= mHighToneHalfCycleDurationMax);
				bool low_tone = (half_cycle_cnt >= mLowToneHalfCycleDurationMin && half_cycle_cnt <= mLowToneHalfCycleDurationMax);


				//  If there is a carrier, detect whether the 1/2 cycle was part of a '0' (1200 Hz) or a '1' (2400 Hz) set of 1/2 cycles
				if (mCarrierDetected) {

					if (!mLowToneDetected && low_tone)
						mLowToneDetected = true;

					mSameToneHalfCycles++;
					if (high_tone) {
						if (mLastTone != 1) {
							if (mDM->debug(DBG_IO_PERIPHERAL))
								cout << "\n#" << dec << mSameToneHalfCycles << "x'" << dec << mLastTone << "' at " << tape_time << "s(" << time << "s)#\n";
							mSameToneHalfCycles = 0;
							updatePort(RxD, 1);
							mLastTone = 1;
						}						
						
					}
					else if (low_tone) {
						if (mLastTone != 0) {
							if (mDM->debug(DBG_IO_PERIPHERAL))
								cout << "\n#" << dec << mSameToneHalfCycles << "x'" << dec << mLastTone << "' at " << tape_time << "s (" << time << "s)#\n";
							mSameToneHalfCycles = 0;
							updatePort(RxD, 0);
							mLastTone = 0;					}
						
					}
					else { // neither a high tone (a 2400 1/2 cycle) nor a low tone (a 1200 1/2 cycle)
						if (mLastTone != -1) {
							if (mDM->debug(DBG_IO_PERIPHERAL))
								cout << "#" << dec << mSameToneHalfCycles << "x'" << dec << mLastTone << "' at " << tape_time << "s (" << time << "s)#\n";
							mSameToneHalfCycles = 0;
							updatePort(RxD, 1);
							mLastTone = -1;
						}
						
					}

					/*
					if (high_tone && mLowToneDetected)
						cout << dec << half_cycle_cnt << " samples High tone 1/2 cycle at " << tape_time << "s (" << time << "s)\n";
					else if (low_tone)
						cout << dec << half_cycle_cnt << " samples Low tone 1/2 cycle at " << tape_time << "s (" << time << "s)\n";
					else if (mLowToneDetected)
						cout << "*** " << dec << half_cycle_cnt << " samples Undefined tone 1/2 cycle at " << tape_time << "s (" << time << "s)\n";
					*/
				}
				else if (mRxD != 1)
					updatePort(RxD, 1);

				// Check for carrier of at least 0.5s; assert Data Carrier Detect (DCD) to the ACIA when detected
				if (high_tone) {
					mHighToneHalfCycles++;
					mLowerToneHalfCycles = 0;
				}
				else {
					mLowerToneHalfCycles++;
					// Deactivate DCD if a complete lower tone cycle was received (normally this is due to a start bit)
					if (mLowerToneHalfCycles >= 2) {
						//if (mDCD == DCD_ACTIVE)
						//	cout << "DCD -> HIGH at " << tape_time << "s\n";
						updatePort(DCD, DCD_INACTIVE);					
						mHighToneHalfCycles = 0;
					}
				}
				if (mHighToneHalfCycles > mMinCarrierHalfCycles) {
					if (mDCD == DCD_INACTIVE) {
						//cout << "\n" << dec << mHighToneHalfCycles << " 1/2 cycles (" << ((double) mHighToneHalfCycles / 2400 / 2) << "s) of Carrier detected at " <<
						//	(mCycleCount / mCPUClock * 1e-6) << "s\n";
						//cout << "DCD -> LOW at " << tape_time << "s\n";
						updatePort(DCD, DCD_ACTIVE);
						mSameToneHalfCycles = 0;
						mCarrierDetected = true;
					}
				}

				

			}

			//
			// Generate cassette output when Request To Send (RTS) is active (i.e., Low)
			//

			if (mRTSI == 0) {

				//
				// Change tone frequency when Tx data from the ACIA changes
				//
				if (mTxD != pTxD) {

					// Terminate ongoing 1/2 cycle if it is longer than half that of a 1/2 cycle (of the current type, i.e. either low or high 1/2 cycle)
					mBitDurationCnt++;
					mToneCnt++;
					if (mToneCnt > mToneHalfCycleDuration / 2) {
							updatePort(CAS_OUT, 1 - mCAS_OUT);
							mHalfCycleCnt++;
						}

					//cout << "Serial ULA starts " << (mTxD == 0 ? "Low Tone" : "High Tone") << " after " << dec << mHalfCycleCnt << " 1/2 cycles of different tone\n";
					//cout << "Same level duration was " << dec << mBitDurationCnt << " cycles\n";
					if (mTxD == 0)
						mToneHalfCycleDuration = mLowToneHalfCycleDuration;
					else
						mToneHalfCycleDuration = mHighToneHalfCycleDuration;
					mToneCnt = 0;
					mHalfCycleCnt = 0;
					mBitDurationCnt = 0;
				}
				else {
					mBitDurationCnt++;
					mToneCnt++;
					if (mToneCnt == mToneHalfCycleDuration) {
						//cout << (mToneHalfCycleDuration == mHighToneHalfCycleDuration ? "H" : "L");
						//cout << dec << mToneCnt << " ";
						updatePort(CAS_OUT, 1 - mCAS_OUT);
						mToneCnt = 0;
						mHalfCycleCnt++;
					}
				}
				pTxD = mTxD;

			}

			else {
				// Keep CAS OUT low when RTS is high as an emulation of 'zero output voltage'
				updatePort(CAS_OUT, 0);
			}


			// Consider a too long same level input as loss of carrier
			if (mLevelCnt > mLowToneHalfCycleDurationMax && mDCD == DCD_ACTIVE) {			
				if (mDCD == DCD_ACTIVE) {
					//cout << "Carrier lost  at " << (mCycleCount / mCPUClock * 1e-6) << "s\n";
					//cout << "DCD -> HIGH at " << (mCycleCount / mCPUClock * 1e-6) << "s\n";
				}				
				updatePort(DCD, DCD_INACTIVE);
				mHighToneHalfCycles = 0;
				mCarrierDetected = false;
			}
		}



		mCycleCount++;
	}

	

	return true;
}


// Process clock updates to drive shifting on changes
void BeebSerialULA::processPortUpdate(int index)
{
	if (SER_ULA_CR_ENA_SER) { // Serial Communication
		if (index == DIn) {
			updatePort(RxD, mDIn); // forward the RS423 Rx data to the ACIA
		}
		else if (index == TxD) {
			updatePort(DOut, mTxD);	// forward the ACIA Tx data to the RS423 Interface
		}
		else if (index == RTSI) {
			updatePort(RTSO, mRTSI);// forward the ACIA RTS to the RS423 Interface
		}
		else if (index == CTSI) {
			updatePort(CTSO, mCTSI);// forward the RS423 CTS to the ACIA
		}
	}

	else {
		//if (index == RTSI)
		//	cout << "Serial ULA receives RTS update to 0x" << hex << (int)mRTSI << "\n";
	}


}

// Get pointer to other device to be able to call its methods
bool BeebSerialULA::connectDevice(Device* dev)
{
	Device::connectDevice(dev);

	if (dev != NULL && dev->devType == ACIA6850_DEV) {
		mACIA = (ACIA6850 *) dev;
		//cout << "ACIA6850 connected!\n";
	}

	return true;
}
