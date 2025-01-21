#include "Device.h"
#include "Tokeniser.h"
#include <iostream>

using namespace std;

ConnectionManager::ConnectionManager(DebugInfo debugInfo) : mDebugInfo(debugInfo)
{

}

ConnectionManager::~ConnectionManager()
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
	
	mDevicePorts[dev][localPort->localIndex] = localPort;
	
	if (mDebugInfo.dbgLevel & DBG_DEVICE) {
		printDevicePort(localPort);
		cout << "CONNECTION MANAGER ADDS " << printDevicePort(mDevicePorts[dev][localPort->localIndex]) << "\n";
	}

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
/*
	if (mRouting.find(src_port.port->globalIndex) == mRouting.end()) {
		Routing routing;
		routing.srcPort = src_port.port;
		mRouting[src_port.port->globalIndex] = routing;
	}
	//mRouting[src_port.port->globalIndex].connections.push_back(connection);
*/
	// Add dst port to src device port's list of connected inputs
	// mask and shifts to be set so that
	// dst = dst & ~mask | (src & mask) when shifts = 0
	// dst = dst & ~mask | ((src >> shifts) & mask) when shifts > 0
	// dst = dst & ~mask | ((src << -shifts) & mask) when shifts < 0
	InputReference input_ref;
	input_ref.port = dst_port.port;
	input_ref.mask = dst_port.bits.mask;
	input_ref.shifts = src_port.bits.lowBit - dst_port.bits.lowBit;
	src_port.port->inputs.push_back(input_ref);
	if (true || mDebugInfo.dbgLevel & DBG_DEVICE)
		cout << "CONNECT " << srcName << " AND " << dstName << " => shifts = " << dec << input_ref.shifts  << " & mask = 0x" << hex << (int) input_ref.mask  << dec << "\n";

	return true;
}

void ConnectionManager::printRouting()
{
	map<Device*, map<int, DevicePort*>>::iterator it;

	for (it = mDevicePorts.begin(); it != mDevicePorts.end(); it++) {
		Device* dev = it->first;
		cout << dev->name << ":\n";
		map<int,DevicePort*> ports = it->second;
		map<int, DevicePort*>::iterator pit;
		for (pit = ports.begin(); pit != ports.end(); pit++) {
			cout << "\t" << printDevicePort(pit->second) << "\n";
		}
		cout << "\n";
	}


}

string ConnectionManager::printDevicePort(DevicePort * device_port)
{
	if (device_port == NULL || device_port->dev == NULL) {
		return "";
	}

	stringstream sout;
	sout << device_port->dev->name << ":" << device_port->name << " " << _PORT_DIR(device_port->dir) << " 0x" << hex << (int)device_port->mask << " #" << dec <<
		device_port->globalIndex << " (" << device_port->localIndex << ") => ";
	for (int i = 0; i < device_port->inputs.size(); i++)
		sout << "\n\t\t" << " >> " << dec << device_port->inputs[i].shifts << " & 0x" << hex << 
		(int)device_port->inputs[i].mask << hex << " @ " << printDevicePort(device_port->inputs[i].port) << dec << "; ";
	return sout.str();

}

string ConnectionManager::printPortSelection(PortSelection & port_selection)
{
	stringstream sout;
	sout << printDevicePort(port_selection.port) << " MASK 0x" << hex << (int) port_selection.bits.mask << " & LOW BIT " << dec << (int) port_selection.bits.lowBit;
	return sout.str();
}