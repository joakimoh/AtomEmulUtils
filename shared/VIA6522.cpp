#include "VIA6522.h"
#include <filesystem>
#include <iostream>

using namespace std;

//
// VIA 6522 Emulation
// 
// Basic I/O handshaking, Timers and interrupts are supported.
// 
// Serial Control is not yet support!
// 
//


VIA6522::VIA6522(string name, uint16_t adr, double clock, double cpuClock, DebugInfo debugInfo, ConnectionManager* connectionManager) :
	MemoryMappedDevice(name, VIA6522_DEV, PERIPHERAL, adr, 0x10, debugInfo, connectionManager), mClock(clock), mCPUClock(cpuClock)
{

	// Specify ports that can be connected to other devices
	registerPort("RESET", IN_PORT, 0x01, RESET, &mRESET);
	registerPort("IRQ", OUT_PORT, 0x01, IRQ, &mIRQ);
	registerPort("PA", IO_PORT, 0xff, PA, &mPA);
	registerPort("PB", IO_PORT, 0xff, PB, &mPB);
	registerPort("CA", IO_PORT, 0x03, CA, &mCA);
	registerPort("CB", IO_PORT, 0x03, CB, &mCB);

	if (mDebugInfo.dbgLevel & DBG_VERBOSE)
		cout << "VIA 6522 '" << name << "' at address 0x" << hex << setfill('0') << setw(4) << mMemorySpace.adr <<
		" to 0x" << mMemorySpace.adr + mMemorySpace.sz - 1 << " (" << dec << mMemorySpace.sz << " bytes)\n";

}

bool VIA6522::reset()
{
	Device::reset();

	if (((mDebugInfo.dbgLevel & DBG_VERBOSE) != 0) && mRESET != pRESET) {
		cout << "VIA 6522 '" << this->name << "' RESET\n";
		pRESET = mRESET;
	}

	return true;
}

