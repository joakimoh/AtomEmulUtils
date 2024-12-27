#include "VIA6522.h"
#include <filesystem>
#include <iostream>
using namespace std;


VIA6522::VIA6522(uint16_t adr, bool verbose): Device(VIA6522_DEV, adr, 0x10, verbose)
{
	// // Set the size of the VIA register vector
	mMem.resize((size_t) mDevSz);

	// Initialise the VIA registers with zeros
	mMem.assign(mDevSz, 0);

	if (mVerbose)
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