#include "VIA6522.h"
#include <filesystem>
#include <iostream>
#include <sstream>

using namespace std;

//
// VIA 6522 Emulation
// 
// Basic I/O handshaking, Timers and interrupts are supported.
// 
// Serial Control is not yet support!
// 
//


VIA6522::VIA6522(string name, uint16_t adr, double clock, double cpuClock, DebugInfo  *debugInfo, ConnectionManager* connectionManager) :
	MemoryMappedDevice(name, VIA6522_DEV, PERIPHERAL, adr, 0x10, debugInfo, connectionManager), mClock(clock), mCPUClock(cpuClock)
{

	// Specify ports that can be connected to other devices
	registerPort("RESET", IN_PORT, 0x01, RESET, &mRESET);
	registerPort("IRQ", OUT_PORT, 0x01, IRQ, &mIRQ);
	registerPort("PA", IO_PORT, 0xff, PA, &mPA);
	registerPort("PB", IO_PORT, 0xff, PB, &mPB);
	registerPort("CA", IO_PORT, 0x03, CA, &mCA);
	registerPort("CB", IO_PORT, 0x03, CB, &mCB);

	if (mDebugInfo->dbgLevel & DBG_VERBOSE)
		cout << "VIA 6522 '" << name << "' at address 0x" << hex << setfill('0') << setw(4) << mMemorySpace.adr <<
		" to 0x" << mMemorySpace.adr + mMemorySpace.sz - 1 << " (" << dec << mMemorySpace.sz << " bytes)\n";

}

bool VIA6522::reset()
{
	Device::reset();

	mIRB = 0x0;
	mORB = 0x0;	
	mIRA = 0x0;
	mORA = 0x0;	
	mDDRB = 0x0;
	mDDRA = 0x0;
	mShiftRegister = 0x0;
	mACR = 0x0;
	mPCR = 0x0;
	mIFR = 0x0;
	mIER = 0x0;
	mORA2 = 0x0;
	mIRA2 = 0x0;

	mPA = 0x0;
	mPB = 0x0;
	mCA = 0x0;
	mCB = 0x0;

	pIFR = 0xff;

	return true;
}

