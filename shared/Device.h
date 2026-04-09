#ifndef DEVICE_H
#define DEVICE_H

#include <cstdint>
#include <vector>
#include <string>
#include <map>
#include "DebugTracing.h"
#include <allegro5/allegro_primitives.h>
#include <iostream>
#include "VideoSettings.h"
#include "DeviceTypes.h"


using namespace std;

// Forward references to classed defined later on to allow them being referenced before their the are declared
class ConnectionManager; 
class Device;
class DeviceManager;
class MemoryMappedDevice;
class VideoDisplayUnit;
class P6502;
class RAM;
class DebugTracing;


enum Scheduling {LOW_RATE, HIGH_RATE, MICROPROCESSOR_RATE, NONE};
#define _SCHEDULING(x) (x==LOW_RATE?"Low Rate":(x==HIGH_RATE?"High Rate":(x==MICROPROCESSOR_RATE?"Microprocessor Rate":"None")))

enum DeviceId {
	DAC_ZN428_DEV, ADC_7002_DEV, SD_CARD, LATCH_74LS259, TI4689_DEV, BEEB_SERIAL_ULA_DEV,
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
(x==LATCH_74LS259?"Beeb IC31/IC32 Latch":(x==SD_CARD?"SD Card":(x==ADC_7002_DEV?"ADC 7002":(x==PROXY_MEM_MAPPED_DEV?"Proxy Memory-mapped Device":(x==DAC_ZN428_DEV?"DAC ZN428":"???")))))\
))))\
))))))))))))))

enum DeviceCategory {
	SOUND_DEVICE, MICROROCESSOR_DEVICE, VDU_DEVICE, KEYBOARD_DEVICE, PERIPHERAL, MEMORY_DEVICE, OTHER_DEVICE
};
#define _DEVICE_CATEGORY(x) (\
	x==MICROROCESSOR_DEVICE?"Microprocessor":(x== PERIPHERAL?"Peripheral":(x==MEMORY_DEVICE?"Memory":(x==VDU_DEVICE?"Video Display Unit":(x==SOUND_DEVICE?"Sound Device":(x==KEYBOARD_DEVICE?"Keyboard":"Other Device"))))))

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
	PortVal			mask = PORT_MASK;		// mask specifiyng the bits of the dst to be updated (set bit <= update)
	bool			invert = false;			// If true, the source port value will be inverted before fed to the destination port
	bool			process = false;		// If true, the receiving device's process method will be called in addition to updating the port value
} InputReference;

// Reference to a source port by a destination port - only used for port arbitration
typedef struct OutputReference_struct {
	DevicePort* srcPort;			// Reference to a source port
	PortVal		dstVal = PORT_MASK;		// Requested value for the destination port based on the source port value
	PortVal		dstMask = PORT_MASK;		// Bit mask specifying the bits of the destination port connected to the source port (copy of InputReference:mask)
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
	PortVal					ioDirMask = PORT_MASK;		// I/O direction for the bits of a bidirectional port:a set bit indicates OUT, a cleared bit IN
	PortVal					mask = 0x1;				// mask to select only the implemented bits
	int						sz = 8;					// no of bits
	PortVal*				valOut = NULL;			// pointer to variable holdding an input port's value (or a bidirectional port's input value)
	PortVal*				valIn = NULL;			// pointer to variabel holut ports wider than 8 bits (e.g., the 16-bit address bus of the 6502 CPU)
	vector<InputReference>	inputs;					// connected inputs (used only if the port is an output port)
	vector<OutputReference> portSources;			// Connected outputs - used if more than one device connects to a port (e.g., an IRQ input connected to many devices)
	bool					arbitration = false;	// true if more than one device is connected to at least to one bit slice of the port (for a destination port)
	int						fanOut = 0;				// The no of destination ports connected to the device port (for a source port)
	int						fanIn = 0;				// The no of source ports connected to the device port (for a destination port)
	bool					dstFanIn = 0;			// True if more than one source port is connected to the destination port (for a source port)
	bool					conToBiDirP = false;	// True if at least one bidirectional destination port is connected
	bool					portDirChanged = false;	// True if the direction of a bidirectional port has been changed from IN to OUT since the last call of portUpdate()
};

// Used for analogue I/O ports (e.g., the 4 channel analog input of the ADC7002) that are not connected to other devices but
// instead to an external signal source (i.e., input to the computer system) or sink (i.e., output from the computer system)
// The port value currently only be accessed by the debugger.
class AnaloguePort {
public:
	Device*					dev = NULL;				// name of the device
	string					name = "";				// name of the I/O port
	int						localIndex = -1;		// local device index for the I/O port
	int						globalIndex = -1;		// unique global index for the port
	PortDirection			dir = IO_PORT;			// I/O direction
	double*					val = nullptr;			// pointer to variable holding the port's value
	vector<AnaloguePort *>	inputs;					// connected inputs (used only if the port is an output port)
};

#define _PORT_ID(x)	(x==NULL||x->dev==NULL?"???":(x->dev->name+":"+x->name))

typedef struct BitsSelection_struct {
	PortVal mask = 0x0;	// specifies the bits of the I/O port to be connected
	int lowBit = 0;	// specifies the lowest bit if the port I/O to be connected (already identified by the mask but pre-calculated for speed reasons)
} BitsSelection;

