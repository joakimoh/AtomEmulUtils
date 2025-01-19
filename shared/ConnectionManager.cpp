#include "Device.h"
#include "Tokeniser.h"
#include <iostream>

using namespace std;

ConnectionManager::ConnectionManager(Devices* devices) : mDevices(devices)
{

}

// Used by a device to tell the availability of a port for routing
bool ConnectionManager::addDevicePort(Device* dev, LocalPort localPort)
{

	int unique_index = mUniqueIndex++;
	UniquePort unique_port;
	unique_port.dev = dev;
	unique_port.localPort = localPort;
	unique_port.globalIndex = unique_index;
	
	mUniquePorts[dev][localPort.localIndex] = unique_port;

	cout << "ADD " << mUniquePorts[dev][localPort.localIndex].dev->name << ":" << mUniquePorts[dev][localPort.localIndex].localPort.name << " #" << dec <<
		mUniquePorts[dev][localPort.localIndex].globalIndex << " (" << _PORT_DIR(mUniquePorts[dev][localPort.localIndex].localPort.dir) << ", #" << dec <<
		mUniquePorts[dev][localPort.localIndex].localPort.localIndex << ")\n";

	return true;
}

bool ConnectionManager::receiveUpdate(Device *dev, int index, uint8_t val)
{

	cout << "RECEIVE UPDATE\n";
	cout << "DEVICE '" << dev->name << "' local port #" << dec << index << " = " << (int)val << "\n";
	cout << "#devices with connectable ports: " << mUniquePorts.size() << "\n";
	cout << "#routings: " << mRouting.size() << "\n";

	// Check for errors
	if (dev == NULL) {
		cout << "INTERNAL ERROR detected in receiveUpdate: NULL pointer provided for device!\n";
		return false;
	}
	
	if (mUniquePorts.find(dev) == mUniquePorts.end()) {
		cout << "Port #" << dec << index << " for device " << dev->name << "' doesn't exist!\n";
		return false;
	}

	// Get global port index
	map<int,UniquePort> &unique_ports = mUniquePorts[dev];
	UniquePort &unique_port = unique_ports[index];

	cout << "DEVICE '" << dev->name << "' local port #" << dec << index << "'" << unique_port.localPort.name << "' (" << unique_port.globalIndex << ") = " <<
		(int)val << "\n";

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

bool ConnectionManager::getRoutingIndex(PortSelection portSelection, Routing *routing)
{
	routing = NULL;
	int index = portSelection.port.globalIndex;

	if (mRouting.find(index) == mRouting.end())
		return false;

	routing = &mRouting[index];
	cout << "ROUTING " << mRouting[index].srcPort.dev->name << ":" << mRouting[index].srcPort.localPort.name <<
		" FOUND WITH #" << mRouting[index].connections.size() << " CONNECTIONS!\n";
	return true;

}


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

		// Get device ("_" <=> data bus input of the device itself)
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
		int local_port_index = -1;
		if (!dev->getPortIndex(port_name, local_port_index))
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

		// Update device-specific part
		port_selection.port.dev = dev;
		port_selection.port.localPort.name = port_name;
		port_selection.port.localPort.localIndex = local_port_index;

		// Update the bit selection part
		port_selection.bits.lowBit = lb;
		port_selection.bits.mask = 0;
		for (int i = lb; i <= hb; i++)
			port_selection.bits.mask |= (1 << i);

		// Get global index
		if (mUniquePorts.find(dev) == mUniquePorts.end()) 
			return false;
		map<int,UniquePort>& unique_ports = mUniquePorts[dev];
		if (unique_ports.find(local_port_index) == unique_ports.end())
			return false;
		UniquePort unique_port = unique_ports[local_port_index];

		port_selection.port.globalIndex = unique_port.globalIndex;
		
		return true;
	
	}
	
	catch (...) {
		return false;
	}
}

//
bool ConnectionManager::connect(string srcName, string dstName)
{
	cout << "\nCONNECT " << srcName << " AND " << dstName << "\n";

	PortSelection src_port;
	if (!extractPort(srcName, src_port)) {
		cout << "Invalid format for source routing '" << srcName << "'\n";
		return false;
	}
	cout << "\tSRC: " << src_port.port.dev->name << ":" << src_port.port.localPort.name << 
		hex << " MASK 0x" << (int) src_port.bits.mask << ", FIRST BIT " << (int) src_port.bits.lowBit <<
		"\n";

	PortSelection dst_port;
	if (!extractPort(dstName, dst_port)) {
		cout << "Invalid format for destination routing '" << dstName << "'\n";
		return false;
	}
	cout << "\tDST: " << dst_port.port.dev->name << ":" << dst_port.port.localPort.name <<
		hex << " MASK 0x" << (int)dst_port.bits.mask << ", FIRST BIT " << (int)dst_port.bits.lowBit <<
		"\n";

	

	Connection connection;
	connection.srcBits = src_port.bits;
	connection.dstPort = dst_port;

	// Lookup routing (if existing). If not existing, create a new one
	Routing *routing_p = NULL;
	if (!getRoutingIndex(src_port, routing_p)) {
		Routing routing;
		routing.srcPort = src_port.port;
		mRouting[src_port.port.globalIndex] = routing;
		routing_p = &routing;
	}
	routing_p->connections.push_back(connection);

	cout << "\tPORTS CONNECTED, ROUTING INDEX IS " << dec << src_port.port.globalIndex << " AND #CONNECTIONS IS " << routing_p->connections.size() <<
		"\n";

	return true;
}
