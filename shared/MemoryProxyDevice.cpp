#include "MemoryProxyDevice.h"
#include "Device.h"
#include "ConnectionManager.h"

MemoryProxyDevice::MemoryProxyDevice(string name, uint16_t adr, uint16_t sz, MemoryMappedDevice *firstDevice, DebugTracing* debugTracing,
ConnectionManager* connectionManager, DeviceManager* deviceManager): mSpace(firstDevice->getClaimedAddressSpace()),
	MemoryMappedDevice(
		name, PROXY_MEM_MAPPED_DEV, DeviceCategory::OTHER_DEVICE,
		0.0,	// Dummy access  rate
		adr, sz, debugTracing, connectionManager, deviceManager)
{
	addDevice(firstDevice);
}

MemoryProxyDevice::~MemoryProxyDevice()
{
	for (int i = 0; i < mDeviceInfos.size(); i++)
		delete mDeviceInfos[i];
}

bool MemoryProxyDevice::addDevice(MemoryMappedDevice* dev)
{

	// Get reference to the device's CS port
	DevicePort *CS_dst_port = NULL;
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

	// Add a new port to the proxy's list of CS inputs
	MemoryDeviceInfo *dev_info = new MemoryDeviceInfo();
	dev_info->dev = dev;
	mDeviceInfos.push_back(dev_info);

	// Register the port
	string port_name = "CS" + to_string(mCSPortIndex++);
	registerPort(port_name, IN_PORT, 0x1, dev_info->CSPortindex, &(dev_info->CS));
	DevicePort* proxy_dst_port = NULL;
	if (!getPortIndex(port_name, proxy_dst_port))
		return false;

	// Modify the connection info to fit this proxy device's CS port entry for the concerned device
	input_ref.process = true;
	input_ref.port = proxy_dst_port;

	// Add the connection to the source port's destination port list
	src_ports_connected_inputs.push_back(input_ref);

	// Add the connection to the destination ports' source port list
	proxy_dst_port->portSources.push_back(CS_src_port_ref);

	//cout << dev->name << ":CS mirrored as " << mConnectionManager->printDevicePort(proxy_dst_port) << "\n";

	return true;
}

// Process a port update directly
void MemoryProxyDevice::processPortUpdate(int index)
{
	for (int i = 0; i < mDeviceInfos.size(); i++) {
		if (index == mDeviceInfos[i]->CSPortindex) {
			//cout << "CS for " << mDeviceInfos[i]->dev->name << " changed to " << (int)mDeviceInfos[i]->CS << "\n";
			if (mDeviceInfos[i]->CS == 0) {
				mSelectedDevice = mDeviceInfos[i]->dev;
				//cout << "Device " << mSelectedDevice->name << " selected\n";
			}
			else if (mSelectedDevice == mDeviceInfos[i]->dev) {
				//cout << "No device selected!\n";
				mSelectedDevice = NULL;
			}
			break;
		}
	}
}