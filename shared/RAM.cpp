#include "RAM.h"
#include <stdexcept>
#include <iostream>
#include <filesystem>

RAM::RAM(string name, uint16_t adr, uint16_t sz, DebugInfo debugInfo) : Device(name, RAM_DEV, adr, sz, debugInfo, NULL)
{

	// Resize the RAM vector
	mMem.resize((size_t) mDevSz);

	// Initialise RAM with zeros
	mMem.assign(mDevSz, 0);

	if (mDebugInfo.dbgLevel & DBG_VERBOSE)
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

bool RAM::write(uint16_t adr, vector<uint8_t>& data, uint16_t sz)
{
	if (!(adr >= mDevAdr && adr + sz <= mDevAdr + mDevSz))
		return false;

	for (int a = adr; a < adr + sz; a++) {
		if (!write(a, data[a - adr]))
			return false;
	}

	if (mDebugInfo.dbgLevel & DBG_DEVICE)
		cout << "Wrote " << dec << sz << " bytes to RAM at location " << hex << setw(4) << setfill('0') << adr << "\n";

	return true;
}

bool RAM::read(uint16_t adr, vector<uint8_t>& data, uint16_t sz)
{
	if (!(adr >= mDevAdr && adr + sz < mDevAdr + mDevSz))
		return false;

	if (data.size() < sz)
		return false;

	for (int a = adr; a < adr + sz; a++) {
		if (!read(a, data[a - adr]))
			return false;
	}

	return true;
}