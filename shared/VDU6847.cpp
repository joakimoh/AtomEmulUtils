#include "VDU6847.h"
#include <filesystem>
#include <iostream>
using namespace std;

VDU6847::VDU6847(uint16_t adr, DebugInfo debugInfo) : Device(VDU6847_DEV, adr, 0x100, debugInfo)
{
	// Set the size of the VDU register vector
	mMem.resize((size_t) mDevSz);

	// Initialise the VDU registers with zeros
	mMem.assign(mDevSz, 0);

	if (mDebugInfo.verbose)
		cout << "VDU 6847 at address 0x" << hex << setfill('0') << setw(4) << mDevAdr <<
		" to 0x" << mDevAdr + mDevSz - 1 << " (" << dec << mDevSz << " bytes)\n";
}

bool VDU6847::read(uint16_t adr, uint8_t& data)
{

	if (!validAdr(adr))
		return false;

	data = mMem[adr - mDevAdr];

	return true;

}
bool VDU6847::write(uint16_t adr, uint8_t data)
{

	if (!validAdr(adr))
		return false;

	mMem[adr - mDevAdr] = data;

	return true;
}