bool VIA6522::advance(uint64_t stopCycle)
{
	if (!mRESET) {
		
		reset();
		mCycleCount = stopCycle;
		return true;
	}

	if (ACR_PA_LATCH && (mCA & 0x1)) // PA shall be latched on a high CA1
		mPA_latched = mPA;

	if (ACR_PB_LATCH && (mCB & 0x1)) // PB shall be latched on a high CB1
		mPB_latched = mPB;

	// Check Timer 1

	mTimer1Counter = mTimer1Counter - (int) round((stopCycle - mCycleCount) * mClock / mCPUClock);
	if (mTimer1Counter <= 0) {
		if (mTimer1Running) {
			mIFR |= IER_T1_MASK;
			if (IER_T1)
				updatePort(IRQ, 0x0);
		}	
		switch (ACR_T1_CTRL) {
		case 0x0:	// One-shot Interrupt, PB7 inactive
			mTimer1Running = false;
			break;
		case 0x1:	// Continuous interrupts, PB7 inactive
			mTimer1Counter = (mTimer1LatchHigh << 8) | mTimer1LatchLow;
			break;
		case 0x2:	// One-shot Interrupt, PB7 low when timer starts and back high when timer reaches zero
			mTimer1Running = false;
			updatePort(PB, mPB | 0x80);
			break;
		case 0x3:	// Continuous interrupts, PB7 toggled
			mTimer1Counter = (mTimer1LatchHigh << 8) | mTimer1LatchLow;
			updatePort(PB, mPB ^ 0x80);
			break;
		default:
			break;
		}
	}


	// Check Timer 2
	switch (ACR_T2_CTRL) {
	case 0x0:	// Timed interrupt
		mTimer2Counter = mTimer2Counter - (int)round((stopCycle - mCycleCount) * mClock / mCPUClock);
		break;
	case 0x1:	// Count down with pulses on PB6
	{
		uint8_t PB6 = (mPB >> 6) & 0x1;
		uint8_t pPB6 = (pPB >> 6) & 0x1;
		if (PB6 != pPB6) {
			mTimer2Counter = mTimer2Counter - 1;
		}
		break;
	}
	default:
		break;
	}	
	if (mTimer2Counter <= 0) {
		if (mTimer2Running) {
			mIFR |= IER_T2_MASK;
			if (IER_T2)
				updatePort(IRQ, 0x0);
		}
		mTimer2Running = false;
	}

	// If no handshaking
	mIRA2 = mPA;

	uint8_t CA1_mode = PCR_CA1_CTRL;
	uint8_t CA2_mode = PCR_CA2_CTRL;
	uint8_t CB1_mode = PCR_CB1_CTRL;
	uint8_t CB2_mode = PCR_CB2_CTRL;
	uint8_t CA1_IRQ_Ena = IER_CA1;
	uint8_t CB1_IRQ_Ena = IER_CB1;
	uint8_t CA2_IRQ_Ena = IER_CA2;	
	uint8_t CB2_IRQ_Ena = IER_CB2;

	//
	// Check for CA2 input transition and set flag and IRQ
	// if a transition is detected and IRQ is enabled.
	//

	uint8_t CA2 = (mCA >> 1) & 0x1;
	uint8_t pCA2 = (pCA >> 1) & 0x1;
	bool CA2_edge = false;
	switch (CA2_mode) {
	case 0x0:	// Input, negative edge
	case 0x1:	// Interrupt input, negative edge
		if (!CA2 && CA2 != pCA2)
			mIFR |= IFR_CA2_MASK;
		break;
		CA2_edge = true;
		break;
	case 0x2:	// Input, positive edge
	case 0x3:	// Interrupt input, positive edge
		if (CA2 && CA2 != pCA2)
			mIFR |= IFR_CA2_MASK;
		break;
		CA2_edge = true;
		break;
	default:
		break;
	}

	if (CA2_edge && CA2_IRQ_Ena)
		updatePort(IRQ, 0x0);

	//
	// Check for CB2 input transition and set flag and IRQ
	// if a transition is detected and IRQ is enabled.
	//


	uint8_t CB2 = (mCB >> 1) & 0x1;
	uint8_t pCB2 = (pCB >> 1) & 0x1;
	bool CB2_edge = false;
	switch (CB2_mode) {
	case 0x0:	// Input, negative edge
	case 0x1:	// Interrupt input, negative edge
		if (!CB2 && CB2 != pCB2)
			mIFR |= IFR_CB2_MASK;
		break;
		CB2_edge = true;
		break;
	case 0x2:	// Input, positive edge
	case 0x3:	// Interrupt input, positive edge
		if (CB2 && CB2 != pCB2)
			mIFR |= IFR_CB2_MASK;
		break;
		CB2_edge = true;
		break;
	default:
		break;
	}

	if (CB2_edge && CB2_IRQ_Ena)
		updatePort(IRQ, 0x0);



	//
	// Check for read and write handshaking using CA1 (Data Ready/Data Ready - input),
	// CA2 (Data Taken/Data Ready - output) and PA (Data - input/output).
	//


	uint8_t CA1 = mCA & 0x1;	
	uint8_t pCA1 = pCA & 0x1;


	// Check for "Data Ready"/"Data Taken" input CA1. 
	// Generate IRQ if enabled.
	bool CA1_edge = false;
	if (CA1_mode == 0x0) { // Interrupt input, negative edge
		if (!CA1 && CA1 != pCA1)
			mIFR |= IFR_CA1_MASK;
		if (CA1_IRQ_Ena)
			updatePort(IRQ, 0x0);
		CA1_edge = true;
	}
	else { // Interrupt input, positive edge
		if (CA1 && CA1 != pCA1)
			mIFR |= IFR_CA1_MASK;
		if (CA1_IRQ_Ena)
			updatePort(IRQ, 0x0);
		CA1_edge = true;
	}
	
	// Deactivate "Data Taken" CA2 output if "Data Ready" was received
	// Deactive "Data Ready" CA2 output if "Data Taken" was received
	if (CA1_edge) {
		switch (CA2_mode) {
		case 0x4:	// Handshake output
		case 0x5:	// Pulse output
			updatePort(CA, mCA | 0x2);
			break;
		default:
			break;
		}
	}

	//
	// Check for write handshaking using CB2 (Data Ready - output),
	// CB1 (Data Taken - input) and PB (Data - output).
	//


	uint8_t CB1 = mCB & 0x1;
	uint8_t pCB1 = pCB & 0x1;

	// Check for "Data Taken" input CB1. 
	bool CB1_edge = false;
	if (CB1_mode == 0x0) { // Interrupt input, negative edge
		if (!CB1 && CB1 != pCB1)
			mIFR |= 0x2;
		if (CB1_IRQ_Ena)
			updatePort(IRQ, 0x0);
		CB1_edge = true;
	}
	else { // Interrupt input, positive edge
		if (CB1 && CB1 != pCB1)
			mIFR |= 0x2;
		if (CB1_IRQ_Ena)
			updatePort(IRQ, 0x0);
		CB1_edge = true;
	}

	// Deactive "Data Ready" CB2 output if "Data Taken" CB1 was received
	if (CB1_edge) {
		switch (CB2_mode) {
		case 0x4:	// Handshake output
		case 0x5:	// Pulse output
			updatePort(CB, mCB | 0x2);
			break;
		default:
			break;
		}
	}



	pCA = mCA;
	pCB = mCB;
	pPA = mPA;
	pPB = mPB;

	mCycleCount = stopCycle;

	return true;
}

