#include "MemoryMappedDevice.h"
#include <iostream>
#include <iomanip>
#include "DebugTracing.h"
#include "DeviceManager.h"
#include "Utility.h"
#include "AddressSpaceInfo.h"
#include <string>

using namespace std;

MemoryMappedDevice::MemoryMappedDevice(
	string name, DeviceId typ, DeviceCategory cat, double accessSpeed, BusAddress adr, BusAddress sz, DebugTracing  *debugTracing,
	ConnectionManager* connectionManager, DeviceManager* deviceManager
): Device(name, typ, cat, debugTracing, connectionManager), mAccessSpeed(accessSpeed), mDeviceManager(deviceManager), mAddressSpace(adr, sz)
{
	mMemoryMapped = true;
	string s1 = ""s + _DEVICE_ID(this->devType) + " '" + name + "' at address "s;
	string s2 = s1 + mAddressSpace;
	if (VERBOSE_EXT_OUTPUT)
		cout << s2 << "\n";

	mStartOfSpace = mAddressSpace.getStartOfSpace();
}

bool MemoryMappedDevice::selected(BusAddress adr)
{
	return mCS == 0 && mAddressSpace.contains(adr);
}

// Register a gap in the device's memory map
void MemoryMappedDevice::registerMemoryGap(BusAddress adr, BusAddress sz)
{
	mAddressSpace.addGap(adr, sz);

	if (VERBOSE_EXT_OUTPUT)
		cout << "Gap in memory space for device '" << this->name << "' between " << hex << setfill('0') << setw(4) << adr <<
			" and " <<adr + sz << "\n";

}

bool MemoryMappedDevice::readByte(BusAddress adr, BusByte& data) {
	data = 0xff;
	return selected(adr);
}

bool MemoryMappedDevice::triggerBeforeRead(BusAddress adr, uint8_t data)
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
					mScheduleOnRead[i].device->name + " at " + to_string(mTicks) + "\n");
			mScheduleOnRead[i].device->processPortUpdate(); // request device on what outputs this device depends to update its outputs
		}
	}
	return true;

}

bool MemoryMappedDevice::triggerAfterWrite(BusAddress adr, uint8_t data)
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
					mScheduleOnWrite[i].device->name + " at " + to_string(mTicks) + "\n");
			mScheduleOnWrite[i].device->processPortUpdate(); // request device that depends on the outputs this device to update its outputs
		}
	}
	return true;
}

bool MemoryMappedDevice::writeByte(BusAddress adr, BusByte data) {

	return selected(adr);
}

// Register that another device shall be triggered on R/W accesses to this device
bool MemoryMappedDevice::registerAccess(Device* dev, BusAddress adr, bool writeAccess) {
	DeviceAccessScheduling dev_sch = { dev, adr };
	if (writeAccess) {
		mTriggerOnWrite = true;
		mScheduleOnWrite.push_back(dev_sch);
	}
	else {
		mTriggerOnRead = true;
		mScheduleOnRead.push_back(dev_sch);
	}
	if (VERBOSE_OUTPUT)
		cout << "TRIGGER " << dev->name << " ON " << this->name << " 0x" << hex << adr << (writeAccess ? " WRITE" : " READ") << " ACCESSES\n";
	return true;
}