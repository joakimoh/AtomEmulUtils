#include "ACIA6850.h"
#include <cmath>
#include "Utility.h"

ACIA6850::ACIA6850(string name, uint16_t adr, double deviceClockRate, double tickRate, uint8_t waitStates, DebugTracing  *debugTracing,
	ConnectionManager* connectionManager, DeviceManager* deviceManager) :
	MemoryMappedDevice(name, ACIA6850_DEV, PERIPHERAL, waitStates, adr, 0x08, debugTracing, connectionManager, deviceManager),
	ClockedDevice(tickRate, deviceClockRate)
{
	registerPort("RxD",		IN_PORT,	0x1,	RxD,	&mRxD);
 	registerPort("CTS",		IN_PORT,	0x1,	CTS,	&mCTS);
 	registerPort("DCD",		IN_PORT,	0x1,	DCD,	&mDCD);
	registerPort("TxD",		OUT_PORT,	0x1,	TxD,	&mTxD);
	registerPort("RTS",		OUT_PORT,	0x1,	RTS,	&mRTS);
	registerPort("IRQ",		OUT_PORT,	0x1,	IRQ,	&mIRQ);

}

bool ACIA6850::readByte(BusAddress adr, BusByte& data)
{
	uint8_t p_SR = mSR;
	uint8_t p_CR = mCR;

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
		mSR &= ~ACIA_SR_RDRF_MASK; // Clear the Receive Data Register Full bit

		// Interrupts caused by Overrun or loss od DCD are cleared by reading the SR (mPendingRxIRQClr is true) and then the RDR
		if (mPendingRxIRQClr) { 
			mSR &= ~ACIA_SR_OVRN_MASK;
			mSR &= ~ACIA_SR_DCD_MASK;
			mPendingRxIRQClr = false;
		}
	}


	
	if (mIRQ == 0 || mSR != p_SR || mCR != p_CR)
		updateIRQ();

	return true;
}

bool ACIA6850::dump(BusAddress adr, uint8_t& data)
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

