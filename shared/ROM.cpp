#include "ROM.h"
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <filesystem>

using namespace std;

ROM::ROM(uint16_t adr, uint16_t sz, string binaryContent, bool verbose) : Device(ROM_DEV, adr, sz, verbose)
{

	ifstream fin(binaryContent, ios::in | ios::binary | ios::ate);

	if (!fin) {
		cout << "couldn't open ROM file " << binaryContent << "\n";
		throw runtime_error("couldn't open ROM file");
	}


	// Get file size (should normally equal ROM size)
	fin.seekg(0, ios::end);
	streamsize file_sz = fin.tellg();
	fin.seekg(0);

	uint16_t upper_sz = sz;
	if (file_sz < (streamsize) sz) {
		cout << "Warning - size of ROM file " << binaryContent << " (" << file_sz <<
			" ) is smaller than the expected one(" << sz << ") => filling up with zeros...\n";
		upper_sz = file_sz;
	} else if (file_sz > (streamsize)sz) {
		cout << "Warning - size of ROM file " << binaryContent << " (" << file_sz <<
			" ) is larger than the expected one(" << sz << ") => truncating...\n";
		mDevSz = 0x10000;
		upper_sz = mDevSz;
	}
	if (file_sz >= 0x10000)
		mDevSz = 0xffff;
	else
		mDevSz = (uint16_t) file_sz;

	// Resize the ROM vector
	mMem.resize((size_t)mDevSz);

	// Initialise ROM with zeros in case the ROM file is smaller than specified ROM size
	mMem.assign(sz, 0);

	// Read ROM content
	fin.read((char*)&mMem[0], upper_sz);

	if (mVerbose) {
		filesystem::path path = binaryContent;
		string file_name = path.filename().string();
		cout << "ROM at address 0x" << hex << setfill('0') << setw(4) << mDevAdr <<
			" to 0x" << mDevAdr + mDevSz - 1 << " (" << dec << mDevSz << " bytes) ['" <<
			file_name << "']\n";
	}

}

bool ROM::read(uint16_t adr, uint8_t& data)
{
	if (!validAdr(adr)) {
		cout << "Invalid read address 0x" << hex << adr << " for ROM at 0x" << mDevAdr << " to " << mDevAdr + mDevSz - 1 << "\n";
		return false;
	}

	data = mMem[adr - mDevAdr];

	return true;

}
bool ROM::write(uint16_t adr, uint8_t data)
{
	if (!validAdr(adr))
		return false;

	mMem[adr - mDevAdr] = data;

	return true;
}