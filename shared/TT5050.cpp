#include "TT5050.h"
#include <iostream>

using namespace std;

TT5050::TT5050(
	string name, uint16_t adr, double clockSpeed, ALLEGRO_BITMAP* disp, uint16_t videoMemAdr, DebugInfo debugInfo, ConnectionManager* connectionManager
) : Device(name, TT_5050_DEV, OTHER_DEVICE, debugInfo, connectionManager)
{
	if (mDebugInfo.dbgLevel & DBG_VERBOSE)
		cout << "Teletext Character Generator SA5050 '" << name << "' added\n";

	registerPort("VS", IN_PORT, 0x1, VS, &mVS);
	registerPort("HS", IN_PORT, 0x1, HS, &mHS);


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

bool TT5050::updateDataOutput(uint8_t& R, uint8_t& G, uint8_t& B)
{
	R = 0x0;
	G = 0x0;
	B = 0x0;

	return true;
}