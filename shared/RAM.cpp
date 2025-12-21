#include "RAM.h"
#include <stdexcept>
#include <iostream>
#include <filesystem>

RAM::RAM(string name, double clockSpeed, uint8_t waitStates, bool DRAM, uint16_t adr, uint16_t sz, DebugManager  *debugManager,
	ConnectionManager* connectionManager, DeviceManager* deviceManager) :
	MemoryMappedDevice(name, RAM_DEV, MEMORY_DEVICE, clockSpeed, waitStates, adr, sz, debugManager, connectionManager, deviceManager)
{

	registerPort("CS", IN_PORT, 0x1, CS, &mCS);

	// Resize the RAM vector
	mMem.resize((size_t) mAddressSpace.getSizeOfSpace());

	// Initialise RAM
	if (DRAM) {
		// Initialise RAM with zeros (for DRAM)
		mMem.assign(mAddressSpace.getSizeOfSpace(), 0);
	}
	else {
		// Initialise RAM with random values (for static RAM)
		// Required for the Acorn Atom as its BASIC random no generator will fail
		// otherwise (as the seed is coming from the RAM assuming it has an initial
		// random content).
		for (int i = 0; i < mAddressSpace.getSizeOfSpace(); i++)
			mMem[i] = rand() % 256;
	}

}


bool RAM::read(uint16_t adr, uint8_t& data)
{

	// Call parent class to trigger scheduling of other devices when applicable
	if (mTriggerOnRead) {
		if (!MemoryMappedDevice::triggerBeforeRead(adr, data) || mCS != 0)
			return false;
	}
	
	data = mMem[adr - mStartOfSpace];

	return true;

}

bool RAM::dump(uint16_t adr, uint8_t& data)
{
	if (selected(adr))
		data = mMem[adr - mStartOfSpace];
	else
		data = 0x0;

	return true;
}

bool RAM::write(uint16_t adr, uint8_t data)
{

	if (!selected(adr) || mCS != 0)
		return false;

	mMem[adr - mStartOfSpace] = data;

	// Call parent class to trigger scheduling of other devices when applicable
	return !mTriggerOnWrite || MemoryMappedDevice::triggerAfterWrite(adr, data);
}

bool RAM::write(uint16_t adr, vector<uint8_t>& data, uint16_t sz)
{
	if (!mAddressSpace.contains(adr))
		return false;

	for (int a = adr; a < adr + sz; a++) {
		if (!write(a, data[a - adr]))
			return false;
	}

	return true;
}

bool RAM::read(uint16_t adr, vector<uint8_t>& data, uint16_t sz)
{
	if (!mAddressSpace.contains(adr))
		return false;

	if (data.size() < sz)
		return false;

	for (int a = adr; a < adr + sz; a++) {
		if (!read(a, data[a - adr]))
			return false;
	}

	return true;
}