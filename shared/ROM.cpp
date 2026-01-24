#include "ROM.h"
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <filesystem>

using namespace std;

ROM::ROM(string name, double clockSpeed, uint8_t waitStates, uint16_t adr, uint16_t sz, string binaryContent, DebugTracing  *debugTracing,
	ConnectionManager* connectionManager, DeviceManager* deviceManager) :
	MemoryMappedDevice(name, ROM_DEV, MEMORY_DEVICE, clockSpeed, waitStates, adr, sz, debugTracing, connectionManager, deviceManager)
{
	registerPort("CS", IN_PORT, 0x1, CS, &mCS);

	ifstream fin(binaryContent, ios::in | ios::binary | ios::ate);

	if (!fin) {
		DBG_LOG(this, DBG_ERROR, "couldn't open ROM file " + binaryContent);
		throw runtime_error("couldn't open ROM file");
	}

	// Get file size (should normally equal ROM size)
	fin.seekg(0, ios::end);
	streamsize file_sz = fin.tellg();
	fin.seekg(0);

	uint16_t upper_sz = mAddressSpace.getSizeOfSpace();
	if (file_sz < (streamsize)sz) {
		DBG_LOG(
			this, DBG_WARNING, "Warning - size of ROM file " + binaryContent + " (" + to_string(file_sz) +
			" ) is smaller than the expected one(" + to_string(sz) + ") => filling up with zeros..."
		);
		upper_sz = (uint16_t)file_sz;
	}
	else if (file_sz > (streamsize)sz) {
		DBG_LOG(
			this, DBG_WARNING, "Warning - size of ROM file " + binaryContent + " (" + to_string(file_sz) +
			" ) is larger than the expected one(" + to_string(sz) + ") => truncating..."
		);
		upper_sz = mAddressSpace.getSizeOfSpace();
	}


	// Resize the ROM vector
	mMem.resize((size_t)mAddressSpace.getSizeOfSpace());

	// Initialise ROM with zeros in case the ROM file is smaller than specified ROM size
	mMem.assign(mAddressSpace.getSizeOfSpace(), 0);

	// Read ROM content
	fin.read((char*)&mMem[0], upper_sz);

	if (VERBOSE_OUTPUT) {
		filesystem::path path = binaryContent;
		string file_name = path.filename().string();
		cout << "ROM '" << name << "' file was '" << file_name << "'\n";
	}

}

bool ROM::read(uint16_t adr, uint8_t& data)
{
	// Call parent class to trigger scheduling of other devices when applicable
	if (!MemoryMappedDevice::triggerBeforeRead(adr, data) || mCS != 0)
		return false;

	data = mMem[adr - mStartOfSpace];

	return true;

}

bool ROM::dump(uint16_t adr, uint8_t& data)
{
	if (selected(adr)) {
		data = mMem[adr - mStartOfSpace];
		return true;
	}
	return false;
}

bool ROM::write(uint16_t adr, uint8_t data)
{
	return false; // ROM is read-only!
}