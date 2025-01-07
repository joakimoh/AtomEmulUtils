#include "VDU6847.h"
#include <filesystem>
#include <iostream>
using namespace std;


bool VDU6847::reset()
{
	Device::reset();
	return true;
}

bool VDU6847::advance(uint64_t stopCycle)
{
	return true;
}

//
// 
// Can be in either Alphanumeric (major mode 1) or Graphic mode (major mode 2)
// 
// 
// Supported Alphanumeric (major mode 1) modes:
// 
// INT/EXT	INV
// 0		0		Internal alphanumerics (standard 5 x 7 dot matrix characters)
// 0		1		Internal alphanumerics inverted (standard 5 x 7 dot matrix characters - inverted)
// 1		0		External alphanumerics (external 8 x 12 dot matrix characters)
// 1		1		External alphanumerics inverted (external 8 x 12 dot matrix characters - inverted)
// 
// For the Acorn Atom, INV is connected to b7 of read graphics memory data, INT/EXT to b6. But Acorn Atom
// doesn't have external aplhanumerics.
// 
// Supported Graphic (Major Mode 2) modes: 
//
// Mode	resolution	#colours	memory usage		description
// 0	64 x 64		4			1kB					colour graphics (CG1)
// 1	128 x 64	2			1kB					resolution graphics one (RG1)
// 2	128 x 64	4			2kB					colour graphics two (CG2)
// 3	128 x 96	2			1.5kB				resolution graphics two (RG2)
// 4	128 x 96	4			3kB					colour graphics three (CG3)
// 5	128 x 192	2			3kB					resolution graphics three (RG3)
// 6	128 x 192	4			6kB					colour graphics six (CG6)
// 7	256 x 192	2			6kB					resolution graphics six (RG6)
//
bool VDU6847::setGraphicMode(uint8_t mode)
{
	mMajorMode = (mode >> 3) & 0x1;
	mGraphicMode = mode & 0x7;
	return true;
}

bool VDU6847::setVideoRam(RAM* ram)
{
	mVideoMem = ram;
	return true;
}

VDU6847::VDU6847(uint16_t adr, uint16_t videoMemAdr, DebugInfo debugInfo) : 
	Device(VDU6847_DEV, adr, 0x100, debugInfo), mVideoMemAdr(videoMemAdr)
{
	// Set the size of the VDU register vector
	mMem.resize((size_t) mDevSz);

	// Initialise the VDU registers with zeros
	mMem.assign(mDevSz, 0);

	if (mDebugInfo.verbose)
		cout << "VDU 6847 at address 0x" << hex << setfill('0') << setw(4) << mDevAdr <<
		" to 0x" << mDevAdr + mDevSz - 1 << " (" << dec << mDevSz << " bytes)\n";
}



bool VDU6847::read(uint16_t adr, uint8_t& data)
{

	if (!validAdr(adr))
		return false;

	data = mMem[adr - mDevAdr];

	return true;

}
bool VDU6847::write(uint16_t adr, uint8_t data)
{

	if (!validAdr(adr))
		return false;

	mMem[adr - mDevAdr] = data;

	return true;
}
