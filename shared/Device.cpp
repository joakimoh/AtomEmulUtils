#include "Device.h"
#include "RAM.h"
#include "ROM.h"
#include "PIA8255.h"
#include "VDU6847.h"
#include "VIA6522.h"
#include "CRTC6845.h"
#include "BeebVideoULA.h"
#include "BeebKeyboard.h"
#include "TT5050.h"
#include "AtomKeyboardDevice.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <allegro5/allegro.h>
#include <string>
#include "P6502.h"
#include <vector>
#include "AtomCUTSInterface.h"
#include "TapeRecorder.h"
#include "AtomSpeaker.h"
#include "MemoryMappedDevice.h"
#include "BeebRomSel.h"
#include "ACIA6850.h"
#include "BeebSerialULA.h"
#include "TI4689.h"
#include "BeebViaLatch.h"
#include "ConnectionManager.h"

using namespace std;

//
// Device class
//

Device::Device(string n, DeviceId typ, DeviceCategory cat, double cpuClock, DebugManager  *debugManager, ConnectionManager *connectionManager) :
	devType(typ), mConnectionManager(connectionManager), mDM(debugManager), name(n), category(cat), mCPUClock(cpuClock)
{
}

Device::~Device()
{
	for (int i = 0; i < mPorts.size(); i++)
		delete mPorts[i];
}

// Get pointer to other device to be able to call its methods
bool Device::connectDevice(Device* dev)
{
	if (DBG_LEVEL(DBG_VERBOSE))
		cout << "DEVICE '" << dev->name << "' can be invoked by device '" << this->name << "'!\n";
	mConnectedDevices.push_back(dev);
	return true;
}

// Used by a device to make a port available for routing
bool Device::registerPort(string name, PortDirection dir, uint8_t mask, int &index, uint8_t *val)
{
	
	index = mPortIndex++;
	DevicePort *device_port = new DevicePort();
	device_port->dev = this;
	device_port->name = name;
	device_port->dir = dir;
	device_port->mask = mask;
	device_port->localIndex = index;
	device_port->val = val;
	device_port->globalIndex = -1;
	if (DBG_LEVEL( DBG_DEVICE))
		cout << "DEVICE ADDS PORT " << mConnectionManager->printDevicePort(device_port) << "\n";

	mPorts.push_back(device_port);
	
	mConnectionManager->addDevicePort(this, device_port);

	if (DBG_LEVEL(DBG_VERBOSE))
		cout << "ADDED " << this->name << " " << _PORT_DIR(dir) << " PORT '" << name << "' #" << dec << index << " (#" << device_port->globalIndex << ")\n";
	

	return true;
}

//
// Update each connected input port (dst) based on the output port (src)
// 
// dst = dst & ~mask | (src & mask) when shifts = 0
// dst = dst & ~mask | ((src >> shifts) & mask) when shifts > 0
// dst = dst & ~mask | ((src << -shifts) & mask) when shifts < 0
//
bool Device::updatePort(int index, uint8_t val)
{
	if (index < 0 && index >= mPorts.size())
		return false;

	*(mPorts[index]->val) = val;
	if (mPorts[index]->inputs.size() > 0) {
		if (!updateConnectedPorts(mPorts[index]->inputs, val, mPorts[index]))
			return false;
	}

	return true;
}

bool Device::updateConnectedPorts(vector<InputReference> &connectedPorts, uint8_t val, DevicePort *port)
{
	
	for (int i = 0; i < connectedPorts.size(); i++) {
		InputReference input = connectedPorts[i];
		uint8_t pval = *(input.port->val);
		uint8_t nval = val;
		if (input.invert)
			nval = ~val;
		uint8_t n_ival;
		if (input.shifts >= 0)
			n_ival = ((pval & ~input.mask) | ((nval >> input.shifts) & input.mask)) & input.port->mask;
		else
			n_ival = ((pval & ~input.mask) | ((nval << (-input.shifts)) & input.mask)) & input.port->mask;

		if (updateDstPortValue(port, input,val)) { // update destination port on change or always for a bidirectional port

			// Call direct processing on the receiving device - if specified by configuration
			if (input.process)
				input.port->dev->processPortUpdate(input.port->localIndex);

		}

	}

	return true;
}

