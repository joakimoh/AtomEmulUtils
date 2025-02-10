#include "BeebPagedROM.h"
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <filesystem>

using namespace std;

BeebPagedROM::BeebPagedROM(string name, int slot, uint16_t adr, uint16_t sz, string binaryContent, DebugInfo debugInfo, ConnectionManager * connectionManager) :
	MemoryMappedDevice(name, BEEB_PAGED_ROM_DEV, MEMORY_DEVICE, adr, sz, debugInfo, connectionManager), mSlot(slot)
{

	registerPort("SEL", IN_PORT, 0x1, SEL, &mSEL);

	ifstream fin(binaryContent, ios::in | ios::binary | ios::ate);

	if (!fin) {
		cout << "couldn't open Beeb Paged ROM file " << binaryContent << "\n";
		throw runtime_error("couldn't open BeebPagedROM file");
	}

	if (sz >= 0x10000)
		mMemorySpace.sz = 0xffff;
	else
		mMemorySpace.sz = sz;

	// Get file size (should normally equal BeebPagedROM size)
	fin.seekg(0, ios::end);
	streamsize file_sz = fin.tellg();
	fin.seekg(0);

	uint16_t upper_sz = mMemorySpace.sz;
	if (file_sz < (streamsize)sz) {
		if (mDebugInfo.dbgLevel & DBG_WARNING)
			cout << "Warning - size of Beeb Paged ROM file " << binaryContent << " (" << file_sz <<
			" ) is smaller than the expected one(" << sz << ") => filling up with zeros...\n";
		upper_sz = (uint16_t)file_sz;
	}
	else if (file_sz > (streamsize)sz) {
		if (mDebugInfo.dbgLevel & DBG_WARNING)
			cout << "Warning - size of Beeb Paged ROM file " << binaryContent << " (" << file_sz <<
			" ) is larger than the expected one(" << sz << ") => truncating...\n";
		upper_sz = mMemorySpace.sz;
	}


	// Resize the BeebPagedROM vector
	mMem.resize((size_t)mMemorySpace.sz);

	// Initialise BeebPagedROM with zeros in case the BeebPagedROM file is smaller than specified BeebPagedROM size
	mMem.assign(mMemorySpace.sz, 0);

	// Read BeebPagedROM content
	fin.read((char*)&mMem[0], upper_sz);

	if (mDebugInfo.dbgLevel & DBG_VERBOSE) {
		filesystem::path path = binaryContent;
		string file_name = path.filename().string();
		if (mDebugInfo.dbgLevel & DBG_ALL)
			cout << "Beeb Paged ROM file was '" << file_name << "'\n";
	}

}

bool BeebPagedROM::read(uint16_t adr, uint8_t& data)
{
	// Call parent class to trigger scheduling of other devices when applicable
	if (!MemoryMappedDevice::read(adr, data) || !mSEL)
		return false;

	data = mMem[adr - mMemorySpace.adr];

	//cout << "READ Beeb Paged ROM AT 0x" << hex << adr << " => 0x" << (int) data << "\n";

	return true;

}
bool BeebPagedROM::write(uint16_t adr, uint8_t data)
{
	return false; // Beeb Paged ROM is read-only!
}