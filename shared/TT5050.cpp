#include "TT5050.h"

TT5050::TT5050(
	string name, uint16_t adr, double clockSpeed, ALLEGRO_BITMAP* disp, uint16_t videoMemAdr, DebugInfo debugInfo, ConnectionManager* connectionManager
) : Device(name, TT_5050_DEV, OTHER_DEVICE, debugInfo, connectionManager)
{

}

// Reset device
bool TT5050::reset()
{
	return true;
}

// Advance until clock cycle stopcycle has been reached
bool TT5050::advance(uint64_t stopCycle)
{
	return true;
}

bool TT5050::trigger(int port)
{
	return true;
}