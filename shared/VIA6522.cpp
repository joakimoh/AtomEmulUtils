#include "VIA6522.h"
#include <filesystem>
#include <iostream>

using namespace std;

bool VIA6522::reset()
{
	Device::reset();

	if (((mDebugInfo.dbgLevel & DBG_VERBOSE) != 0) && mRESET != pRESET) {
		cout << "VIA 6522 RESET\n";
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

	if ((mACR & 0x1) && (mCB & 0x1)) // PA shall be latched
		mPA_latched = mPA;

	if ((mACR & 0x2) && (mCB & 0x1)) // PB shall be latched
		mPB_latched = mPB;



	mCycleCount = stopCycle;

	return true;
}


VIA6522::VIA6522(string name, uint16_t adr, DebugInfo debugInfo, ConnectionManager* connectionManager):
	MemoryMappedDevice(name, VIA6522_DEV, PERIPHERAL, adr, 0x10, debugInfo, connectionManager)
{

	// Specify ports that can be connected to other devices
	registerPort("RESET",	IN_PORT,	0x01, RESET,	&mRESET);
	registerPort("IRQ",		OUT_PORT,	0x01, IRQ,		&mIRQ);
	registerPort("PA",		IO_PORT,	0xff, PA,		&mPA);	registerPort("PB",		IO_PORT,	0xff, PB,		&mPB);
	registerPort("CA",		IO_PORT,	0x03, CA,		&mCA);
	registerPort("CB",		IO_PORT,	0x03, CB,		&mCB);

	if (mDebugInfo.dbgLevel & DBG_VERBOSE)
		cout << "VIA 6522 at address 0x" << hex << setfill('0') << setw(4) << mDevAdr <<
		" to 0x" << mDevAdr + mDevSz - 1 << " (" << dec << mDevSz << " bytes)\n";
}

bool VIA6522::read(uint16_t adr, uint8_t &data)
{
	// Call parent class to trigger scheduling of other devices when applicable
	if (!MemoryMappedDevice::read(adr, data))
		return false;

	switch ((adr - mDevAdr) & 0xf) {

	case IRB:
		// Input Register B - if PBi is acting as input: if latching is disabled (ACR b1=0) IRB reflects PB; if not (ACR b1=1) the PB value latched by CB1
	{
		if (mACR & 0x2) // PB is latched
			data = mPB_latched;
		else
			data = mPB;
		for (int i = 0; i < 8; i++) {
			uint8_t mask = 1 << i;
			if (DDRB & mask) // PBi is output => use ORB value
				data = ~(data & ~mask) | (ORB & mask);
		}
		data = mIRB;
		break;
	}

	case IRA:
		// Input Register A- if PA is acting as input: if latching is disabled (ACR b0=0) IRB reflects PA; if not (ACR b1=1) the PB value latched by CA1
	{
		if (mACR & 0x1) // PB is latched
			data = mPA_latched;
		else
			data = mPA;
		for (int i = 0; i < 8; i++) {
			uint8_t mask = 1 << i;
			if (DDRA & mask) // PBi is output => use ORB value
				data = ~(data & ~mask) | (ORA & mask);
		}
		data = mIRA;
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
		data = mT1CL;
		break;

	case T1CH:
		// -
		data = mT1CH;
		break;

	case T1LL:
		// -
		data = 0xff;
		break;

	case T1LH:
		// -
		data = 0xff;
		break;

	case T2CL:
		// T2 Low-Order Counter
		data = mT2CL;
		break;

	case T2CH:
		// T2 High-Order Counter
		data = mT2CH;
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
		data = mIER;
		break;

	case IRA2:
		// Input Register A
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


	switch ((adr - mDevAdr) & 0xf) {


	case ORB:
		// Output Register B - when PB acts as output, the bit value in ORB will be output on PB; otherwise the "old" value of PB will be kept
	{
		data = mPB; // use input values of PB as a start
		for (int i = 0; i < 8; i++) {
			uint8_t mask = 1 << i;
			if (DDRB & mask) // PBi is output => use ORB value
				data = ~(data & ~mask) | (ORB & mask);
		}
		if (!updatePort(PB, data)) // update PB
			return false;
		break;
	}
	case ORA:
		// Output Register A - when PA acts as output, the bit value in ORA will be output on PA; otherwise the "old" value of PA will be kept
	{
		data = mPA; // use input values of PB as a start
		for (int i = 0; i < 8; i++) {
			uint8_t mask = 1 << i;
			if (DDRA & mask) // PBi is output => use ORB value
				data = ~(data & ~mask) | (ORA & mask);
		}
		if (!updatePort(PA, data)) // update PA
			return false;
		break;
	}
	case DDRB:
		// Data Direction Register B - '0' means corresponding PB acts as input; otherwise as output

		mDDRB = data;
		break;

	case DDRA:
		// Data Direction Register A - '0' means corresponding PA acts as input; otherwise as output

		mDDRA = data;
		break;

	case T1CL:
		// T1 Low-Order Latches

		mT1CL = data;
		break;

	case T1CH:
		// T1 High-Order Counter

		mT1CH = data;
		break;

	case T1LL:
		// T1 Low-Order Latches

		mT1LL = data;
		break;

	case T1LH:
		// T1 High-Order Latches
		mT1LH = data;
		break;

	case T2CL:
		// T2 Low-Order Latches

		mT2CL = data;
		break;

	case T2CH:
		// T2 High-Order Counter

		mT2CH = data;
		break;

	case SR:
		// Shift Register

		mSR = data;
		break;

	case ACR:
		// Auxiliary Control Register

		mACR = data;
		break;

	case PCR:
		// Peripehral Control Register

		mPCR = data;
		break;

	case IFR:
		// Interrupt Flag Register

		mIFR = data;
		break;

	case IER:
		// Interrupt Enable Register

		mIER = data;
		break;

	case ORA2:
		// Output Register A

		mORA2 = data;
		break;

	default:
		return false;
		break;
	}

	return true;
}