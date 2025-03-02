#include "BeebRomSel.h"
#include <stdexcept>
#include <iostream>
#include <filesystem>

BeebROMSel::BeebROMSel(string name, uint16_t adr, DebugInfo  *debugInfo, ConnectionManager * connectionManager) :
	MemoryMappedDevice(name, BEEB_PAGED_ROM_SEL_DEV, MEMORY_DEVICE, adr, 1, debugInfo, connectionManager)
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
bool BeebROMSel::write(uint16_t adr, uint8_t data)
{
	if (!selected(adr))
		return false;

	mReg = data;
	switch (mReg & 0x3) {
	case 0x3:
		updatePort(NW, 0x0);
		updatePort(NE, 0x1);
		updatePort(SW, 0x1);
		updatePort(SE, 0x1);
		break;
	case 0x2:
		updatePort(NW, 0x1);
		updatePort(NE, 0x0);
		updatePort(SW, 0x1);
		updatePort(SE, 0x1);
		break;
	case 0x1:
		updatePort(NW, 0x1);
		updatePort(NE, 0x1);
		updatePort(SW, 0x0);
		updatePort(SE, 0x1);
		break;
	case 0x0:
		updatePort(NW, 0x1);
		updatePort(NE, 0x1);
		updatePort(SW, 0x1);
		updatePort(SE, 0x0);
		break;
	default:
		break;
	}

	// Call parent class to trigger scheduling of other devices when applicable
	return MemoryMappedDevice::triggerAfterWrite(adr, data);
}

bool BeebROMSel::addROMs(vector<ROM*> &ROMs)
{
	for (int i = 0; i < ROMs.size(); i++) {
		if (mDebugInfo->dbgLevel & DBG_VERBOSE)
			cout << "Adding Paged ROM '" << ROMs[i]->name << "\n";
		mROMs.push_back(ROMs[i]);
	}

	return true;
}