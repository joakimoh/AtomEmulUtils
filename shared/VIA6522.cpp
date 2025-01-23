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

	mCycleCount = stopCycle;

	return true;
}


VIA6522::VIA6522(string name, uint16_t adr, DebugInfo debugInfo, ConnectionManager* connectionManager):
	Device(name, VIA6522_DEV, PERIPERHAL, adr, 0x10, debugInfo, connectionManager)
{
	// // Set the size of the VIA register vector
	mMem.resize((size_t) mDevSz);

	// Initialise the VIA registers with zeros
	mMem.assign(mDevSz, 0);

	// Specify ports that can be connected to other devices
	addPort("RESET", IN_PORT, 0x01, RESET, &mRESET);
	addPort("IRQ", OUT_PORT, 0x01, IRQ, &mIRQ);

	if (mDebugInfo.dbgLevel & DBG_VERBOSE)
		cout << "VIA 6522 at address 0x" << hex << setfill('0') << setw(4) << mDevAdr <<
		" to 0x" << mDevAdr + mDevSz - 1 << " (" << dec << mDevSz << " bytes)\n";
}

bool VIA6522::read(uint16_t adr, uint8_t &data)
{
	if (!validAdr(adr))
		return false;

	data = mMem[adr - mDevAdr];

	return true;

}
bool VIA6522::write(uint16_t adr, uint8_t data)
{
	if (!validAdr(adr))
		return false;

	mMem[adr - mDevAdr] = data;

	return true;
}