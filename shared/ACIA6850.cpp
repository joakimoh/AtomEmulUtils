#include "ACIA6850.h"

ACIA6850::ACIA6850(string name, uint16_t adr, double clock, double cpuClock, uint8_t waitStates, DebugManager  *debugManager, ConnectionManager* connectionManager) :
	MemoryMappedDevice(name, ACIA6850_DEV, PERIPHERAL, cpuClock, waitStates, adr, 0x08, debugManager, connectionManager), mClock(clock)
{	registerPort("RxD",		IN_PORT,	0x1,	RxD,	&mRxD);			// Receive Data 	registerPort("CTS",		IN_PORT,	0x1,	CTS,	&mCTS);			// Clear To Send 	registerPort("DCD",		IN_PORT,	0x1,	DCD,	&mDCD);			// Data Carrier Detect	registerPort("TxD",		OUT_PORT,	0x1,	TxD,	&mTxD);			// Transmit Data	registerPort("RTS",		OUT_PORT,	0x1,	RTS,	&mRTS);			// Request To Send	registerPort("IRQ",		OUT_PORT,	0x1,	IRQ,	&mIRQ);			// Interrupt Request
}

bool ACIA6850::read(uint16_t adr, uint8_t& data)
{
	// Call parent class to trigger scheduling of other devices when applicable
	if (!MemoryMappedDevice::triggerBeforeRead(adr, data))
		return false;

	if ((adr & 0x1) == 0) {
		// Status Register
		data = mSR;
		mSR &= ~ACIA_SR_TDRE_MASK; // Clear the Transmit Data Register Empty bit
		if (ACIA_SR_DCD && mDCDClrCnt == 0)
			mDCDClrCnt++; // Prepare for clearing the Data Carrier Detect bit
	}
	else {
		// Receive Data Register
		data = mRDR;
		mSR &= ~ACIA_SR_RDRF_MASK; // Clear the Receive Data Register Full bit
		if (ACIA_SR_DCD && mDCDClrCnt == 1) {
			mDCDClrCnt = 0;
			mSR &= ~ACIA_SR_DCD_MASK; // Clear the Data Carrier Detect bit if the SR was first read
			mDCDClrCnt++;
		}
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
			mCR = 0;
			mSR &= mCTS; // Clear all bits except the CTS bit that shall still reflect the state of the CTS input
			if (mPowerOn) {
				updatePort(RTS, 1);
				mPowerOn = false;
			}
			updatePort(IRQ, 1);
			return true;
		default:
			break;
		}
		mRxDivCycles = mRxClkRate * mClkDiv;
		mTxDivCycles = mTxClkRate * mClkDiv;

		// Rx & Tx data bits
		switch ((mCR >> 2) & 0x7) {
		case 0x0:
			mNDataBits = 7;
			mParity = 0;
			mStopBits = 2;
			mInBits = mOutBits = 11;
			break;
		case 0x1:
			mNDataBits = 7;
			mParity = 1;
			mStopBits = 2;
			mInBits = mOutBits = 11;
			break;
		case 0x2:
			mNDataBits = 7;
			mParity = 0;
			mStopBits = 1;
			mInBits = mOutBits = 10;
			break;
		case 0x3:
			mNDataBits = 7;
			mParity = 1;
			mStopBits = 1;
			mInBits = mOutBits = 10;
			break;
		case 0x4:
			mNDataBits = 8;
			mParity = -1;
			mStopBits = 2;
			mInBits = mOutBits = 11;
			break;
		case 0x5:
			mNDataBits = 8;
			mParity = -1;
			mStopBits = 1;
			mInBits = mOutBits = 10;
			break;
		case 0x6:
			mNDataBits = 8;
			mParity = 0;
			mStopBits = 1;
			mInBits = mOutBits = 11;
			break;
		case 0x7:
			mNDataBits = 8;
			mParity = 1;
			mStopBits = 1;
			mInBits = mOutBits = 11;
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
		mSR &= ~ACIA_SR_TDRE_MASK; // Clear the Transmit Data Register Empty bit
		if (mTxState != NO_BIT)
			mTxPending = true;
		else
			mTxState = START_BIT;
	}

	return true;
}

// Reset device
// The ACIA doesn't have a reset pin but can be reset by writing to the control register
bool ACIA6850::reset()
{
	Device::reset();
	return true;
}

