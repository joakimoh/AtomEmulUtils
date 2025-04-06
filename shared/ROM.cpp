#include "ROM.h"
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <filesystem>

using namespace std;

ROM::ROM(string name, double clockSpeed, uint8_t waitStates, uint16_t adr, uint16_t sz, string binaryContent, DebugManager  *debugManager, ConnectionManager* connectionManager) :
	MemoryMappedDevice(name, ROM_DEV, MEMORY_DEVICE, clockSpeed, waitStates, adr, sz, debugManager, connectionManager)
{
	registerPort("CS", IN_PORT, 0x1, CS, &mCS);

	ifstream fin(binaryContent, ios::in | ios::binary | ios::ate);

	if (!fin) {
		cout << "couldn't open Beeb Paged ROM file " << binaryContent << "\n";
		throw runtime_error("couldn't open ROM file");
	}

	if (sz >= 0x10000)
		mMemorySpace.sz = 0xffff;
	else
		mMemorySpace.sz = sz;

	// Get file size (should normally equal ROM size)
	fin.seekg(0, ios::end);
	streamsize file_sz = fin.tellg();
	fin.seekg(0);

	uint16_t upper_sz = mMemorySpace.sz;
	if (file_sz < (streamsize)sz) {
		if (mDM->debug(DBG_WARNING))
			cout << "Warning - size of Beeb Paged ROM file " << binaryContent << " (" << file_sz <<
			" ) is smaller than the expected one(" << sz << ") => filling up with zeros...\n";
		upper_sz = (uint16_t)file_sz;
	}
	else if (file_sz > (streamsize)sz) {
		if (mDM->debug(DBG_WARNING))
			cout << "Warning - size of Beeb Paged ROM file " << binaryContent << " (" << file_sz <<
			" ) is larger than the expected one(" << sz << ") => truncating...\n";
		upper_sz = mMemorySpace.sz;
	}


	// Resize the ROM vector
	mMem.resize((size_t)mMemorySpace.sz);

	// Initialise ROM with zeros in case the ROM file is smaller than specified ROM size
	mMem.assign(mMemorySpace.sz, 0);

	// Read ROM content
	fin.read((char*)&mMem[0], upper_sz);

	if (mDM->debug(DBG_VERBOSE)) {
		filesystem::path path = binaryContent;
		string file_name = path.filename().string();
		if (mDM->debug(DBG_ALL))
		mDM->log(this, DBG_ALL, "Beeb Paged ROM file was '" + file_name + "'\n");
	}

}

bool ROM::read(uint16_t adr, uint8_t& data)
{
	// Call parent class to trigger scheduling of other devices when applicable
	if (!MemoryMappedDevice::triggerBeforeRead(adr, data) || mCS != 0)
		return false;

	data = mMem[adr - mMemorySpace.adr];

	return true;

}
bool ROM::write(uint16_t adr, uint8_t data)
{
	return false; // ROM is read-only!
}