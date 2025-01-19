#include "Device.h"
#include "Tokeniser.h"
#include <iostream>

using namespace std;

ConnectionManager::ConnectionManager(DebugInfo debugInfo) : mDebugInfo(debugInfo)
{

}

void ConnectionManager::setDevices(Devices* devices)
{
	mDevices = devices;
}

// Used by a device to tell the availability of a port for routing
bool ConnectionManager::addDevicePort(Device* dev, DevicePort *localPort)
{
	localPort->globalIndex = mDevicePortIndex++;

	printDevicePort(*localPort);
	
	mDevicePorts[dev][localPort->localIndex] = localPort;
	

	cout << "ADD " << mDevicePorts[dev][localPort->localIndex]->dev->name << ":" << mDevicePorts[dev][localPort->localIndex]->name << " #" << dec <<
		mDevicePorts[dev][localPort->localIndex]->globalIndex << " (" << _PORT_DIR(mDevicePorts[dev][localPort->localIndex]->dir) << ", #" << dec <<
		mDevicePorts[dev][localPort->localIndex]->localIndex << ")\n";

	return true;
}
/*
bool ConnectionManager::receiveUpdate(Device *dev, int index, uint8_t val)
{
	if (mDebugInfo.dbgLevel & DBG_DEVICE) {
		cout << "RECEIVE UPDATE\n";
		cout << "DEVICE '" << dev->name << "' local port #" << dec << index << " = " << (int)val << "\n";
		cout << "#devices with connectable ports: " << mDevicePorts.size() << "\n";
		cout << "#routings: " << mRouting.size() << "\n";
	}

	// Check for errors
	if (dev == NULL) {
		cout << "INTERNAL ERROR detected in receiveUpdate: NULL pointer provided for device!\n";
		return false;
	}
	
	if (mDevicePorts.find(dev) == mDevicePorts.end()) {
		cout << "Port #" << dec << index << " for device " << dev->name << "' doesn't exist!\n";
		return false;
	}

	// Get global port index
	map<int,DevicePort> &unique_ports = mDevicePorts[dev];
	DevicePort &unique_port = unique_ports[index];

	if (mDebugInfo.dbgLevel & DBG_DEVICE) {
		cout << "DEVICE '" << dev->name << "' local port #" << dec << index << "'" << unique_port.localPort.name << "' (" << unique_port.globalIndex << ") = " <<
			(int)val << "\n";
	}

	// Check that routing exists for the port (not an error if it doesn't exist!)
	if (mRouting.find(unique_port.globalIndex) == mRouting.end())
		return true;
	
	// Get routing for the port
	Routing &routing = mRouting[unique_port.globalIndex];

	// Get all the specified connections
	vector<Connection> &connections = routing.connections;
		
	// Iterate over the connections
	for (int i = 0; i < connections.size(); i++) {

		Connection &connection = connections[i];
		PortSelection& dst_port = connection.dstPort;

		// Extract bits of src port to use
		uint8_t v = val;
		v &= connection.srcBits.mask;
		v = v >> connection.srcBits.lowBit;

		dst_port.port.dev->updateInput(dst_port, v);

	}

	return true;

}
*/

