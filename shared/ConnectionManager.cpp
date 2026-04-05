#include "Device.h"
#include "DeviceManager.h"
#include "ConnectionManager.h"
#include "Tokeniser.h"
#include <iostream>

using namespace std;

ConnectionManager::ConnectionManager(DebugTracing  *debugTracing) : mDM(debugTracing)
{

}

ConnectionManager::~ConnectionManager()
{
}

void ConnectionManager::setDeviceManager(DeviceManager* devices)
{
	mDevices = devices;
}


// Used by a device to make an analogue port available externally (e.g., to the debugger)
bool ConnectionManager::addAnaloguePort(Device* dev, AnaloguePort* localPort)
{
	localPort->globalIndex = mAnaloguePortIndex++;

	mAnaloguePorts[dev][localPort->localIndex] = localPort;

	return true;
}


// Used by a device to tell the availability of a port for routing
bool ConnectionManager::addDevicePort(Device* dev, DevicePort *localPort)
{
	localPort->globalIndex = mDevicePortIndex++;
	
	mDevicePorts[dev][localPort->localIndex] = localPort;

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

Device *ConnectionManager::getDevice(string name)
{
	Device* dev;
	if (!mDevices->getDevice(name, dev))
		return nullptr;

	return dev;
	
}

bool ConnectionManager::extractAnaloguePort(string name, AnaloguePort *&port)
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
		if (!dev->getAnaloguePort(port_name, port)) {
			cout << "Failed to find analogue port '" << port_name << "' for device '" << dev->name << "'!\n";
			return false;
		}
		return true;
	}
	catch (...) {
		return false;
	}
}


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
		if (!dev->getPort(port_name, device_port)) {
			cout << "Failed to find port '" << port_name << "' for device '" << dev->name << "'!\n";
			return false;
		}

		// Get bit range (if present)
		string hb_s, lb_s;
		int lb = 0;
		int hb_max = device_port->sz - 1;
		int hb = hb_max;
		if (port_tok.nextToken(hb_s)) {
			hb = stoi(hb_s);
			lb = hb;
		}
		if (port_tok.nextToken(lb_s))
			lb = stoi(lb_s);
		if (hb < 0 || hb > hb_max || lb < 0 || lb > hb_max)
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
bool ConnectionManager::connect(string srcName, string dstName, bool invert, bool process)
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

	if (src_port.port->dir == IN_PORT) {
		cout << "Attempt to use the input port " << _PORT_ID(src_port.port) << " as a source port!\n";
		return false;
	}

	if (dst_port.port->dir == OUT_PORT) {
		cout << "Attempt to use the output port " << _PORT_ID(dst_port.port) << " as a destination port!\n";
		return false;
	}

	Connection connection;
	connection.srcBits = src_port.bits;
	connection.dstPort = dst_port;

	InputReference input_ref;
	input_ref.port = dst_port.port;
	input_ref.mask = dst_port.bits.mask;
	input_ref.shifts = src_port.bits.lowBit - dst_port.bits.lowBit;
	input_ref.invert = invert;
	input_ref.process = process;
	src_port.port->inputs.push_back(input_ref);

	// Add the source port to the destination port's list of source ports
	// - used when more than one source port is connected to the same destination port
	// The destination port will as default assume an 'AND' logic to arbitrate between
	// serval source port values. I.e., only if all source ports require a high value
	// will the destination port be assigned a high value; otherwise its value will
	// be low as long as at least one destination port requires a low value.
	OutputReference output_ref;
	output_ref.srcPort = src_port.port;
	output_ref.dstMask = dst_port.bits.mask;
	output_ref.srcShifts = input_ref.shifts;
	dst_port.port->portSources.push_back(output_ref);
	src_port.port->fanOut++;
	dst_port.port->fanIn++;
	if (dst_port.port->dir == PortDirection::IO_PORT) {
		src_port.port->conToBiDirP = true;
		//cout << "BIDIRECTIONAL CONNECTION " << src_port.port->dev->name << ":" << src_port.port->name << " => " <<
		//	dst_port.port->dev->name << ":" << dst_port.port->name << "\n";
	}

	// Check for fan in on destination port
	// Set arbitration flag on destination port if fan in > 1.
	// Also set fan in flag on source port
	// Only pure output ports are considered (and not bidirectional ports)
	bool arbitration = false;
	if (dst_port.port->portSources.size() > 1) {
		PortVal dst_mask = 0xffff;
		int cnt = 0;
		for (int i = 0; i < dst_port.port->portSources.size(); i++) {
			if (dst_port.port->portSources[i].srcPort->dir == OUT_PORT) {
				dst_mask &= dst_port.port->portSources[i].dstMask;
				cnt++;
			}
			//cout << "*** " << dst_port.port->portSources[i].srcPort->name << " mask: " << hex << (int) dst_port.port->portSources[i].dstMask << "\n";
		}
		arbitration = cnt>1 && (arbitration || (dst_mask != 0));
		dst_port.port->arbitration = arbitration;
		if (arbitration)
			src_port.port->dstFanIn = true;
		//cout << "*** arbitration = " << (int)dst_port.port->arbitration << " as mask became 0x" << hex << (int) dst_mask << "\n\n";
	}
	if (arbitration) {
		for (int i = 0; i < dst_port.port->portSources.size(); i++) {
			auto& sp = dst_port.port->portSources[i].srcPort;
			dst_port.port->portSources[i].srcPort->dstFanIn = true;
			//cout << "ARBITRATION " << sp->dev->name << ":" << sp->name << " => " << dst_port.port->dev->name << ":" << dst_port.port->name << "\n";
		}
	}
	

	if (VERBOSE_EXT_OUTPUT)
		cout << "CONNECT " << srcName << " AND " << dstName << " => shifts = " << dec << input_ref.shifts << ", mask = 0x" <<
		hex << (int)input_ref.mask << dec << ", invert = " << (invert ? "true" : "false") << ", process = " << (process ? "true" : "false") << "\n";


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