bool ACIA6850::writeByte(BusAddress adr, BusByte data)
{
	uint8_t p_SR = mSR;
	uint8_t p_CR = mCR;

	// Call parent class to trigger scheduling of other devices when applicable
	MemoryMappedDevice::triggerAfterWrite(adr, data);

	if ((adr & 0x1) == 0) {
		// Control Register
		mCR = data;

		// Rx & Tx clock divide
		string div_s;
		switch (ACIA_CR_DIV) {
		case 0x0:
			mClkDiv = 1;
			div_s = "Rx/Tx Clk DIV 1";
			break;
		case 0x1:
			mClkDiv = 16;
			div_s = "Rx/Tx Clk DIV 16";
			break;
		case 0x2:
			mClkDiv = 64;
			div_s = "Rx/Tx Clk DIV 64";
			break;
		case 0x3:
			// master reset
			mCR = 0;
			mSR &= ~ACIA_SR_CTS_MASK; // Clear all bits except the CTS bit that shall still reflect the state of the CTS input
			mSR |= ACIA_SR_TDRE_MASK; // Also set the Transmit Data Register Empty bit (makes sense even if not specified by the MC6850 datasheet)
			if (mPowerOn) {
				updatePort(RTS, 1);
				mPowerOn = false;
			}
			mRxState = NO_BIT;
			mTxState = NO_BIT;
			div_s = "Master Reset";
			return true;
		default:
			break;
		}

		// Rx & Tx data bits
		string w_sel_s;
		switch (ACIA_CR_WORD) {
		case 0x0:
			mNDataBits = 7;
			mParity = 0;
			mStopBits = 2;
			mInBits = mOutBits = 11;
			w_sel_s = "7 data bits, even parity, 2 stop bits";
			break;
		case 0x1:
			mNDataBits = 7;
			mParity = 1;
			mStopBits = 2;
			mInBits = mOutBits = 11;
			w_sel_s = "7 data bits, odd parity, 2 stop bits";
			break;
		case 0x2:
			mNDataBits = 7;
			mParity = 0;
			mStopBits = 1;
			mInBits = mOutBits = 10;
			w_sel_s = "7 data bits, even parity, 1 stop bit";
			break;
		case 0x3:
			mNDataBits = 7;
			mParity = 1;
			mStopBits = 1;
			mInBits = mOutBits = 10;
			w_sel_s = "7 data bits, odd parity, 1 stop bit";
			break;
		case 0x4:
			mNDataBits = 8;
			mParity = -1;
			mStopBits = 2;
			mInBits = mOutBits = 11;
			w_sel_s = "8 data bits, no parity, 2 stop bits";
			break;
		case 0x5:
			mNDataBits = 8;
			mParity = -1;
			mStopBits = 1;
			mInBits = mOutBits = 10;
			w_sel_s = "8 data bits, no parity, 1 stop bit";
			break;
		case 0x6:
			mNDataBits = 8;
			mParity = 0;
			mStopBits = 1;
			mInBits = mOutBits = 11;
			w_sel_s = "8 data bits, even parity, 1 stop bit";
			break;
		case 0x7:
			mNDataBits = 8;
			mParity = 1;
			mStopBits = 1;
			mInBits = mOutBits = 11;
			w_sel_s = "8 data bits, odd parity, 1 stop bit";
			break;
		default:
			break;
		}

		// Tx control
		string tx_ctrl_s;
		switch (ACIA_CR_Tx) {
		case 0x0:
			updatePort(RTS, 0);
			tx_ctrl_s = "RTS low; no Tx IRQ";
			break;
		case 0x1:
			updatePort(RTS, 0);
			tx_ctrl_s = "RTS low; Tx IRQ";
			break;
		case 0x2:
			updatePort(RTS, 1);
			tx_ctrl_s = "RTS high; no Tx IRQ";
			break;
		case 0x3:
			updatePort(RTS, 0);
			updatePort(TxD, 0); // break level (space) on data out
			tx_ctrl_s = "RTS low; transmit space;no Tx IRQ";
			break;
		default:
			break;
		}

		DBG_LOG_COND(mCR != p_CR, this, DBG_IO_PERIPHERAL, "Control Register updated to 0x" + Utility::int2HexStr(mCR,2) + " [" +
				div_s + ", " + w_sel_s + ", " + tx_ctrl_s + ", " + (ACIA_CR_RIE ? "Rx IRQ" : "No Rx IRQ") + "]");


	}
	else {
		// Transmit Data Register
		mTDR = data;
		mSR &= ~ACIA_SR_TDRE_MASK; // Clear the Transmit Data Register Empty bit
		if (mTxState == NO_BIT) {
			mTxPending = true;
			DBG_LOG_COND(mTxState == NO_BIT, this, DBG_IO_PERIPHERAL, "Write 0x" + Utility::int2HexStr(data,2) + " to transmit into the TDR [no ongoing Tx]");
		}
		else {
			DBG_LOG_COND(mTxState != NO_BIT, this, DBG_IO_PERIPHERAL, "Write 0x" + Utility::int2HexStr(data,2) +
				" to transmit into the TDR [ongoing Tx - state is " +
				(mTxState==DATA_BIT?"Data":(mTxState==PARITY_BIT?"Parity Bit":(mTxState==STOP_BIT?"Stop bit":"???"))) +"]"
			);
		}
	}

	

	update_settings();

	if (mIRQ == 0 || mSR != p_SR || mCR != p_CR)
		updateIRQ(); // Update IRQ line based on condition bits and interrupt enable status

	return true;
}

void ACIA6850::update_settings()
{
	bool change = (pCR != mCR || mRxClkRate != pRxClkRate || mTxClkRate != pTxClkRate);
	pCR = mCR;
	pRxClkRate = mRxClkRate;
	pTxClkRate = mTxClkRate;


	mRxDivClkRate = mRxClkRate / mClkDiv;
	mTxDivClkRate = mTxClkRate / mClkDiv;
	mRxDivCycles = (int)round(mTickRate * 1e6 / mRxDivClkRate);
	mTxDivCycles = (int)round(mTickRate * 1e6 / mTxDivClkRate);

	if (DBG_LEVEL_DEV(this, DBG_IO_PERIPHERAL) && change) {
		stringstream sout;
		outputState(sout);
		DBG_LOG(this, DBG_IO_PERIPHERAL, "ACIA Settings:" + sout.str());
	}

}