// Advance until clock cycle stopcycle has been reached
bool ACIA6850::advance(uint64_t stopCycle)
{

	while (mCycleCount < stopCycle) {

		// Special treatment of start bit sampling as it needs to be made
		// with higher frequency (the RxClk frequency)
		if (mCycleCount % mRxClkCycles == 0) { // External RxClk transition
			mRxCLKCnt++;
			if (mRxState == START_BIT) {
				if (mRxD == 0)
					mRxLowSamples++;
				else
					mRxLowSamples = 0;
				if (mRxLowSamples >= mClkDiv / 2 && mCycleCount % mRxDivCycles == mRxDivCycles / 2) {
					mRxState = DATA_BIT;
					mRxBits = 0;
					mRxBuffer = 0;
					mSR &= ~ACIA_SR_PE_MASK; // Reset Framing Error
					mSR &= ~ACIA_SR_PE_MASK; // Reset parity Error
					if (mParity == 0)
						mRxPar = 0;
					else
						mRxPar = 1;
				}
			}
		}


		if (mCycleCount % mRxDivCycles == mRxDivCycles) { // Internal RxClk transition
			switch (mRxState) {
			case DATA_BIT:
			{
				mRxBuffer = ((mRxBuffer << 1) | mRxD) & 0xff;
				mRxPar ^= mRxD;
				mRxBits++;
				if (mRxBits == mNDataBits) {
					if (mParity == -1)
						mRxState = STOP_BIT;
					else
						mRxState = PARITY_BIT;
				}
				break;
			}
			case PARITY_BIT:
			{
				mRxPar ^= mRxD;
				if (mRxD != mRxPar)
					mSR |= ACIA_SR_PE_MASK; // Parity Error
				mRxState = STOP_BIT;
				break;
			}
			case STOP_BIT:
			{
				if (mRxD != 1)
					mSR |= ACIA_SR_PE_MASK; // Set Framing Error bit
				if (!ACIA_SR_RDRF) // Only transfer received data if the RDR is not already Full
					mRDR = mRxBuffer;
				mSR |= ACIA_SR_RDRF_MASK; // Set Receive Data Register Full bit
				mRxState = NO_BIT;
				break;
			}
			default: // NO_BIT
				break;
			}
		}
		if (mCycleCount % mTxDivCycles == mTxDivCycles) { // Internal TxClk transition
			if (mTxPending && mTxState == NO_BIT) {
				mTxState = START_BIT;
				mTxPending = false;
			}
			switch (mTxState) {
			case START_BIT:
			{
				mTxD = 0;
				mTxState = DATA_BIT;
				mTxBits = 0;
				if (mParity == 0)
					mTxPar = 0;
				else
					mTxPar = 1;
				break;
			}
			case DATA_BIT:
			{
				mTxD = mTDR & 0x1;
				mTDR = (mTDR >> 1) & 0x7f;
				mTxPar ^= mTxD;
				mTxBits++;
				if (mTxBits == mNDataBits) {
					if (mParity == -1)
						mTxState = STOP_BIT;
					else
						mTxState = PARITY_BIT;
				}
				break;
			}
			case PARITY_BIT:
			{
				mTxD = mTxPar;
				mTxState = STOP_BIT;
				break;
			}
			case STOP_BIT:
			{
				mTxD = 1;
				mSentStopBits++;
				if (mSentStopBits == mStopBits) {
					mTxState = NO_BIT;
					if (!ACIA_SR_CTS)
						mSR |= ACIA_SR_TDRE_MASK; // Set the Transmit Data Register Empty bit (if not inhibited by the CTS)
				}
				break;
			}
			default: // NO_BIT
				break;
			}

		}

		mCycleCount++;
	}
	return true;
}

// Process clock updates to drive shifting on changes
void ACIA6850::processPortUpdate(int index)
{
	
	if (index = DCD) {
		if (mDCD != pDCD) {
			if (mDCD == 1) {
				mSR |= ACIA_SR_DCD_MASK; // Set Data Carrier Detect bit (cleared by reading of SR + RDR or master RESET)
				mDCDClrCnt = 0;
				if (ACIA_CR_RIE) {
					updatePort(IRQ, 0);
					mSR |= ACIA_SR_IRQ_MASK;
				}
			}
		}
		pDCD = mDCD;
	} 

	else if (index = CTS == 1) {
		if (mCTS != pCTS) {
			mSR &= ~ACIA_SR_CTS;
			if (mCTS == 1)
				mSR |= ACIA_SR_CTS_MASK;
			else if (mTxState == NO_BIT)
				mSR |= ACIA_SR_TDRE_MASK;
		}
		pCTS = mCTS;
	}

}