bool VIA6522::advance(uint64_t stopCycle)
{
	bool reset_transition = (mRESET == 0 && mRESET != pRESET);
	pRESET = mRESET;

	if (!mRESET && reset_transition) {
		reset();
		mCycleCount = stopCycle;
		return true;
	}

	while (mCycleCount < stopCycle) {

		// Advance exactly one phi2 cycle (or at least one CPU cycle)
		int one_phi2_cycle = (int)round(mCPUClock / mClock);
		mCycleCount += max(1, one_phi2_cycle);

		// Modes
		uint8_t CA1_mode = PCR_CA1_CTRL;
		uint8_t CA2_mode = PCR_CA2_CTRL;
		uint8_t CB1_mode = PCR_CB1_CTRL;
		uint8_t CB2_mode = PCR_CB2_CTRL;

		// Ports
		uint8_t CA1 = mCA & 0x1;
		uint8_t pCA1 = pCA & 0x1;
		uint8_t CA2 = (mCA >> 1) & 0x1;
		uint8_t pCA2 = (pCA >> 1) & 0x1;
		uint8_t CB1 = mCB & 0x1;
		uint8_t pCB1 = pCB & 0x1;
		uint8_t CB2 = (mCB >> 1) & 0x1;
		uint8_t pCB2 = (pCB >> 1) & 0x1;

		uint8_t PB6 = (mPB >> 6) & 0x1;
		uint8_t pPB6 = (pPB >> 6) & 0x1;


		if ((mDebugInfo->dbgLevel & DBG_IO_PERIPHERAL) && (mCA & 0x2) != (pCA & 0x2)) {
			cout << "CA1 = " << (int)(mCA & 0x1) << ", CA2 = " << (int)((mCA >> 1) & 0x1) << "\n";
		}

		if (ACR_PA_LATCH && (mCA & 0x1)) // PA shall be latched on a high CA1
			mPA_latched = mPA;

		if (ACR_PB_LATCH && (mCB & 0x1)) // PB shall be latched on a high CB1
			mPB_latched = mPB;

		// Shift Register Function
		if (mStartShifting) {
			switch (ACR_SR_CTRL) {
			case 0x0:	// Shift in on positive edge of CB1 - no interrupt flag set
				mCB1ShiftPulse = CB1;
				mShiftInterrupt = false;
				break;
			case 0x1:	// Shift in on timeout of T2 (lower bits only) and generate shift pulses on CB1 - interrupt flag set
				if (mTimer2Counter % 256 == 0 && pTimer2Counter % 256 != 0)
					mCB1ShiftPulse = 1 - mCB1ShiftPulse;
				mShiftInterrupt = true;
				mShiftGenerateCB1 = true;
				break;
			case 0x2:	// Shift in under control of phi2 and generate shift pulses on CB1 - interrupt flag set
				mCB1ShiftPulse = 1 - mCB1ShiftPulse;
				mShiftInterrupt = true;
				mShiftGenerateCB1 = true;
				break;
			case 0x3:	// Shift in under control of external clock (on CB1) -  - interrupt flag set
				mCB1ShiftPulse = CB1;
				mShiftInterrupt = true;
				mShiftGenerateCB1 = false;
				break;
			case 0x4:	// Shift out fre-running at T2 rate
				break;
			case 0x5:	// Shift out under control of T2
				if (mTimer2Counter % 256 == 0 && pTimer2Counter % 256 != 0)
					mCB1ShiftPulse = 1 - mCB1ShiftPulse;
				mShiftInterrupt = true;
				mShiftGenerateCB1 = true;
				break;
			case 0x6:	// Shift out under control of phi2
				mCB1ShiftPulse = 1 - mCB1ShiftPulse;
				break;
			case 0x7:	// Shift out under control of external clock
				mCB1ShiftPulse = CB1;
				break;
			}

			// Shift Register Function
			switch (ACR_SR_CTRL) {
			case 0x0:	// Shift in on positive edge of CB1 - no interrupt flag set
			case 0x1:	// Shift in on timeout of T2 (lower bits only) and generate shift pulses on CB1 - interrupt flag set
			case 0x2:	// Shift in under control of phi2 and generate shift pulses on CB1 - interrupt flag set
			case 0x3:	// Shift in under control of external clock (on CB1) - negative transition used
			{
				if (mCB1ShiftPulse) { // shift in on HIGH CB1 shift pulse
					mShiftRegister = (mShiftRegister << 1) | CB2;
					mShifts = mShifts % 8;
					if (mShifts == 0) {
						if (mShiftInterrupt)
							setIFR(IFR_SR_MASK);
						mCB1ShiftPulse = 0;
						mStartShifting = false;
					}
				}
				if (mShiftGenerateCB1)
					updatePort(CB, (mCB & ~0x1) | mCB1ShiftPulse);
				break;
			}
			case 0x4:	// Shift out fre-running at T2 rate
			case 0x5:	// Shift out under control of T2
			case 0x6:	// Shift out under control of phi2
			case 0x7:	// Shift out under control of external clock
			{
				if (mCB1ShiftPulse) { // shift in out HIGH CB1 shift pulse
					updatePort(CB, (mCB & ~0x2) | ((mShiftRegister & 0x1) << 1)); // output b0 on CB2
					mShiftRegister = (mShiftRegister >> 1) | ((mShiftRegister << 7) & 0x80); // shift register right one bit with b0 going into b7
					mShifts = mShifts % 8;
					if (mShifts == 0) {
						if (mShiftInterrupt)
							setIFR(IFR_SR_MASK);
						mCB1ShiftPulse = 0;
						if (ACR_SR_CTRL != 0x4 && ACR_SR_CTRL != 0x7) // shifting not to be stopped for modes 4 & 7!
							mStartShifting = false;
					}
				}
				if (mShiftGenerateCB1)
					updatePort(CB, (mCB & ~0x1) | mCB1ShiftPulse);
				break;
				break;
			}
			default:
			{
				break;
			}
			}

		}


		// Check Timer 1
		mTimer1Counter--;
		if (mTimer1Counter <= 0) {
			mTimer1Counter = 0;
			if (mTimer1Running) {
				setIFR(IFR_T1_MASK);
			}
			switch (ACR_T1_CTRL) {
			case 0x0:	// One-shot Interrupt (One-Shot Mode), PB7 inactive
				mTimer1Running = false;
				break;
			case 0x1:	// Continuous interrupts (Fre-Run Mode), PB7 inactive
				mTimer1Counter = (mTimer1LatchHigh << 8) | mTimer1LatchLow;
				break;
			case 0x2:	// One-shot Interrupt (One-Shot Mode), PB7 low when timer starts and back high when timer reaches zero
				mTimer1Running = false;
				updatePort(PB, mPB | 0x80);
				break;
			case 0x3:	// Continuous interrupts (Fre-Run Mode), PB7 toggled
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
			mTimer2Counter = mTimer2Counter - 1;
			break;
		case 0x1:	// Count down with pulses on PB6
		{

			if (PB6 != pPB6) {
				mTimer2Counter = mTimer2Counter - 1;
			}
			break;
		}
		default:
			break;
		}
		if (mTimer2Counter <= 0 && mTimer2Running) {
			mTimer2Counter = 0;
			if (mTimer2Running) {
				setIFR(IFR_T2_MASK);
			}
			mTimer2Running = false;
		}

		// If no handshaking
		mIRA2 = mPA;



		//
		// Check for CA2 input transition and set flag and IRQ
		// if a transition is detected and IRQ is enabled.
		//


		switch (CA2_mode) {
		case 0x0:	// Input, negative edge
		case 0x1:	// Interrupt input, negative edge
			if (!CA2 && CA2 != pCA2) {
				setIFR(IFR_CA2_MASK);
			}
			break;
		case 0x2:	// Input, positive edge
		case 0x3:	// Interrupt input, positive edge
			if (CA2 && CA2 != pCA2) {
				setIFR(IFR_CA2_MASK);
			}
			break;
		default:
			break;
		}


		//
		// Check for CB2 input transition and set flag and IRQ
		// if a transition is detected and IRQ is enabled.
		//



		switch (CB2_mode) {
		case 0x0:	// Input, negative edge
		case 0x1:	// Interrupt input, negative edge
			if (!CB2 && CB2 != pCB2) {
				setIFR(IFR_CB2_MASK);
			}
			break;
		case 0x2:	// Input, positive edge
		case 0x3:	// Interrupt input, positive edge
			if (CB2 && CB2 != pCB2) {
				setIFR(IFR_CB2_MASK);
			}
			break;
		default:
			break;
		}



		//
		// Check for read and write handshaking using CA1 (Data Ready/Data Ready - input),
		// CA2 (Data Taken/Data Ready - output) and PA (Data - input/output).
		//




		// Check for "Data Ready"/"Data Taken" input CA1. 
		// Generate IRQ if enabled.
		bool CA1_edge = false;
		if (CA1_mode == 0x0) { // Interrupt input, negative edge
			if (!CA1 && CA1 != pCA1) {
				setIFR(IFR_CA1_MASK);
				CA1_edge = true;
			}
		}
		else { // Interrupt input, positive edge
			if (CA1 && CA1 != pCA1) {
				setIFR(IFR_CA1_MASK);
				CA1_edge = true;
			}
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



		// Check for "Data Taken" input CB1. 
		bool CB1_edge = false;
		if (CB1_mode == 0x0) { // Interrupt input, negative edge
			if (!CB1 && CB1 != pCB1) {
				setIFR(IFR_CB1_MASK);
				CB1_edge = true;
			}

		}
		else { // Interrupt input, positive edge
			if (CB1 && CB1 != pCB1) {
				setIFR(IFR_CB1_MASK);
				CB1_edge = true;
			}
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

		pTimer1Counter = mTimer1Counter;
		pTimer2Counter = mTimer2Counter;

	}

	return true;
}

void VIA6522::updateIRQ()
{
	if ((mIFR & mIER & 0x7f) == 0) { // No pending interrupts
		updatePort(IRQ, 0x1);
		if ((mDebugInfo->dbgLevel & DBG_INTERRUPTS) != 0 && mIRQ != pIRQ) {
			cout << "*** VIA RESET IRQ\n\tIFR = " << IFR2Str() << "\n\tIER = " << IER2Str() << "\n\tACR = " << ACR2Str() << "\n\tPCR = " << PCR2Str() << "\n\n";
		}
		pIRQ = mIRQ;
	}
	else { // Pending interrupts
		updatePort(IRQ, 0x0);
		if ((mDebugInfo->dbgLevel & DBG_INTERRUPTS) != 0 && mIRQ != pIRQ) {
			cout << "*** VIA SET IRQ\n\tIFR = " << IFR2Str() << "\n\tIER = " << IER2Str() << "\n\tACR = " << ACR2Str() << "\n\tPCR = " << PCR2Str() << "\n\n";
		}
		pIRQ = mIRQ;
	}
}


void VIA6522::clearIFR(uint8_t mask)
{
	uint8_t oIFR = mIFR;
	mIFR &= ~mask;
	if (mIFR != oIFR) {
		//cout << "CLEAR IFR 0x" << hex << (int)oIFR << " => " << IFR2Str() << " for PCR = " << PCR2Str() << " and ACR = " << ACR2Str() << "\n";
	}
	pIFR = mIFR;
	updateIRQ();
}

void VIA6522::setIFR(uint8_t mask)
{
	uint8_t oIFR = mIFR;
	mIFR |= mask;
	//if (mIFR != oIFR)
		//cout << "SET IFR 0x" << hex << (int)oIFR << " => " << IFR2Str() << " for PCR = " << PCR2Str() << " and ACR = " << ACR2Str() << "\n";
	pIFR = mIFR;
	updateIRQ();
}

bool VIA6522::read(uint16_t adr, uint8_t &data)
{
	// Call parent class to trigger scheduling of other devices when applicable
	if (!MemoryMappedDevice::triggerBeforeRead(adr, data))
		return false;

	// Advance VIA one cycle to check for transitions before read operation
	advance(mCycleCount + 1);

	uint16_t a = (adr - mMemorySpace.adr) & 0xf;
	switch (a) {

	case IRB:
		// Input Register B - if PBi is acting as input: if latching is disabled (ACR b1=0) IRB reflects PB; if not (ACR b1=1) the PB value latched by CB1
	{
		if (mACR & 0x2) // PB is latched
			data = (mIRB & mDDRB) | (mPB_latched & ~mDDRB);
		else
			data = (mIRB & mDDRB) | (mPB & ~mDDRB);
		break;

	}

	case IRA:
		// Input Register A - if PA is acting as input: if latching is disabled (ACR b0=0) IRB reflects PA; if not (ACR b1=1) the PB value latched by CA1
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
			clearIFR(IFR_CA2_MASK);
			break;
		default:
			break;
		}

		// Clear IFR's CA1 active edge bit on read
		clearIFR(IFR_CA1_MASK);


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
		clearIFR(IFR_T1_MASK);
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
		// T1 Low-Order Latch
		data = mTimer1LatchHigh;
		break;

	case T2CL:
		// T2 Low-Order Counter
		data = mTimer2Counter & 0xff;
		clearIFR(IFR_T2_MASK); // Clear T2 interrupt flag

		break;

	case T2CH:
		// T2 High-Order Counter
		data = (mTimer2Counter >> 8) & 0xff;
		break;

	case SR:
		// Shift Register
	{
		
		data = mShiftRegister;


		// Start input shifting on read
		if (mShifts == 0) {
			switch (ACR_SR_CTRL) {
			case 0x0:	// Shift in on positive edge of CB2 - no interrupt flag set
			case 0x1:	// Shift in on timeout of T2 (lower bits only) and generate shift pulses on CB1 - interrupt flag set
			case 0x2:	// Shift in under control of phi2 and generate shift pulses on CB1 - interrupt flag set	
			case 0x3:	// Shift in under control of external clock (on CB1) - negative transition used
			case 0x4:	// Shift out fre-running at T2 rate
			case 0x5:	// Shift out under control of T2	
			case 0x7:	// Shift out under control of external clock
				mStartShifting = true;

				break;
			default:
				break;
			}
		}

		// Clear IFR's SR bit on read
		clearIFR(IFR_SR_MASK);

		break;

	}

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
		if (((mIFR & mIER) & 0x7f) == 0)
			data = mIFR & 0x7f; // Clear IRQ bit
		else
			data = mIFR | 0x80; // Set IRQ bit
		if ((mDebugInfo->dbgLevel & DBG_IO_PERIPHERAL) != 0)
			cout << "*READ* VIA 6522 at 0x" << hex << adr << " IFR = 0x" << (int)mIFR << " (" << IFR2Str() << ")\n";
		break;

	case IER:
		// Interrupt Enable Register
		data = mIER | 0x80;
		break;

	case IRA2:
		// Input Register A when no handshaking
		{
			mIRA2 = (mIRA2 & mDDRA) | (mPA & ~mDDRA);
		data = mIRA2;
		//cout << "*READ* IRA2 WITH DDR 0x" << hex << (int)mDDRA << " and PA 0x" << (int)mPA << " = > 0x" << (int)mIRA2 << "\n";
		break;
	}

	default:
		data = 0xff;
		return false;
		break;
	}

	return true;

}
bool VIA6522::write(uint16_t adr, uint8_t data)
{

	if (!selected(adr))
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
			clearIFR(IFR_CB2_MASK);
			break;
		default:
			break;
		}

		uint8_t oPB = mPB;
		// PB 0x2
		// data 0x3
		// DDR 0xf
		// (0x2 & ~0xf) | (0x3 & 0xf) = (0x2 & 0xf0) | 0x3 = 0x3;
		if (!updatePort(PB, (mPB & ~mDDRB) | (data & mDDRB)))
			return false;

		if (mDebugInfo->dbgLevel & DBG_IO_PERIPHERAL)
			cout << "WRITE TO PB WITH DDR 0x" << hex << (int)mDDRB << " and PB 0x" << (int)oPB << " => 0x" << (int)mPB << "\n";
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
			clearIFR(IFR_CA2_MASK);
			break;
		default:
			break;
		}

		// Clear IFR's CA1 active edge bit on write
		clearIFR(IFR_CA1_MASK);


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
		//cout << "DDRA = 0x" << hex << (int)mDDRA << "\n";
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
		clearIFR(IFR_T1_MASK); // Clear T1 interrupt flag
		if (mTimer1XCounterHWrite) {
			mTimer1Counter = (data << 8) | mTimer1LatchLow;		
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
		clearIFR(IFR_T2_MASK); // Clear T2 interrupt flag
		mTimer2Running = true;

		break;
	}
	case SR:
		// Shift Register
	{
		mShiftRegister = data;


		// Start input shifting on write
		if (mShifts == 0) {
			switch (ACR_SR_CTRL) {		
			case 0x0:	// Shift in on positive edge of CB2 - no interrupt flag set
			case 0x1:	// Shift in on timeout of T2 (lower bits only) and generate shift pulses on CB1 - interrupt flag set
			case 0x2:	// Shift in under control of phi2 and generate shift pulses on CB1 - interrupt flag set	
			case 0x3:	// Shift in under control of external clock (on CB1) - negative transition used
			case 0x4:	// Shift out fre-running at T2 rate
			case 0x5:	// Shift out under control of T2	
			case 0x7:	// Shift out under control of external clock
				mStartShifting = true;
				break;
			default:
				break;
			}
		}

		// Clear IFR's SR bit on write
		clearIFR(IFR_SR_MASK);


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
			updatePort(CB, mCB & ~0x2);
		else if (CB2_mode == 0x7) // High output
			updatePort(CB, mCB | 0x2);

		break;
	}
	case IFR:
		// Interrupt Flag Register - writing an '1' will clear the corresponding flag!!!
	{
		mIFR &= (~data) & 0x7f;
		if ((mDebugInfo->dbgLevel & DBG_IO_PERIPHERAL) != 0)
			cout << "*WRITE* VIA 6522 at 0x" << hex << adr << " IFR = 0x" << (int)mIFR << " (" << IFR2Str() << ")\n";
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
		//cout << "*WRITE* VIA 6522 at 0x" << hex << adr << " IER = 0x" << (int)mIER << " (" << IER2Str() << ")\n";

		break;
	}
	case ORA2:
		// Output Register A when no handshaking
	{
		mORA2 = data;
		uint8_t oPA = mPA;
		updatePort(PA, (mPA & ~mDDRA) | (data & mDDRA));

		if (mDebugInfo->dbgLevel & DBG_IO_PERIPHERAL)
			cout << "WRITE TO PA WITH DDR 0x" << hex << (int)mDDRA << " and PA 0x" << (int)oPA << " => 0x" << (int)mPA << "\n";


		break;
	}
	default:
	{
		return false;
	}
	}

	// Call parent class to trigger scheduling of other devices when applicable
	return MemoryMappedDevice::triggerAfterWrite(adr, data);
}

