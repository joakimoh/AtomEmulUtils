#include "Latch74LS259.h"
#include "Utility.h"

//
// Emulates an 8-bit Addressable Latch 74LS259 with eight latches Q0:7 that can be individually controlled
// by means of three inputs S0:S2 (CTRL b0:2) and a data input D (CTRL b3)
//
// 
Latch74LS259::Latch74LS259(string name, double tickRate, DebugTracing* debugTracing, ConnectionManager* connectionManager):
	Device(name, LATCH_74LS259, PERIPHERAL, debugTracing, connectionManager)
{
	registerPort("CLR",		IN_PORT,	0x1, CLR,	&mCLR);
	registerPort("ENA",		IN_PORT,	0x1, ENA,	&mENA);
	registerPort("CTRL",	IN_PORT,	0x0f, CTRL,	&mCTRL);
	registerPort("Q",		OUT_PORT,	0xff, Q,	&mQ);
}

// Process a port update directly (and not just next time the advanceUntil() method is called)
void Latch74LS259::processPortUpdate(int index)
{
	if (index == CTRL && mCLR != 0 && mENA == 0) {
		uint8_t ctrl_sel = mCTRL & 0x7;
		uint8_t ctrl_val = (mCTRL >> 3) & 0x1;
		uint8_t new_val = (mQ & ~(1 << ctrl_sel)) | ((ctrl_val & 0x1) << ctrl_sel);
		updatePort(Q, new_val);
		
	}
	else if (index == CLR && mCLR == 0) {
		mQ = 0;
	}
}

// Outputs the internal state of the device
bool Latch74LS259::outputState(ostream& sout)
{
	sout << "Q = 0x" << Utility::int2HexStr(mQ, 2) << " <=> 0b" << Utility::int2BinStr(mQ, 8) << "\n";

	return true;
}