#include "ConnectionManager.h"
#include "Tokeniser.h"

ConnectionManager::ConnectionManager(Devices* devices) : mDevices(devices)
{

}

bool ConnectionManager::receiveUpdate(int index, uint8_t val)
{
	if (mRouting.find(index) == mRouting.end())
		return false;
	
	Routing routing = mRouting[index];

	vector<Connection>::iterator connections = routing.connections;
		
	// Iterate over all connections
	for (vector<Connection>::iterator it = connector.begin(); ; it != connections.end(); it++) {

		it->dstPort.dev->updateInput(it, val);

		cout << "UPDATE: " << connector.srcName << " => " << connector.dstName << " = 0x" << hex << src_val << dec << "\n";
	}

	return true;

}

bool ConnectionManager::getRoutingIndex(Port port, Routing &routing)
{
	inex = -1;
	map<int, vector<Routing>>::iterator it;
	for (it = mRouting.begin(); it != mRouting.size();) {
		routing = it->second;
		if (routing.srcDev->name == port.dev->name && routing.srcPortName == port.name) {
			return true;
		}
	}
	return false;
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
// CONNECT	VDU:FS			PIA:PortC;7			- Connect VDU FS output to PIA Port C b7			
// CONNECT	PIA:PortA;7		VDU:A/G				- Connect PIA Port A b7:4 to VDU A/G input and GM0:2 inputs
// CONNECT	PIA:PortA;6;4	VDU:GM
// CONNECT	PIA:_;7			VDU:INV				- Connect data bus b7:6 to VDU inputs INV,A/S & INT/EXT
// CONNECT	PIA:_;6			VDU:A/S
// CONNECT	PIA:_;6			VUD:INT/EXT
//
bool ConnectionManager::extractPort(string name, Port &port)
{
	try {

		// Get device ("_" <=> data bus input of the device itself)
		Tokeniser dev_tok(name, ':');
		string dev_name;
		if (!dev_tok.nextToken(dev_name))
			return false;
		Device* dev =  mDevices->getDevice(dev_name);
		if (dev == NULL)
				return false;

		// Get port reference
		string port_ref;
		if (!dev_tok.nextToken(port_ref))
			return false;
	
		// Get port name
		Tokeniser port_tok(port_ref, ';');
		string port_name;
		if (1port_tok.nextToken(port_name))
			return false;
		int index = -1;
		if (port_name != "_") {
			if (!dev->getPortIndex(port_name, &index))
				return false;
		}

		// Get bit range (if present)
		Tokeniser bits_tok(port, ';';
		string hb_s, lb_s;
		int lb = 0;
		int hb = 7;
		if (port_tok.nextToken(hb)) {
			hb = stoi(hb_s);
			lb = hb;
		}
		if (dev_tok.nextToken(lb))
			lb = stoi(lb_s);
		if (hb < 0 || hb > 7 || lb < 0 || hb > 7)
			return false;

		// Create I/O port
		port.dev = dev;
		port.lowBit = lb;
		port.mask = 0;
		for (int i = lb; i < hb; i++) {
			port.mask |= (1 << i);
		}
		port.name = port_name;
		port.index = index;
		
		return true;
		
		
	}
	catch {
		return false;
	}
}

//
bool ConnectionManager::connect(string srcName, string dstName, int &index)
{
	Port src_port;
	if (!extractPort(srcName, src_port)) {
		cout << "Invalid format for source routing '" << srcName << "'\n";
		return false;
	}

	Port dst_port;
	if (!extractPort(dstName, dst_port)) {
		cout << "Invalid format for destination routing '" << dstName << "'\n";
		return false;
	}

	Connection connection;
	connection.srcPort = src_port;
	connection.dstPort = dst_port;

	// Lookup existing routing (if existing). If not existing, create a new one
	Routing routing;
	if (!getRoutingIndex(src_port, routing)) {		
		routing.index = mIndex++;
		routing.srcDev = src_dev;
		routing.srcPortIndex = src_port.localPort.index;
		routing.srcName = scrName;
	}

	routing.connections.push_back(connection);
	routing.srcDev.assingGlobalPortIndex(src_port.localPort.index, routing.index);

	cout << "CONNECTING " << connector.srcName << " WITH " << connector.dstName << "\n";

	return true;
}