// Reset device
// The ACIA doesn't have a reset pin but can be reset by writing to the control register
bool ACIA6850::reset()
{
	Device::reset();
	return true;
}

// Advance until time tickTime
bool ACIA6850::advanceUntil(uint64_t tickTime)
{
	uint8_t p_SR;
	uint8_t p_CR;

	if (mDataStart && mDataStartCount < 0) {
			mDataStartCount = mTicks;
			DBG_LOG(this, DBG_IO_PERIPHERAL, "Rx DATA STARTS AT " + to_string(mDataStartCount) + " cycles (" + to_string(mTicks / mTickRate * 1e-6) + "s)");
	}

	while (mTicks < tickTime) {

		p_SR = mSR;
		p_CR = mCR;

		mDataCount = mTicks - mDataStartCount;
		double data_time, time;
		if (DBG_LEVEL_DEV(this,DBG_IO_PERIPHERAL)) {
			data_time = mDataCount / mTickRate * 1e-6;
			time = mTicks / mTickRate * 1e-6;
		}

		// Check for a start bit at the external Rx clock rate to find and 
		// synchronise to the middle of a bit
		if (mRxState == START_BIT && mTicks % mRxClkCycles == 0) {
			if (mRxD == 0)
				mRxLowSamples++;
			else {
				mRxLowSamples = 0;
			}
			if (mRxLowSamples > mClkDiv / 2 ) { // Got LOW samples for half the bit duration?

				mRxMidSampleClkCycle = mTicks; // Save mid sample reference for use as sample point later on
				mRxState = DATA_BIT;
				mRxBits = 0;
				mRxBuffer = 0;
				mSR &= ~ACIA_SR_PE_MASK; // Reset Framing Error
				mSR &= ~ACIA_SR_PE_MASK; // Reset parity Error
				mRxPar = 0;
				DBG_LOG(
					this, DBG_IO_PERIPHERAL, to_string(mRxLowSamples) + " samples detected for start bit (" + to_string(mClkDiv / 2) + ") at " +
					to_string(data_time) + "s (" + to_string(time) + "s)"
				);
			}
		}

		// Check for data/parity/stop bits at the internal Rx DIV clock rate
		else if (mRxState != START_BIT && mRxState != NO_BIT && (mTicks - mRxMidSampleClkCycle) % mRxDivCycles == 0) {

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
				}

				DBG_LOG(this, DBG_IO_PERIPHERAL, "Received byte 0x" + Utility::int2HexStr(mRDR, 2) + " (" + Utility::int2HexStr(mRxBuffer, 2));

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

		if (mTicks % mTxDivCycles == 0) { // Internal Tx DIV clock rate
			if (mTxState == NO_BIT) {
				if (mTxPending) {
					mTxState = START_BIT;
					mTxPending = false;
					mTDRBuf = mTDR; // Copy the TDR data into a buffer to free  the TDR for new data

				}
				else if (mTxD != 1) {
					// Make output high before a potential later start bit
					updatePort(TxD, 1);
				}
			}
			switch (mTxState) {
			case START_BIT:
			{
				DBG_LOG(this, DBG_IO_PERIPHERAL, "Writing start bit for byte 0x" +Utility::int2HexStr(mTDRBuf,2));
				updatePort(TxD, 0);
				mSentStopBits = 0;
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
				DBG_LOG(this, DBG_IO_PERIPHERAL, "Writing data bit '" + Utility::int2HexStr(mTDRBuf & 0x1, 2) + "'");
				updatePort(TxD, mTDRBuf & 0x1);
				mTDRBuf = (mTDRBuf >> 1) & 0x7f;
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
				DBG_LOG(this, DBG_IO_PERIPHERAL, "Writing stop bit");
				updatePort(TxD, 1);
				mSentStopBits++;
				if (mSentStopBits == mStopBits) {
					mTxState = NO_BIT;
					if (!ACIA_SR_CTS) {
						DBG_LOG(this, DBG_IO_PERIPHERAL, "TDRE set");
						mSR |= ACIA_SR_TDRE_MASK; // Set the Transmit Data Register Empty bit (if not inhibited by the CTS)
					}
					else
					{
						DBG_LOG(this, DBG_IO_PERIPHERAL, "CTS High (SR = 0x" + Utility::int2HexStr(mSR,2) + ") = > inhibits TDRE being set");
					}

				}
				
				break;
			}
			default: // NO_BIT
				break;
			}

		}

		if (mSR != p_SR || mCR != p_CR)
			updateIRQ();

		mTicks++;
	}
	return true;
}

