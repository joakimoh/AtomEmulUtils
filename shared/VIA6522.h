#ifndef VIA6522_H
#define VIA6522_H

#include <cstdint>
#include <string>
#include "MemoryMappedDevice.h"
#include <cmath>

using namespace std;


class VIA6522 : public MemoryMappedDevice {

private:

	// Ports that can be connected to other devices
	uint8_t mIRQ = 0x1;
	uint8_t pIRQ = 0x1;
	uint8_t mPA = 0xff;
	uint8_t mPB = 0xff;
	uint8_t mCA = 0x3;
	uint8_t mCB = 0x3;
	int IRQ, RESET, PA, PB, CA, CB;
	uint8_t pPA = 0xff;
	uint8_t pPB = 0xff;
	uint8_t pCA = 0x3;
	uint8_t pCB = 0x3;

	// Latched content of PA & PB (when latching is enabled, PA/PB are latched by CA1/CB1
	uint8_t mPA_latched = 0xff;
	uint8_t mPB_latched = 0xff;

	// Registers
	//												WRITE						READ
	uint8_t mIRB = 0x0, mORB = 0x0;		// base address + 0x00	Output Register B			Input Register B
	uint8_t mIRA = 0x0, mORA = 0x0;		// base address + 0x01	Output Register A			Input Register A
	uint8_t mDDRB = 0x0;			// base address + 0x02	Data Direction Register B =>
	uint8_t mDDRA = 0x0;			// base address + 0x03	Data Direction Register A =>
	//uint8_t mT1CL;			// base address + 0x04	T1 Low-Order Latches		T1 Low-Order Counter
	//uint8_t mT1CH;			// base address + 0x05	T1 High-Order Counter		-
	//uint8_t mT1LL;			// base address + 0x06	T1 Low-Order Latches		-
	//uint8_t mT1LH;			// base address + 0x07	T1 High-Order Latches		-
	///uint8_t mT2CL;			// base address + 0x08	T2 Low-Order Latches		T2 Low-Order Counter
	//uint8_t mT2CH;			// base address + 0x09	T2 High-Order Counter =>
	uint8_t mShiftRegister = 0x0;			// base address + 0x0a	Shift Register =>
	uint8_t mACR = 0x0;			// base address + 0x0b	Auxiliary Control Register =>
	uint8_t mPCR = 0x0;			// base address + 0x0c	Peripheral Control Register =>
	uint8_t mIFR = 0x0;			// base address + 0x0d	Interrupt Flag Register =>
	uint8_t mIER = 0x0;			// base address + 0x0e	Interrupt Enable Register =>
	uint8_t mORA2 = 0x0, mIRA2 = 0x0;	// base address + 0x0f	Output Register A*			Input Register A (when no handshaking)

