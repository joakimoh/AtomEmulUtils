#include "PIA8255.h"
#include "Device.h"
#include <iostream>
#include <filesystem>
#include "AtomKeyboard.h"
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include "AtomKeyboard.h"
#include "VDU6847.h"
#include "Connectionmanager.h"

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
	if (mCycleCount % mN60HzCycles < mN60HzCycles / 2) {
		mSync60HzEvent = 1;
	}
	else {
		mSync60HzEvent = 0;
	}

	return true;
}

/*
	Port A - #B000

		Output bits
		0 - 3 Keyboard row
		4 - 7 Graphics mode

	Port B - #B001

		Input bits
		0 - 5 Keyboard column
		6 CTRL key (low when pressed)
		7 SHIFT keys (low when pressed)

	Port C - #B002

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
			1-3 Port C bit no
			4-6 Not used
			7 Set to '0' for BSR mode

		Input-Output mode
			0 I/O function of port C lower
			1 I/O function of port B
			2 Port B Mode (0: M1, 1: M2)
			3 I/O function of port C upper
			4 I/O function of Port A
			6-5 Port A Mode (00: M0, 01: M1, 1x: M2)
			7 Set to '1' for Input-Output mode

			M0: Simple I/O without interrupts
			M1: Handshake I/O mode or strobed I/O mode. In this mode
				either port A or port B can work as simple input port
				or simple output port, and port C bits are used for
				handshake signals before actual data transmission.
				It has interrupt handling capacity and input and output
				are latched.
			M2:	Port A is bi-directional Port Bcan be in M0 or M1.
				Port C (6 bits) are used for handshaking. Interrupts
				can be generated.



*/

#define PIA8255_PORT_A (mDevAdr + 0)
#define PIA8255_PORT_B (mDevAdr + 1)
#define PIA8255_PORT_C (mDevAdr + 2)
#define PIA8255_CONTROL (mDevAdr + 3)



PIA8255::PIA8255(string name, uint16_t adr, int n60HzCycles, AtomKeyboard* keyboard, DebugInfo debugInfo, ConnectionManager* connectionManager) :
	Device(name, PIA8255_DEV, adr, 4, debugInfo, connectionManager), mKeyboard(keyboard), mN60HzCycles(n60HzCycles)
{
	// Specify the inputs to allow other devices to connect
#define PORT_A 0
#define PORT_B 1
#define PORT_C 2
	DevicePort A_port = { "PortA", PORT_A, -1, &mPortA };
	DevicePort B_port = { "PortB", PORT_B, -1, &mPortB };
	DevicePort C_port = { "PortC", PORT_C, -1, &mPortC };
	mPorts[PORT_A] = A_port;
	mPorts[PORT_B] = B_port;
	mPorts[PORT_C] = C_port;

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
		// No bits  should be configured as readable?
		data = mPortA;
	}
	else if (adr == PIA8255_PORT_B) {
		// 0 - 5 Keyboard column (low when a key pressed), 6 CTRL key(low when pressed), 7 SHIFT key (low when pressed)
		data = mKeyboard->readColumn();
		if (mKeyboard->ctrlPressed())
			data &= ~0x40;
		if (mKeyboard->shiftPressed())
			data &= ~0x80;

	}
	else if (adr == PIA8255_PORT_C) {
		data |= 0x40; // set REPEAT key bit (inactive LOW)
		data &= ~0x80; // clear 60 Hz sync bit
		// 4 2.4 kHz input, 5 Cassette input, 6 REPT key(low when pressed), 7 60 Hz sync signal (low during flyback) - from 6847's Field Sync (FS)
		if (mKeyboard->repeatPressed())
			data &= ~0x40;
		if (mSync60HzEvent == 1)
			data |= 0x80;

		if (mDebugInfo.dbgLevel & DBG_DEVICE)
			cout << "READ 0x" << setw(2) << setfill('0') << hex << (int)data << " from 0x" << setw(4) << adr << "\n";
	}
	else { // adr == PIA8255_CONTROL
		data = mMem[adr - mDevAdr];
	}

	return true;

}
bool PIA8255::write(uint16_t adr, uint8_t data)
{
	if (!validAdr(adr))
		return false;

	if (adr == PIA8255_PORT_A) {
		mPortA = data;
		// 0 - 3 Keyboard row, 4 - 7 Graphics mode
		if (mKeyboard != NULL)
			mKeyboard->selectRow(data & 0xf);
		if (mVdu != NULL)
			mVdu->setGraphicMode((data >> 4) & 0xf);
	}
	else if (adr == PIA8255_PORT_B) {
		mPortB = data;
		// No bits  should be configured as writable
	}
	else if (adr == PIA8255_PORT_C) {
		if ((mCR & 0x80) && (mCR & 0x08)) { // Port C upper bits are writable
			mPortC &= 0x0f;
			mPortC |= (data << 4) & 0xf0;
		}
		if ((mCR & 0x80) && (mCR & 0x02)) { // Port C lower bits are writable
			mPortC &= 0xf0;
			mPortC |= data& 0x0f;
		}

		// 0 Tape output, 1 Enable 2.4 kHz to cassette output, 2 Loudspeaker, 3 colour palette selection for (semi)graphics
		mVdu->setCSS((data >> 3) & 0x1);
	}
	else if (adr == PIA8255_CONTROL) {
		mCR = data;
		// 
		if (mCR & 0x80) {
			if (mDebugInfo.dbgLevel & DBG_DEVICE) {
				cout << "I/O Mode: ";
				cout << " Port A " << (mCR & 0x40 ? "M0" : ((mCR & 0x60) == 0x40 ? "M1" : "M2"));
				cout << " " << (mCR & 0x10 ? "IN" : "OUT");
				cout << ", Port B " << (mCR & 0x04 ? "M1" : "M0");
				cout << " " << (mCR & 0x02 ? "IN" : "OUT");
				cout << ", Port C upper " << (mCR & 0x08 ? "IN" : "OUT");
				cout << ", Port C lower " << (mCR & 0x01 ? "IN" : "OUT");
				cout << "\n";
			}
		} 
		else {
			uint8_t bit = (mCR << 1) & 0x7;
			uint8_t val = mCR & 0x1;
			mPortC &= ~(1 << bit);
			mPortC |= (val << bit);
			updateOutput(PORT_C, mPortC);
			if (mDebugInfo.dbgLevel & DBG_DEVICE)
				cout << "Set PIA Port C b" << bit << " to '" << val << "'\n";
		}
	}

	if (mDebugInfo.dbgLevel & DBG_DEVICE)
		cout << "WROTE 0x" << setw(2) << setfill('0') << hex << (int)data << " to 0x" << setw(4) << adr << "\n";

	return true;
}

void PIA8255::updateInput(uint8_t port, uint8_t bit, uint8_t val)
{
	if (port >= 0 && port <= 2 && bit >= 0 && bit <= 7 && val >= 0 && val <= 1) {
		mMem[port] &= ~(1 << bit);
		mMem[port] |= (val << bit);
	}
}

void PIA8255::setVdu(VDU6847* vdu)
{
	mVdu = vdu;
}