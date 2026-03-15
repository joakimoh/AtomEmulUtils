#include "RAM.h"
#include <stdexcept>
#include <iostream>
#include <filesystem>

RAM::RAM(string name, uint8_t waitStates, bool DRAM, BusAddress adr, BusAddress sz, DebugTracing  *debugTracing,
	ConnectionManager* connectionManager, DeviceManager* deviceManager) :
	MemoryMappedDevice(name, RAM_DEV, MEMORY_DEVICE, waitStates, adr, sz, debugTracing, connectionManager, deviceManager)
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
		for (BusAddress i = 0; i < mAddressSpace.getSizeOfSpace(); i++)
			mMem[i] = rand() % 256;
	}

}


bool RAM::readByte(BusAddress adr, BusByte& data)
{

	// Call parent class to trigger scheduling of other devices when applicable
	if (mTriggerOnRead) {
		if (!MemoryMappedDevice::triggerBeforeRead(adr, data) || mCS != 0)
			return false;
	}
	
	data = mMem[adr - mStartOfSpace];

	return true;

}

bool RAM::dump(BusAddress adr, uint8_t& data)
{
	if (selected(adr))
		data = mMem[adr - mStartOfSpace];
	else
		data = 0x0;

	return true;
}

bool RAM::writeByte(BusAddress adr, BusByte data)
{

	if (!selected(adr) || mCS != 0)
		return false;

	mMem[adr - mStartOfSpace] = data;

	// Call parent class to trigger scheduling of other devices when applicable
	return !mTriggerOnWrite || MemoryMappedDevice::triggerAfterWrite(adr, data);
}

bool RAM::writeBytes(BusAddress adr, vector<BusByte>& data, BusAddress sz)
{
	if (!mAddressSpace.contains(adr))
		return false;

	for (BusAddress a = adr; a < adr + sz; a++) {
		if (!writeByte(a, data[a - adr]))
			return false;
	}

	return true;
}

bool RAM::readBytes(BusAddress adr, vector<BusByte>& data, BusAddress sz)
{
	if (!mAddressSpace.contains(adr))
		return false;

	if (data.size() < sz)
		return false;

	for (BusAddress a = adr; a < adr + sz; a++) {
		if (!readByte(a, data[a - adr]))
			return false;
	}

	return true;
}


// Outputs the internal state of the device
bool RAM::outputState(ostream& sout)
{
	sout << "CS = " << (int)mCS << "\n";

	return true;
}