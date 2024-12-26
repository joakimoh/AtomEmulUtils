#include "VIA6522.h"

VIA6522::VIA6522(uint16_t adr) : Device(VIA6522_DEV, adr, 0x10)
{
	// // Set the size of the VIA register vector
	mMem.resize((size_t) mDevSz);

	// Initialise the VIA registers with zeros
	mMem.assign(mDevSz, 0);
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