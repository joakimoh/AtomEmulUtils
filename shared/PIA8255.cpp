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
	mPortA = 0xff;
	mPortB = 0xff;
	mPortC = 0xff;
	mCR = 0x1b; // All ports are set to the input mode

	if (((mDebugInfo.dbgLevel & DBG_VERBOSE) != 0) && mRESET != pRESET) {
		cout << "PIA 8255 RESET\n";
		pRESET = mRESET;
	}

	return true;
}

bool PIA8255::advance(uint64_t stopCycle)
{
	if (!mRESET) {
		reset();
		mCycleCount = stopCycle;
		return true;
	}

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

#define PIA8255_PORT_A (mMemorySpace.adr + 0)
#define PIA8255_PORT_B (mMemorySpace.adr + 1)
#define PIA8255_PORT_C (mMemorySpace.adr + 2)
#define PIA8255_CONTROL (mMemorySpace.adr + 3)


//
// The PIA 8255 has three modes of operation that can independently be defined for
// Group A (Port A + Port C upper)  and group B  (Port B and Port C lower)
//
//											Both groups		Group A				Group B
// Control Register Setting					b7				b6b5				b2
// Mode 0: Basic I/O						1				00					0
// Mode 1: Strobed I/O						1				01					1
// Mode 2: Strobed Bi-directional I/O		1				1x					not supported
//
// In Mode 0 to 2, the port direction is given by
//
//					CR:b4		CR:b1		CR:b0		CR:b3
//					0	1		0	1		0	1		0	1
// Port A			out	in
// Port B						out	in
// Port C lower								out	in
// Port C upper											out	in
//
// ----------------------------------------------------------
// Mode 1 Strobed I/O
// ----------------------------------------------------------
// 
// Port C is used for handshaking.
// 
// INPUT CONTROL
// 
// For input control of Port A (Control Register b7b6b5b4 = 1010;b3=PortC b6b7 direction, the Port C upper pins are
// used as shown below:
// PC7,PC6	I/O
// PC5		Output	IBF_A	Input Buffer Full	- Set by a low STB; cleared when port A is read
// PC4		Input	STB_A	Strobe Input		- A low will latch Port A data
// PC3		Output	INTR_A	Interrupt Request	- set when STB=1, IBF=1 and INTE=1; reset when port A is read
// 
// For input control of Port B (Control Register b7 = 1 & b2b1=11, the Port C lower pins are
// used as shown below:
// PC1		Output	IBF_B	Input Buffer Full	- Set by a low STB; cleared when port B is read
// PC2		Input	STB_B	Strobe Input		- A low will latch Port B data
// PC0		Output	INTR_B	Interrupt Request	- set when STB=1, IBF=1 and INTE=1; reset when port B is read
// 
// OUTPUT CONTROL
// 
// For output control of Port A (Control Register b7b6b5 = 1011;b3=PortC b5b4 direction, the Port C upper pins are
// used as shown below:
// PC4,PC5	I/O
// PC7		Output	OBF_A	Output Buffer Full	- A strobe when CPU writes data to Port A; ends with low ACK
// PC6		Input	ACK_A	Acknowledge Input	- Low when a peripheral has accepted data
// PC3		Output	INTR_A	Interrupt Request	- set when OBF=1, ACK=1 and INTE=1; set by ACK going high
// 
// For output control of Port B (Control Register b7 = 1 & b2b1=10, the Port C lower pins are
// used as shown below:
// PC1		Output	OBF_B	Output Buffer Full	- A strobe when CPU writes data to Port B; ends with low ACK
// PC2		Input	ACK_B	Acknowledge Input	- Low when a peripheral has accepted data
// PC0		Output	INTR_B	Interrupt Request	- set when OBF=1, ACK=1 and INTE=1; set by ACK going high
// // 
// 
// -----------------------------------------
// Mode 2 Strobed bidirectional I/O
// -----------------------------------------
// 
// Port C is used for handshaking.
// Only used for Port A.
// 
// PC7		Output	OBF_A	Output Buffer Full	- A strobe when CPU writes data to Port A; ends with low ACK
// PC6		Input	ACK_A	Acknowledge Input	- Low when a peripheral has accepted data
// PC5		Output	IBF_A	Input Buffer Full	- Set by a low STB; cleared when port A is read
// PC4		Input	STB_A	Strobe Input		- A low will latch Port A data
// PC3		Output	INTR_A	Interrupt Request	- set when STB=1, IBF=1 and INTE=1; reset when port A is read
// PC2:0	I/O decided by CR b0
//
// 
// ----------------
// BSR Mode
// ----------------
// There is also a Single Bit Set/Reset Mode which is active if Control Register b7=1
// In this mode, b3:b1 selects the Port C pin to change value of and b0 is the value to set/reseet (1=Set, 0=RSeset).
// 
// 
// 
// The outputs are latched but the inputs are only latched in the Strobed I/O mode.
// At reset all ports will be set to input mode (Control Register becomes 10011011 = 0x9b)
// All output registers will be reset when the mode is changed.
//
PIA8255::PIA8255(string name, uint16_t adr, DebugInfo debugInfo, ConnectionManager* connectionManager) :
	MemoryMappedDevice(name, PIA8255_DEV, PERIPHERAL, adr, 4, debugInfo, connectionManager)
{
	// Specify ports that can be connected to other devices
	registerPort("RESET", IN_PORT, 0x01, RESET, &mRESET);
	registerPort("PortA", IO_PORT, 0xff, PIA_PORT_A, &mPortA);
	registerPort("PortB", IO_PORT, 0xff, PIA_PORT_B, &mPortB);
	registerPort("PortC", IO_PORT, 0xff, PIA_PORT_C, &mPortC);

	if (mDebugInfo.dbgLevel & DBG_VERBOSE)
		cout << "PIA 8255 at address 0x" << hex << setfill('0') << setw(4) << mMemorySpace.adr <<
		" to 0x" << mMemorySpace.adr + mMemorySpace.sz - 1 << " (" << dec << mMemorySpace.sz << " bytes)\n";
}



bool PIA8255::read(uint16_t adr, uint8_t& data)
{
	if (!selected(adr))
		return false;

	// Call parent class to trigger scheduling of other devices when applicable
	if (!MemoryMappedDevice::read(adr, data))
		return false;

	if (adr == PIA8255_PORT_A) {
		data = mPortA;
	}

	else if (adr == PIA8255_PORT_B)
		// For the Acorn Atom the bits are used according to below:
		//	0-5		Keyboard column (low when a key is pressed)
		//	6		CTRL key (low when pressed)
		//	7		SHIFT key (low when pressed)
	{		
		data = mPortB;
	}

	else if (adr == PIA8255_PORT_C)
		// For the Acorn Atom the bits are used according to below.
		// 
		// The lower bits are outputs (but can be read) 
		//	0		Output		Tape output
		//	1		Output		Enable 2.4 kHz to cassette output
		//	2		Output		Loudspeaker
		//	3		Output		Colour palette selection for (semi)graphics
		// 
		// The upper bits are inputs
		//  4		Input		2.4 kHz input
		//	5		Input		Cassette input
		//  6		Input		REPT key (low when pressed)
		//  7		Input		60 Hz sync signal
	{

		data = mPortC;

		if (mDebugInfo.dbgLevel & DBG_DEVICE)
			cout << "PIA EXECUTED READ 0x" << setw(2) << setfill('0') << hex << (int)data << " from 0x" << setw(4) << adr << "\n";
	}

	else { // adr == PIA8255_CONTROL
		data = mCR; // the Control Register cannot be read
	}

	return true;

}
bool PIA8255::write(uint16_t adr, uint8_t data)
{
	if (!selected(adr))
		return false;

	// Call parent class to trigger scheduling of other devices when applicable
	if (!MemoryMappedDevice::write(adr, data))
		return false;

	uint8_t group_A_mode = (mCR >> 5) & 0x3;
	uint8_t group_B_mode = (mCR >> 2) & 0x1;
	bool port_A_input = (mCR >> 4) & 0x1;
	bool port_B_input = (mCR >> 1) & 0x1;
	bool port_C_lower_input = mCR & 0x1;
	bool port_C_upper_input = (mCR >> 3) & 0x1;

	if (adr == PIA8255_PORT_A)
		// For the Acorn Atom the bits are used according to below:
		//	0-3		Keyboard row
		//	4-7		Graphics mode
	{
		if (!port_A_input) // Mode 1; Port A bits are writable
			updatePort(PIA_PORT_A, data);
	}

	else if (adr == PIA8255_PORT_B)
	{
		if (!port_B_input) // Mode 1; Port B bits are writable
			updatePort(PIA_PORT_B, data);
	}

	else if (adr == PIA8255_PORT_C)
		// For the Acorn Atom the bits are used according to below.
		// 
		// Only the lower bits are writable: 
		//	0		Output		Tape output
		//	1		Output		Enable 2.4 kHz to cassette output
		//	2		Output		Loudspeaker
		//	3		Output		Colour palette selection for (semi)graphics
		// 
		// The upper bits are inputs and cannot be written to
		//  4		Input		2.4 kHz input
		//	5		Input		Cassette input
		//  6		Input		REPT key (low when pressed)
		//  7		Input		60 Hz sync signal
	{
		uint8_t new_portC = mPortC;

		if (!port_C_lower_input) {
			new_portC &= 0xf0;
			new_portC |= data & 0x0f;
		}
		if (!port_C_upper_input) {
			new_portC &= 0x0f;
			new_portC |= (data << 4) & 0xf0;
		}

		updatePort(PIA_PORT_C, new_portC);

		
	}

	else if (adr == PIA8255_CONTROL) {

		uint8_t BSR_mode = 1 - ((data >> 7) & 0x1);

		if (!BSR_mode)
		// Basic I/O (Mode 0), Strobed I/O (Mode 1) or Bi-directional bus (Mode 2)
		{
			mCR = data;

			uint8_t new_group_A_mode = (data >> 5) & 0x3;
			uint8_t new_group_B_mode = (data >> 2) & 0x1;
			

			if (new_group_A_mode != group_A_mode) // reset Port A outputs after a mode change
				updatePort(PIA_PORT_A, 0x0);

			if (new_group_B_mode != group_B_mode) // reset Port B outputs after a mode change
				updatePort(PIA_PORT_B, 0x0);

			if (new_group_A_mode > 0 || new_group_B_mode > 0) {
				cout << "*** PIA 8255 ERROR *** Requested group A mode " << (int)new_group_A_mode << " & group B mode " << (int)new_group_B_mode <<
					" when no other modes than 0 - Basic I / O are currently supported!\n";
				return false;
			}

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
		// Bit Set Mode in which any single PortC output can be directly controlled by the writing to the Control Register
		// The value of the Control Register will not be updated in this case, i.e. previous Control Register settings
		// will b reserved.
		// The Acorn Atom uses it to output sound on PC2 e.g.
		{
			uint8_t bit = (data >> 1) & 0x7;
			uint8_t val = data & 0x1;
			uint8_t port_C_data = mPortC;
			port_C_data &= ~(1 << bit);
			port_C_data |= (val << bit);
			updatePort(PIA_PORT_C, port_C_data);
			if (mDebugInfo.dbgLevel & DBG_DEVICE)
				cout << "Set PIA PortSelection C b" << (int) bit << " to 0x" << hex << (int) val << " => PortC = 0x" << hex << (int) mPortC << dec << "\n";
		}
	}

	if (mDebugInfo.dbgLevel & DBG_DEVICE)
		cout << "PIA EXECUTED WRITE OF 0x" << setw(2) << setfill('0') << hex << (int)data << " to 0x" << setw(4) << adr << "\n";

	return true;
}