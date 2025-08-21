#ifndef DEVICE_H
#define DEVICE_H

#include <cstdint>
#include <vector>
#include <string>
#include <map>
#include "DebugManager.h"
#include <allegro5/allegro_primitives.h>
#include <iostream>
#include "VideoSettings.h"


using namespace std;

// Forward references to classed defined later on to allow them being referenced before their the are declared
class ConnectionManager; 
class Device;
class DeviceManager;
class MemoryMappedDevice;
class VideoDisplayUnit;
class P6502;
class RAM;
class DebugManager;

enum Scheduling {BASE_RATE, SUB_RATE, INSTR_RATE, NONE};
#define _SCHEDULING(x) (x==BASE_RATE?"Field":(x==SUB_RATE?"1/2 line":(x==INSTR_RATE?"Instruction":"None")))

enum DeviceId {
	ADC_7002_DEV, SD_CARD, BEEB_VIA_LATCH, TI4689_DEV, BEEB_SERIAL_ULA_DEV,
	ACIA6850_DEV, BEEB_PAGED_ROM_SEL_DEV, CRTC6845_DEV, TT_5050_DEV,
	BEEB_KEYBOARD_DEV, BEEB_VDU_DEV, TAPE_RECORDER_DEV, ATOM_SPEAKER_DEV, ATOM_CUTS_DEV,
	ROM_DEV, RAM_DEV, PIA8255_DEV, VDU6847_DEV, VIA6522_DEV, ATOM_KB_DEV, P6502_DEV, PROXY_MEM_MAPPED_DEV,
	UNDEFINED_DEV
};
#define _DEVICE_ID(x) (\
	x==ROM_DEV?"ROM":(x==RAM_DEV?"RAM":(x==PIA8255_DEV?"PIA 8255":(x==VDU6847_DEV?"VDU 6847":(x==VIA6522_DEV?"VIA 6522":\
(x==ATOM_KB_DEV?"ATOM KB":(x==P6502_DEV?"6502":(x==ATOM_CUTS_DEV?"CUTS":(x==TAPE_RECORDER_DEV?"Tape Recorder":\
(x==ATOM_SPEAKER_DEV?"Atom Speaker":(x==BEEB_VDU_DEV?"Beeb VDU":(x==BEEB_KEYBOARD_DEV?"Beeb Keyboard":(x==CRTC6845_DEV?"CRTC 6845":\
(x==TT_5050_DEV?"TT SA5050":(x==BEEB_PAGED_ROM_SEL_DEV?"Beeb Paged Rom Register":(x==ACIA6850_DEV?"ACIA 6850":\
(x==BEEB_SERIAL_ULA_DEV?"Beeb Serial ULA":(x==TI4689_DEV?"TI4689 Sound Chip":\
(x==BEEB_VIA_LATCH?"Beeb IC31/IC32 Latch":(x==SD_CARD?"SD Card":(x==ADC_7002_DEV?"ADC 7002":(x==PROXY_MEM_MAPPED_DEV?"Proxy Memory-mapped Device":"???"))))\
))))\
))))))))))))))

enum DeviceCategory {
	SOUND_DEVICE, MICROROCESSOR_DEVICE, VDU_DEVICE, KEYBOARD_DEVICE, PERIPHERAL, MEMORY_DEVICE, OTHER_DEVICE
};
#define _DEVICE_CATEGORY(x) (\
	x==MICROROCESSOR_DEVICE?"Microprocessor":(x== PERIPHERAL?"Peripheral":(x==MEMORY_DEVICE?"Memory":(x==VDU_DEVICE?"Video Data Unit":(x==SOUND_DEVICE?"Sound Device":(x==KEYBOARD_DEVICE?"Keyboard":"Other Device"))))))

class Program {
public:
	string fileName = "";
	int loadAdr = -1;
};


class DevicePort;

// dst = dst & ~mask | ((src >> shifts) & mask)
typedef struct InputReference_struct {
	DevicePort *	port;
	int				shifts = 0;				// no of steps to downshift src value to fit dst start bit
	uint8_t			mask = 0xff;			// mask specifiyng the bits of the dst to be updated (set bit <= update)
	bool			invert = false;			// If true, the source port value will be inverted before fed to the destination port
	bool			process = false;		// If true, the receiving device's process method will be called in addition to updating the port value
} InputReference;

// Reference to a source port by a destination port - only used for port arbitration
typedef struct OutputReference_struct {
	DevicePort* srcPort;			// Reference to a source port
	uint8_t		dstVal = 0xff;		// Requested value for the destination port based on the source port value
	uint8_t		dstMask = 0xff;		// Bit mask specifying the bits of the destination port connected to the source port (copy of InputReference:mask)
	int			srcShifts = 0;		// no of steps to downshift src value to fit dst start bit (copy of InputReference:shifts)
} OutputReference;

enum PortDirection {IN_PORT, OUT_PORT, IO_PORT};
#define _PORT_DIR(x) (x==IN_PORT?"IN":(x==OUT_PORT?"OUT":"IN/OUT"))

