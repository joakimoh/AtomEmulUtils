#include "BeebRomSel.h"
#include <stdexcept>
#include <iostream>
#include <filesystem>
#include "Utility.h"

//
// Emulates IC20 (74LS139) and IC76 (74LS163) on the BBC Model B.
// 
// These circuits together implement an addressable 4-bit latch that selects 16KB sideways ROMS and RAMs.
// There are 16 slots but on an unmodified BBC Micro Model B, only the upper slots (12-15) can be selected.
// These on board sideways ROM sockets are IC52 (slot 12), IC88 (slot 13), IC100 (slot 14), IC101 (slot 15),
// left to right on the board after the OS socket.
//

BeebROMSel::BeebROMSel(string name, double tickRate, uint8_t waitStates, uint16_t adr, DebugTracing  *debugTracing, ConnectionManager * connectionManager,
	DeviceManager *deviceManager) :
	MemoryMappedDevice(name, BEEB_PAGED_ROM_SEL_DEV, OTHER_DEVICE, waitStates, adr, 1, debugTracing, connectionManager, deviceManager),
	TimedDevice(tickRate)
{
	registerPort("NW", OUT_PORT, 0x1, NW, &mNW);
	registerPort("NE", OUT_PORT, 0x1, NE, &mNE);
	registerPort("SW", OUT_PORT, 0x1, SW, &mSW);
	registerPort("SE", OUT_PORT, 0x1, SE, &mSE);
	registerPort("B0", OUT_PORT, 0xf, B0, &mB0);
	registerPort("B1", OUT_PORT, 0xf, B1, &mB1);
	registerPort("B2", OUT_PORT, 0xf, B2, &mB2);

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

	uint8_t p_reg = mReg;
	mReg = data & 0xf;

	// Select a bank (default is bank #3)
	uint16_t bank_sel = ~(1 << mReg);
	updatePort(B0, bank_sel & 0xf);
	updatePort(B1, (bank_sel >> 4) & 0xf);
	updatePort(B2, (bank_sel >> 8) & 0xf);
	updatePort(NW, (bank_sel >> 12) & 0x1);
	updatePort(NE, (bank_sel >> 13) & 0x1);
	updatePort(SW, (bank_sel >> 14) & 0x1);
	updatePort(SE, (bank_sel >> 15) & 0x1);
	DBG_LOG_COND(mReg != p_reg, this, DBG_IO_PERIPHERAL, "ROMSel: Slot " + to_string(mReg) + " selected => Bank Selection " + Utility::int2BinStr(bank_sel, 16) + "\n");

	// Call parent class to trigger scheduling of other devices when applicable
	return MemoryMappedDevice::triggerAfterWrite(adr, data);
}

// Outputs the internal state of the device
bool BeebROMSel::outputState(ostream& sout)
{
	uint16_t bank_sel = ~(1 << mReg);
	sout << "Bank = 0x" << hex << (int)mReg << " <=>\n";
	sout << "\tSelection strobe = " << Utility::int2BinStr(bank_sel, 16) << "\n";
	sout << "\tNW =               " << (int)mNW << "\n";
	sout << "\tNE =               " << (int)mNE << "\n";
	sout << "\tSW =               " << (int)mSW << "\n";
	sout << "\tSE =               " << (int)mSE << "\n";

	return true;
}