string VIA6522::IFR2Str()
{
	stringstream sout;
	string prefix = "";
	sout << "0x" << hex << (int)mIFR;
	if (mIFR != 0)
		sout << " <=> ";
	if (mIFR & IFR_CA1_MASK) {
		sout << prefix << "CA1";
		prefix = ", ";
	}
	if (mIFR & IFR_CA2_MASK) {
		sout << prefix << "CA2";
		prefix = ", ";
	}
	if (mIFR & IFR_CB1_MASK) {
		sout << prefix << "CB1";
		prefix = ", ";
	}
	if (mIFR & IFR_CB2_MASK) {
		sout << prefix << "CB2";
		prefix = ", ";
	}
	if (mIFR & IFR_SR_MASK) {
		sout << prefix << "SR";
		prefix = ", ";
	}
	if (mIFR & IFR_T1_MASK) {
		sout << prefix << "T1";
		prefix = ", ";
	}
	if (mIFR & IFR_T2_MASK) {
		sout << prefix << "T2";
		prefix = ", ";
	}

	return sout.str();
}

string VIA6522::IER2Str()
{
	stringstream sout;
	string prefix = "";
	sout << "0x" << hex << (int)mIER;
	if (mIER != 0)
		sout << " <=> ";
	if (mIER & IER_CA1_MASK) {
		sout << prefix << "CA1";
		prefix = ", ";
	}
	if (mIER & IER_CA2_MASK) {
		sout << prefix << "CA2";
		prefix = ", ";
	}
	if (mIER & IER_CB1_MASK) {
		sout << prefix << "CB1";
		prefix = ", ";
	}
	if (mIER & IER_CB2_MASK) {
		sout << prefix << "CB2";
		prefix = ", ";
	}
	if (mIER & IER_SR_MASK) {
		sout << prefix << "SR";
		prefix = ", ";
	}
	if (mIER & IER_T1_MASK) {
		sout << prefix << "T1";
		prefix = ", ";
	}
	if (mIER & IER_T2_MASK) {
		sout << prefix << "T2";
		prefix = ", ";
	}

	return sout.str();
}
string VIA6522::ACR2Str()
{
	stringstream sout;
	sout << "0x" << hex << (int)mACR <<  " <=> ";
	sout << "PA Latch:" << ACRLE2Str(ACR_PA_LATCH);
	sout << ", PB Latch:" << ACRLE2Str(ACR_PB_LATCH);
	sout << ", SR:" << ACRSR2Str(ACR_SR_CTRL);
	sout << ", T2:" << ACRT22Str(ACR_T2_CTRL);
	sout << ", T1:" << ACRT12Str(ACR_T1_CTRL);

	return sout.str();
}
string VIA6522::PCR2Str()
{
	stringstream sout;
	sout << "0x" << hex << (int)mPCR <<  " <=> ";
	sout << "CA1:" << PCRCx12Str(PCR_CA1_CTRL);
	sout << ", CA2:" << PCRCx22Str(PCR_CA2_CTRL);
	sout << ", CB1:" << PCRCx12Str(PCR_CB1_CTRL);
	sout << ", CB2:" << PCRCx22Str(PCR_CB2_CTRL);

	return sout.str();
}

