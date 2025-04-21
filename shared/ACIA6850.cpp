#include "ACIA6850.h"

ACIA6850::ACIA6850(string name, uint16_t adr, double clock, double cpuClock, uint8_t waitStates, DebugManager  *debugManager, ConnectionManager* connectionManager) :
	MemoryMappedDevice(name, ACIA6850_DEV, PERIPHERAL, cpuClock, waitStates, adr, 0x08, debugManager, connectionManager), mClock(clock)
{	registerPort("RxD",		IN_PORT,	0x1,	RxD,	&mRxD); 	registerPort("CTS",		IN_PORT,	0x1,	CTS,	&mCTS); 	registerPort("DCD",		IN_PORT,	0x1,	DCD,	&mDCD);	registerPort("TxD",		OUT_PORT,	0x1,	TxD,	&mTxD);	registerPort("RTS",		OUT_PORT,	0x1,	RTS,	&mRTS);	registerPort("IRQ",		OUT_PORT,	0x1,	IRQ,	&mIRQ);
}

bool ACIA6850::read(uint16_t adr, uint8_t& data)
{
	// Call parent class to trigger scheduling of other devices when applicable
	if (!MemoryMappedDevice::triggerBeforeRead(adr, data))
		return false;

	if ((adr & 0x1) == 0) {
		// Status Register
		data = mSR;

		// Interrupts caused by Overrun or loss od DCD are cleared by reading the SR and then the RDR (or by master reset)
		if (ACIA_SR_OVRN || ACIA_SR_DCD)
			mPendingRxIRQClr = true;
	}
	else {
		// Receive Data Register (RDR)
		data = mRDR;
		//if (ACIA_SR_RDRF)
		//	cout << "CPU Read of ACIA RDR when RDRF SR bit set gave 0x" << hex << (int)data << " at " << mCycleCount / mCPUClock * 1e-6 << "s\n";
		mSR &= ~ACIA_SR_RDRF_MASK; // Clear the Receive Data Register Full bit

		// Interrupts caused by Overrun or loss od DCD are cleared by reading the SR (mPendingRxIRQClr is true) and then the RDR
		if (mPendingRxIRQClr) { 
			mSR &= ~ACIA_SR_OVRN_MASK;
			mSR &= ~ACIA_SR_DCD_MASK;
			mPendingRxIRQClr = false;
		}
	}
	
	updateIRQ();

	return true;
}

bool ACIA6850::dump(uint16_t adr, uint8_t& data)
{
	if (selected(adr)) {
		if ((adr & 0x1) == 0)
			data = mSR;
		else 
			data = mRDR;
		return true;
	}
	return false;
}

