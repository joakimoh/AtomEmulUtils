#include "MemoryMappedDevice.h"
#include <iostream>
#include <iomanip>
#include "DebugManager.h"
#include "Utility.h"

using namespace std;

MemoryMappedDevice::MemoryMappedDevice(
	string name, DeviceId typ, DeviceCategory cat, double cpuClock, uint8_t waitStates, uint16_t adr, uint16_t sz, DebugManager  *debugManager, ConnectionManager* connectionManager
): Device(name, typ, cat, cpuClock, debugManager, connectionManager), mWaitStates(waitStates)
{
	mMemorySpace = { adr, sz };
	mMemoryMapped = true;

	if (DBG_LEVEL(DBG_VERBOSE))
		cout << _DEVICE_ID(this->devType) << " (" << this->name << ") at address 0x" << hex << setfill('0') << setw(4) << mMemorySpace.adr <<
		" to 0x" << mMemorySpace.adr + mMemorySpace.sz - 1 << " (" << dec << mMemorySpace.sz << " bytes)\n";
}

bool MemoryMappedDevice::selected(uint16_t adr)
{
	bool valid = true;

	if (adr < mMemorySpace.adr || adr >= mMemorySpace.adr + mMemorySpace.sz)
		return false;
	for (int i = 0; i < mMemoryGaps.size(); i++) {
		if (adr >= mMemoryGaps[i].adr && adr < mMemoryGaps[i].adr + mMemoryGaps[i].sz) {
			return false;		
		}
	}

	return true;
}

void MemoryMappedDevice::addMemoryGap(uint16_t adr, uint16_t sz)
{
	MemoryRange gap = { adr, sz };
	mMemoryGaps.push_back(gap);
	if (DBG_LEVEL(DBG_VERBOSE))
		cout << "Gap in memory space for device '" << this->name << "' between " << hex << setfill('0') << setw(4) << gap.adr <<
			" and " << gap.adr + gap.sz << "\n";
}

bool MemoryMappedDevice::read(uint16_t adr, uint8_t& data) {
	data = 0xff;
	return selected(adr);
}

bool MemoryMappedDevice::triggerBeforeRead(uint16_t adr, uint8_t data)
{
	if (!selected(adr)) {
		//cout << "READ ADDRESS 0x" << hex << adr << " DOESN'T BELONG TO DEVICE '" << this->name << "\n";
		return false;
	}
	for (int i = 0; i < mScheduleOnRead.size(); i++) {
		if (mScheduleOnRead[i].triggeringAdr == adr) {
			DBG_LOG(this, DBG_TRGGERING, "READ 0x" + Utility::int2hexStr(adr, 4) +  " => triggers " +
					mScheduleOnRead[i].device->name + " at " + to_string(mCycleCount) + "\n");
			mScheduleOnRead[i].device->advance(mCycleCount); // align the triggered device's time with this device
		}
	}
	return true;

}

bool MemoryMappedDevice::triggerAfterWrite(uint16_t adr, uint8_t data)
{
	if (!selected(adr)) {
		//cout << "READ ADDRESS 0x" << hex << adr << " DOESN'T BELONG TO DEVICE '" << this->name << "\n";
		return false;
	}
	for (int i = 0; i < mScheduleOnWrite.size(); i++) {
		if (mScheduleOnWrite[i].triggeringAdr == adr) {
			DBG_LOG(this, DBG_TRGGERING, "WRITE 0x" + Utility::int2hexStr(data,2) + " TO 0x" + Utility::int2hexStr(adr,4) + " => triggers " +
					mScheduleOnWrite[i].device->name + " at " + to_string(mCycleCount) + "\n");
			mScheduleOnWrite[i].device->advance(mCycleCount); // align the triggered device's time with this device
		}
	}
	return true;
}

bool MemoryMappedDevice::write(uint16_t adr, uint8_t data) {

	return selected(adr);
}


bool MemoryMappedDevice::registerAccess(Device* dev, uint16_t adr, bool writeAccess) {
	DeviceAccessScheduling dev_sch = { dev, adr };
	if (writeAccess)
		mScheduleOnWrite.push_back(dev_sch);
	else
		mScheduleOnRead.push_back(dev_sch);
	if (DBG_LEVEL(DBG_VERBOSE))
		cout << "TRIGGER " << dev->name << " ON " << this->name << " 0x" << hex << adr << (writeAccess ? " WRITE" : " READ") << " ACCESSES\n";
	return true;
}