void VIA6522::updateIRQ()
{
	if (((mIFR & mIER) & 0x7f) == 0) { // No pending interrupts
		updatePort(IRQ, 0x1);
		mIFR &= ~0x80; // clear IRQ bit
	}
	else { // Pending interrupts
		mIFR |= 0x80; // set IRQ bit
	}
}

bool VIA6522::read(uint16_t adr, uint8_t &data)
{
	// Call parent class to trigger scheduling of other devices when applicable
	if (!MemoryMappedDevice::read(adr, data))
		return false;

	uint16_t a = (adr - mMemorySpace.adr) & 0xf;
	switch (a) {

	case IRB:
		// Input Register B - if PBi is acting as input: if latching is disabled (ACR b1=0) IRB reflects PB; if not (ACR b1=1) the PB value latched by CB1
	{
		if (mACR & 0x2) // PB is latched
			data = (mIRB & DDRB) | (mPB_latched & ~DDRB);
		else
			data = (mIRB & DDRB) | (mPB & ~DDRB);
		break;

	}

	case IRA:
		// Input Register A- if PA is acting as input: if latching is disabled (ACR b0=0) IRB reflects PA; if not (ACR b1=1) the PB value latched by CA1
	{
		// Get latched or non-latched Port A data
		if (mACR & 0x1) // PA is latched
			data = (mIRA & mDDRA) | (mPA_latched & ~mDDRA);
		else
			data = (mIRA & mDDRA) | (mPA & ~mDDRA);

		// Clear IFR's CA2 active edge bit if read handshaking (without interrupt) was configured
		uint8_t CA2_mode = (mPCR >> 1) & 0x7;
		switch (CA2_mode) {
		case 0x4:	// Handshake output
		case 0x5:	// Pulse output
			mIFR &= ~0x1;
			break;
		default:
			break;
		}


		break;
	}
	case DDRB:
		// Data Direction Register B - '0' means corresponding PB acts as input; otherwise as output
		data = mDDRB;
		break;

	case DDRA:
		// Data Direction Register A - '0' means corresponding PA acts as input; otherwise as output
		data = mDDRA;
		break;

	case T1CL:
		// T1 Low-Order Counter
		data = mTimer1Counter & 0xff;
		mIER &= ~IER_T1_MASK;
		break;

	case T1CH:
		// T1 High-Order Counter
		data = (mTimer1Counter >> 8) & 0xff;
		break;

	case T1LL:
		// T1 Low-Order Latch
		data = mTimer1LatchLow;
		break;

	case T1LH:
		// T1 Low-Roder Latch
		data = mTimer1LatchHigh;
		break;

	case T2CL:
		// T2 Low-Order Counter
		data = mTimer2Counter & 0xff;
		if (ACR_T2_CTRL == 0)
			mIFR &= ~IFR_T2_MASK; // Clear T2 interrupt flag
		updateIRQ();

		break;

	case T2CH:
		// T2 High-Order Counter
		data = (mTimer2Counter >> 8) & 0xff;
		break;

	case SR:
		// Shift Register
		data = mSR;
		break;

	case ACR:
		// Auxiliary Control Register
		data = mACR;
		break;

	case PCR:
		// Peripheral Control Register 
		data = mPCR;
		break;

	case IFR:
		// Interrupt Flag Register
		data = mIFR;
		break;

	case IER:
		// Interrupt Enable Register
		data = mIER | 0x80;
		break;

	case IRA2:
		// Input Register A when no handshaking
		mIRA2 = (mIRA2 & mDDRA) | (mPA & ~mDDRA);
		data = mIRA2;
		break;

	default:
		data = 0xff;
		return false;
		break;
	}

	return true;

}
bool VIA6522::write(uint16_t adr, uint8_t data)
{

	// Call parent class to trigger scheduling of other devices when applicable
	if (!MemoryMappedDevice::write(adr, data))
		return false;


	switch ((adr - mMemorySpace.adr) & 0xf) {


	case ORB:
		// Output Register B - when PB acts as output, the bit value in ORB will be output on PB; otherwise the "old" value of PB will be kept
	{
		// Clear IFR's CB2 active edge bit if write handshaking (without interrupt) was configured
		uint8_t CB2_mode = (mPCR >> 5) & 0x7;
		switch (CB2_mode) {
		case 0x4:	// Handshake output
		case 0x5:	// Pulse output
			mIFR &= ~0x8;
			break;
		default:
			break;
		}

		if (!updatePort(PB, (mPB & ~DDRB) | (data & DDRB)))
			return false;
		break;
	}
	case ORA:
		// Output Register A - when PA acts as output, the bit value in ORA will be output on PA; otherwise the "old" value of PA will be kept
	{

		if (!updatePort(PA, (mPA & ~mDDRA) | (data & mDDRA)))
			return false;
		

		// Clear IFR's CA2 active edge bit if write handshaking (without interrupt) was configured
		uint8_t CA2_mode = (mPCR >> 1) & 0x7;
		switch (CA2_mode) {
		case 0x4:	// Handshake output
		case 0x5:	// Pulse output
			mIFR &= ~0x1;
			break;
		default:
			break;
		}

		break;
	}
	case DDRB:
		// Data Direction Register B - '0' means corresponding PB acts as input; otherwise as output
	{
		mDDRB = data;
		break;
	}
	case DDRA:
		// Data Direction Register A - '0' means corresponding PA acts as input; otherwise as output
	{
		mDDRA = data;
		break;
	}
	case T1CL:
		// T1 Low-Order Counter
	{
		mTimer1LatchLow = data;
		mTimer1Counter = (mTimer1Counter & 0xff00) | data;
		mTimer1XCounterHWrite = true;
		break;
	}
	case T1CH:
		// T1 High-Order Counter
	{
		mTimer1LatchHigh = data;
		if (mTimer1XCounterHWrite) {
			mTimer1Counter = (data << 8) | mTimer1LatchLow;
			mIER &= ~IER_T1_MASK;
		}
		else
			mTimer1Counter = (mTimer1Counter & 0x00ff) | (data << 8);
		mTimer1XCounterHWrite = false;
		mTimer1Running = true;
		switch (ACR_T1_CTRL) {
		case 0x0:	// One-shot Interrupt, PB7 inactive
			break;
		case 0x1:	// Continuous interrupts, PB7 inactive
			break;
		case 0x2:	// One-shot Interrupt, PB7 low when timer starts and back high when timer reaches zero
			updatePort(PB, mPB & ~0x80);
			break;
		case 0x3:	// Continuous interrupts, PB7 toggled
			updatePort(PB, mPB ^ 0x80);
			break;
		default:
			break;
		}
		break;
	}
	case T1LL:
		// T1 Low-Order Latch
	{
		mTimer1LatchLow = data;
		mTimer1XCounterHWrite = false;
		break;
	}
	case T1LH:
		// T1 High-Order Latch
	{
		mTimer1LatchHigh = data;
		break;
	}
	case T2CL:
		// T2 Low-Order Latch
	{
		mTimer1LatchLow = data;
		break;
	}
	case T2CH:
		// T2 High-Order Counter
	{
		mTimer1LatchHigh = data;
		mIFR &= ~IFR_T2_MASK; // Clear T2 interrupt flag
		updateIRQ();
		mTimer2Running = true;

		break;
	}
	case SR:
		// Shift Register
	{
		mSR = data;
		break;
	}
	case ACR:
		// Auxiliary Control Register
	{
		mACR = data;
		break;
	}
	case PCR:
		// Peripheral Control Register
	{
		mPCR = data;

		// Check for explicit CA2 control
		int CA2_mode = PCR_CA2_CTRL;
		if (CA2_mode == 0x6) // Low output
			updatePort(CA, mCA & ~0x2);
		else if (CA2_mode == 0x7) // High output
			updatePort(CA, mCA | 0x2);

		// Check for explicit CB2 control
		int CB2_mode = PCR_CB2_CTRL;
		if (CB2_mode == 0x6) // Low output
			updatePort(CA, mCB & ~0x2);
		else if (CB2_mode == 0x7) // High output
			updatePort(CA, mCB | 0x2);

		break;
	}
	case IFR:
		// Interrupt Flag Register
	{
		mIFR = (data & 0x7f);
		updateIRQ();

		break;
	}
	case IER:
		// Interrupt Enable Register
	{
		if (data & 0x80) { // enable interrupts
			mIER = (mIER | data) & 0x7f;
		}
		else { // disable interrupts		
			mIER = (mIER & ~data) & 0x7f;
		}
		cout << "VIA 6522 at 0x" << hex  << adr << " IER = 0x" <<  (int)mIER << "\n";
		break;
	}
	case ORA2:
		// Output Register A when no handshaking
	{
		mORA2 = data;
		updatePort(PA, (mPA & ~mDDRA) | (data & mDDRA));

		//cout << "PA = 0x" << hex << (int) mPA << "\n";
		break;
	}
	default:
	{
		return false;
	}
	}

	return true;
}