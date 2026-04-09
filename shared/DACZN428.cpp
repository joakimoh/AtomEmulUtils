#include "DACZN428.h"

DACZN428::DACZN428(string name, double tickRate, double deviceClockRate, uint16_t adr, uint16_t sz, int waitStates, DebugTracing* debugTracing, ConnectionManager* connectionManager, DeviceManager* deviceManager) :
	MemoryMappedDevice(name, DAC_ZN428_DEV, OTHER_DEVICE, waitStates, adr, sz, debugTracing, connectionManager, deviceManager),
	ClockedDevice(tickRate, deviceClockRate)
{
	registerPort("ENABLE", IN_PORT, 0x01, ENABLE, &mENABLE);	// ENA input

	registerAnaloguePort("AOut", OUT_PORT, AOut, &mAOut);
}

bool DACZN428::writeByte(BusAddress adr, BusByte data)
{
	if (selected(adr) && !mENABLE) {	
		mDIn = data;
		if (mVREF < 2.475)
			mVREF = 2.475;
		else if (mVREF > 2.625)
			mVREF = 2.625;
		double analogue_out = mDIn * (mVREF - mOS) / 256 + mOS;
		updateAnaloguePort(AOut, analogue_out);
		return true;
	}
	return false;
}