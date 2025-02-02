#include "ROM.h"
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <filesystem>

using namespace std;

ROM::ROM(string name, uint16_t adr, uint16_t sz, string binaryContent, DebugInfo debugInfo) : MemoryMappedDevice(name, ROM_DEV, MEMORY_DEVICE, adr, sz, debugInfo, NULL)
{

	ifstream fin(binaryContent, ios::in | ios::binary | ios::ate);

	if (!fin) {
		cout << "couldn't open ROM file " << binaryContent << "\n";
		throw runtime_error("couldn't open ROM file");
	}

	if (sz >= 0x10000)
		mDevSz = 0xffff;
	else
		mDevSz = sz;

	// Get file size (should normally equal ROM size)
	fin.seekg(0, ios::end);
	streamsize file_sz = fin.tellg();
	fin.seekg(0);

	uint16_t upper_sz = mDevSz;
	if (file_sz < (streamsize) sz) {
		if (mDebugInfo.dbgLevel & DBG_WARNING)
			cout << "Warning - size of ROM file " << binaryContent << " (" << file_sz <<
			" ) is smaller than the expected one(" << sz << ") => filling up with zeros...\n";
		upper_sz = (uint16_t) file_sz;
	} else if (file_sz > (streamsize)sz) {
		if (mDebugInfo.dbgLevel & DBG_WARNING)
			cout << "Warning - size of ROM file " << binaryContent << " (" << file_sz <<
			" ) is larger than the expected one(" << sz << ") => truncating...\n";
		upper_sz = mDevSz;
	}


	// Resize the ROM vector
	mMem.resize((size_t)mDevSz);

	// Initialise ROM with zeros in case the ROM file is smaller than specified ROM size
	mMem.assign(mDevSz, 0);

	// Read ROM content
	fin.read((char*)&mMem[0], upper_sz);

	if (mDebugInfo.dbgLevel & DBG_VERBOSE) {
		filesystem::path path = binaryContent;
		string file_name = path.filename().string();
		if (mDebugInfo.dbgLevel & DBG_ALL)
			cout << "ROM at address 0x" << hex << setfill('0') << setw(4) << mDevAdr <<
			" to 0x" << mDevAdr + mDevSz - 1 << " (" << dec << mDevSz << " bytes) ['" <<
			file_name << "']\n";
	}

}

bool ROM::read(uint16_t adr, uint8_t& data)
{
	// Call parent class to trigger scheduling of other devices when applicable
	if (!MemoryMappedDevice::read(adr, data))
		return false;

	data = mMem[adr - mDevAdr];

	return true;

}
bool ROM::write(uint16_t adr, uint8_t data)
{
	// Call parent class to trigger scheduling of other devices when applicable
	if (!MemoryMappedDevice::write(adr, data))
		return false;

	mMem[adr - mDevAdr] = data;

	return true;
}