#include "VIA6522.h"
#include <filesystem>
#include <iostream>
#include <sstream>
#include "Utility.h"
#include <cmath>

using namespace std;

//
// VIA 6522 Emulation
//


VIA6522::VIA6522(string name, uint16_t adr, double clock, double cpuClock, uint8_t waitStates, DebugManager  *debugManager, ConnectionManager* connectionManager,
	DeviceManager *deviceManager) :
	MemoryMappedDevice(name, VIA6522_DEV, PERIPHERAL, cpuClock, waitStates, adr, 0x10, debugManager, connectionManager, deviceManager), mClock(clock)
{

	// Specify ports that can be connected to other devices
	registerPort("RESET", IN_PORT, 0x01, RESET, &mRESET);
	registerPort("IRQ", OUT_PORT, 0x01, IRQ, &mIRQ);
	registerPort("PA", IO_PORT, 0xff, PA, &mPAIn, &mPAOut);
	registerPort("PB", IO_PORT, 0xff, PB, &mPBIn, &mPBOut);
	registerPort("CA", IO_PORT, 0x03, CA, &mCAIn, &mCAOut);
	registerPort("CB", IO_PORT, 0x03, CB, &mCBIn, &mCBOut);

	mCPUCyclesPerPhi2Cycle = max(1, (int)round(mCPUClock / mClock));

	DBG_LOG(this, DBG_VERBOSE, "VIA 6522 at address 0x" + Utility::int2HexStr(mAddressSpace.getStartOfSpace(),4) +
		" to 0x" + Utility::int2HexStr(mAddressSpace.getStartOfSpace() + mAddressSpace.getEndOfSpace(),4) + " (" + to_string(mAddressSpace.getSizeOfSpace()) + " bytes)\n");

}

bool VIA6522::reset()
{
	Device::reset();

	
	mDDRB = 0x0;
	mDDRA = 0x0;
	mShiftRegister = 0x0;
	mACR = 0x0;
	mPCR = 0x0;
	mIFR = 0x0;
	mIER = 0x0;

	mPAOut = 0xff;
	mPBOut = 0xff;
	mCAOut = 0xff;
	mCBOut = 0xff;

	pIFR = 0xff;

	return true;
}

