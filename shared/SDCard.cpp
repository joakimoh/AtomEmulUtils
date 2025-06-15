#include "SDCard.h"
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <filesystem>
#include "Utility.h"

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
				//cout << "MOSI = '" << dec << (int)mMOSI << "', MISO = '" << (int)mMISO << "'\n";
				if (!mInitialised) {
					if (mMOSI == 1) {
						mReceivedBits++;
						//cout << "SPI Not Init Phase - " << dec << mReceivedBits << " bits received - last bit was '" << (int)mMOSI << "'\n";
					}
					else
						mReceivedBits = 0;
					if (mReceivedBits == 74) {
						mSPIMode = SPI_CMD_PREAMBLE_WAIT;
						mReceivedBits = 0;
						mInitialised = true;
						cout << "SD Card: SPI Reset (74 x '1') completed  - Waiting for Reset command CMD0!\n";
					}
				}
				else if (mSEL == 0) {

					if (mSPIMode == SPI_IDLE_WAIT) {
						if (mMOSI == 1) {
							mSPIMode = SPI_CMD_PREAMBLE_WAIT;
							//cout << "SD Card: Idle level '1' detected\n";
						}
					}
					else if (mSPIMode == SPI_CMD_PREAMBLE_WAIT) {
						if (mMOSI == 0)
							mReceivedBits++;
						else { // mMOSI == 1
							if (mReceivedBits == 1) {
								mSPIMode = SPI_CMD_WAIT; // Preamble '01' detected
								mShiftRegister = 0b01; // update the command shift register with the first received '01' bits shifted left one step
								mReceivedBits = 2;
								mReceivedBytes = 0;
								//cout << "SD Card: Preamble '01' detected\n";
							}
							else {
								mReceivedBits = 0;// restart search for '01'
								mReceivedBytes = 0;
								//cout << "SD Card: Restart preamble search!\n";
							}
						}
					}
					else if (mSPIMode == SPI_CMD_WAIT) {
						mReceivedBits++;
						mShiftRegister = ((mShiftRegister << 1) & 0xfe) | mMOSI;
						if (mReceivedBits % 8 == 0) {
							//cout << "CMD byte #" << mReceivedBytes << " = 0x" << hex << (int)mShiftRegister << "\n";
							mCommand[mReceivedBytes++] = mShiftRegister;						
						}
						if (mReceivedBits == 48) {
							execCmd(mCommand);
							mReceivedBits = 0;
							mReceivedBytes = 0;
							mFirstBit = true;
							mSPIMode = SPI_RESP_WAIT;
						}
				}

					else { // mSPIMode == SPI_RESP_WAIT
						if (mSentBits % 8 == 0) {
							//cout << "RSP byte #" << mSentBytes << " (bits " << mSentBits << "+) = 0x" << hex << (int)mResponse[mSentBytes] << "\n";
							mShiftRegister = mResponse[mSentBytes++];
						}
						else
							mShiftRegister <<= 1;
						updatePort(MISO, (mShiftRegister >> 7) & 0x1);
						mSentBits++;
						cout << "MISO = " << (int)mMISO << ", Shift Register = 0b" << Utility::int2binStr(mShiftRegister, 8) << " = 0x" << hex << (int)mShiftRegister << "\n";
						if (mSentBits == nResponseBits) {
							cout << "Response of type R" << mResponseType << " (response 0x" << hex << bytes2str(mResponse) << ") sent\n";
							mSentBits = 0;
							mSentBytes = 0;
							mSPIMode = SPI_IDLE_WAIT;
						}
					}
				}
			}
		}
		pCLK = mCLK;
		pMOSI = mMOSI;
	}
}

