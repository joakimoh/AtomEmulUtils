#include "BeebSerialULA.h"
#include "ACIA6850.h"
#include "Device.h"

BeebSerialULA::BeebSerialULA(
	string name, uint16_t adr, double cpuClock, uint8_t waitStates, DebugManager* debugManager, ConnectionManager* connectionManager) :
	MemoryMappedDevice(name, BEEB_SERIAL_ULA_DEV, PERIPHERAL, cpuClock, waitStates, adr, 1, debugManager, connectionManager)
{
	registerPort("RxD", OUT_PORT, 0x1, RxD, &mRxD);			// Receive Data to ACIA
	registerPort("CTS", OUT_PORT, 0x1, CTS, &mCTS);			// Clear To Send to ACIA
	registerPort("DCD", OUT_PORT, 0x1, DCD, &mDCD);			// Data Carrier Detect to ACIA
	registerPort("TxD", IN_PORT, 0x1, TxD, &mTxD);			// Transmit Data from ACIA
	registerPort("RTS", IN_PORT, 0x1, RTS, &mRTS);			// Request To Send from ACIA

	registerPort("CASMO", OUT_PORT, 0x1, CASMO, &mCASMO);	// Cassette Motor control
	registerPort("CAS_IN", IN_PORT, 0x1, CAS_IN, &mCAS_IN);	// Cassette In
	registerPort("CAS_OUT", OUT_PORT, 0x1, CAS_OUT, &mCAS_OUT);	// Cassette Out

	registerPort("DIn", IN_PORT, 0x1, DIn, &mDIn);			// Serial In
	registerPort("DOut", OUT_PORT, 0x1, DOut, &mDOut);		// Serial Out
	registerPort("RTSO", OUT_PORT, 0x1, RTSO, &mRTSO);		// Request To Send Out


}

bool BeebSerialULA::read(uint16_t adr, uint8_t& data)
{
	return true;
}

/*
* 
* b5 b4	b3		Rx clock		ACIA Rx baud rate (16/13 MHz ÷ 64)
* b2 b1	b0		Tx clock		ACIA Tx baud rate (16/13 MHz ÷  64)
* ========		==========		===================
*  0  0	 0		1228.8 kHz		19200 baud
*  1  0  0		614.4 kHz		9600 baud
*  0  1	 0		307.2 kHz		4800 baud
*  1  1	 0		153.6 kHz		2400 baud
*  0  0	 1		76.8 kHz		1200 baud
*  1  0	 1		19.2 kHz		300 baud
*  0  1	 1		9.6 kHz			150 baud
*  1  1	 1		4.8 kHz			75 baud


*/
bool BeebSerialULA::write(uint16_t adr, uint8_t data)
{
	mCR = data;
	switch (SER_ULA_CR_RxRate) {
	case 0x0:
		mRxClkRate = 1228800; // Gives 19200 with ÷ 64 made by the ACIA
		break;
	case 0x1:
		mRxClkRate = 614400; // Gives 9600 with ÷ 64 made by the ACIA
		break;
	case 0x2:
		mRxClkRate = 307200; // Gives 4800 with ÷ 64 made by the ACIA
		break;
	case 0x3:
		mRxClkRate = 153600; // Gives 2400 with ÷ 64 made by the ACIA
		break;
	case 0x4:
		mRxClkRate = 76800; // Gives 1200 with ÷ 64 made by the ACIA
		break;
	case 0x5:
		mRxClkRate = 19200; // Gives 300 with ÷ 64 made by the ACIA
		break;
	case 0x6:
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

	if (mACIA != NULL) {
		mACIA->setRxClkRate(mTxClkRate);
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


// Advance until clock cycle stopcycle has been reached
bool BeebSerialULA::advance(uint64_t stopCycle)
{
	return true;
}


// Process clock updates to drive shifting on changes
void BeebSerialULA::processPortUpdate(int index)
{
	if (SER_ULA_CR_ENA_SER) { // Serial Communication
		if (index == DIn) {
			updatePort(RxD, mDIn); // forward received external data to the ACIA
		}
		else if (index == TxD) {
			updatePort(DOut, mTxD);	// forward ACIA Tx data externally
		}
		else if (index == RTS) {
			updatePort(RTSO, mRTS);// forward ACIA RTS externally
		}
	}

	else { // Cassette Interface

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