// Process clock updates to drive shifting on changes
void ACIA6850::processPortUpdate(int index)
{
	uint8_t p_SR = mSR;
	uint8_t p_CR = mCR;

	if (index == DCD) {
		if (mDCD != pDCD) {
			if (mDCD == 0) {
				mRxLowSamples = 0;
				mRxState = START_BIT; // Initiate a first start bit synchronisation			
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
				DBG_LOG(this, DBG_IO_PERIPHERAL, "ACIA CTS Input Low (active)");
				mSR |= ACIA_SR_CTS_MASK;
			}
			else { // Loss of CTS (mCTS == 1)
				mSR &= ~ACIA_SR_CTS_MASK;
				// Loss of CTS shall clear the TDRE bit
				mSR &= ~ACIA_SR_TDRE_MASK;
				DBG_LOG(this, DBG_IO_PERIPHERAL, "CARRIER LOST");
			}
		}
		pCTS = mCTS;
	}

	if (mIRQ == 0 || mSR != p_SR || mCR != p_CR)
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
		if (DBG_LEVEL_DEV(this,DBG_INTERRUPTS) && p_IRQ == 1) {
			stringstream sout;
			sout << ((ACIA_CR_TIE && ACIA_SR_TDRE) ? "TDRE " : "");
			sout << ((ACIA_CR_RIE && ACIA_SR_RDRF) ? "RDRF " : "");
			sout << ((ACIA_CR_RIE && ACIA_SR_OVRN) ? "OVERN " : "");
			sout << ((ACIA_CR_RIE && ACIA_SR_DCD) ? "DCD " : "");
			DBG_LOG_COND(p_IRQ == 1, this, DBG_INTERRUPTS, "IRQ Active (Low) (" + sout.str() + ")");
		}
	}
	else {
		updatePort(IRQ, 1);
		mSR &= ~ACIA_SR_IRQ_MASK;
		DBG_LOG_COND(p_IRQ == 0, this, DBG_IO_PERIPHERAL,"IRQ Inactive(High)");
	}
}

void ACIA6850::setRxClkRate(long clkRate) {
	mRxClkRate = clkRate;
	mRxClkCycles = (int)round(mTickRate * 1e6 / clkRate);	
	update_settings();
}

void ACIA6850::setTxClkRate(long clkRate) {
	mTxClkRate = clkRate;
	mTxClkCycles = (int)round(mTickRate * 1e6 / clkRate);
	update_settings();
}

// Outputs the internal state of the device
bool ACIA6850::outputState(ostream& sout)
{
	sout << "Receive Data register =  0x" << Utility::int2HexStr(mRDR, 2) << "\n";
	sout << "Transmit Data register = 0x" << Utility::int2HexStr(mTDR, 2) << "\n";
	sout << "Rx Bit State =           " << _ACIA_STATE(mRxState) << "\n";
	sout << "Tx BitState =            " << _ACIA_STATE(mTxState) << "\n";
	sout << "Control Register =       0x" << Utility::int2HexStr(mCR, 2) << " <=> \n";
	sout << "\tEna Rx IRQ =             " << (int)ACIA_CR_RIE << "\n";
	sout << "\tEna Tx IRQ =             " << (int)ACIA_CR_TIE << "\n";
	sout << "\tClock Div =              " << (int)mClkDiv << "\n";
	sout << "\tNo of data bits =        " << (int)mNDataBits << "\n";
	sout << "\tParity =                 " << (mParity==0?"Even":(mParity==1?"Odd":"None")) << "\n";
	sout << "\tNo of stop bits =        " << (int)mStopBits << "\n";
	sout << "\tRx DIV clock rate =      " << mRxDivClkRate << "\n";
	sout << "\tTx DIV clock rate =      " << mTxDivClkRate << "\n";
	sout << "\tRx Clock rate =          " << mRxClkRate << "\n";
	sout << "\tTx Clock rate =          " << mRxClkRate << "\n";
	sout << "IRQ = " << (int)mIRQ << "\n";

	return true;
}