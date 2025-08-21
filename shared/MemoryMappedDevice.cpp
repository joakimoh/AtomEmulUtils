#include "MemoryMappedDevice.h"
#include <iostream>
#include <iomanip>
#include "DebugManager.h"
#include "DeviceManager.h"
#include "Utility.h"
#include "AddressSpaceInfo.h"
#include <string>

using namespace std;

MemoryMappedDevice::MemoryMappedDevice(
	string name, DeviceId typ, DeviceCategory cat, double cpuClock, uint8_t waitStates, uint16_t adr, uint16_t sz, DebugManager  *debugManager,
	ConnectionManager* connectionManager, DeviceManager* deviceManager
): Device(name, typ, cat, cpuClock, debugManager, connectionManager), mWaitStates(waitStates), mDeviceManager(deviceManager), mAddressSpace(adr, sz)
{
	mMemoryMapped = true;
	string s1 = _DEVICE_ID(this->devType) + " at address "s;
	string s2 = s1 + mAddressSpace;
	DBG_LOG(this, DBG_VERBOSE, s2);

}

bool MemoryMappedDevice::selected(uint16_t adr)
{
	return mCS == 0 && mAddressSpace.contains(adr);
}

void MemoryMappedDevice::registerMemoryGap(uint16_t adr, uint16_t sz)
{
	mAddressSpace.addGap(adr, sz);

	if (DBG_LEVEL_DEV(this,DBG_VERBOSE))
		cout << "Gap in memory space for device '" << this->name << "' between " << hex << setfill('0') << setw(4) << adr <<
			" and " <<adr + sz << "\n";

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
			mScheduleOnRead[i].device->advanceUntil(mCycleCount); // align the triggered device's time with this device
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
			mScheduleOnWrite[i].device->advanceUntil(mCycleCount); // align the triggered device's time with this device
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