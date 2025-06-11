#include "SDCard.h"
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <filesystem>

SDCard::SDCard(string name, double cpuClock, string cardImage, DebugManager* debugManager, ConnectionManager* connectionManager):
	Device(name, SD_CARD, OTHER_DEVICE, cpuClock, debugManager, connectionManager)
{
	registerPort("CLK",		IN_PORT,	0x1,	CLK,	&mCLK);		// Clock
	registerPort("MOSI",	IN_PORT,	0x1,	MOSI,	&mMOSI);	// Data In
	registerPort("SEL",		IN_PORT,	0x1,	SEL,	&mSEL);		// Select
	registerPort("MISO",	OUT_PORT,	0x1,	MISO,	&mMISO);	// Data Out
/*
	mCardImage = new ifstream(cardImage, ios::in | ios::binary | ios::ate);

	if (mCardImage == NULL || !*mCardImage) {
		cout << "couldn't open SD Card Image " << cardImage << "\n";
		throw runtime_error("couldn't open  SD Card Image");
	}

	cout << "SD Card Image '" << cardImage << " added...\n";

	// Get file size (should normally equal ROM size)
	mCardImage->seekg(0, ios::end);
	streamsize file_sz = mCardImage->tellg();
	mCardImage->seekg(0);
	*/
}

SDCard::~SDCard()
{
	if (mCardImage != NULL) {
		mCardImage->close();
		delete mCardImage;
	}
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
// Data is sent with MOSI intially set to '1'.
// Then the most significant bit is placed on the MOSI with Clock 0->1->0 to latch and shift the bit.
// This is repeated until a complete command has been received.
// 
// To respond to a command, the SD card requires the CLK signal to toggle for at least 8 cycles while
// keeping MOSI high.
// 
// The SD card is placed into the SPI mode by setting the MOSI and SEL lines to '1' and toggle the CLK for at least 74 cycles.
// Then the SEL line shall be set to '0' and the reset command (CMD0) shall be sent.
// 
// A successful reception of a command will be replied with an 8-bit respone of the format below:
// +---+---+---+---+---+---+---+---+
// | b7| b6| b5| b4| b3 |b2| b1| b0|		PE = Parameter Error, AE = Address Error, EE = Erase sequence Error
// +---+---+---+---+---+---+---+---+		CE = CRC Error, IC = Illegal Command, ER = Erase Reset
// | 1 | PE| AE| EE| CE| IC| ER| IS|		IS = In Idle State
// +---+---+---+---+---+---+---+---+ 
//
// To receive this message (data in MISO), the SD CLK signal is toggled while keeping the MOSI line high and the SEL line low.
// 
// Commands:
// CMD0:	01 000000 00000000 00000000 00000000 00000000 1001010 1			0x40000000000095	RESET
//
void SDCard::processPortUpdate(int port)
{
	//cout << "Port " << port << ", SPI Clock = '" << (int)mCLK << "', SEL = '" << (int)mSEL << "', MOSI(Din) = '" << (int)mMOSI << "'\n";

	// Mode 0 assumed
	if (port == CLK) {
		if (mCLK != pCLK) {
			if (mCLK) {
				if (mSPIMode == SPI_NOT_INIT) {
					if (mMOSI == 1){//}&& mSEL == 1) {
						mReceviedBits++;
						cout << "SPI Init Phase - " << dec << mReceviedBits << " bits received - last bit was '" << (int)mMOSI << "'\n";
					}
					else
						mReceviedBits = 0;
					if (mReceviedBits == 74) {
						mSPIMode = SPI_INIT_PENDING;
						cout << "SD Card SPI Reset completed!\n";
					}
				}
				else if (mSEL == 0) {

					if (mSPIMode == SPI_INIT_PENDING) {
						mReceviedBits++;
						cout << "SPI Ready Pending Phase - " << dec << mReceviedBits << " bits received - last bit was '" << (int)mMOSI << "'\n";
						if (!mMOSI) {
							mReceviedBits = 0;
							mSPIMode = SPI_INIT_CMD_WAIT;
							mCommand = 0x0;
						}
					}

					if (mSPIMode == SPI_INIT_CMD_WAIT) {
						mReceviedBits++;
						cout << "SPI First Cmd Phase - " << dec << mReceviedBits << " bits received - last bit was '" << (int)mMOSI << "'\n";
						mCommand = ((mCommand << 1) & 0xfffffffffffe) | mMOSI;
						if (mReceviedBits == 48) {
							execCmd(mCommand);
							mReceviedBits = 0;
							mCommand = 0x0;
						}
					}
				}
			}
		}
		pCLK = mCLK;
	}
}

bool SDCard::execCmd(uint64_t cmd)
{
	cout << "Execute command 0x" << hex << cmd << "\n";
	if (mSPIMode == SPI_INIT_CMD_WAIT) {
		if (cmd == SPI_CMD0) {
			mSPIMode = SPI_READY;
			cout << "Reset cmd received => SPI Ready Phase!\n";
		}
	}
	else {
		cout << "SPI Ready Phase\n";
	}

	return true;
}