string VIA6522::PCRCx22Str(uint8_t op)
{
	switch (op) {
	case 0x0: return "Input Neg Act Edge";
	case 0x1: return "Ind. IRQ Input Neg Edge";
	case 0x2: return "Input Pos Act Edge";
	case 0x3: return "Ind. IRQ Input Pos Edge";
	case 0x4: return "Handshake Output";
	case 0x5: return "Pulse Output";
	case 0x6: return "Low Output";
	case 0x7: return "High Output";
	default: return "???";
	}
}

string VIA6522::PCRCx12Str(uint8_t op)
{
	switch (op) {
	case 0x0: return "Neg. Act. Edge";
	case 0x1: return "Pos. Act. Edge";
	default: return "???";
	}
}

string VIA6522::ACRT12Str(uint8_t t)
{
	switch (t) {
	case 0x0: return "T1 Timed IRQ; no PB7";
	case 0x1: return "Cont. IRQ; no PB7";
	case 0x2: return "T1 Timed IRQ; PB7";
	case 0x3: return "Cont. IRQ; PB7";
	default: return "???";
	}
}

string VIA6522::ACRT22Str(uint8_t t)
{
	switch (t) {
	case 0x0: return "Timed IRQ";
	case 0x1: return "PB6 Count";
	default: return "???";
	}
}

string VIA6522::ACRSR2Str(uint8_t s)
{
	switch (s) {
	case 0x0: return "Disabled";
	case 0x1: return "T2 Shift In";
	case 0x2: return "Phi2 Shift In";
	case 0x3: return "Ext Shift In";
	case 0x4: return "T2 Rate FR Shift Out";
	case 0x5: return "T2 Shift Out";
	case 0x6: return "Phi2 Shift Out";
	case 0x7: return "Ext Shift Out";
	default: return "???";
	}
}

string VIA6522::ACRLE2Str(uint8_t l)
{
	switch (l) {
	case 0x0: return "No";
	case 0x1: return "Yes";
	default: return "???";
	}
}