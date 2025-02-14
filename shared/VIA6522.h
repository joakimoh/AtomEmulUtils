#ifndef VIA6522_H
#define VIA6522_H

#include <cstdint>
#include <string>
#include "MemoryMappedDevice.h"

using namespace std;


class VIA6522 : public MemoryMappedDevice {

private:

	// Ports that can be connected to other devices
	uint8_t mIRQ = 0x1;
	uint8_t mRESET = 0x1;
	uint8_t pRESET = 0x0;
	uint8_t mPA = 0xff;
	uint8_t mPB = 0xff;
	uint8_t mCA = 0x3;
	uint8_t mCB = 0x3;
	int IRQ, RESET, PA, PB, CA, CB;

	// Latched content of PA & PB (when latching is enabled, PA/PB are latched by CA1/CB1
	uint8_t mPA_latched = 0xff;
	uint8_t mPB_latched = 0xff;

	// Registers
	//												WRITE						READ
	uint8_t mIRB, mORB;		// base address + 0x00	Output Register B			Input Register B
	uint8_t mIRA, mORA;		// base address + 0x01	Output Register A			Input Register A
	uint8_t mDDRB;			// base address + 0x02	Data Direction Register B =>
	uint8_t mDDRA;			// base address + 0x03	Data Direction Register A =>
	uint8_t mT1CL;			// base address + 0x04	T1 Low-Order Latches		T1 Low-Order Counter
	uint8_t mT1CH;			// base address + 0x05	T1 High-Order Counter		-
	uint8_t mT1LL;			// base address + 0x06	T1 Low-Order Latches		-
	uint8_t mT1LH;			// base address + 0x07	T1 High-Order Latches		-
	uint8_t mT2CL;			// base address + 0x08	T2 Low-Order Latches		T2 Low-Order Counter
	uint8_t mT2CH;			// base address + 0x09	T2 High-Order Counter =>
	uint8_t mSR;			// base address + 0x0a	Shift Register =>
	uint8_t mACR;			// base address + 0x0b	Auxiliary Control Register =>
	uint8_t mPCR;			// base address + 0x0c	Peripheral Control Register =>
	uint8_t mIFR;			// base address + 0x0d	Interrupt Flag Register =>
	uint8_t mIER;			// base address + 0x0e	Interrupt Enable Register =>
	uint8_t mORA2, mIRA2;	// base address + 0x0f	Output Register A*			Input Register A (when no handshaking)

	enum VIARegAdr {
		IRB = 0x0, ORB = 0x0,
		IRA = 0x1, ORA = 0x1,
		DDRB = 0x2, DDRA = 0x3,
		T1CL = 0x4, T1CH = 0x5, T1LL = 0x6, T1LH = 0x7,
		T2CL = 0x8, T2CH = 0x9,
		SR   = 0xa, ACR  = 0xb,
		PCR  = 0xc, IFR  = 0xd, IER  = 0xe,
		IRA2  = 0xf, ORA2  = 0xf
	};

public:

	VIA6522(string name, uint16_t adr, DebugInfo debugInfo, ConnectionManager* connectionManager);

	bool read(uint16_t adr, uint8_t& data);
	bool write(uint16_t adr, uint8_t data);

	// Reset device
	bool reset();

	// Advance until clock cycle stopcycle has been reached
	bool advance(uint64_t stopCycle);

};

#endif