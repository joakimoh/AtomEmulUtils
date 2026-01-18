#ifndef ACIA_6850_H
#define ACIA_6850_H

#include <cstdint>
#include <string>
#include "MemoryMappedDevice.h"
#include "ClockedDevice.h"

using namespace std;


class ACIA6850 : public MemoryMappedDevice, ClockedDevice {

public:
	bool mDataStart = false; // for debugging only
private:

	bool mPowerOn = true;

	// For debugging only
	long mDataStartCount = -1;
	long mDataCount = 0;

	// Ports
	int RxD, TxD, RTS, CTS, DCD, IRQ;
	uint8_t mRxD = 0x1;
	uint8_t mTxD = 0x1;
	uint8_t mRTS = 0x1;
	uint8_t mCTS = 0x1;
	uint8_t mDCD = 0x1;
	uint8_t mIRQ = 1;
	uint8_t pDCD = 1;
	uint8_t pCTS = 1;

	// Registers
	uint8_t mCR = 0x0;	// base address + 0 - Write-only
	uint8_t mSR = 0x0;	// base address + 0 - Read-only
	uint8_t mTDR = 0x0;	// base address + 1 - Write-only
	uint8_t mRDR = 0x0;	// base address + 1 - Read-only

	uint8_t pCR = mCR;

	uint8_t mTDRBuf = 0x0;


	// Control Register (CR) Fields
#define ACIA_CR_DIV_MASK	(0x3 << 0)
#define ACIA_CR_WORD_MASK	(0x7 << 2)
#define ACIA_CR_TX_MASK		(0x3 << 5)
#define ACIA_CR_RIE_MASK	(0x1 << 7)
#define ACIA_CR_DIV			((mCR >> 0) & 0x3)
#define ACIA_CR_WORD		((mCR >> 2) & 0x7)
#define ACIA_CR_Tx			((mCR >> 5) & 0x3)
#define ACIA_CR_RIE			((mCR >> 7) & 0x1)
#define ACIA_CR_TIE			(((mCR >> 5) & 0x3) == 0x01)

	// Status Register (SR) Fields
#define ACIA_SR_RDRF_MASK	(0x1 << 0)
#define ACIA_SR_TDRE_MASK	(0x1 << 1)
#define ACIA_SR_DCD_MASK	(0x1 << 2)
#define ACIA_SR_CTS_MASK	(0x1 << 3)
#define ACIA_SR_FE_MASK		(0x1 << 4)
#define ACIA_SR_OVRN_MASK	(0x1 << 5)
#define ACIA_SR_PE_MASK		(0x1 << 6)
#define ACIA_SR_IRQ_MASK	(0x1 << 7)
#define ACIA_SR_RDRF		((mSR >> 0) & 0x1)
#define ACIA_SR_TDRE		((mSR >> 1) & 0x1)
#define ACIA_SR_DCD			((mSR >> 2) & 0x1)
#define ACIA_SR_CTS			((mSR >> 3) & 0x1)
#define ACIA_SR_FE			((mSR >> 4) & 0x1)
#define ACIA_SR_OVRN		((mSR >> 5) & 0x1)
#define ACIA_SR_PE			((mSR >> 6) & 0x1)
#define ACIA_SR_IRQ			((mSR >> 7) & 0x1)

	uint8_t mClkDiv = 1;	// Rx/Tx clock divide (from mCR b1b0)
	uint8_t mNDataBits = 7;		// no of data bits (from mCR b4b3b2)
	int8_t mParity = -1;	// parity: -1 <=> none, 0 <=> even, 1 <=> odd (from mCR b4b3b2)
	int8_t mStopBits = 1;	// no of stop bits (from mCR b4b3b2)

	int mTxCLKCnt = 0;

	uint16_t mOutReg = 0; // 12-bit output shift register (1 start bit, 7-8 data bits, 0/1 parity bit, 1-2 stop bits
	uint16_t mInReg = 0; // 12-bit output shift register (1 start bit, 7-8 data bits, 0/1 parity bit, 1-2 stop bits
	int mOutBits = 0; // No of bits to shift out
	int mInBits = 0; // No of bits to shift in
	enum State { START_BIT, DATA_BIT, PARITY_BIT, STOP_BIT, NO_BIT};
	#define _ACIA_STATE(x) (x==START_BIT?"Start Bit":(x==DATA_BIT?"Data bit":(x==PARITY_BIT?"Parity Bit":(x==STOP_BIT?"Stop Bit":"No Bit"))))
	State mRxState = NO_BIT;
	State mTxState = NO_BIT;
	int mRxLowSamples = 0;
	int mRxBits = 0;
	int mTxBits = 0;
	uint8_t mRxPar = 0;
	uint8_t mTxPar = 0;
	int mSentStopBits = 0;

	bool mTxPending = false;

	bool mPendingRxIRQClr = false;

	uint8_t mRxBuffer = 0;

	long mRxClkRate = 1;
	long mTxClkRate = 1;
	long pRxClkRate = mRxClkRate;
	long pTxClkRate = mTxClkRate;

	int mRxDivClkRate = 1;
	int mTxDivClkRate = 1;

	int mRxClkCycles = 1;
	int mTxClkCycles = 1;

	int mRxDivCycles = 1;
	int mTxDivCycles = 1;

	uint64_t mRxMidSampleClkCycle = 0;

	void update_settings();
	void updateIRQ();


public:

	ACIA6850(string name, uint16_t adr, double clock, double cpuClock, uint8_t waitStates, DebugManager  *debugManager,
		ConnectionManager* connectionManager, DeviceManager *deviceManager);

	bool read(uint16_t adr, uint8_t& data);
	bool dump(uint16_t adr, uint8_t& data) override;
	bool write(uint16_t adr, uint8_t data);

	// Reset device
	bool reset();

	// Device power on
	bool power() { return reset(); }

	// Advance until clock cycle stopcycle has been reached
	bool advanceUntil(uint64_t stopCycle);

	// Process clock updates to drive shifting on changes
	void processPortUpdate(int index);

	void setRxClkRate(long clkRate);
	void setTxClkRate(long clkRate);

	// Outputs the internal state of the device
	bool outputState(ostream& sout) override;

};

#endif