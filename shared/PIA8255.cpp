#include "PIA8255.h"
#include "Device.h"
#include <iostream>
#include <filesystem>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include "VDU6847.h"
#include "Device.h"

using namespace std;

bool PIA8255::reset()
{
	Device::reset();
	mPortA = 0x0;
	mPortB = 0x0;
	mPortC = 0x0;
	mCR = 0x0;

	return true;
}

bool PIA8255::advance(uint64_t stopCycle)
{
	mCycleCount = stopCycle;

	return true;
}

/*
	PortSelection A - #B000

		Output bits
		0 - 3 Keyboard row
		4 - 7 Graphics mode

	PortSelection B - #B001

		Input bits
		0 - 5 Keyboard column
		6 CTRL key (low when pressed)
		7 SHIFT keys (low when pressed)

	PortSelection C - #B002

		Output bits
		0 Tape output
		1 Enable 2.4 kHz to cassette output
		2 Loudspeaker
		3 Not used

		Input bits
		4 2.4 kHz input
		5 Cassette input
		6 REPT key (low when pressed)
		7 60 Hz sync signal (low during flyback)

	Control Register - #B003

		Bit set reset (BSR) mode 
			0 Set value for port C output line (1 SET 0 CLEAR)
			1-3 PortSelection C bit no
			4-6 Not used
			7 Set to '0' for BSR mode

		Input-Output mode
			0 I/O function of port C lower
			1 I/O function of port B
			2 PortSelection B Mode (0: M1, 1: M2)
			3 I/O function of port C upper
			4 I/O function of PortSelection A
			6-5 PortSelection A Mode (00: M0, 01: M1, 1x: M2)
			7 Set to '1' for Input-Output mode

			M0: Simple I/O without interrupts
			M1: Handshake I/O mode or strobed I/O mode. In this mode
				either port A or port B can work as simple input port
				or simple output port, and port C bits are used for
				handshake signals before actual data transmission.
				It has interrupt handling capacity and input and output
				are latched.
			M2:	PortSelection A is bi-directional PortSelection Bcan be in M0 or M1.
				PortSelection C (6 bits) are used for handshaking. Interrupts
				can be generated.



*/

#define PIA8255_PORT_A (mDevAdr + 0)
#define PIA8255_PORT_B (mDevAdr + 1)
#define PIA8255_PORT_C (mDevAdr + 2)
#define PIA8255_CONTROL (mDevAdr + 3)



PIA8255::PIA8255(string name, uint16_t adr, int n60HzCycles, DebugInfo debugInfo, ConnectionManager* connectionManager) :
	Device(name, PIA8255_DEV, PERIPERHAL, adr, 4, debugInfo, connectionManager), mN60HzCycles(n60HzCycles)
{
	// Specify ports that can be connected to other devices
	addPort("PortA", IO_PORT, 0xff, PIA_PORT_A, &mPortA);
	addPort("PortB", IO_PORT, 0xff, PIA_PORT_B, &mPortB);
	addPort("PortC", IO_PORT, 0xff, PIA_PORT_C, &mPortC);

	// Set the size of the PIA register vector
	mMem.resize((size_t)mDevSz);

	// Initialise the PIA registers with zeros
	mMem.assign(mDevSz, 0);

	if (mDebugInfo.dbgLevel & DBG_VERBOSE)
		cout << "PIA 8255 at address 0x" << hex << setfill('0') << setw(4) << mDevAdr <<
		" to 0x" << mDevAdr + mDevSz - 1 << " (" << dec << mDevSz << " bytes)\n";
}



bool PIA8255::read(uint16_t adr, uint8_t& data)
{
	if (!validAdr(adr))
		return false;

	if (adr == PIA8255_PORT_A) {

		data = mPortA;
	}

	else if (adr == PIA8255_PORT_B)
		// For the Acorn Atom the bits are used according to below:
		//	0-5		Keyboard column (low when a key pressed)
		//	6		CTRL key(low when pressed)
		//	7		SHIFT key (low when pressed)
	{
		
		data = mPortB;

	}

	else if (adr == PIA8255_PORT_C)
		// For the Acorn Atom the bits are used according to below:
		//	0-3		Writable bits - see write method below
		//	4		2.4 kHz input
		//	5		Cassette input
		//	6		REPT key (low when pressed)
		//	7		60 Hz sync signal (low during VDU flyback) - from 6847's Field Sync (FS)
	{

		data = mPortC;

		if (mDebugInfo.dbgLevel & DBG_DEVICE)
			cout << "READ 0x" << setw(2) << setfill('0') << hex << (int)data << " from 0x" << setw(4) << adr << "\n";
	}
	else { // adr == PIA8255_CONTROL
		data = mCR;
	}

	return true;

}
bool PIA8255::write(uint16_t adr, uint8_t data)
{
	if (!validAdr(adr))
		return false;

	if (adr == PIA8255_PORT_A)
		// For the Acorn Atom the bits are used according to below:
		//	0-3		Keyboard row
		//	4-7		Graphics mode
	{

		updateOutput(PIA_PORT_A, data);
	}

	else if (adr == PIA8255_PORT_B) {

		updateOutput(PIA_PORT_B, data);
	}

	else if (adr == PIA8255_PORT_C)
		// For the Acorn Atom the bits are used according to below:
		//	0		Tape output
		//	1		Enable 2.4 kHz to cassette output
		//	2		Loudspeaker
		//	3		Colour palette selection for (semi)graphics
		//	4-7		Not writable (used as inputs instead - see read  method above)
	{

		if ((mCR & 0x80) && (mCR & 0x08)) { // PortSelection C upper bits are writable
			data &= 0x0f;
			data |= (data << 4) & 0xf0;
		}
		if ((mCR & 0x80) && (mCR & 0x02)) { // PortSelection C lower bits are writable
			data &= 0xf0;
			data |= data& 0x0f;
		}

		updateOutput(PIA_PORT_C, data);
	}

	else if (adr == PIA8255_CONTROL) {

		mCR = data;

		if (mCR & 0x80)
		// I/O Mode
		{
			if (mDebugInfo.dbgLevel & DBG_DEVICE) {
				cout << "I/O Mode: ";
				cout << " PortSelection A " << (mCR & 0x40 ? "M0" : ((mCR & 0x60) == 0x40 ? "M1" : "M2"));
				cout << " " << (mCR & 0x10 ? "IN" : "OUT");
				cout << ", PortSelection B " << (mCR & 0x04 ? "M1" : "M0");
				cout << " " << (mCR & 0x02 ? "IN" : "OUT");
				cout << ", PortSelection C upper " << (mCR & 0x08 ? "IN" : "OUT");
				cout << ", PortSelection C lower " << (mCR & 0x01 ? "IN" : "OUT");
				cout << "\n";
			}
		} 
		else
		// Bit Set Mode in which PortC is directly controlled by the writing to the Control Register
		{
			uint8_t bit = (mCR << 1) & 0x7;
			uint8_t val = mCR & 0x1;
			uint8_t port_C_data;
			port_C_data &= ~(1 << bit);
			port_C_data |= (val << bit);
			updateOutput(PIA_PORT_C, port_C_data);
			if (mDebugInfo.dbgLevel & DBG_DEVICE)
				cout << "Set PIA PortSelection C b" << bit << " to '" << val << "'\n";
		}
	}

	if (mDebugInfo.dbgLevel & DBG_DEVICE)
		cout << "WROTE 0x" << setw(2) << setfill('0') << hex << (int)data << " to 0x" << setw(4) << adr << "\n";

	return true;
}