#ifndef BEEB_SERIAL_ULA_H
#define BEEB_SERIAL_ULA_H

#include <cstdint>
#include <string>
#include "MemoryMappedDevice.h"

using namespace std;


class BeebSerialULA : public MemoryMappedDevice {

private:

	double mClock = 1.0;


	// Ports
	int RxD, TxD, RTS, CTS, DCD, RxCLK, TxCLK;
	uint8_t mRxD = 0x0;
	uint8_t mTxD = 0x0;
	uint8_t mRTS = 0x1;
	uint8_t mCTS = 0x1;
	uint8_t mDCD = 0x1;
	uint8_t mRxCLK = 1;
	uint8_t mTxCLK = 1;
	uint8_t mIRQ = 1;

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

	/*
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
	* 
	* The ULA itself ignores bits 5 to 3 and generates a receive clock of 19.2 kHz when bit 6 is clear.
	* (The SERPROC also disregards bits 5 and 4 but reassigns bit 3 for another purpose, see below.)
	*  When saving, bits 2 to 0 as well as the ACIA clock divider govern the baud rate, as in serial mode,
	* but again the MOS only adjusts the divider.
	*/

public:

	BeebSerialULA(string name, uint16_t adr, double cpuClock, uint8_t waitStates, DebugManager* debugManager, ConnectionManager* connectionManager);

	bool read(uint16_t adr, uint8_t& data);
	bool write(uint16_t adr, uint8_t data);

	// Reset device
	bool reset();

	// Advance until clock cycle stopcycle has been reached
	bool advance(uint64_t stopCycle);

	// Process clock updates to drive shifting on changes
	void processPortUpdate(int index);

};

#endif