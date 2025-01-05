#include "PIA8255.h"
#include "Device.h"
#include <iostream>
#include <filesystem>
#include "AtomKeyBoard.h"
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include "AtomKeyBoard.h"

using namespace std;

PIA8255::PIA8255(uint16_t adr, AtomKeyBoard *keyboard, DebugInfo debugInfo) : Device(PIA8255_DEV, adr, 4, debugInfo), mKeyBoard(keyboard)
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

	data = mMem[adr - mDevAdr];

	return true;

}
bool PIA8255::write(uint16_t adr, uint8_t data)
{
	if (!validAdr(adr))
		return false;

	mMem[adr - mDevAdr] = data;

	return true;
}