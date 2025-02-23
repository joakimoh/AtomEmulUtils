#include "ACIA6850.h"

ACIA6850::ACIA6850(string name, uint16_t adr, double clock, double cpuClock, DebugInfo debugInfo, ConnectionManager* connectionManager) :
	MemoryMappedDevice(name, ACIA6850_DEV, PERIPHERAL, adr, 0x08, debugInfo, connectionManager), mClock(clock), mCPUClock(cpuClock)
{

}

bool ACIA6850::read(uint16_t adr, uint8_t& data)
{
	data = 0x00;

	return true;
}

bool ACIA6850::write(uint16_t adr, uint8_t data)
{
	return true;
}

// Reset device
bool ACIA6850::reset()
{
	Device::reset();
	return true;
}

// Advance until clock cycle stopcycle has been reached
bool ACIA6850::advance(uint64_t stopCycle)
{
	mCycleCount = stopCycle;

	return true;
}