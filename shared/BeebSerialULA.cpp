#include "BeebSerialULA.h"

BeebSerialULA::BeebSerialULA(
	string name, uint16_t adr,double cpuClock, uint8_t waitStates, DebugManager* debugManager, ConnectionManager* connectionManager):
	MemoryMappedDevice(name, BEEB_SERIAL_ULA_DEV, PERIPHERAL, cpuClock, waitStates, adr, 1, debugManager, connectionManager)
{

}

bool BeebSerialULA::read(uint16_t adr, uint8_t& data)
{
	return true;
}

/*
* 
* b5 b4	b3		Rx clock		Rx baud rate (16/13 MHz ÷ 64)
* b2 b1	b0		Tx clock		Tx baud rate (16/13 MHz ÷  64)
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

}