class PortSelection {
public:
	DevicePort* port = nullptr;	// port identity
	BitsSelection	bits;	// bits selection
};

class Device {

private:

	bool mTracingEnabled = false;

	bool updateDstPortValue(DevicePort *srcPort, InputReference &dstPort, PortVal srcVal);

	void getPortSelection(DevicePort* srcPort, InputReference& dstPort, string& srcSel, string& dstSel);

protected:

	bool mPowerOnReset = true;

	DebugTracing *mDM = NULL;
	
	vector<DevicePort*> mPorts; // the device's ports that can be connected to by other devices

	vector<AnaloguePort*> mAnaloguePorts; // the device's analogue ports that are externally accessible (e.g., by the debugger)(

	ConnectionManager* mConnectionManager;

	int mPortIndex = 0;
	int mAnaloguePortIndex = 0;

	vector<Device*> mConnectedDevices;

	bool mMemoryMapped = false;

	int RESET;
	PortVal mRESET = 0x1;
	PortVal pRESET = 0x1;


	double mEmulationSpeed = 1.0;

public:

	Scheduling scheduling = MICROPROCESSOR_RATE; // default scheduling if nothing specified

	string name;

	DeviceId devType;

	DeviceCategory category;

	Device(string name, DeviceId typ, DeviceCategory cat, DebugTracing *debugTracing, ConnectionManager *connectionManager);
	virtual ~Device();

	bool tracingEnabled() { return mTracingEnabled; }
	void enableTracing() { mTracingEnabled = true; }
	void disableTracing() { mTracingEnabled = false; }

	// Reset device
	virtual bool reset() {

		if (mPowerOnReset) {
			mPowerOnReset = false;
		}

		if (DBG_LEVEL_DEV(this,DBG_VERB_DEV)) {
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

	

	// Update an output and propagate it to inputs of potentially connected other devices via the connection manager
	bool updatePort(int index, PortVal val, bool forceUpdate = false);

	// Update an analogue output port
	bool updateAnaloguePort(int index, double val);

	

	// Register that the direction of a bidirectional port (PORT_IO) has changed
	bool registerPortDirChange(int index, PortVal mask);

	// In the case the port value need to be processed immediately (if the qualifier 'P' was added to 'CONNECT')
	// then this method will be called for the device receiving the port update
	virtual void processPortUpdate(int index) {};

	// As processPortUpdate above but assumes all input portss have been updated
	virtual void processPortUpdate() { }

	// Force an update of a device's all ports to secure that all connected devices have the correct port value
	// Should be made after all device's have been connected. No triggering of connected devices are made.
	bool updatePorts();

	// Get a reference to the device's list of (digital) ports
	bool getPorts(vector<DevicePort*>*& ports) { ports = &mPorts; return true; }

	// Get a reference to the device's list of analogue ports
	bool getAnaloguePorts(vector<AnaloguePort*>*& ports) { ports = &mAnaloguePorts; return true; }

	// Get a port's current value
	static PortVal getPortVal(DevicePort* port, int &sz, PortVal &dir);
	static PortVal getPortVal(DevicePort* port) { int sz; PortVal dir; return  getPortVal(port, sz, dir); }
	PortVal getPortVal(int index, int& sz, PortVal& dir);
	PortVal getPortVal(int index) { int sz; PortVal dir; return  getPortVal(index, sz, dir); }

	// Get local port for a named I/O (used by connection manager at initialisation)
	bool getPort(string name, DevicePort * &port);

	// Get local analogue port for a named I/O (used by connection manager at initialisation)
	bool getAnaloguePort(string name, AnaloguePort*& port);

	// Used by a device to make a port available for routing
	bool registerPort(string name, PortDirection dir, PortVal mask, int& index, PortVal* val);
	bool registerPort(string name, PortDirection dir, PortVal mask, int& index, PortVal* valIn, PortVal* valOut);

	// Used by a device to make an analogue port available to the debugger
	bool registerAnaloguePort(string name, PortDirection dir, int& index, double* val);

	// Get pointer to other device to be able to call its methods
	virtual bool connectDevice(Device* dev);

	bool memoryMapped() { return mMemoryMapped;  }

	// Called by other device to get next memory address to fetch data from
	// Mainly used my video devices like the M6845 that calculates memory addresses
	// for fetching char/graphics data from to be used by the mian video devices
	// like the Beeb Video ULA that consumes the data.
	virtual bool getMemFetchAdr(uint16_t& adr, uint16_t& cursor) { adr = 0xffff;  return false; }

	// Called by a other device when the device is asked to process/transform data.
	virtual bool getDeviceData(PortVal dIn, PortVal& dOut) { dOut = PORT_MASK;  return false; }

	// Called by debugger normally to dump a periphal device's register content
	virtual bool outputState(ostream& sout) { return true; }

	// Serialise the device's state into an array that can
	// be added to an execution trace easily.
	virtual bool serialiseState(SerialisedState &serialisedState) { return true; }

	// Output a single serialised device state
	virtual bool outputSerialisedState(SerialisedState& serialisedState, ostream& sout) { return true; }

	// Tells the device about the current emulation rate, should this be used by the device
	// Normally overriden by each device that uses this rate
	virtual void setEmulationSpeed(double emulationSpeed) { mEmulationSpeed = emulationSpeed; }

};

#endif
