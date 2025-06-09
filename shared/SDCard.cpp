#include "SDCard.h"

SDCard::SDCard(string name, double cpuClock, DebugManager* debugManager, ConnectionManager* connectionManager): 
	Device(name, SD_CARD, OTHER_DEVICE, cpuClock, debugManager, connectionManager)
{
	registerPort("CLK",		IN_PORT,	0x1, &mCLK);	// Clock
	registerPort("MISO",	OUT_PORT,	0x1, &mMISO);	// Data Out
	registerPort("MOSI",	IN_PORT,	0x1, &mMOSI);	// Data In
	registerPort("SEL",		IN_PORT,	0x1, &mSEL);	// Select
}

//
// In SPI, data shift and data latch are done on opposite clock edges.
// 
// There are four different modes (from the combination of clock's polarity and phase)
// that the master IC can configure.
// 
// The microprocessor is the master and the SD Card is the slave.
//
// Mode		Clock Pulses	Latch		Shift
// 0		Positive		LOW->HIGH	HIGH>LOW
// 1		Positive		HIGH->LOW	LOW->HIGH
// 2		Negative		HIGH->LOW	LOW->HIGH
// 3		Negative		LOW->HIGH	HIGH>LOW
//
// Commands sent to the SD Card are 48-bits and have the following format:
// 
// +---+---------------+-----------------+-----------+-+
// |0|1| 6-bit Command | 32-bit Argument | 7-bit CRC |1|
// +-------------------+-----------------+-----------+-+
// 
// Data is sent with MOSI intially High ('1') as the idle state. When data transmission starts
// Then the most significant bit is placed on the MOSI with Clock 0->1->0 to latch and shift the bit.
// This is repeated until a complet command has been received.
// 
// To respond to a command, the SD card requires the SD CLK signal to toggle for at least 8 cycles while
// keeping MOSI high.
// 
// The SD card is placed into the SPI mode by setting the MOSI and CS lines to logic value 1 and toggle SD CLK for at least 74 cycles.
// Then the CS line shal lbe set to 0 and the reset command CMD0 shall be sent.
// 
// A successful reception of a command will be replied with an 8-bit respone of the format below:
// +---+---+---+---+---+---+---+---+
// | b7| b6| b5| b4| b3 |b2| b1| b0|		PE = Parameter Error, AE = Address Error, EE = Erase sequence Error
// +---+---+---+---+---+---+---+---+		CE = CRC Error, IC = Illegal Command, ER = Erase Reset
// | 1 | PE| AE| EE| CE| IC| ER| IS|		IS = In Idle State
// +---+---+---+---+---+---+---+---+ 
//
// To receive this message (data in MISO), the SD CLK signal is toggled while keeping the MOSI line high and the CS line low.
// 
// Commands:
// CMD0:	01 000000 00000000 00000000 00000000 00000000 1001010 1			RESET
//
void SDCard::processPortUpdate(int port)
{
	// Mode 0 assumed
	if (port == CLK) {
		if (mCLK != pCLK) {
			if (mCLK) {
				if (mSPIMode == SPI_NOT_INIT) {
					if (mMOSI == 1 && mSEL == 1)
						mReceviedBits++;
					else
						mReceviedBits = 0;
					if (mReceviedBits == 74)
						mSPIMode = SPI_READY_PEND;
				}
				else if (mSEL == 0) {
					mCommand = ((mCommand >> 1) & 0x7f) | mMOSI;
					mReceviedBits++;
					if (mReceviedBits == 48) {
						execCmd(mCommand);
						mReceviedBits = 0;
					}
				}
			}
		}
		pCLK = mCLK;
	}
}

bool SDCard::execCmd(uint64_t cmd)
{
	if (mSPIMode == SPI_READY_PEND) {

	}
	else {

	}

	return true;
}