string ConnectionManager::printAnaloguePort(AnaloguePort* device_port)
{
	if (device_port == NULL || device_port->dev == NULL) {
		return "";
	}

	stringstream sout;
	sout << device_port->dev->name << ":" << device_port->name << " " << _PORT_DIR(device_port->dir) << device_port->globalIndex << " (" << device_port->localIndex << ")";
	sout << device_port->val;


	return sout.str();
}

string ConnectionManager::printDevicePort(DevicePort * device_port)
{
	if (device_port == NULL || device_port->dev == NULL) {
		return "";
	}

	stringstream sout;
	sout << device_port->dev->name << ":" << device_port->name << " " << _PORT_DIR(device_port->dir) << " 0x" << hex << (int)device_port->mask << " #" << dec <<
		device_port->globalIndex << " (" << device_port->localIndex << ")";
	
	// The port is a source port
	if (device_port->dir != IN_PORT && device_port->inputs.size() > 0) {
		sout << " => ";
		for (int i = 0; i < device_port->inputs.size(); i++)
			sout << "\n\t\t" << " >> " << dec << device_port->inputs[i].shifts << " & 0x" << hex <<
			(int)device_port->inputs[i].mask << hex << " @ " << printDevicePort(device_port->inputs[i].port) << dec << "; ";
	}

	// The port is a destination port
	if (device_port->dir != OUT_PORT && device_port->portSources.size() > 0) {
		sout << " <= ";
		for (int i = 0; i < device_port->portSources.size(); i++) {
			OutputReference &out_ref = device_port->portSources[i];
			DevicePort* port = out_ref.srcPort;
			Device* src_dev = port->dev;
			sout << "\n\t\t" << src_dev->name << ":" << port->name << dec << " >> " << out_ref.srcShifts << " & 0x" << hex << (int)out_ref.dstMask;
		}
	
	}


	return sout.str();

}

string ConnectionManager::printPortSelection(PortSelection & port_selection)
{
	stringstream sout;
	sout << printDevicePort(port_selection.port) << " MASK 0x" << hex << (int) port_selection.bits.mask << " & LOW BIT " << dec << (int) port_selection.bits.lowBit;
	return sout.str();
}