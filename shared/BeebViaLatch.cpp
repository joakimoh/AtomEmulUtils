#include "BeebViaLatch.h"

BeebViaLatch::BeebViaLatch(string name, double cpuClock, DebugManager* debugManager, ConnectionManager* connectionManager):
	Device(name, BEEB_VIA_LATCH, PERIPHERAL, cpuClock, debugManager, connectionManager)
{
	registerPort("CTRL", IN_PORT, 0x0f, CTRL, &mCTRL);
	registerPort("Q", OUT_PORT, 0xff, Q, &mQ);
}

// Process a port update directly (and not just next time the advance() method is called)
void BeebViaLatch::processPortUpdate(int index)
{
	if (index == CTRL) {
		uint8_t ctrl_sel = mCTRL & 0x7;
		uint8_t ctrl_val = (mCTRL >> 3) & 0x1;
		uint8_t new_val = (mQ & ~(1 << ctrl_sel)) | ((ctrl_val & 0x1) << ctrl_sel);
		//cout << "Q" << dec << (int)ctrl_sel << " = " << (int) ctrl_val << ": 0x" << hex << (int) mQ << " = > 0x" << (int)new_val << "\n";
		updatePort(Q, new_val);
		
	}
}