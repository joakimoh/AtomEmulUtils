#include "ACIA6850.h"

ACIA6850::ACIA6850(string name, uint16_t adr, double clock, double cpuClock, DebugManager  *debugManager, ConnectionManager* connectionManager) :
	MemoryMappedDevice(name, ACIA6850_DEV, PERIPHERAL, cpuClock, adr, 0x08, debugManager, connectionManager), mClock(clock)
{	registerPort("RxD",		IN_PORT,	0x1,	RxD,	&mRxD);			// Receive Data 	registerPort("CTS",		IN_PORT,	0x1,	CTS,	&mCTS);			// Clear To Send 	registerPort("DCD",		IN_PORT,	0x1,	DCD,	&mDCD);			// Data Carrier Detect 	registerPort("RxCLK",	IN_PORT,	0x1,	RxCLK,	&mRxCLK);		// Receive Clock	registerPort("TxCLK",	IN_PORT,	0x1,	TxCLK,	&mTxCLK);		// Transmit Clock	registerPort("TxD",		OUT_PORT,	0x1,	TxD,	&mTxD);			// Transmit Data	registerPort("RTS",		OUT_PORT,	0x1,	RTS,	&mRTS);			// Request To Send
}

bool ACIA6850::read(uint16_t adr, uint8_t& data)
{
	// Call parent class to trigger scheduling of other devices when applicable
	if (!MemoryMappedDevice::triggerBeforeRead(adr, data))
		return false;

	if ((adr & 0x1) == 0) {
		// Status Register
		data = 0x00;
	}
	else {
		// Receive Data Register
		data = 0x00;
	}
	


	return true;
}

bool ACIA6850::write(uint16_t adr, uint8_t data)
{

	// Call parent class to trigger scheduling of other devices when applicable
	return MemoryMappedDevice::triggerAfterWrite(adr, data);

	if ((adr & 0x1) == 0) {
		// Control Register

	}
	else {
		// Transmit Data Register

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