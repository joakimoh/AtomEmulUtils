#ifndef VIA6522_H
#define VIA6522_H

#include <cstdint>
#include <string>
#include "MemoryMappedDevice.h"
#include "ClockedDevice.h"
#include <cmath>

using namespace std;


class VIA6522 : public MemoryMappedDevice, public ClockedDevice {

private:

	// Ports that can be connected to other devices
	PortVal mIRQ = 0x1;
	PortVal pIRQ = 0x1;
	PortVal mPAIn = 0, mPAOut = 0xff;
	PortVal mPBIn = 0, mPBOut = 0xff;
	PortVal mCAIn = 0, mCAOut = 0x3;
	PortVal mCBIn = 0, mCBOut = 0x3;
	int IRQ, RESET, PA, PB, CA, CB;
	PortVal pPAIn = 0xff;
	PortVal pPBIn = 0xff;
	PortVal pCAIn = 0x3;
	PortVal pCBIn = 0x3;

	// Latched content of PA & PB (when latching is enabled, PA/PB are latched by CA1/CB1
	uint8_t mPA_latched = 0xff;
	uint8_t mPB_latched = 0xff;

	// Registers
	//												WRITE						READ
	//uint8_t mIRB = 0x0, mORB = 0x0;		// base address + 0x00	Output Register B			Input Register B
	//uint8_t mIRA = 0x0, mORA = 0x0;		// base address + 0x01	Output Register A			Input Register A
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
	//uint8_t mORA2 = 0x0, mIRA2 = 0x0;	// base address + 0x0f	Output Register A*			Input Register A (when no handshaking)

	uint8_t pIFR = 0x0;

	// Direction for the CA/CB port - updated when the ACR and PCR registers are changed
	uint8_t mCA1Dir = 0x0;		// CA1 is always an input!
	uint8_t mCA2Dir = 0x0;		// CA2 direction is determined by the PCR setting (PCR_CA2_CTRL)
	uint8_t mCB1Dir = 0x0;		// CB1 direction is detmined by the ACR shift mode (ACR_SR_CTRL)
	uint8_t mCB2Dir = 0x0;		// CB2 direction is determined by the PCR setting (PCR_CB2_CTRL)

// Auxiliary Control Register (ACR) Fields
#define ACR_PA_LATCH_MASK	(0x1 << 0)
#define ACR_PB_LATCH_MASK	(0x1 << 1)
#define ACR_SR_CTRL_MASK	(0x7 << 2)
#define ACR_T2_CTRL_MASK	(0x1 << 5)
#define ACR_T1_CTRL_MASK	(0x3 << 6)
#define ACR_PA_LATCH(mACR)	((mACR >> 0) & 0x1)
#define ACR_PB_LATCH(mACR)	((mACR >> 1) & 0x1)
#define ACR_SR_CTRL(mACR)	((mACR >> 2) & 0x7)
#define ACR_T2_CTRL(mACR)	((mACR >> 5) & 0x1)
#define ACR_T1_CTRL(mACR)	((mACR >> 6) & 0x3)


// Periperhal Control Register (PCR) Fields
#define PCR_CA1_CTRL_MASK	0x01
#define PCR_CA2_CTRL_MASK	0x0e
#define PCR_CB1_MASK		0x10
#define PCR_CB2_CTRL_MASK	0xe0
#define PCR_CA1_CTRL(mPCR)	((mPCR >> 0) & 0x1)
#define PCR_CA2_CTRL(mPCR)	((mPCR >> 1) & 0x7)
#define PCR_CB1_CTRL(mPCR)	((mPCR >> 4) & 0x1)
#define PCR_CB2_CTRL(mPCR)	((mPCR >> 5) & 0x7)


// Interrupt Enable Register (IER) Fields
#define IER_CA2_MASK		0x01
#define IER_CA1_MASK		0x02
#define IER_SR_MASK			0x04
#define IER_CB2_MASK		0x08
#define IER_CB1_MASK		0x10
#define IER_T2_MASK			0x20
#define IER_T1_MASK			0x40
#define IER_ENA_MASK		0x80
#define IER_CA2(mIER)		((mIER >> 0) & 1)
#define IER_CA1(mIER)		((mIER >> 1) & 1)
#define IER_SR(mIER)		((mIER >> 2) & 1)
#define IER_CB2(mIER)		((mIER >> 3) & 1)
#define IER_CB1(mIER)		((mIER >> 4) & 1)
#define IER_T2(mIER)		((mIER >> 5) & 1)
#define IER_T1(mIER)		((mIER >> 6) & 1)
#define IER_ENA(mIER)		((mIER >> 7) & 1)

// Interrupt Flag Register (IFR) Fields
#define IFR_CA2_MASK		0x01
#define IFR_CA1_MASK		0x02
#define IFR_SR_MASK			0x04
#define IFR_CB2_MASK		0x08
#define IFR_CB1_MASK		0x10
#define IFR_T2_MASK			0x20
#define IFR_T1_MASK			0x40
#define IFR_CLR_IRQ_MASK	0x80
#define IFR_CA2(mIFR)		((mIFR >> 0) & 1)
#define IFR_CA1(mIFR)		((mIFR >> 1) & 1)
#define IFR_SR(mIFR)		((mIFR >> 2) & 1)
#define IFR_CB2(mIFR)		((mIFR >> 3) & 1)
#define IFR_CB1(mIFR)		((mIFR >> 4) & 1)
#define IFR_T2(mIFR)		((mIFR >> 5) & 1)
#define IFR_T1(mIFR)		((mIFR >> 6) & 1)
#define IFR_CLR_IRQ(mIFR)	((mIFR >> 7) & 1)

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
	int mShiftedInBytes = 0;
	uint8_t mCB1ShiftPulseLevel = 0;
	uint8_t pCB1ShiftPulseLevel = 0;
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

