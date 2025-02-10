#include "BeebROMSel.h"
#include <stdexcept>
#include <iostream>
#include <filesystem>

BeebROMSel::BeebROMSel(string name, uint16_t adr, DebugInfo debugInfo, ConnectionManager * connectionManager) :
	MemoryMappedDevice(name, BEEB_PAGED_ROM_SEL_DEV, MEMORY_DEVICE, adr, 1, debugInfo, connectionManager)
{
	registerPort("SEL_H", OUT_PORT, 0xff, SEL_H, &mSEL_H);
	registerPort("SEL_L", OUT_PORT, 0xff, SEL_L, &mSEL_L);
}


bool BeebROMSel::read(uint16_t adr, uint8_t& data)
{

	// Call parent class to trigger scheduling of other devices when applicable
	if (!MemoryMappedDevice::read(adr, data))
		return false;

	data = mReg;

	cout << "READ 0x" << hex << data << " FROM ROMSEL REGISTER!\n";

	return true;

}
bool BeebROMSel::write(uint16_t adr, uint8_t data)
{
	// Call parent class to trigger scheduling of other devices when applicable
	if (!MemoryMappedDevice::write(adr, data))
		return false;

	mReg = data;
	uint16_t sel = 1 << mReg;
	uint8_t sel_L = sel & 0xff;
	uint8_t sel_H = (sel >> 8) & 0xff;
	updatePort(SEL_L, sel_L);
	updatePort(SEL_H, sel_H);

	return true;
}

bool BeebROMSel::addROMs(vector<BeebPagedROM*> &ROMs)
{
	for (int i = 0; i < ROMs.size(); i++) {
		if (mDebugInfo.dbgLevel & DBG_VERBOSE)
			cout << "Adding Paged ROM '" << ROMs[i]->name << " at slot #" << ROMs[i]->getSlot() << "\n";
		mROMs.push_back(ROMs[i]);
	}

	return true;
}