bool SDCard::execCmd(vector <uint8_t> request)
{
	
	uint8_t cmd = request[0] & 0x3f;

	//cout << "cmd = 0x" << hex << (int)cmd << "\n";

	bool valid_cmd = true;

	if (mResetCmdReceived || cmd == SPI_CMD_0) {

		switch (cmd) {
		case SPI_CMD_0:
		case SPI_CMD_1:
		case SPI_CMD_6:
		case SPI_CMD_8:
		case SPI_CMD_9:
		case SPI_CMD_10:
		case SPI_CMD_12:
		case SPI_CMD_13:
		case SPI_CMD_16:
		case SPI_CMD_17:
		case SPI_CMD_18:
		case SPI_CMD_24:
		case SPI_CMD_25:
		case SPI_CMD_27:
		case SPI_CMD_28:
		case SPI_CMD_29:
		case SPI_CMD_30:
		case SPI_CMD_32:
		case SPI_CMD_33:
		case SPI_CMD_38:
		case SPI_CMD_42:
		case SPI_CMD_55:
		case SPI_CMD_56:
		case SPI_CMD_58:
		case SPI_CMD_59:
			break;
		default:
			valid_cmd = false;
			break;
		}

		if (valid_cmd) {

			SPICmdInfo cmd_info = spiCmdInfo[SPICmdEnum(cmd)];
			mResponseType = cmd_info.respType;
			SPIRspInfo rsp_info = spiRspInfo[mResponseType];
			uint8_t req_crc_byte = (crc7(request, 0, 5) << 1) | 0x1;
			nResponseBits = rsp_info.nBytes * 8;
			mResponse = rsp_info.okResponse;
			cout << "Command CMD" << dec << (int)cmd << " (request 0x" << bytes2str(request) << ") with expected CRC byte 0x" << hex << (int)req_crc_byte <<
				" received - will send a response of type R" << mResponseType << " (response 0x" << bytes2str(mResponse) << ")" <<
				 " with " << dec << nResponseBits << " bits (" << rsp_info.nBytes << " bytes)...\n";


			switch (cmd) {

			case SPI_CMD_0:
				mResetCmdReceived = true;
				break;

			case SPI_CMD_1:
				break;

			case SPI_CMD_6:
				break;

			case SPI_CMD_8:
			{
				uint8_t supply_voltage = request[3] & 0xf;
				uint8_t check_pattern = request[4];
				mResponse[4] |= supply_voltage;
				mResponse[5] = check_pattern;
				mResponse[6] = (crc7(mResponse, 1, 5) << 1) | 0x1;
				break;
			}

			case SPI_CMD_9:
				break;

			case SPI_CMD_10:
				break;

			case SPI_CMD_12:
				break;

			case SPI_CMD_13:
				break;

			case SPI_CMD_16:
				break;

			case SPI_CMD_17:
				break;

			case SPI_CMD_18:
				break;

			case SPI_CMD_24:
				break;

			case SPI_CMD_25:
				break;

			case SPI_CMD_27:
				break;

			case SPI_CMD_28:
				break;

			case SPI_CMD_29:
				break;

			case SPI_CMD_30:
				break;

			case SPI_CMD_32:
				break;

			case SPI_CMD_33:
				break;

			case SPI_CMD_38:
				break;

			case SPI_CMD_42:
				break;

			case SPI_CMD_55:
				break;

			case SPI_CMD_56:
				break;

			case SPI_CMD_58:
				break;

			case SPI_CMD_59:
				break;

			default:
				valid_cmd = false;
				break;

			}

		}

		if (!valid_cmd) {
			cout << "invalid command CMD" << dec << (int)cmd << " (request 0x" << bytes2str(request) << ") received !\n";
		}

	}

	return valid_cmd;
}

//
// Compute the SPI CRC7 with the polynom below:
//  G(x) = x7 + x3 + 1
//
// The CRC is the remainder you get by dividing each
// data byte with the polynom above.
//
uint8_t SDCard::crc7(vector <uint8_t>& data, int startPos, int len)
{
	uint8_t polynom = 0x89;
	uint8_t crc = 0x0;
	for (int i = startPos; i < startPos+len; i++) {
		crc = (crc << 1) ^ data[i];
		crc = (crc & 0x80) ? crc ^ polynom : crc;
		for (int j = 1; j < 8; j++) {
			crc <<= 1;
			if (crc & 0x80)
				crc ^= polynom;
		}
	}
	return crc;
}

//
// Compute the SPI CRC16 with the polynom below:
//  x16 +x12 +x5 +1 
//
// The CRC is the remainder you get by dividing each
// data byte with the polynom above.
//
uint16_t SDCard::crc16(vector <uint8_t>& data, int startPos, int len)
{

	return 0;
}

string SDCard::bytes2str(vector <uint8_t> data)
{
	string s;
	for (int i = 0; i < data.size(); i++)
		s += Utility::int2hexStr(data[i],2);

	return s;
}