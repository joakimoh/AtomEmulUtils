#include "MemoryMappedDevice.h"
#include <iostream>
#include <iomanip>
#include "DebugManager.h"
#include "DeviceManager.h"
#include "Utility.h"

using namespace std;

MemoryMappedDevice::MemoryMappedDevice(
	string name, DeviceId typ, DeviceCategory cat, double cpuClock, uint8_t waitStates, uint16_t adr, uint16_t sz, DebugManager  *debugManager,
	ConnectionManager* connectionManager, DeviceManager* deviceManager
): Device(name, typ, cat, cpuClock, debugManager, connectionManager), mWaitStates(waitStates), mDeviceManager(deviceManager)
{
	mMemorySpace = { adr, sz };
	mMemoryMapped = true;

	mDeviceManager->registerMemorySpace(this, adr, sz);

	DBG_LOG(
		this, DBG_VERBOSE, _DEVICE_ID(this->devType) +  " at address 0x"s + Utility::int2HexStr(mMemorySpace.adr,4) +
		" to 0x" + Utility::int2HexStr(mMemorySpace.adr + mMemorySpace.sz - 1, 4) + " (" + to_string(mMemorySpace.sz) + " bytes)"
	);
}

bool MemoryMappedDevice::selected(uint16_t adr)
{
	bool valid = true;

	if (mCS != 0 || adr < mMemorySpace.adr || adr >= mMemorySpace.adr + mMemorySpace.sz)
		return false;

	for (int i = 0; i < mMemoryGaps.size(); i++) {
		if (adr >= mMemoryGaps[i].adr && adr < mMemoryGaps[i].adr + mMemoryGaps[i].sz) {
			return false;		
		}
	}

	return true;
}

void MemoryMappedDevice::registerMemoryGap(uint16_t adr, uint16_t sz)
{
	MemoryRange gap = { adr, sz };
	mMemoryGaps.push_back(gap);
	if (DBG_LEVEL_DEV(this,DBG_VERBOSE))
		cout << "Gap in memory space for device '" << this->name << "' between " << hex << setfill('0') << setw(4) << gap.adr <<
			" and " << gap.adr + gap.sz << "\n";

	mDeviceManager->registerMemoryGap(this, adr, sz);
}

bool MemoryMappedDevice::read(uint16_t adr, uint8_t& data) {
	data = 0xff;
	return selected(adr);
}

bool MemoryMappedDevice::triggerBeforeRead(uint16_t adr, uint8_t data)
{
	if (!mTriggerOnRead)
		return true;

	if (!selected(adr)) {
		//cout << "READ ADDRESS 0x" << hex << adr << " DOESN'T BELONG TO DEVICE '" << this->name << "\n";
		return false;
	}
	for (int i = 0; i < mScheduleOnRead.size(); i++) {
		if (mScheduleOnRead[i].triggeringAdr == adr) {
			DBG_LOG(this, DBG_TRGGERING, "READ 0x" + Utility::int2HexStr(adr, 4) +  " => triggers " +
					mScheduleOnRead[i].device->name + " at " + to_string(mCycleCount) + "\n");
			mScheduleOnRead[i].device->advance(mCycleCount); // align the triggered device's time with this device
		}
	}
	return true;

}

bool MemoryMappedDevice::triggerAfterWrite(uint16_t adr, uint8_t data)
{
	if (!mTriggerOnWrite)
		return true;

	if (!selected(adr)) {
		//cout << "READ ADDRESS 0x" << hex << adr << " DOESN'T BELONG TO DEVICE '" << this->name << "\n";
		return false;
	}
	for (int i = 0; i < mScheduleOnWrite.size(); i++) {
		if (mScheduleOnWrite[i].triggeringAdr == adr) {
			DBG_LOG(this, DBG_TRGGERING, "WRITE 0x" + Utility::int2HexStr(data,2) + " TO 0x" + Utility::int2HexStr(adr,4) + " => triggers " +
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
	if (writeAccess) {
		mTriggerOnWrite = true;
		mScheduleOnWrite.push_back(dev_sch);
	}
	else {
		mTriggerOnRead = true;
		mScheduleOnRead.push_back(dev_sch);
	}
	if (DBG_LEVEL_DEV(this,DBG_VERBOSE))
		cout << "TRIGGER " << dev->name << " ON " << this->name << " 0x" << hex << adr << (writeAccess ? " WRITE" : " READ") << " ACCESSES\n";
	return true;
}