#include "RAM.h"
#include <stdexcept>
#include <iostream>
#include <filesystem>

RAM::RAM(uint16_t adr, uint16_t sz, bool verbose) : Device(RAM_DEV, adr, sz, verbose)
{

	// Resize the RAM vector
	mMem.resize((size_t) mDevSz);

	// Initialise RAM with zeros
	mMem.assign(mDevSz, 0);

	if (mVerbose)
		cout << "RAM at address 0x" << hex << setfill('0') << setw(4) << mDevAdr <<
		" to 0x" << mDevAdr + mDevSz - 1 << " (" << dec << mDevSz << " bytes)\n";
}


bool RAM::read(uint16_t adr, uint8_t& data)
{
	if (!validAdr(adr))
		return false;

	data = mMem[adr - mDevAdr];

	return true;

}
bool RAM::write(uint16_t adr, uint8_t data)
{
	if (!validAdr(adr))
		return false;

	mMem[adr - mDevAdr] = data;

	return true;
}