//
// Shift Register Function
//
void VIA6522::checkForShift()
{

	uint8_t shift_mode = ACR_SR_CTRL;
	uint8_t p_shift_mode = ACR_SR_CTRL_VAL(pACR);

	// Generate shift pulse

	bool p_shifting_active = shifting_active;
	shifting_active = mStartShifting || mShifts > 0 || shift_mode == 0;
	if (shifting_active) {

		// Generate shift pulse from either CB1 input, timer T2 or input clock phi2
		// The generated pulse is always visible on the CB1 line (irrespecively if it is configured as an input or output)
		switch (shift_mode) {

		case 0x0:	// Shift in on positive edge of CB1 - no interrupt flag set
			mCB1ShiftPulseLevel = CB1_In;
			DBG_LOG_COND(mCB1ShiftPulseLevel != CB1_In, this, DBG_IO_ERIPHERAL, "Mode 0: CB1 shift in pulse '" + to_string(mCB1ShiftPulseLevel) + "' generated from CB1 input...\n");
			mShiftInterrupt = false;
			break;

		case 0x1:	// Shift in on timeout of T2 (lower bits only) and generate shift pulses on CB1 - interrupt flag set
			if (mTimer2Counter % 256 == 0 && pTimer2Counter % 256 != 0) {
				mCB1ShiftPulseLevel = 1 - mCB1ShiftPulseLevel;
				DBG_LOG(this, DBG_IO_PERIPHERAL, "Mode 1: CB1 shift in pulse '" + to_string(mCB1ShiftPulseLevel) + "' generated from Timer T2...\n");
			}
			mShiftInterrupt = true;
			mShiftGenerateCB1 = true;

			break;

		case 0x2:	// Shift in under control of phi2 and generate shift pulses on CB1 - interrupt flag set
			mCB1ShiftPulseLevel = 1 - mCB1ShiftPulseLevel; // the loop advances one phi2 cycle => CB1 can be toggled each loop
			mShiftInterrupt = true;
			mShiftGenerateCB1 = true;
			DBG_LOG(this, DBG_IO_PERIPHERAL, "Mode 2: CB1 shift in pulse '" + to_string(mCB1ShiftPulseLevel) + "' generated from phi2 system clock...\n");
			break;

		case 0x3:	// Shift in under control of external clock (on CB1) -  - interrupt flag set
			DBG_LOG_COND(mCB1ShiftPulseLevel != CB1_In, this, DBG_IO_PERIPHERAL, "Mode 3: CB1 shift in pulse '" + to_string(mCB1ShiftPulseLevel) + "' generated from CB1 input...\n");
			mCB1ShiftPulseLevel = CB1_In;
			mShiftInterrupt = true;
			break;

		case 0x4:	// Shift out fre-running at T2 rate
			if (mTimer2Counter % 256 == 0 && pTimer2Counter % 256 != 0) {
				mCB1ShiftPulseLevel = 1 - mCB1ShiftPulseLevel;
				DBG_LOG(this, DBG_IO_PERIPHERAL, "Mode 4: CB1 shift out pulse '" + to_string(mCB1ShiftPulseLevel) + "' generated from timer T2 - frerunning...\n");
			}
			mShiftGenerateCB1 = true;
			break;

		case 0x5:	// Shift out under control of T2
			if (mTimer2Counter % 256 == 0 && pTimer2Counter % 256 != 0) {
				mCB1ShiftPulseLevel = 1 - mCB1ShiftPulseLevel;
				DBG_LOG(this, DBG_IO_PERIPHERAL, "Mode 5: CB1 shift out pulse '" + to_string(mCB1ShiftPulseLevel) + "' generated from timer T2...\n");
			}
			mShiftInterrupt = true;
			mShiftGenerateCB1 = true;
			break;

		case 0x6:	// Shift out under control of phi2
			mCB1ShiftPulseLevel = 1 - mCB1ShiftPulseLevel;
			mShiftGenerateCB1 = true;
			DBG_LOG(this, DBG_IO_PERIPHERAL, "Mode 6: CB1 shift out pulse '" + to_string(mCB1ShiftPulseLevel) + "' generated from phi2 system clock...\n");
			break;

		case 0x7:	// Shift out under control of external clock (CB1 input)
			mCB1ShiftPulseLevel = CB1_In;
			DBG_LOG_COND(mCB1ShiftPulseLevel != CB1_In, this, DBG_IO_PERIPHERAL, "Mode 7: CB1 shift out pulse '" + to_string(mCB1ShiftPulseLevel) + "' generated from CB1 input...\n");
			break;

		}


		bool shift_pulse_goes_high = mCB1ShiftPulseLevel && !pCB1ShiftPulseLevel;


		// shift in or out based on shift pulse generated above
		switch (shift_mode) {
		case 0x0:	// Shift in on positive edge of CB1 - no interrupt flag set
		case 0x1:	// Shift in on timeout of T2 (lower bits only) and generate shift pulses on CB1 - interrupt flag set
		case 0x2:	// Shift in under control of phi2 and generate shift pulses on CB1 - interrupt flag set
		case 0x3:	// Shift in under control of external clock (on CB1) - negative transition used
		{
			// Update CB1 shift clock output to trigger a connected device to update the CB2 data input for shift in operations
			if (mShiftGenerateCB1) {
				updatePort(CB, (mCBOut & ~0x1) | mCB1ShiftPulseLevel);
				CB1_Out = mCBOut & 0x1;
			}

			// Shift in CB2 data for a low to high shift CB1 pulse transition
			if (shift_pulse_goes_high) {

				CB2_In = (mCBIn >> 1) & 0x1; // update data from connected device on CB2 input 
				mShiftRegister = (mShiftRegister << 1) | CB2_In; // shift the data into the shift register

				if (shift_mode != 0) { // Continuous shifting for mode 0 => restart of shifting every 8 bits not applicable
					mShifts = (mShifts + 1) % 8;
					if (mShifts == 0) {
						if (mShiftInterrupt)
							setIFR(IFR_SR_MASK);
						mStartShifting = false;
						mShiftedInBytes++;
					}
				}
				else
					mShiftedInBytes = 0;

				DBG_LOG(
					this, DBG_IO_PERIPHERAL,
					"Shift Mode " + to_string(shift_mode) + ": " + to_string(mShifts == 0 ? 8 : mShifts) + "th SHIFT IN of byte #" +
					to_string(mShiftedInBytes) + " from CB2 = '" + Utility::int2HexStr(CB2_In, 1) + "' = > Shift Register = 0x" +
					Utility::int2HexStr(mShiftRegister, 2) + "\n"
				);

			}
			break;
		}
		case 0x4:	// Shift out fre-running at T2 rate
		case 0x5:	// Shift out under control of T2
		case 0x6:	// Shift out under control of phi2
		case 0x7:	// Shift out under control of external clock
		{
			// Shift out data on CB2 for a low to high shift pulse transition
			if (shift_pulse_goes_high) {
				DBG_LOG(this, DBG_IO_PERIPHERAL, "SHIFT OUT\n");
				updatePort(CB, (mCBOut & ~0x2) | ((mShiftRegister & 0x1) << 1)); // output b0 on CB2
				CB1_Out = mCBOut & 0x1;
				CB2_Out = (mCBOut >> 1) & 0x1;
				mShiftRegister = (mShiftRegister >> 1) | ((mShiftRegister << 7) & 0x80); // shift register right one bit with b0 going into b7
				mShifts = (mShifts + 1) % 8;
				DBG_LOG(this, DBG_IO_PERIPHERAL, "Shift Mode " + to_string(shift_mode) + ": " + to_string(mShifts + 1) + " SHIFT OUT from CB2 = '" + Utility::int2HexStr(CB2_Out, 1) + "' => Shift Register = 0x" +
					Utility::int2HexStr(mShiftRegister, 2) + "\n");
				if (mShifts == 0) {
					if (mShiftInterrupt)
						setIFR(IFR_SR_MASK);
					if (shift_mode != 0x4 && shift_mode != 0x7) // shifting not to be stopped for modes 4 & 7!
						mStartShifting = false;
				}
			}

			// Update CB1 shift clock output to trigger a connected device to read the now updated CB2 data output for shift out operations
			if (mShiftGenerateCB1) {
				updatePort(CB, (mCBOut & ~0x1) | mCB1ShiftPulseLevel);
				CB1_Out = mCBOut & 0x1;
				CB2_Out = (mCBOut >> 1) & 0x1;
			}
			break;
		}
		default:
		{
			break;
		}
		}

	}

	else if (!shifting_active && shifting_active != p_shifting_active) {
		//mCB1ShiftPulseLevel = 1; // Make sure that no phantom 0->1 transitions are created
	}
}