class DevicePort {
public:
	Device *				dev = NULL;				// name of the device
	string					name = "";				// name of the I/O port
	int						localIndex = -1;		// local device index for the I/O port
	int						globalIndex = -1;		// unique global index for the port
	PortDirection			dir = IO_PORT;			// I/O direction
	uint8_t					ioDirMask = 0xff;			// I/O direction for the bits of a bidirectional port:a set bit indicates OUT, a cleared bit IN
	uint8_t					mask = 0x1;				// mask to select only the implemented bits
	uint8_t	*				valOut = NULL;			// pointer to variable holding an output port's value (or a bidirectional port's output value)
	uint8_t *				valIn = NULL;			// pointer to variabel holding an input port's value (or a bidirectional port's input value)
	vector<InputReference>	inputs;					// connected inputs (used only if the port is an output port)
	vector<OutputReference> portSources;			// Connected outputs - used if more than one device connects to a port (e.g., an IRQ input connected to many devices)
	bool					arbitration = false;	// true if more than one device is connected to at least to one bit slice of the port (for a destination port)
	int						fanOut = 0;				// The no of destination ports connected to the device port (for a source port)
	int						fanIn = 0;				// The no of source ports connected to the device port (for a destination port)
	bool					dstFanIn = 0;			// True if more than one source port is connected to the destination port (for a source port)
	bool					conToBiDirP = false;	// True if at least one bidirectional destination port is connected
	bool					portDirChanged = false;	// True if the direction of a bidirectional port has been changed from IN to OUT since the last call of portUpdate()
};

#define _PORT_ID(x)	(x==NULL||x->dev==NULL?"???":(x->dev->name+":"+x->name))

typedef struct BitsSelection_struct {
	uint8_t mask = 0x0;	// specifies the bits of the I/O port to be connected
	uint8_t lowBit = 0;	// specifies the lowest bit if the port I/O to be connected (already identified by the mask but pre-calculated for speed reasons)
} BitsSelection;

class PortSelection {
public:
	DevicePort* port;	// port identity
	BitsSelection	bits;	// bits selection
};

typedef struct Connector_struct { // specifies the receiving unique part of a routing
	BitsSelection srcBits; // specifies the bits of the output port to be connected
	PortSelection dstPort; // specifies the the input port to be connected
} Connection;

typedef struct Routing_struct { // specifies how an output port of one device is connected to the input ports of one or more receving devices
	DevicePort *		srcPort;
	vector <Connection>	connections;
} Routing;

class Device {

private:
	bool updateDstPortValue(DevicePort *srcPort, InputReference &dstPort, uint8_t srcVal);
	bool updateConnectedPorts(vector<InputReference>& connectedPorts, uint8_t val, DevicePort* port, bool changed);

	void getPortSelection(DevicePort* srcPort, InputReference& dstPort, string& srcSel, string& dstSel);

protected:

	bool mPowerOnReset = true;

	DebugManager *mDM = NULL;

	uint64_t mCycleCount = 0;
	
	vector<DevicePort*> mPorts; // the device's input ports that can be connected to by other devices

	ConnectionManager* mConnectionManager;

	int mPortIndex = 0;

	vector<Device*> mConnectedDevices;

	bool mMemoryMapped = false;

	int RESET;
	uint8_t mRESET = 0x1;
	uint8_t pRESET = 0x1;

	bool mKeepsTime = true;

public:

	double mCPUClock = 2.0;

	Scheduling scheduling = INSTR_RATE; // default scheduling if nothing specified

	string name;

	DeviceId devType;

	DeviceCategory category;

	Device(string name, DeviceId typ, DeviceCategory cat, double cpuClock, DebugManager *debugManager, ConnectionManager *connectionManager);
	~Device();

	uint64_t getCycleCount() { return mCycleCount; }

	// Reset device
	virtual bool reset() {

		if (mPowerOnReset) {
			mCycleCount = 0;
			mPowerOnReset = false;
		}

		if (DBG_LEVEL_DEV(this,DBG_VERBOSE)) {
			cout << "'" << this->name << "' RESET\n";
			//pRESET = mRESET;
		}
		return true;
	}

	// Power On
	virtual bool power() {
		reset();
		return true;
	}

	//  Advance until clock cycle stopcycle has been reached
	virtual bool advanceUntil(uint64_t stopCycle) { mCycleCount = stopCycle; return true; }

	// Update an output and propagate it to inputs of potentially connected other devices via the connection manager
	bool updatePort(int index, uint8_t val, bool forceUpdate = false);

	// Register that the direction of a bidirectional port (PORT_IO) has changed
	bool registerPortDirChange(int index, uint8_t mask);

	// In the case the port value need to be processed immediately (if the qualifier 'P' was added to 'CONNECT')
	// then this method will be called for the device receiving the port update
	virtual void processPortUpdate(int index) {};

	// Force an update of a device's all ports to secure that all connected devices have the correct port value
	// Should be made after all device's have been connected. No triggering of connected devices are made.
	bool updatePorts();

	// Get local port index for a named I/O (used by connection manager at initialisation)
	bool getPortIndex(string name, DevicePort * &port);

	// Used by a device to make a port available for routing
	bool registerPort(string name, PortDirection dir, uint8_t mask, int& index, uint8_t* val);
	bool registerPort(string name, PortDirection dir, uint8_t mask, int& index, uint8_t* valIn, uint8_t* valOut);

	// Get pointer to other device to be able to call its methods
	virtual bool connectDevice(Device* dev);

	bool memoryMapped() { return mMemoryMapped;  }

	// Called by other device to get next memory address to fetch data from
	// Mainly used my video devices like the M6845 that calculates memory addresses
	// for fetching char/graphics data from to be used by the mian video devices
	// like the Beeb Video ULA that consumes the data.
	virtual bool getMemFetchAdr(uint16_t& adr, uint16_t& cursor) { adr = 0xffff;  return false; }

	// Called by a other device when the device is asked to process/transform data.
	virtual bool getDeviceData(uint8_t dIn, uint8_t& dOut) { dOut = 0xff;  return false; }

	// Called by debugger normally to dump a periphal device's register content
	virtual bool outputState(ostream& sout) { return true; }

	//
	bool getTimeSec(double& t) { t = mCycleCount * 1e-6 / mCPUClock; return mKeepsTime; }

};

#endif
