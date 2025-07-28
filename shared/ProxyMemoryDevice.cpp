#include "ProxyMemoryDevice.h"
#include "Device.h"
#include "ConnectionManager.h"

ProxyMemoryDevice::ProxyMemoryDevice(string name, uint16_t adr, uint16_t sz, DebugManager* debugManager,
ConnectionManager* connectionManager, DeviceManager* deviceManager) :mAdr(adr), mSz(sz),
	MemoryMappedDevice(
		name, PROXY_MEM_MAPPED_DEV, DeviceCategory::OTHER_DEVICE,
		1.0, // Dummy clock speed
		0,	// Dummy wait states
		adr, sz, debugManager, connectionManager, deviceManager)
{

}

bool ProxyMemoryDevice::addDevice(MemoryMappedDevice* dev)
{
	MemoryRange adr_space;
	vector<MemoryRange> *gaps;
	dev->getAddressAllocation(adr_space, gaps);

	if (adr_space.adr != mAdr || adr_space.sz != mSz)
		return false;

	

	// Get reference to the device's CS port
	DevicePort * CS_dst_port = NULL;
	if (!dev->getPortIndex("CS", CS_dst_port)) {
		return false;
	}

	// Get reference to the source device's port that drives the CS port
	vector<OutputReference>& CS_src_ports = CS_dst_port->portSources;
	if (CS_src_ports.size() != 1) {
		return false;
	}
	OutputReference& CS_src_port_ref = CS_src_ports[0];
	DevicePort *CS_src_port = CS_src_port_ref.srcPort;

	// Get a copy of the source port's CS connection info
	vector<InputReference> &src_ports_connected_inputs = CS_src_port->inputs;
	InputReference input_ref;
	for (int i = 0; i < src_ports_connected_inputs.size(); i++) {
		InputReference &in_ref = src_ports_connected_inputs[i];
		if (in_ref.port == CS_dst_port) {
			input_ref = in_ref;
		}
	}
	
	// Modify the connection info to fit this proxy device's CS port entry for the concerned device
	input_ref.process = true;
	DevicePort *proxy_dst_port = new DevicePort();
	proxy_dst_port = CS_dst_port;
	proxy_dst_port->dev = this;
	// TBD Modify more of the info
	input_ref.port = proxy_dst_port;

	// Add the new input reference to the source port
	CS_src_port->inputs.push_back(input_ref);

	// Add a new port to the proxy's list of CS inputs
	MemoryDeviceInfo* dev_info = new MemoryDeviceInfo();
	dev_info->dev = dev;
	mDevices.push_back(dev_info);

	// Register the port
	registerPort("CS" + to_string(dev_info->index), IN_PORT, 0x1, dev_info->index, &(dev_info->val));

	return true;
}

// Process a port update directly
void ProxyMemoryDevice::processPortUpdate(int index)
{
	for (int i = 0; i < mDevices.size(); i++) {
		if (index == mDevices[i]->index) {
			if (mDevices[i]->val == 0)
				mSelectedDevice = mDevices[i]->dev;
			else if (mSelectedDevice == mDevices[i]->dev)
				mSelectedDevice = NULL;
			break;
		}
	}
}