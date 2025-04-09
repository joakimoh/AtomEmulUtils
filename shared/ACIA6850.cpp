#include "ACIA6850.h"

ACIA6850::ACIA6850(string name, uint16_t adr, double clock, double cpuClock, uint8_t waitStates, DebugManager  *debugManager, ConnectionManager* connectionManager) :
	MemoryMappedDevice(name, ACIA6850_DEV, PERIPHERAL, cpuClock, waitStates, adr, 0x08, debugManager, connectionManager), mClock(clock)
{	registerPort("RxD",		IN_PORT,	0x1,	RxD,	&mRxD);			// Receive Data 	registerPort("CTS",		IN_PORT,	0x1,	CTS,	&mCTS);			// Clear To Send 	registerPort("DCD",		IN_PORT,	0x1,	DCD,	&mDCD);			// Data Carrier Detect 	registerPort("RxCLK",	IN_PORT,	0x1,	RxCLK,	&mRxCLK);		// Receive Clock	registerPort("TxCLK",	IN_PORT,	0x1,	TxCLK,	&mTxCLK);		// Transmit Clock	registerPort("TxD",		OUT_PORT,	0x1,	TxD,	&mTxD);			// Transmit Data	registerPort("RTS",		OUT_PORT,	0x1,	RTS,	&mRTS);			// Request To Send
}

bool ACIA6850::read(uint16_t adr, uint8_t& data)
{
	// Call parent class to trigger scheduling of other devices when applicable
	if (!MemoryMappedDevice::triggerBeforeRead(adr, data))
		return false;

	if ((adr & 0x1) == 0) {
		// Status Register
		data = mSR;
	}
	else {
		// Receive Data Register
		data = mRDR;
	}
	


	return true;
}

bool ACIA6850::write(uint16_t adr, uint8_t data)
{

	// Call parent class to trigger scheduling of other devices when applicable
	return MemoryMappedDevice::triggerAfterWrite(adr, data);

	if ((adr & 0x1) == 0) {
		// Control Register
		mCR = data;

		// Rx & Tx clock divide
		switch (mCR & 0x3) {
		case 0x0:
			mClkDiv = 1;
			break;
		case 0x1:
			mClkDiv = 16;
			break;
		case 0x2:
			mClkDiv = 64;
			break;
		case 0x3:
			// master reset
			reset();
			return true;
		default:
			break;
		}

		// Rx & Tx data bits
		switch ((mCR >> 2) & 0x7) {
		case 0x0:
			mNDataBits = 7;
			mParity = 0;
			mStopBits = 2;
			break;
		case 0x1:
			mNDataBits = 7;
			mParity = 1;
			mStopBits = 2;
			break;
		case 0x2:
			mNDataBits = 7;
			mParity = 0;
			mStopBits = 1;
			break;
		case 0x3:
			mNDataBits = 7;
			mParity = 1;
			mStopBits = 1;
			break;
		case 0x4:
			mNDataBits = 8;
			mParity = -1;
			mStopBits = 2;
			break;
		case 0x5:
			mNDataBits = 8;
			mParity = -1;
			mStopBits = 1;
			break;
		case 0x6:
			mNDataBits = 8;
			mParity = 0;
			mStopBits = 1;
			break;
		case 0x7:
			mNDataBits = 8;
			mParity = 1;
			mStopBits = 1;
			break;
		default:
			break;
		}

		// Tx control
		switch ((mCR >> 5) & 0x3) {
		case 0x0:
			updatePort(RTS, 0);
			mEnaTxIRQ = false;
			break;
		case 0x1:
			updatePort(RTS, 0);
			mEnaTxIRQ = true;
			break;
		case 0x2:
			updatePort(RTS, 1);
			mEnaTxIRQ = false;
			break;
		case 0x3:
			updatePort(RTS, 0);
			updatePort(TxD, 0); // break level (space) on data out
			mEnaTxIRQ = false;
			break;
		default:
			break;
		}
		mEnaRxIRQ = (mCR & 0x80) != 0;
	}
	else {
		// Transmit Data Register
		mTDR = data;
	}

	return true;
}

// Reset device
bool ACIA6850::reset()
{
	Device::reset();
	return true;
}

// Advance until clock cycle stopcycle has been reached
bool ACIA6850::advance(uint64_t stopCycle)
{
	mCycleCount = stopCycle;

	return true;
}