	uint8_t pIFR = 0x0;

// Auxiliary Control Register (ACR) Fields
#define ACR_PA_LATCH_MASK	(0x1 << 0)
#define ACR_PB_LATCH_MASK	(0x1 << 1)
#define ACR_SR_CTRL_MASK	(0x7 << 2)
#define ACR_T2_CTRL_MASK	(0x1 << 5)
#define ACR_T1_CTRL_MASK	(0x3 << 6)
#define ACR_PA_LATCH		((mACR >> 0) & 0x1)
#define ACR_PB_LATCH		((mACR >> 1) & 0x1)
#define ACR_SR_CTRL			((mACR >> 2) & 0x7)
#define ACR_T2_CTRL			((mACR >> 5) & 0x1)
#define ACR_T1_CTRL			((mACR >> 6) & 0x3)


// Periperhal Control Register (PCR) Fields
#define PCR_CA1_CTRL_MASK	0x01
#define PCR_CA2_CTRL_MASK	0x0e
#define PCR_CB1_MASK		0x10
#define PCR_CB2_CTRL_MASK	0xe0
#define PCR_CA1_CTRL		((mPCR >> 0) & 0x1)
#define PCR_CA2_CTRL		((mPCR >> 1) & 0x7)
#define PCR_CB1_CTRL		((mPCR >> 4) & 0x1)
#define PCR_CB2_CTRL		((mPCR >> 5) & 0x7)


// Interrupt Enable Register (IER) Fields
#define IER_CA2_MASK		0x01
#define IER_CA1_MASK		0x02
#define IER_SR_MASK			0x04
#define IER_CB2_MASK		0x08
#define IER_CB1_MASK		0x10
#define IER_T2_MASK			0x20
#define IER_T1_MASK			0x40
#define IER_ENA_MASK		0x80
#define IER_CA2		((mIER >> 0) & 1)
#define IER_CA1		((mIER >> 1) & 1)
#define IER_SR		((mIER >> 2) & 1)
#define IER_CB2		((mIER >> 3) & 1)
#define IER_CB1		((mIER >> 4) & 1)
#define IER_T2		((mIER >> 5) & 1)
#define IER_T1		((mIER >> 6) & 1)
#define IER_ENA		((mIER >> 7) & 1)

// Interrupt Flag Register (IFR) Fields
#define IFR_CA2_MASK		0x01
#define IFR_CA1_MASK		0x02
#define IFR_SR_MASK			0x04
#define IFR_CB2_MASK		0x08
#define IFR_CB1_MASK		0x10
#define IFR_T2_MASK			0x20
#define IFR_T1_MASK			0x40
#define IFR_CLR_IRQ_MASK	0x80
#define IFR_CA2		((mIFR >> 0) & 1)
#define IFR_CA1		((mIFR >> 1) & 1)
#define IFR_SR		((mIFR >> 2) & 1)
#define IFR_CB2		((mIFR >> 3) & 1)
#define IFR_CB1		((mIFR >> 4) & 1)
#define IFR_T2		((mIFR >> 5) & 1)
#define IFR_T1		((mIFR >> 6) & 1)
#define IFR_CLR_IRQ	((mIFR >> 7) & 1)

// Timers
	int mTimer1Counter = 0x0;
	uint8_t mTimer1LatchHigh = 0x0;
	uint8_t mTimer1LatchLow = 0x0;
	int mTimer2Counter = 0x0;
	uint8_t mTimer2LatchHigh = 0x0;
	uint8_t mTimer2LatchLow = 0x0;
	bool mTimer1XCounterHWrite = false;
	bool mTimer2XCounterHWrite = false;
	bool mTimer1Running = false;
	bool mTimer2Running = false;
	bool mTimer1FirstRun = false;

	uint16_t pTimer1Counter = 0x0;
	uint16_t pTimer2Counter = 0x0;
	uint8_t mShifts = 0;
	uint8_t mCB1ShiftPulse = 0;
	bool mStartShifting = false;
	bool mShiftInterrupt = false;
	bool mShiftGenerateCB1 = false;



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

	double mClock = 1.0;

	void updateIRQ();
	void clearIFR(uint8_t mask);
	void setIFR(uint8_t mask);

	string IFR2Str();
	string IER2Str();
	string ACR2Str();
	string PCR2Str();
	string PCRCx22Str(uint8_t op);
	string PCRCx12Str(uint8_t op);
	string ACRT12Str(uint8_t t);
	string ACRT22Str(uint8_t t);
	string ACRSR2Str(uint8_t s);
	string ACRLE2Str(uint8_t l);

public:

	VIA6522(string name, uint16_t adr, double clock, double cpuClock, uint8_t waitStates, DebugManager  *debugManager, ConnectionManager* connectionManager);

	bool read(uint16_t adr, uint8_t& data);
	bool dump(uint16_t adr, uint8_t& data) override;
	bool write(uint16_t adr, uint8_t data);

	// Reset device
	bool reset();

	// Advance until clock cycle stopcycle has been reached
	bool advance(uint64_t stopCycle);

	// Process a port update directly (and not just next time the advance() method is called)
	void processPortUpdate(int index) override;

};

#endif