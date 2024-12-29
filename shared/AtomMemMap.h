#ifndef ATOM_MEM_MAP_H
#define ATOM_MEM_MAP_H

#include <vector>
#include "../shared/Device.h"
#include <cstdint>
#include <string>

using namespace std;

const vector<DeviceAllocation> AtomMemMap = {
	{DeviceEnum::RAM_DEV,		0x0000, 0x0400, ""},						// Block Zero RAM
	{DeviceEnum::VDU6847_DEV,	0x0800, 0x0100, ""},						// VDU CRT Controller 6847
	{DeviceEnum::RAM_DEV,		0x2800, 0x5800, ""},						// Program RAM - 22K
	{DeviceEnum::RAM_DEV,		0x8000,	0x1800, ""},						// Graphics RAM - 6K
	{DeviceEnum::ROM_DEV,		0xA000,	0x1000, "Willow.rom"},				// Willow Utility ROM
	{DeviceEnum::PIA8255_DEV,	0xb000,	0x0004, ""},						// PIA 8255
	{DeviceEnum::VIA6522_DEV,	0xb800,	0x0010, ""},						// VIA 6522
	{DeviceEnum::ROM_DEV,		0xc000,	0x1000, "Atom_basic.rom"},			// BASIC ROM
	{DeviceEnum::ROM_DEV,		0xd000,	0x1000, "Atom_FloatingPoint.rom"},	// Floating-point ROM
	{DeviceEnum::ROM_DEV,		0xf000,	0x1000, "Atom_Kernel.rom"},			// CAS ROM
};

#endif