void VIA6522::checkTimers()
{
	//
	// Check Timer 1
	// 
	// Timing is different for different modes:
	// 
	// Mode			#cycles from write to				Remark
	//				T1C-H to IRQ low
	//				(and PB7 toggled)
	// One-shot		n + 1.5								same applies to timer 2
	// Free-run		n + 1.5 for first countdown
	//				n + 2 for subsequent countdowns
	// 
	if (mTimer1Running) {
		mTimer1Counter--;
		if (mTimer1FirstRun && mTimer1Counter <= -1 || !mTimer1FirstRun && mTimer1Counter <= -2) {
			mTimer1FirstRun = false;
			mTimer1Counter = 0;
			setIFR(IFR_T1_MASK);
			switch (ACR_T1_CTRL) {
			case 0x0:	// One-shot Interrupt (One-Shot Mode), PB7 inactive
				mTimer1Running = false;
				break;
			case 0x1:	// Continuous interrupts (Fre-Run Mode), PB7 inactive
				mTimer1Counter = (mTimer1LatchHigh << 8) | mTimer1LatchLow;
				break;
			case 0x2:	// One-shot Interrupt (One-Shot Mode), PB7 low when timer starts and back high when timer reaches zero
				mTimer1Running = false;
				updatePort(PB, mPBOut | 0x80);
				break;
			case 0x3:	// Continuous interrupts (Fre-Run Mode), PB7 toggled
				mTimer1Counter = (mTimer1LatchHigh << 8) | mTimer1LatchLow;
				updatePort(PB, mPBOut ^ 0x80);
				break;
			default:
				break;
			}
		}
	}


	// Check Timer 2
	switch (ACR_T2_CTRL) {
	case 0x0:	// Timed interrupt
		mTimer2Counter = mTimer2Counter - 1;
		break;
	case 0x1:	// Count down with pulses on PB6
	{

		if (PB6_In != pPB6_In) {
			mTimer2Counter = mTimer2Counter - 1;
		}
		break;
	}
	default:
		break;
	}
	if (mTimer2Counter <= -1 && mTimer2Running) {
		mTimer2Counter = 0;
		if (mTimer2Running) {
			setIFR(IFR_T2_MASK);
		}
		mTimer2Running = false;
	}
}

