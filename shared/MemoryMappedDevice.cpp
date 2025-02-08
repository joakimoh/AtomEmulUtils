#include "MemoryMappedDevice.h"
#include <iostream>
#include <iomanip>
#include "DebugInfo.h"

using namespace std;

MemoryMappedDevice::MemoryMappedDevice(
	string name, DeviceId typ, DeviceCategory cat, uint16_t adr, uint16_t sz, DebugInfo debugInfo, ConnectionManager* connectionManager
): Device(name, typ, cat, debugInfo, connectionManager)
{
	mMemorySpace = { adr, sz };
	mMemoryMapped = true;

	if (mDebugInfo.dbgLevel & DBG_VERBOSE)
		cout << _DEVICE_ID(this->devType) << " (" << this->name << ") at address 0x" << hex << setfill('0') << setw(4) << mMemorySpace.adr <<
		" to 0x" << mMemorySpace.adr + mMemorySpace.sz - 1 << " (" << dec << mMemorySpace.sz << " bytes)\n";
}

bool MemoryMappedDevice::selected(uint16_t adr)
{
	bool valid = true;
	if (adr < mMemorySpace.adr || adr >= mMemorySpace.adr + mMemorySpace.sz)
		return false;
	for (int i = 0; i << mMemoryGaps.size(); i++) {
		if (adr < mMemoryGaps[i].adr || adr >= mMemoryGaps[i].adr + mMemoryGaps[i].sz)
		return false;
	}
	return true;
}

void MemoryMappedDevice::addMemoryGap(uint16_t adr, uint16_t sz)
{
	MemoryRange gap = { adr, sz };
	mMemoryGaps.push_back(gap);
	if (mDebugInfo.dbgLevel & DBG_VERBOSE)
		cout << "Gap in memory space for device '" << this->name << "' between " << hex << setfill('0') << setw(4) << adr <<
			" and " << adr + sz << "\n";
}

bool MemoryMappedDevice::read(uint16_t adr, uint8_t& data) {
	if (!selected(adr)) {
		//cout << "READ ADDRESS 0x" << hex << adr << " DOESN'T BELONG TO DEVICE '" << this->name << "\n";
		return false;
	}
	for (int i = 0; i < mScheduleOnRead.size(); i++) {
		if (mScheduleOnRead[i].triggeringAdr == adr) {
			if (mDebugInfo.dbgLevel & DBG_DEVICE)
				cout << "READ 0x" << hex << adr << dec << " => triggers " << mScheduleOnRead[i].device->name << " at " << dec << mCycleCount << "\n";
			mScheduleOnRead[i].device->advance(mCycleCount); // align the triggered device's time with this device
		}
	}
	return true;
}

bool MemoryMappedDevice::write(uint16_t adr, uint8_t data) {
	if (!selected(adr))
		return false;
	for (int i = 0; i < mScheduleOnWrite.size(); i++) {
		if (mScheduleOnWrite[i].triggeringAdr == adr) {
			mScheduleOnWrite[i].device->advance(mCycleCount); // align the triggered device's time with this device
			if (mDebugInfo.dbgLevel & DBG_DEVICE)
				cout << "WRITE 0x" << hex << data << " TO 0x" << adr << " => triggers " << mScheduleOnRead[i].device->name << " at " << dec << mCycleCount << "\n";
		}
	}
	return true;
}


bool MemoryMappedDevice::registerAccess(Device* dev, uint16_t adr, bool writeAccess) {
	DeviceAccessScheduling dev_sch = { dev, adr };
	if (writeAccess)
		mScheduleOnWrite.push_back(dev_sch);
	else
		mScheduleOnRead.push_back(dev_sch);
	if (mDebugInfo.dbgLevel & DBG_VERBOSE)
		cout << "TRIGGER " << dev->name << " ON " << this->name << " 0x" << hex << adr << (writeAccess ? " WRITE" : " READ") << " ACCESSES\n";
	return true;
}