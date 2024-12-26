#include "RAM.h"
#include <stdexcept>
#include <iostream>
#include <fstream>

RAM::RAM(uint16_t adr, uint16_t sz) : Device(RAM_DEV, adr, sz)
{

	// Resize the RAM vector
	mMem.resize((size_t) mDevSz);

	// Initialise RAM with zeros
	mMem.assign(mDevSz, 0);
}
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