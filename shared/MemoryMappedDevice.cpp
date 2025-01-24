#include "MemoryMappedDevice.h"


MemoryMappedDevice::MemoryMappedDevice(
	string name, DeviceId typ, DeviceCategory cat, uint16_t adr, uint16_t sz, DebugInfo debugInfo, ConnectionManager* connectionManager
): Device(name, typ, cat, debugInfo, connectionManager), mDevAdr(adr), mDevSz(sz)
{

}

bool MemoryMappedDevice::selected(uint16_t adr)
{
	return (adr >= mDevAdr && adr < mDevAdr + mDevSz);
}

bool MemoryMappedDevice::validAdr(uint16_t adr)
{
	return selected(adr);
}