	uint8_t pPCR = mPCR;
	uint8_t pACR = mACR;

	void updateIRQ();
	void clearIFR(uint8_t mask);
	void setIFR(uint8_t mask);

	static string IFR2Str(uint8_t data);
	string IFR2Str() { return IFR2Str(mIFR); }
	static string IER2Str(uint8_t data);
	string IER2Str() { return IER2Str(mIER); }
	static string ACR2Str(uint8_t data);
	string ACR2Str() { return ACR2Str(mACR); }
	static string PCR2Str(uint8_t data);
	string PCR2Str() { return PCR2Str(mACR); }
	static string PCRCx22Str(uint8_t op);
	static string PCRCx12Str(uint8_t op);
	static string ACRT12Str(uint8_t t);
	static string ACRT22Str(uint8_t t);
	static string ACRSR2Str(uint8_t s);
	static string ACRLE2Str(uint8_t l);
	static string ddr2Str(char port, uint8_t ddr);

	void checkForShift();
	void checkTimers();
	void checkForHandShaking();

	// Ports
	uint8_t CA1_In = mCAIn & 0x1;
	uint8_t CA1_Out = mCAOut & 0x1;
	uint8_t pCA1_In = pCAIn & 0x1;
	uint8_t CA2_In = (mCAIn >> 1) & 0x1;
	uint8_t CA2_Out = (mCAOut >> 1) & 0x1;
	uint8_t pCA2_In = (pCAIn >> 1) & 0x1;
	uint8_t CB1_In = mCBIn & 0x1;
	uint8_t CB1_Out = mCBOut & 0x1;
	uint8_t pCB1_In = pCBIn & 0x1;
	uint8_t CB2_In = (mCBIn >> 1) & 0x1;
	uint8_t CB2_Out = (mCBOut >> 1) & 0x1;
	uint8_t pCB2_In = (pCBIn >> 1) & 0x1;
	uint8_t PB6_In = (mPBIn >> 6) & 0x1;
	uint8_t pPB6_In = (pPBIn >> 6) & 0x1;

	uint8_t CA1_mode = PCR_CA1_CTRL(mPCR);
	uint8_t CA2_mode = PCR_CA2_CTRL(mPCR);
	uint8_t CB1_mode = PCR_CB1_CTRL(mPCR);
	uint8_t CB2_mode = PCR_CB2_CTRL(mPCR);

	bool shifting_active = false;

	uint8_t pCB1_IRQ = 1;

public:

	VIA6522(string name, uint16_t adr, double deviceClockRate, double tickRate, uint8_t accessSpeed, DebugTracing  *debugTracing, ConnectionManager* connectionManager,
		DeviceManager* deviceManager);

	bool readByte(BusAddress adr, BusByte& data);
	bool dump(BusAddress adr, uint8_t& data) override;
	bool writeByte(BusAddress adr, BusByte data);

	// Reset device
	bool reset();

	// Device power on
	bool power() { return reset(); }

	// Advance until time tickTime
	bool advanceUntil(uint64_t tickTime) override;

	// Outputs the internal state of the device
	bool outputState(ostream& sout) override;

	// Serialise the device's state into an array that can
	// be added to an execution trace easily.
	bool serialiseState(SerialisedState& serialisedState) override;

	// Output a single serialised device state
	bool outputSerialisedState(SerialisedState& serialisedState, ostream& sout) override;

};

#endif