//
// Update a destinaton port based on a source port value
//
// Returns true if there was an change; otherwise false
//
bool Device::updateDstPortValue(DevicePort *srcPort, InputReference &dstPort, uint8_t srcVal)
{
	uint8_t* dst_val_p = dstPort.port->val;
	uint8_t pval = *dst_val_p;

	// Calculate new port val based on source port value
	int shifts = dstPort.shifts;
	int dst_sel_mask = dstPort.mask;
	int dst_mask = dstPort.port->mask;
	uint8_t src_val = srcVal;
	if (dstPort.invert)
		src_val = ~srcVal;
	uint8_t nval;
	uint8_t nval_or;
	if (shifts >= 0)
		nval_or = (src_val >> shifts) & dst_sel_mask;
	else
		nval_or = (src_val << (-shifts)) & dst_sel_mask;
	nval = ((pval & ~dst_sel_mask) | nval_or) & dst_mask;

	// update on change or always for a bidirectional
	if (pval != nval || dstPort.port->dir == IO_PORT) {

	
		// Arbitrate with 'AND' logic between the new source port-based value and and other source port-based values
		// Example: The IRQ line on the 6502 CPU is updated from several devices but shall remain low as long as at least
		//			one device requests the IRQ line to be low even if another device requests it to be high.
		if (dstPort.port->arbitration) {

			uint8_t aval = dst_mask; // initialise abritrated value with the destination port's mask <=> all bits set (High)

			//cout << "\nABRITRATION FOR PORT " << dstPort.port->dev->name << ":" << dstPort.port->name << " = 0x" << hex << (int) pval << "\n";

			for (int i = 0; i < dstPort.port->portSources.size(); i++) {
				OutputReference &src_ref = dstPort.port->portSources[i];
				if (src_ref.srcPort == srcPort) {
					src_ref.dstVal = nval_or;
					//cout << "*";
				}
				// update destination port's source port entry with new requested value		
				aval &= src_ref.dstVal | ~src_ref.dstMask; // arbitrate with each source port's value
				//cout << "SOURCE PORT " << src_ref.srcPort->dev->name << ":" << src_ref.srcPort->name << " = 0x" << hex << (int) src_ref.dstVal <<
				//	" (mask 0x" << (int)src_ref.dstMask << ")\n";
			}
			//cout << "ARBITRATED VALUE BECAME 0x" << hex << (int)aval << "\n";

			*dst_val_p = aval;
		}
		else { // Only one source device connected to the port => arbitration not needed
			*dst_val_p = nval;
		}
#ifdef DGB_ON
		if (/*dstPort.port->arbitration ||*/ (
			mDM->matchPort(dstPort.port) &&
			(DBG_LEVEL(DBG_PORT) && *(dstPort.port->val) != pval)
			)) {
			string shift_s, c_dir;
			if (dstPort.shifts >= 0)
				shift_s = "((src >> shifts) & mask)";
			else
				shift_s = "((src << shifts) & mask)";
			cout << this->name << ":" << srcPort->name << " = 0x" << hex << (int)srcVal << " => " <<
				dstPort.port->dev->name << ":" << dstPort.port->name << " = " <<
				dstPort.port->name << " &  ~mask | " << shift_s << " = 0x" << hex <<
				(int)pval << " & 0x" << hex << setfill('0') << setw(2) << (int)(uint8_t)(~dst_sel_mask) << " | ((0x" << hex << (int)nval <<
				(shifts >= 0 ? " >> " : " << ") << setfill(' ') << dec << (shifts >= 0 ? shifts : -shifts) <<
				") & 0x" << hex << (int)dst_sel_mask << ")" << setfill('0') << setw(2) <<
				" = 0x" << hex << (int)(*dst_val_p) << dec;
			if (dstPort.process)
				cout << "; processing\n";
			else
				cout << "\n";
		}
#endif
		return true;
	}

	return false;
}

bool Device::updatePorts()
{
	for (int i = 0; i < mPorts.size(); i++) {
		if (!updatePort(i, *(mPorts[i]->val)))
			return false;
	}

	DBG_LOG(this, DBG_VERBOSE, "All ports for "  + this->name + " have been shared...\n");

	return true;
}

bool Device::getPortIndex(string name, DevicePort * &port) {
	for (int i = 0; i < mPorts.size(); i++) {
		if (mPorts[i]->name == name) {
			port = mPorts[i];
			return true;
		}
	}
	return false;
}