bool ACIA6850::write(uint16_t adr, uint8_t data)
{

	// Call parent class to trigger scheduling of other devices when applicable
	MemoryMappedDevice::triggerAfterWrite(adr, data);

	if ((adr & 0x1) == 0) {
		// Control Register
		mCR = data;

		// Rx & Tx clock divide
		switch (ACIA_CR_DIV) {
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
			mSR &= ~ACIA_SR_CTS_MASK; // Clear all bits except the CTS bit that shall still reflect the state of the CTS input
			mSR |= ACIA_SR_TDRE_MASK; // Also set the Transmit Data Register Empty bit (makes sense even if not clear by the MC6850 datasheet)
			if (mPowerOn) {
				updatePort(RTS, 1);
				mPowerOn = false;
			}
			mRxState = NO_BIT;
			mTxState = NO_BIT;
			return true;
		default:
			break;
		}

		// Rx & Tx data bits
		switch (ACIA_CR_WORD) {
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
		switch (ACIA_CR_Tx) {
		case 0x0:
			updatePort(RTS, 0);
			break;
		case 0x1:
			updatePort(RTS, 0);
			break;
		case 0x2:
			updatePort(RTS, 1);
			break;
		case 0x3:
			updatePort(RTS, 0);
			updatePort(TxD, 0); // break level (space) on data out
			break;
		default:
			break;
		}

		if (mDM->debug(DBG_IO_PERIPHERAL))
			cout << "\nACIA Control Register updated to 0x" << hex << (int)mCR << "\n";

	}
	else {
		// Transmit Data Register
		mTDR = data;
		mSR &= ~ACIA_SR_TDRE_MASK; // Clear the Transmit Data Register Empty bit
		if (mTxState == NO_BIT) {
			mTxPending = true;
			if (mDM->debug(DBG_IO_PERIPHERAL))
				cout << "Write first byte 0x" << hex << (int) data << " to transmit into the ACIA TDR\n";
		}
		else {
			if (mDM->debug(DBG_IO_PERIPHERAL))
				cout << "Write next byte 0x" << hex << (int) data << " to transmit into the ACIA TDR\n";
			mTxState = START_BIT;
		}
	}

	

	update_settings();

	updateIRQ(); // Update IRQ line based on condition bits and interrupt enable status

	return true;
}

void ACIA6850::update_settings()
{

	mRxDivClkRate = mRxClkRate / mClkDiv;
	mTxDivClkRate = mTxClkRate / mClkDiv;
	mRxDivCycles = (int)round(mCPUClock * 1e6 / mRxDivClkRate);
	mTxDivCycles = (int)round(mCPUClock * 1e6 / mTxDivClkRate);

	if (mDM->debug(DBG_VERBOSE)) {
		cout << "\nACIA Settings:\n" << dec <<
			"Ena Rx IRQ =        " << (int)ACIA_CR_RIE << "\n" <<
			"Ena Tx IRQ =        " << (int)ACIA_CR_TIE << "\n" <<
			"Tx State =          " << _ACIA_STATE(mTxState) << "\n" <<
			"Clock Div =         " << (int)mClkDiv << "\n" <<
			"No of data bits =   " << (int)mNDataBits << "\n" <<
			"Parity =            " << (int)mParity << "\n" <<
			"No of stop bits =   " << (int)mStopBits << "\n" <<
			"No of Rx bits =     " << (int)mInBits << "\n" <<
			"Rx DIV clock rate = " << mRxDivClkRate << "\n" <<
			"Tx DIV clock rate = " << mTxDivClkRate << "\n" <<
			"Rx DIV cycles =     " << mRxDivCycles << "\n" <<
			"Tx DIV cycles =     " << mTxDivCycles << "\n" <<
			"Rx Clock rate =     " << mRxClkRate << "\n" <<
			"Tx Clock rate =     " << mRxClkRate << "\n" <<
			"Rx Sample cycles =  " << mRxClkCycles << "\n" <<
			"Tx Sample cycles =  " << mTxClkCycles << "\n";
	}

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
	if (mDataStart && mDataStartCount < 0) {
			mDataStartCount = mCycleCount;
			if (mDM->debug(DBG_IO_PERIPHERAL))
				cout << "*** Rx DATA STARTS AT " << dec << mDataStartCount << " cycles (" << mCycleCount / mCPUClock * 1e-6 << "s)\n";
	}

	while (mCycleCount < stopCycle) {

		mDataCount = mCycleCount - mDataStartCount;
		double data_time = mDataCount / mCPUClock * 1e-6;
		double time = mCycleCount / mCPUClock * 1e-6;
			
		if (mDataStartCount > 0 && mRxState != NO_BIT && mCycleCount % mRxClkCycles == 0) {
			//cout << dec << (int)mRxD;
		}

		// Check for a start bit at the external Rx clock rate to find and 
		// synchronise to the middle of a bit
		if (mRxState == START_BIT && mCycleCount % mRxClkCycles == 0) {
			if (mRxD == 0)
				mRxLowSamples++;
			else {
				mRxLowSamples = 0;
			}
			if (mRxLowSamples > mClkDiv / 2 ) { // Got LOW samples for half the bit duration?
				//cout << "#";
				mRxMidSampleClkCycle = mCycleCount; // Save mid sample reference for use as sample point later on
				mRxState = DATA_BIT;
				mRxBits = 0;
				mRxBuffer = 0;
				mSR &= ~ACIA_SR_PE_MASK; // Reset Framing Error
				mSR &= ~ACIA_SR_PE_MASK; // Reset parity Error
				mRxPar = 0;
				if (mDM->debug(DBG_IO_PERIPHERAL))
					cout << "\n" << dec << mRxLowSamples << " samples detected for start bit (" << mClkDiv / 2 << ") at " <<
					data_time << "s (" << time << "s)\n";
			}
		}

		// Check for data/parity/stop bits at the internal Rx DIV clock rate
		else if (mRxState != START_BIT && mRxState != NO_BIT && (mCycleCount - mRxMidSampleClkCycle) % mRxDivCycles == 0) {
			//cout << "*";
			//cout << _ACIA_STATE(mRxState) << ": Sampled bit at " << mCycleCount / mCPUClock * 1e-6 << "s (" << mCycleCount - mRxMidSampleClkCycle << ") = '" << (int)mRxD << "'\n";
			switch (mRxState) {
			case DATA_BIT:
			{
				mRxBuffer = ((mRxBuffer >> 1) | (mRxD << 7)) & 0xff;
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
				if (mRxD != (mRxPar ^ mParity))
					mSR |= ACIA_SR_PE_MASK; // Parity Error
				mRxState = STOP_BIT;
				break;
			}
			case STOP_BIT:
			{
				if (mRxD != 1)
					mSR |= ACIA_SR_PE_MASK; // Set Framing Error bit
				if (!ACIA_SR_RDRF) {// Only transfer received data if the RDR is not already Full
					mRDR = mRxBuffer;
					//cout << "ACIA Receive Data Register EMPTY - can update it with received data!\n";
				}
				else {
					//cout << "ACIA Receive Data Register FULL - cannot update it with received data!\n";
				}

				//if (mDM->debug(DBG_IO_PERIPHERAL))
				//cout << "\nACIA received byte 0x" << hex << (int)mRDR << " (" << (int)mRxBuffer << ") at " << data_time << "s (" << time << "s)\n";

				mSR |= ACIA_SR_RDRF_MASK; // Set Receive Data Register Full bit
				mRxState = START_BIT;
				mRxLowSamples = 0;
				mRxBuffer = 0;
				break;
			}
			default: // NO_BIT
				mRxLowSamples = 0;
				mRxBuffer = 0;
				break;
			}
		}

		if (mCycleCount % mTxDivCycles == 0) { // Internal Tx DIV clock rate
			if (mTxPending && mTxState == NO_BIT) {
				mTxState = START_BIT;			
				mTxPending = false;
			}
			switch (mTxState) {
			case START_BIT:
			{
				updatePort(TxD, 0);
				mSentStopBits = 0;
				if (mDM->debug(DBG_IO_PERIPHERAL))
					cout << "ACIA writes start bit\n";
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
				updatePort(TxD, mTDR & 0x1);
				if (mDM->debug(DBG_IO_PERIPHERAL))
					cout << "ACIA writes data bit '" << (int) mTxD << "'\n";
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
				updatePort(TxD, mTxPar);
				mTxState = STOP_BIT;
				break;
			}
			case STOP_BIT:
			{
				updatePort(TxD, 1);
				if (mDM->debug(DBG_IO_PERIPHERAL))
					cout << "ACIA writes stop bit\n";
				mSentStopBits++;
				if (mSentStopBits == mStopBits) {
					mTxState = NO_BIT;
					if (!ACIA_SR_CTS) {
						if (mDM->debug(DBG_IO_PERIPHERAL))
							cout << "TDRE set\n";
						mSR |= ACIA_SR_TDRE_MASK; // Set the Transmit Data Register Empty bit (if not inhibited by the CTS)
					}
					else
					{
						if (mDM->debug(DBG_IO_PERIPHERAL))
							cout << "CTS High (SR = 0x" << hex << (int) mSR << ") = > inhibits TDRE being set\n";
					}
				}
				
				break;
			}
			default: // NO_BIT
				break;
			}

		}

		updateIRQ();

		mCycleCount++;
	}
	return true;
}

// Process clock updates to drive shifting on changes
void ACIA6850::processPortUpdate(int index)
{
	
	if (index == DCD) {
		if (mDCD != pDCD) {
			if (mDCD == 0) {
				mRxLowSamples = 0;
				mRxState = START_BIT; // Initiate a first start bit synchronisation
				//cout << "ACIA DCD Input Low (active)\n";				
			}
			else { // Loss of Carrier (a low-to-high transition of the DCD input)
				//mRxState = NO_BIT;
				mSR |= ACIA_SR_DCD_MASK; // Set Data Carrier Detect (Lost) bit (cleared by reading of SR + RDR or master RESET)
			}
		}
		pDCD = mDCD;
	} 

	else if (index == CTS) {
		if (mCTS != pCTS) {
			mSR &= ~ACIA_SR_CTS_MASK;
			if (mCTS == 0) {
				if (mDM->debug(DBG_IO_PERIPHERAL))
					cout << "ACIA CTS Input Low (active)\n";
				mSR |= ACIA_SR_CTS_MASK;
			}
			else { // Loss of CTS (mCTS == 1)
				mSR &= ~ACIA_SR_CTS_MASK;
				// Loss of CTS shall clear the TDRE bit
				mSR &= ~ACIA_SR_TDRE_MASK;
				if (mDM->debug(DBG_IO_PERIPHERAL))
					cout << "*** CARRIER LOST\n";
			}
		}
		pCTS = mCTS;
	}

	updateIRQ();

}

void ACIA6850::updateIRQ()
{
	uint8_t p_IRQ = mIRQ;
	if (
		ACIA_CR_TIE && ACIA_SR_TDRE ||
		ACIA_CR_RIE && (ACIA_SR_RDRF || ACIA_SR_OVRN || ACIA_SR_DCD)
		) {
		updatePort(IRQ, 0);
		mSR |= ACIA_SR_IRQ_MASK;
		if (mDM->debug(DBG_INTERRUPTS) && p_IRQ == 1) {
			stringstream sout;
			sout << ((ACIA_CR_TIE && ACIA_SR_TDRE) ? "TDRE " : "");
			sout << ((ACIA_CR_RIE && ACIA_SR_RDRF) ? "RDRF " : "");
			sout << ((ACIA_CR_RIE && ACIA_SR_OVRN) ? "OVERN " : "");
			sout << ((ACIA_CR_RIE && ACIA_SR_DCD) ? "DCD " : "");
			cout << "ACIA: IRQ Active (Low) (" << sout.str() << ")\n";
		}
	}
	else {
		updatePort(IRQ, 1);
		mSR &= ~ACIA_SR_IRQ_MASK;
		if (mDM->debug(DBG_INTERRUPTS) && p_IRQ == 0) {
			cout << "ACIA: IRQ Inactive (High)\n";
		}
	}
}

void ACIA6850::setRxClkRate(long clkRate) {
	mRxClkRate = clkRate;
	mRxClkCycles = (int)round(mCPUClock * 1e6 / clkRate);	
	//cout << "ACIA Rx Clock set to " << dec << mRxClkRate << " Hz, and CPU cycles per Rx Clock cycles set to " << mRxClkCycles << 
	//	" for a CPU Clock of " << mCPUClock << " MHz\n";
	update_settings();
}

void ACIA6850::setTxClkRate(long clkRate) {
	mTxClkRate = clkRate;
	mTxClkCycles = (int)round(mCPUClock * 1e6 / clkRate);
	//cout << "ACIA Tx Clock set to " << dec << mTxClkRate << " Hz, and CPU cycles per Tx Clock cycles set to " << mTxClkCycles << 
	//	" for a CPU Clock of " << mCPUClock << " MHz\n";
	update_settings();
}