//
// Extract I/O port reference from string literal
// 
// <device>	::=					<name> || '_'
// <I/O reference>	::=			<name>[=<high bit>:<low bit>]
// Valid format:				<device>:<I/O reference>
//
// Routing examples:
// 
// CONNECT	VDU:FS			PIA:PortC;7			- Connect VDU FS output to PIA PortSelection C b7			
// CONNECT	PIA:PortA;7		VDU:A/G				- Connect PIA PortSelection A b7:4 to VDU A/G input and GM0:2 inputs
// CONNECT	PIA:PortA;6;4	VDU:GM
// CONNECT	PIA:_;7			VDU:INV				- Connect data bus b7:6 to VDU inputs INV,A/S & INT/EXT
// CONNECT	PIA:_;6			VDU:A/S
// CONNECT	PIA:_;6			VUD:INT/EXT
//
bool ConnectionManager::extractPort(string name, PortSelection &port_selection)
{
	try {

		// Get device 
		Tokeniser dev_tok(name, ':');
		string dev_name;
		if (!dev_tok.nextToken(dev_name))
			return false;
		Device* dev;
		if (!mDevices->getDevice(dev_name, dev))
				return false;

		// Get port reference
		string port_ref;
		if (!dev_tok.nextToken(port_ref))
			return false;
	
		// Get port name
		Tokeniser port_tok(port_ref, ';');
		string port_name;
		if (!port_tok.nextToken(port_name))
			return false;

		// Get port
		DevicePort * device_port;
		if (!dev->getPortIndex(port_name, device_port))
				return false;

		// Get bit range (if present)
		string hb_s, lb_s;
		int lb = 0;
		int hb = 7;
		if (port_tok.nextToken(hb_s)) {
			hb = stoi(hb_s);
			lb = hb;
		}
		if (port_tok.nextToken(lb_s))
			lb = stoi(lb_s);
		if (hb < 0 || hb > 7 || lb < 0 || hb > 7)
			return false;

		// Create I/O port

		// Update port selection with the device's port info
		port_selection.port = device_port;

		// Update the bit selection part
		port_selection.bits.lowBit = lb;
		port_selection.bits.mask = 0;
		for (int i = lb; i <= hb; i++)
			port_selection.bits.mask |= (1 << i);
		
		return true;
	
	}
	
	catch (...) {
		return false;
	}
}

//
bool ConnectionManager::connect(string srcName, string dstName)
{

	PortSelection src_port;
	if (!extractPort(srcName, src_port)) {
		cout << "Invalid format for source routing '" << srcName << "'\n";
		return false;
	}


	PortSelection dst_port;
	if (!extractPort(dstName, dst_port)) {
		cout << "Invalid format for destination routing '" << dstName << "'\n";
		return false;
	}

	Connection connection;
	connection.srcBits = src_port.bits;
	connection.dstPort = dst_port;

	// Lookup routing (if existing). If not existing, create a new one
	if (mRouting.find(src_port.port->globalIndex) == mRouting.end()) {
		Routing routing;
		routing.srcPort = src_port.port;
		mRouting[src_port.port->globalIndex] = routing;
	}
	mRouting[src_port.port->globalIndex].connections.push_back(connection);

	// Add dst port to src device port's list ot connected inputs
	InputReference input_ref;
	input_ref.port = dst_port.port;
	input_ref.mask = src_port.bits.mask >> src_port.bits.lowBit;
	input_ref.shifts = src_port.bits.lowBit;
	src_port.port->inputs.push_back(input_ref);


	return true;
}

void ConnectionManager::printRouting()
{
	map<int, Routing>::iterator it;
	for (it = mRouting.begin(); it != mRouting.end(); it++) {
		Routing& routing = it->second;
		cout << "#" << routing.srcPort->globalIndex << " " << routing.srcPort->dev->name << ":" << routing.srcPort->name << 
			" (" << routing.srcPort->localIndex << "):\n";
		vector<Connection> &connections = routing.connections;
		for (int c = 0; c < connections.size(); c++) {
			Connection connection = connections[c];
			cout << "\t0x" << hex << (int)connection.srcBits.mask << ", " << dec << (int)connection.srcBits.lowBit << " => " <<
				connection.dstPort.port->dev->name << 
				":" << connection.dstPort.port->name << " (" << connection.dstPort.port->localIndex <<
				") 0x" << hex << (int)connection.dstPort.bits.mask << "," << dec << (int)connection.dstPort.bits.lowBit << "\n";
		}
	}

}

void ConnectionManager::printDevicePort(DevicePort& device_port)
{
	cout << device_port.dev->name << ":" << device_port.name << " " << _PORT_DIR(device_port.dir) << " 0x" << hex << (int) device_port.mask <<
		"\n";
}