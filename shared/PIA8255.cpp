#include "PIA8255.h"
#include "Device.h"
#include <iostream>
#include <filesystem>
#include "AtomKeyboard.h"
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include "AtomKeyboard.h"

using namespace std;

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

PIA8255::PIA8255(uint16_t adr, AtomKeyboard *keyboard, DebugInfo debugInfo) : 
	Device(PIA8255_DEV, adr, 4, debugInfo), mKeyboard(keyboard)
{

	// Set the size of the PIA register vector
	mMem.resize((size_t)mDevSz);

	// Initialise the PIA registers with zeros
	mMem.assign(mDevSz, 0);

	if (mDebugInfo.verbose)
		cout << "PIA 8255 at address 0x" << hex << setfill('0') << setw(4) << mDevAdr <<
		" to 0x" << mDevAdr + mDevSz - 1 << " (" << dec << mDevSz << " bytes)\n";
}

bool PIA8255::read(uint16_t adr, uint8_t& data)
{
	if (!validAdr(adr))
		return false;

	if (adr == PIA8255_PORT_A) {
		// No bits  should be configured as readable?
		data = mMem[adr - mDevAdr];
	}
	else if (adr == PIA8255_PORT_B) {
		// 0 - 5 Keyboard column (low when a key pressed), 6 CTRL key(low when pressed), 7 SHIFT keys(low when pressed)
		uint8_t selected_row = mMem[PIA8255_PORT_A - mDevAdr] & 0x0f;
		data = 0xff; // set all key bits (as they are low only if a key is pressed)
		for (uint8_t col = 0; col < 6; col++) {
			if (mKeyboard->PA0_03_selectedRow_keys[selected_row][col] > 0) {
				data &= ~(1 << col);
				//cout << "COL " << (int)col << " SELECTED\n";
			}
		}
		if (mKeyboard->PB6_ctrl)
			data &= ~0x40;
		if (mKeyboard->PB7_shift)
			data &= ~0x80;
		
	}
	else if (adr == PIA8255_PORT_C) {
		data |= 0x40; // set REPEAT key bit (inactive LOW)
		data &= ~0x80; // clear 60 Hz sync bit
		// 4 2.4 kHz input, 5 Cassette input, 6 REPT key(low when pressed), 7 60 Hz sync signal (low during flyback)
		if (mKeyboard->PC6_repeat)
			data &= ~0x40;
		if (mSync60HzState == 1)
			data |= 0x80;
		//cout << "READ 0x" << setw(2) << setfill('0') << hex << (int)data << " from 0x" << setw(4) << adr << "\n";
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
		// 0 - 3 Keyboard row, 4 - 7 Graphics mode
	}
	else if (adr == PIA8255_PORT_B) {
		// No bits  should be configured as writable
	}
	else if (adr == PIA8255_PORT_C) {
		// 0 Tape output, 1 Enable 2.4 kHz to cassette output, 2 Loudspeaker, 3 Not used
	}
	else if (adr == PIA8255_CONTROL) {
		// 
		if (data & 0x80) {
			cout << "I/O Mode: ";
			cout << " Port A " << (data & 0x40 ? "M0" : ((data & 0x60) == 0x40 ? "M1" : "M2"));
			cout << " " << (data & 0x10 ? "IN" : "OUT");
			cout << ", Port B " << (data & 0x04 ? "M1" : "M0");
			cout << " " << (data & 0x02 ? "IN" : "OUT");
			cout << ", Port C upper " << (data & 0x08 ? "IN" : "OUT");
			cout << ", Port C lower " << (data & 0x01 ? "IN" : "OUT");
			cout << "\n";
		} 
		else {
			cout << "BSR Mode\n";
		}
	}

	mMem[adr - mDevAdr] = data;

	//cout << "WROTE 0x" << setw(2) << setfill('0') << hex << (int)data << " to 0x" << setw(4) << adr << "\n";

	return true;
}

void PIA8255::toggle60Hz()
{
	mSync60HzState = 1 - mSync60HzState;
}