void VIA6522::checkForHandShaking()
{

	//
	// Check for CA2 input transition and set flag and IRQ
	// if a transition is detected and IRQ is enabled.
	//


	switch (CA2_mode) {
	case 0x0:	// Input, negative edge
	case 0x1:	// Interrupt input, negative edge
		if (!CA2_In && CA2_In != pCA2_In) {
			setIFR(IFR_CA2_MASK);
		}
		break;
	case 0x2:	// Input, positive edge
	case 0x3:	// Interrupt input, positive edge
		if (CA2_In && CA2_In != pCA2_In) {
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
		if (!CB2_In && CB2_In != pCB2_In) {
			setIFR(IFR_CB2_MASK);
		}
		break;
	case 0x2:	// Input, positive edge
	case 0x3:	// Interrupt input, positive edge
		if (CB2_In && CB2_In != pCB2_In) {
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
		if (!CA1_In && CA1_In != pCA1_In) {
			setIFR(IFR_CA1_MASK);
			CA1_edge = true;
		}
	}
	else { // Interrupt input, positive edge
		if (CA1_In && CA1_In != pCA1_In) {
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
			updatePort(CA, mCAOut | 0x2);
			CA1_Out = mCAOut & 0x1;
			CA2_Out = (mCAOut >> 1) & 0x1;
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
		if (!CB1_In && CB1_In != pCB1_In) {
			setIFR(IFR_CB1_MASK);
			CB1_edge = true;
		}

	}
	else { // Interrupt input, positive edge
		if (CB1_In && CB1_In != pCB1_In) {
			setIFR(IFR_CB1_MASK);
			CB1_edge = true;
		}
	}

	// Deactive "Data Ready" CB2 output if "Data Taken" CB1 was received
	if (CB1_edge) {
		switch (CB2_mode) {
		case 0x4:	// Handshake output
		case 0x5:	// Pulse output
			updatePort(CB, mCBOut | 0x2);
			CB1_Out = mCBOut & 0x1;
			CB2_Out = (mCBOut >> 1) & 0x1;
			break;
		default:
			break;
		}
	}
}

bool VIA6522::advanceUntil(uint64_t stopCycle)
{
	bool reset_transition = mRESET != pRESET;
	pRESET = mRESET;

	DBG_LOG_COND(reset_transition, this, DBG_RESET, "RESET => " + to_string(mRESET) + "\n");

	if (!mRESET && reset_transition) {
		reset();
		mCycleCount = stopCycle;
		return true;
	}

	uint8_t shift_mode = ACR_SR_CTRL;
	uint8_t p_shift_mode = ACR_SR_CTRL_VAL(pACR);

	// reset #shifts if the shift mode was changed
	if (shift_mode != p_shift_mode) {
		DBG_LOG(this, DBG_IO_PERIPHERAL, "Shift Mode changed from " + to_string(p_shift_mode) + " to " + to_string(shift_mode) + "\n");
		mShifts = 0;
	}

	// Modes
	CA1_mode = PCR_CA1_CTRL;
	CA2_mode = PCR_CA2_CTRL;
	CB1_mode = PCR_CB1_CTRL;
	CB2_mode = PCR_CB2_CTRL;

	shifting_active = false;

	while (mCycleCount < stopCycle) {

		if (mCycleCount % mCPUCyclesPerPhi2Cycle == 0) {

			// Update port aliases
			CA1_In = mCAIn & 0x1;
			CA1_Out = mCAOut & 0x1;
			pCA1_In = pCAIn & 0x1;
			CA2_In = (mCAIn >> 1) & 0x1;
			CA2_Out = (mCAOut >> 1) & 0x1;
			pCA2_In = (pCAIn >> 1) & 0x1;
			CB1_In = mCBIn & 0x1;
			CB1_Out = mCBOut & 0x1;
			pCB1_In = pCBIn & 0x1;
			CB2_In = (mCBIn >> 1) & 0x1;
			CB2_Out = (mCBOut >> 1) & 0x1;
			pCB2_In = (pCBIn >> 1) & 0x1;
			PB6_In = (mPBIn >> 6) & 0x1;
			pPB6_In = (pPBIn >> 6) & 0x1;


			if (DBG_LEVEL_DEV(this, DBG_IO_PERIPHERAL) && (mCAIn & 0x2) != (pCAIn & 0x2)) {
				DBG_LOG(this, DBG_IO_PERIPHERAL, "CA1 (in) = " + to_string(mCAIn & 0x1) + ", CA2 (in) = " + to_string((mCAIn >> 1) & 0x1) + "\n");
			}

			if (ACR_PA_LATCH && CA1_In) // PA shall be latched on a high CA1
				mPA_latched = mPAIn;

			if (ACR_PB_LATCH && CB1_In) // PB shall be latched on a high CB1
				mPB_latched = mPBIn;


			checkForShift();

			checkTimers();

			checkForHandShaking();

			pCAIn = mCAIn;
			pCBIn = mCBIn;
			pPAIn = mPAIn;
			pPBIn = mPBIn;


			pCB1ShiftPulseLevel = mCB1ShiftPulseLevel;

			pTimer1Counter = mTimer1Counter;
			pTimer2Counter = mTimer2Counter;

			updateIRQ();

			// Advance one phi2 cycle		
			mCycleCount += mCPUCyclesPerPhi2Cycle;

		}
		else
			mCycleCount++;
	}

	pPCR = mPCR;
	pACR = mACR;

	return true;
}

void VIA6522::updateIRQ()
{
	if ((mIFR & mIER & 0x7f) == 0) { // No pending interrupts
		updatePort(IRQ, 0x1);
		if (DBG_LEVEL_DEV(this,DBG_INTERRUPTS) && mIRQ != pIRQ) {
			DBG_LOG(this, DBG_INTERRUPTS, "VIA deactivates the IRQ line (makes it HIGH)\n\tIFR = " + IFR2Str() + "\n\tIER = " + IER2Str() + "\n\tACR = " + ACR2Str() + "\n\tPCR = " + PCR2Str() + "\n\n");
		}
		pIRQ = mIRQ;
	}
	else { // Pending interrupts
		updatePort(IRQ, 0x0);
		if (DBG_LEVEL_DEV(this,DBG_INTERRUPTS) && mIRQ != pIRQ) {
			DBG_LOG(this, DBG_INTERRUPTS, "Via activates the IRQ line (makes it LOW)\n\tIFR = " + IFR2Str() + "\n\tIER = " + IER2Str() + "\n\tACR = " + ACR2Str() + "\n\tPCR = " + PCR2Str() + "\n\n");
		}
		pIRQ = mIRQ;
	}
}

void VIA6522::clearIFR(uint8_t mask)
{
	uint8_t oIFR = mIFR;
	mIFR &= ~mask;
	if (mIFR != oIFR) {
		DBG_LOG(this, DBG_IO_PERIPHERAL, "VIA clear IFR 0x" + Utility::int2HexStr(oIFR,2) + " => " + IFR2Str() + " for PCR = " + PCR2Str() + ", ACR = " + ACR2Str() + " and IER = " + IER2Str() + "\n");
	}
	pIFR = mIFR;
}

void VIA6522::setIFR(uint8_t mask)
{
	uint8_t oIFR = mIFR;
	mIFR |= mask;
	if (mIFR != oIFR) {
		DBG_LOG(this, DBG_IO_PERIPHERAL, "VIA sets IFR 0x" + Utility::int2HexStr(oIFR, 2) + " => " + IFR2Str() + " for PCR = " + PCR2Str() + ", ACR = " + ACR2Str() + " and IER = " + IER2Str() + "\n");
	}
	pIFR = mIFR;
}

bool VIA6522::read(uint16_t adr, uint8_t &data)
{
	// Call parent class to trigger scheduling of other devices when applicable
	if (!MemoryMappedDevice::triggerBeforeRead(adr, data))
		return false;

	// Advance VIA one cycle to check for transitions before read operation
	advanceUntil(mCycleCount + mCPUCyclesPerPhi2Cycle);

	uint16_t a = (adr - mAddressSpace.getStartOfSpace()) & 0xf;
	switch (a) {

	case IRB:
		// Input Register B - if PB is acting as input: if latching is disabled (ACR b1=0) IRB reflects PB; if not (ACR b1=1) the PB value latched by CB1
	{
		if (mACR & 0x2) // PB is latched
			data = (mPBOut & mDDRB) | (mPB_latched & ~mDDRB);
		else
			data = (mPBOut & mDDRB) | (mPBIn & ~mDDRB);
		break;

	}

	case IRA:
		// Input Register A - if PA is acting as input: if latching is disabled (ACR b0=0) IRB reflects PA; if not (ACR b1=1) the PB value latched by CA1
	{
		// Get latched or non-latched Port A data
		if (mACR & 0x1) // PA is latched
			data = (mPAOut & mDDRA) | (mPA_latched & ~mDDRA);
		else
			data = (mPAOut & mDDRA) | (mPAIn & ~mDDRA);

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
				mShifts = 0;
				DBG_LOG(this, DBG_IO_PERIPHERAL, ": Start shifting by reading the Shift Register - mode is "+ to_string(ACR_SR_CTRL) + "\n");
				break;
			default:
				break;
			}
		}

		DBG_LOG(this, DBG_IO_PERIPHERAL, ": Read Shift Register => 0x" + Utility::int2HexStr(data,2) + "\n");

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
		DBG_LOG(this, DBG_IO_PERIPHERAL, "Read at 0x" + Utility::int2HexStr(adr,4) + " IFR = 0x" + Utility::int2HexStr(mIFR,2) + " (" + IFR2Str() + ")\n");
		break;

	case IER:
		// Interrupt Enable Register
		data = mIER | 0x80;
		break;

	case IRA2:
		// Input Register A when no handshaking
		{
		data = (mPAOut & mDDRA) | (mPAIn & ~mDDRA);
		break;
	}

	default:
		data = 0xff;
		return false;
		break;
	}

	updateIRQ();

	return true;

}

bool VIA6522::dump(uint16_t adr, uint8_t& data)
{
	if (selected(adr)) {
		uint16_t a = (adr - mAddressSpace.getStartOfSpace()) & 0xf;
		switch (a) {

		case IRB:
			// Input Register B - if PBi is acting as input: if latching is disabled (ACR b1=0) IRB reflects PB; if not (ACR b1=1) the PB value latched by CB1
		{
			if (mACR & 0x2) // PB is latched
				data = (mPBOut & mDDRB) | (mPB_latched & ~mDDRB);
			else
				data = (mPBOut & mDDRB) | (mPBIn & ~mDDRB);
			break;

		}

		case IRA:
			// Input Register A - if PA is acting as input: if latching is disabled (ACR b0=0) IRB reflects PA; if not (ACR b1=1) the PB value latched by CA1
		{
			// Get latched or non-latched Port A data
			if (mACR & 0x1) // PA is latched
				data = (mPAOut & mDDRA) | (mPA_latched & ~mDDRA);
			else
				data = (mPAOut & mDDRA) | (mPAIn & ~mDDRA);


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

			break;

		case T2CH:
			// T2 High-Order Counter
			data = (mTimer2Counter >> 8) & 0xff;
			break;

		case SR:
			// Shift Register
		{

			data = mShiftRegister;

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
			break;

		case IER:
			// Interrupt Enable Register
			data = mIER | 0x80;
			break;

		case IRA2:
			// Input Register A when no handshaking
		{ 
			data = (mPAOut & mDDRA) | (mPAIn & ~mDDRA);;
			break;
		}

		default:
			data = 0xff;
			return false;
			break;
		}
	}
	return false;
}

bool VIA6522::write(uint16_t adr, uint8_t data)
{

	if (!selected(adr))
		return false;


	switch ((adr - mAddressSpace.getStartOfSpace()) & 0xf) {


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

		uint8_t oPB = mPBOut;
		// PB 0x2
		// data 0x3
		// DDR 0xf
		// (0x2 & ~0xf) | (0x3 & 0xf) = (0x2 & 0xf0) | 0x3 = 0x3;
		if (!updatePort(PB, (mPBOut & ~mDDRB) | (data & mDDRB)))
			return false;

		DBG_LOG(this, DBG_IO_PERIPHERAL, "Write to PB with DDR 0x" + Utility::int2HexStr(mDDRB,2) + " and PB 0x" + Utility::int2HexStr(oPB,2) + " => 0x" + Utility::int2HexStr(mPBOut,2) + "\n");
		break;
	}
	case ORA:
		// Output Register A - when PA acts as output, the bit value in ORA will be output on PA; otherwise the "old" value of PA will be kept
	{

		if (!updatePort(PA, (mPAOut & ~mDDRA) | (data & mDDRA)))
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
		if (mDDRB != data) {
			DBG_LOG(this, DBG_IO_PERIPHERAL, "Write to DDRB at 0x" + Utility::int2HexStr(adr, 4) +
				": DDRB = 0x" + Utility::int2HexStr(data, 2) + " (" + ddr2Str('B', data) + ")\n");
			registerPortDirChange(PB, data);
		}
		mDDRB = data;
		break;
	}
	case DDRA:
		// Data Direction Register A - '0' means corresponding PA acts as input; otherwise as output
	{
		if (mDDRA != data) {
			DBG_LOG(this, DBG_IO_PERIPHERAL, "Write to DDRA at 0x" + Utility::int2HexStr(adr, 4) +
				": DDRA = 0x" + Utility::int2HexStr(data, 2) + " (" + ddr2Str('A', data) + ")\n");
			registerPortDirChange(PA, data);
		}
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
		clearIFR(IFR_T1_MASK); // Clear T1 interrupt flag
		if (mTimer1XCounterHWrite) {
			mTimer1Counter = (data << 8) | mTimer1LatchLow;		
		}
		else
			mTimer1Counter = (mTimer1Counter & 0x00ff) | (data << 8);
		mTimer1XCounterHWrite = false;
		mTimer1Running = true;
		mTimer1FirstRun = true;
		switch (ACR_T1_CTRL) {
		case 0x0:	// One-shot Interrupt, PB7 inactive
			break;
		case 0x1:	// Continuous interrupts, PB7 inactive
			break;
		case 0x2:	// One-shot Interrupt, PB7 low when timer starts and back high when timer reaches zero
			updatePort(PB, mPBOut & ~0x80);
			break;
		case 0x3:	// Continuous interrupts, PB7 toggled
			updatePort(PB, mPBOut ^ 0x80);
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
		mTimer2LatchHigh = data;
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
				mShifts = 0;
				DBG_LOG(this, DBG_IO_PERIPHERAL, ": Start shifting by writing to the Shift Register - mode is " + to_string(ACR_SR_CTRL) + "\n");
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

		// Check for shift mode changes impacting the direction of port CB
		uint8_t p_shift_mode = ACR_SR_CTRL;
		uint8_t p_ACR = mACR;
		mACR = data;

		DBG_LOG_COND(mACR != p_ACR, this, DBG_IO_PERIPHERAL, "\nWrite to ACR:\n" + ACR2Str() + "\n");

		if (ACR_SR_CTRL != p_shift_mode) {
			switch (ACR_SR_CTRL) {
			case 0x0:	// Shift in on positive edge of CB1 - no interrupt flag set
			case 0x3:	// Shift in under control of external clock (on CB1) -  - interrupt flag set
				mCB1Dir = 0x0;	// CB1 IN
				mCB2Dir = 0x0;	// CB2 IN
				break;

			case 0x1:	// Shift in on timeout of T2 (lower bits only) and generate shift pulses on CB1 - interrupt flag set
			case 0x2:	// Shift in under control of phi2 and generate shift pulses on CB1 - interrupt flag set
				mCB1Dir = 0x1;	// CB1 OUT
				mCB2Dir = 0x0;	// CB2 IN
				break;
			case 0x4:	// Shift out fre-running at T2 rate
			case 0x5:	// Shift out under control of T2
			case 0x6:	// Shift out under control of phi2
				mCB1Dir = 0x1;	// CB1 OUT
				mCB2Dir = 0x2;	// CB2 OUT
				break;
			case 0x7:	// Shift out under control of external clock (CB1 input)	
				mCB1Dir = 0x0;	// CB1 IN
				mCB2Dir = 0x2;	// CB2 OUT
				break;
			}
			uint8_t CB_dir = mCB1Dir | mCB2Dir;
			registerPortDirChange(CB, CB_dir);
		}

		break;
	}
	case PCR:
		// Peripheral Control Register
	{

	
		mPCR = data;
		uint8_t p_PCR = mPCR;

		int CA1_mode = PCR_CA1_CTRL;
		int CA2_mode = PCR_CA2_CTRL;
		int CB1_mode = PCR_CB1_CTRL;
		int CB2_mode = PCR_CB2_CTRL;

		if (mPCR != p_PCR) {

			DBG_LOG(this, DBG_IO_PERIPHERAL, "\nWrite to PCR:\n" + PCR2Str() + "\n");

			// Conservatively assume changes to the PCR always result in changes in the direction of ports CA & CB
			// In lower modes CA2/CB2 are outputs whereas they are inputs for higher modes.
			// CA1 is alwways an input (i.e. not a bidrectional port!)
			// CB1 is an input for higher modes; for lower modes it is defined by the ACR shift mode instead
			mCA1Dir = 0;
			mCA2Dir = (CA2_mode < 0x3 ? 0x2 : 0x0); // lower mode:CA2 OUT (2), higher mode:CA2 IN (0)
			uint8_t CA_dir = mCA1Dir | mCA2Dir;
			mCB1Dir = (CB2_mode < 0x3 ? mCB1Dir : 0x0); // lower mode: -, higher mode: CB1 IN (0)
			mCB2Dir = (CB2_mode < 0x3 ? 0x2 : 0x0); // lower mode:CB2 OUT (2), higher mode:CB2 IN (0):
			uint8_t CB_dir = mCB1Dir | mCB2Dir;
			registerPortDirChange(CA, CA_dir);
			registerPortDirChange(CB, CB_dir);
		}

		// Check for explicit CA2 control

		if (CA2_mode == 0x6) // Low output
			updatePort(CA, mCAOut & ~0x2);
		else if (CA2_mode == 0x7) // High output
			updatePort(CA, mCAOut | 0x2);

		// Check for explicit CB2 control

		if (CB2_mode == 0x6) // Low output
			updatePort(CB, mCBOut & ~0x2);
		else if (CB2_mode == 0x7) // High output
			updatePort(CB, mCBOut | 0x2);

		break;
	}
	case IFR:
		// Interrupt Flag Register - writing an '1' will clear the corresponding flag!!!
	{
		mIFR &= (~data) & 0x7f;
		DBG_LOG(this, DBG_IO_PERIPHERAL, "Write to IFR at 0x" + Utility::int2HexStr(adr,4) + ": IFR = 0x" + Utility::int2HexStr(mIFR,2) + " (" + IFR2Str() + ")\n");
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
		DBG_LOG(this, DBG_IO_PERIPHERAL, "Write to IER at 0x" + Utility::int2HexStr(adr, 4) + ": IER = 0x" + Utility::int2HexStr(mIER, 2) + " (" + IER2Str() + ")\n");


		break;
	}
	case ORA2:
		// Output Register A when no handshaking
	{
		uint8_t oPA = mPAOut;
		updatePort(PA, (mPAOut & ~mDDRA) | (data & mDDRA));

		DBG_LOG(this, DBG_IO_PERIPHERAL, "Write to PA with DDR 0x" + Utility::int2HexStr(mDDRA,2) + " and PA 0x" + Utility::int2HexStr(oPA,2) + " => 0x" + Utility::int2HexStr(mPAOut,2) + "\n");


		break;
	}
	default:
	{
		return false;
	}
	}

	updateIRQ();

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
	sout << ", Shifting:" << ACRSR2Str(ACR_SR_CTRL);
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

string VIA6522::ddr2Str(char port, uint8_t ddr)
{
	stringstream sout;
	bool out = false;
	for (int b = 0; b < 8; b++) {
		if ((ddr & (1 << b)) != 0) {
			if (out)
				sout << ", ";
			else
				sout << "OUT: ";
			sout << "P" << port << (int)b;
			out = true;
		}
	}
	bool in = false;
	for (int b = 0; b < 8; b++) {
		if ((ddr & (1 << b)) == 0) {
			if (in)
				sout << ", ";
			else {
				if (out)
					sout << "; IN: ";
				else
					sout << "IN: ";
			}
			sout << "P" << port << (int)b;
			in = true;
		}
	}
	return sout.str();
}

// Outputs the internal state of the device
bool VIA6522::outputState(ostream& sout)
{
	sout << "IER = " << IER2Str() << "\n";
	sout << "IFR = " << IFR2Str() << "\n";
	sout << "ACR = " << ACR2Str() << "\n";
	sout << "PCR = " << PCR2Str() << "\n";
	sout << "PA = 0x" << Utility::int2HexStr(mPAIn, 2) << " (In) 0x" << Utility::int2HexStr(mPAOut, 2) << " (out) 0b" <<
		Utility::int2BinStr(mDDRA,8) << " (dir mask - '1' <=> Out)\n";
	sout << "PB = 0x" << Utility::int2HexStr(mPBIn, 2) << " (In) 0x" << Utility::int2HexStr(mPBOut, 2) << " (out) 0b" <<
		Utility::int2BinStr(mDDRB,8) << " (dir mask - '1' <=> Out)\n";
	sout << "CA1 = 0x" << Utility::int2HexStr(mCAIn & 0x1, 1) << " (In) " << Utility::int2HexStr(mCAOut & 1, 1) << " (out)\n";
	sout << "CA2 = 0x" << Utility::int2HexStr((mCAIn >> 1) & 0x1, 1) << " (In) " << Utility::int2HexStr((mCAOut >> 1) & 1, 1) << " (out)\n";
	sout << "CB1 = 0x" << Utility::int2HexStr(mCBIn & 0x1, 1) << " (In) " << Utility::int2HexStr(mCBOut & 1, 1) << " (out)\n";
	sout << "CB2 = 0x" << Utility::int2HexStr((mCBIn >> 1) & 0x1, 1) << " (In) " << Utility::int2HexStr((mCBOut >> 1) & 1, 1) << " (out)\n";
	sout << "Shift Register = 0x" << Utility::int2HexStr(mShiftRegister, 2) << "\n";
	sout << "T1 Counter = 0x" << Utility::int2HexStr(mTimer1Counter & 0xffff, 4) << "\n";
	sout << "T2 Counter = 0x" << Utility::int2HexStr(mTimer2Counter & 0xffff, 4) << "\n";

	return true;
}