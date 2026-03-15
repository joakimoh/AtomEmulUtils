#ifndef BEEB_SERIAL_ULA_H
#define BEEB_SERIAL_ULA_H

#include <cstdint>
#include <string>
#include "MemoryMappedDevice.h"
#include "TimedDevice.h"

using namespace std;

class ACIA6850;

class BeebSerialULA : public MemoryMappedDevice, public TimedDevice {

private:

	long mTapeStartCount = -1;
	long mTapeCount = 0;
	bool mLowToneDetected = false;
	uint8_t pCAS_IN = 0;
	bool mfirstTapeSample = true;

	// Ports
	int RxD, TxD, RTSI, CTSO, CTSI, DCD, RxCLK, TxCLK;
	int CASMO, CAS_IN, CAS_OUT, DIn, DOut, RTSO;
	PortVal mCASMO = 0;
	PortVal mCAS_IN = 0;
	PortVal mCAS_OUT = 0;
	PortVal mDIn = 1;
	PortVal mDOut = 0;
	PortVal mRTSO = 0;
	PortVal mRxD = 0x1;
	PortVal mTxD = 0x0;
	PortVal mRTSI = 0x1;
	PortVal mCTSI = 0x0;
	PortVal mCTSO = 0x0;
#define DCD_ACTIVE	0
#define DCD_INACTIVE	1

	PortVal mDCD = DCD_ACTIVE;
	PortVal mRxCLK = 1;
	PortVal mTxCLK = 1;
	PortVal mIRQ = 1;

	bool mCarrierDetected = false;
	int mLowerToneHalfCycles = 0;

	// Registers
	uint8_t mCR = 0x0;	// base address + 0

	// Control Register (CR) Fields
#define SER_ULA_CR_TxRate_MASK	(0x7 << 0)
#define SER_ULA_CR_RxRate_MASK	(0x7 << 3)
#define SER_ULA_CR_ENA_SER_MASK	(0x1 << 6)
#define SER_ULA_CR_CAS_MOT_MASK	(0x1 << 7)
#define SER_ULA_CR_TxRate		((mCR >> 0) & 0x7)
#define SER_ULA_CR_RxRate		((mCR >> 3) & 0x7)
#define SER_ULA_CR_ENA_SER		((mCR >> 6) & 0x1)
#define SER_ULA_CR_CAS_MOT		((mCR >> 7) & 0x1)

	long mRxClkRate = 4800; // with ÷ 64 by the ACIA this results in a baud rate of 75 for the serial communication
	long mTxClkRate = 4800; // with ÷ 64 by the ACIA this results in a baud rate of 75 for the serial communication

	double mTapeTime = 0;
	double mTime = 0;


	ACIA6850 *mACIA = NULL;

	int mLevelCnt = 0;
	uint8_t mLevel = 0;
	int mHighToneHalfCycles = 0;
	int mLongHalfCycles = 0;
	int mHalfCycleCnt = 0;
	int mBitDurationCnt = 0;

	// Tone synthesis
	uint8_t mTone = 0;
	int mToneCnt = 0;
	uint8_t pTxD = 1;

	int mToneHalfCycleDuration = 0;
	int mHighToneHalfCycleDuration = 0;
	int mHighToneHalfCycleDurationMin = 0;
	int mHighToneHalfCycleDurationMax = 0;
	int mLowToneHalfCycleDuration = 0;
	int mLowToneHalfCycleDurationMin = 0;
	int mLowToneHalfCycleDurationMax = 0;
	int mMinCarrierHalfCycles = 0;

	int mSameToneHalfCycles = 0; // For debugging only
	int mLastTone = -1;


	enum CassetteState {NO_CARRIER, CARRIER, START_BIT, DATA_BIT, PARITY_BIT, STOP_BIT};
	CassetteState mCassetteState = NO_CARRIER;

	bool isNewHalfCycle(int& nCpuCycles);

public:

	BeebSerialULA(string name, uint16_t adr, double tickRate, uint8_t waitStates, DebugTracing* debugTracing, ConnectionManager* connectionManager, DeviceManager* deviceManager);

	bool readByte(BusAddress adr, BusByte& data);
	bool dump(BusAddress adr, uint8_t& data) override;
	bool writeByte(BusAddress adr, BusByte data);

	// Reset device
	bool reset();

	// Device power on
	bool power() { return reset(); }

	// Advance until time tickTime
	bool advanceUntil(uint64_t tickTime) override;

	// Process clock updates to drive shifting on changes
	void processPortUpdate(int index);

	// Get pointer to the ACIA device to be able to call its methods
	bool connectDevice(Device* dev);

	// Outputs the internal state of the device
	bool outputState(ostream& sout) override;

};

#endif