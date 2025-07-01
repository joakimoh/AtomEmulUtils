#include "BeebRomSel.h"
#include <stdexcept>
#include <iostream>
#include <filesystem>

//
// Emulates IC20 (74LS139) and IC76 (74LS163) on the BBC Model B.
// 
// These circuits together implements an addressable 4-bit latch that selects 16KB sideways ROMS and RAMs.
// There are 16 slots but on an unmodified BBC Micro Model B, only the upper slots (12-15) can be selected.
// These on board sideways ROM sockets are IC52 (slot 12), IC88 (slot 13), IC100 (slot 14), IC101 (slot 15),
// left to right on the board after the OS socket.
//

BeebROMSel::BeebROMSel(string name, double cpuClock, uint8_t waitStates, uint16_t adr, DebugManager  *debugManager, ConnectionManager * connectionManager) :
	MemoryMappedDevice(name, BEEB_PAGED_ROM_SEL_DEV, MEMORY_DEVICE, cpuClock, waitStates, adr, 1, debugManager, connectionManager)
{
	registerPort("NW", OUT_PORT, 0x1, NW, &mNW);
	registerPort("NE", OUT_PORT, 0x1, NE, &mNE);
	registerPort("SW", OUT_PORT, 0x1, SW, &mSW);
	registerPort("SE", OUT_PORT, 0x1, SE, &mSE);

}


bool BeebROMSel::read(uint16_t adr, uint8_t& data)
{

	// Call parent class to trigger scheduling of other devices when applicable
	if (!MemoryMappedDevice::triggerBeforeRead(adr, data))
		return false;

	data = mReg;

	return true;

}

bool BeebROMSel::dump(uint16_t adr, uint8_t& data)
{
	if (selected(adr)) {
		data = mReg;
		return true;
	}
	return false;
}

bool BeebROMSel::write(uint16_t adr, uint8_t data)
{
	if (!selected(adr))
		return false;

	uint8_t p_reg = mReg & 0x3;
	uint8_t reg = data & 0x3;
	mReg = data;
	switch (mReg & 0x3) {
	case 0x3:
		updatePort(NW, 0x0);
		updatePort(NE, 0x1);
		updatePort(SW, 0x1);
		updatePort(SE, 0x1);
		DBG_LOG_COND(reg != p_reg, this, DBG_VERBOSE, "ROMSel: Slot 12 NW - IC52 (Normally DFS) Selected\n");
		break;
	case 0x2:
		updatePort(NW, 0x1);
		updatePort(NE, 0x0);
		updatePort(SW, 0x1);
		updatePort(SE, 0x1);
		DBG_LOG_COND(reg != p_reg, this, DBG_VERBOSE, "ROMSel: Slot 13 NE - IC88 (Normally Aux ROM) Selected\n");
		break;
	case 0x1:
		updatePort(NW, 0x1);
		updatePort(NE, 0x1);
		updatePort(SW, 0x0);
		updatePort(SE, 0x1);
		DBG_LOG_COND(reg != p_reg, this, DBG_VERBOSE, "ROMSel: Slot 14 SW- IC100 (Normally Aux ROM) Selected\n");
		break;
	case 0x0:
		updatePort(NW, 0x1);
		updatePort(NE, 0x1);
		updatePort(SW, 0x1);
		updatePort(SE, 0x0);
		DBG_LOG_COND(reg != p_reg, this, DBG_VERBOSE, "ROMSel: Slot 15 SE - IC101 (Normally BASIC) Selected\n");
		break;
	default:
		break;
	}

	// Call parent class to trigger scheduling of other devices when applicable
	return MemoryMappedDevice::triggerAfterWrite(adr, data);
}