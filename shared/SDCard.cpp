#include "SDCard.h"
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <filesystem>
#include "Utility.h"

namespace fs = std::filesystem;

SDCard::SDCard(string name, double cpuClock, string cardImage, DebugManager* debugManager, ConnectionManager* connectionManager):
	Device(name, SD_CARD, OTHER_DEVICE, cpuClock, debugManager, connectionManager)
{
	registerPort("CLK",		IN_PORT,	0x1,	CLK,	&mCLK);		// Clock
	registerPort("MOSI",	IN_PORT,	0x1,	MOSI,	&mMOSI);	// Data In
	registerPort("SEL",		IN_PORT,	0x1,	SEL,	&mSEL);		// Select
	registerPort("MISO",	OUT_PORT,	0x1,	MISO,	&mMISO);	// Data Out

	mDataResponse.resize(512);

	mCardImage = new ifstream(cardImage, ios::in | ios::binary | ios::ate);

	if (mCardImage == NULL || !mCardImage->is_open()) {
		cout << "couldn't open SD Card Image " << cardImage << "\n";
		throw runtime_error("couldn't open  SD Card Image");
	}

	cout << "SD Card Image '" << cardImage << " added...\n";

	// Get file size (should normally equal ROM size)
	mCardImage->seekg(0, ios::end);
	streamsize file_sz = mCardImage->tellg();
	mCardImage->seekg(0);

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
						mSPIRxMode = SPI_Rx_PREAMBLE_WAIT;
						mReceivedBits = 0;
						mInitialised = true;
						cout << "SD Card: SPI Reset (74 x '1') completed  - Waiting for Reset command CMD0!\n";
					}
				}
				else if (mSEL == 0) {

					//
					// Rx processing
					//
					if (mSPIRxMode == SPI_Rx_IDLE) {
						if (mMOSI == 1) {
							mSPIRxMode = SPI_Rx_PREAMBLE_WAIT;
							//cout << "SD Card: High level '1' detected\n";
						}
					}
					else if (mSPIRxMode == SPI_Rx_PREAMBLE_WAIT) {
						if (mMOSI == 0)
							mReceivedBits++;
						else { // mMOSI == 1
							if (mReceivedBits == 1) {
								mSPIRxMode = SPI_Rx_CMD_WAIT; // Preamble '01' detected
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
					else if (mSPIRxMode == SPI_Rx_CMD_WAIT) {
						mReceivedBits++;
						mShiftRegister = ((mShiftRegister << 1) & 0xfe) | mMOSI;
						if (mReceivedBits % 8 == 0) {
							//cout << "CMD byte #" << mReceivedBytes << " = 0x" << hex << (int)mShiftRegister << "\n";
							mCommand[mReceivedBytes++] = mShiftRegister;						
						}
						//cout << "MOSI = " << (int)mMOSI << ", Shift Register = 0b" << Utility::int2binStr(mShiftRegister, 8) << " = 0x" << hex << (int)mShiftRegister << "\n";
						if (mReceivedBits == 48) {
							execCmd(mCommand);
							mReceivedBits = 0;
							mReceivedBytes = 0;
							mFirstBit = true;
							mSPIRxMode = SPI_Rx_IDLE;
						}

				}

					//
					// Tx processing
					//
					if (mSPITxMode == SPI_Tx_IDLE) {
						// Nothing to respond to => do nothing
					}
					else { // mSPITxMode == SPI_Tx_RSP_WAIT, mSPITxMode == SPI_Tx_RSP_DATA_WAIT or mSPITxMode == SPI_Tx_DATA_WAIT
						if (mSentBits % 8 == 0) {
							if (mSPITxMode != SPI_Tx_DATA_WAIT) {
								//cout << "RSP byte #" << dec << mSentBytes << " (bits " << mSentBits << "+) = 0x" << hex << (int)mCmdResponse[mSentBytes] << "\n";

								mShiftRegister = mCmdResponse[mSentBytes++];
							}
							else {
								//cout << "DATA byte #" << dec << mSentBytes << " (bits " << mSentBits << "+) = 0x" << hex << (int)mDataResponse[mSentBytes] << "\n";

								mShiftRegister = mDataResponse[mSentBytes++];
							}
						}
						else
							mShiftRegister <<= 1;
						updatePort(MISO, (mShiftRegister >> 7) & 0x1);
						mSentBits++;
						int response_bits = (mSPITxMode != SPI_Tx_DATA_WAIT ? nCmdResponseBits : mDataResponseBits);
						cout << "bytes = " << dec << mSentBytes << " (" << response_bits / 8 << ")"  << ", bits = " << dec <<
							mSentBits << " (" << response_bits << "), MISO = " << (int)mMISO << ", Shift Register = 0b" << Utility::int2binStr(mShiftRegister, 8) << " = 0x" <<
							hex << (int)mShiftRegister << "\n";
						if (mSentBits == response_bits) {
							if (mSPITxMode != SPI_Tx_DATA_WAIT)
								cout << "Response of type R" << mResponseType << " (response 0x" << hex << bytes2str(mCmdResponse) << 
								") [" << response_bits << " bits] sent\n";
							else // mSPITxMode == SPI_Tx_DATA_WAIT
								cout << "Data token '" << bytes2str(mDataResponse) << "' [" << response_bits << " bits] sent\n";
							mSentBits = 0;
							mSentBytes = 0;
							if (mSPITxMode == SPI_Tx_RSP_WAIT || mSPITxMode == SPI_Tx_DATA_WAIT) {
								mSPITxMode = SPI_Tx_IDLE;
							}
							else { // mSPITxMode == SPI_Tx_RSP_DATA_WAIT
								cout << "Response sent - will now be followed by a data token!\n";
								mSPITxMode = SPI_Tx_DATA_WAIT;
							}
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
			nCmdResponseBits = rsp_info.nBytes * 8;
			mCmdResponse = rsp_info.okResponse;
			mSPITxMode = SPI_Tx_RSP_WAIT;
			cout << "Command CMD" << dec << (int)cmd << " (request 0x" << bytes2str(request) << ") " << cmd_info.mnemonic << " with expected CRC byte 0x" << hex << (int)req_crc_byte <<
				" received - will send a response of type R" << mResponseType << " (response 0x" << bytes2str(mCmdResponse) << ")" <<
				 " with " << dec << nCmdResponseBits << " bits (" << rsp_info.nBytes << " bytes)...\n";


			switch (cmd) {

			case SPI_CMD_0:
				mCmdResponse[1] = 0x1; // set idle status bit
				mResetCmdReceived = true;
				break;

			case SPI_CMD_1:
				mCmdResponse[1] = 0x0; // clear idle status bit
				break;

			case SPI_CMD_6:
				break;

			case SPI_CMD_8:
			{
				uint8_t supply_voltage = request[3] & 0xf;
				uint8_t check_pattern = request[4];
				mCmdResponse[4] |= supply_voltage;
				mCmdResponse[5] = check_pattern;
				mCmdResponse[6] = (crc7(mCmdResponse, 1, 5) << 1) | 0x1;
				break;
			}

			case SPI_CMD_9:
				break;

			case SPI_CMD_10:
			{
				// SEND_CID
				// Respond with R1 followed by the 128-bit Card Identifier as a data token.
				// A data token is made up of: start block (0xfe byte) + user data + crc16

				// Command response
				mCmdResponse[1] = 0x0; // OK status

				// CID data token response

				// Define its size and make sure the response vector can hold it
				int data_response_bytes = 128 + 3;
				mDataResponseBits = data_response_bytes * 8;
				if (mDataResponse.size() != data_response_bytes)
					mDataResponse.resize(data_response_bytes);

				// Add Start of block id
				mDataResponse[0] = 0xfe;

				// Add CID  (emulation only)
				for (int i = 0; i < data_response_bytes; i++)
					mDataResponse[i + 1] = 0x0;

				// Add CRC
				uint16_t crc = crc16(mDataResponse, 1, data_response_bytes - 3);
				mDataResponse[mBlockLen + 1] = crc >> 8;
				mDataResponse[mBlockLen + 2] = crc & 0xff;

				mSPITxMode = SPI_Tx_RSP_DATA_WAIT;

				break;
			}

			case SPI_CMD_12:
				break;

			case SPI_CMD_13:
				break;

			case SPI_CMD_16:
				// SET_BLOCKLEN
				mBlockLen = (request[1] << 24) | (request[2] << 16) | (request[3] << 8) | request[1];
				cout << "CMD16 : set block length to " << dec << mBlockLen << "\n";
				mCmdResponse[1] = 0x0; // OK status
				break;

			case SPI_CMD_17:
				{
				// READ_SINGLE_BLOCK
				// Respond with R1 followed by a read block as a data token.
				// A data token is made up of: start block (0xfe byte) + user data + crc16

				// Command response
				mCmdResponse[1] = 0x0; // OK status

				// Define its size and make sure the response vector can hold it
				int data_response_bytes = mBlockLen + 3;
				mDataResponseBits = data_response_bytes * 8;
				if (mDataResponse.size() != data_response_bytes)
					mDataResponse.resize(data_response_bytes);

				// Add Start of block id
				mDataResponse[0] = 0xfe;

				// Read block into response (emulation only)
				int block_adr = (request[1] << 24) | (request[2] << 16) | (request[3] << 8) | request[1];
				mCardImage->seekg(block_adr, mCardImage->beg);
				mCardImage->read((char*)  & mDataResponse[1], mBlockLen);

				// Add CRC
				uint16_t crc = crc16(mDataResponse, 1, data_response_bytes - 3);
				mDataResponse[mBlockLen + 1] = crc >> 8;
				mDataResponse[mBlockLen + 2] = crc & 0xff;

				cout << "CMD17: address = 0x" << hex << block_adr << "\n";

				mSPITxMode = SPI_Tx_RSP_DATA_WAIT;
				break;
				}

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
	/*
	for (int i = startPos; i < startPos + len; i++) {
		if ((i - startPos) % 16 == 0)
			cout << "\n";
		cout << hex << setfill('0') << setw(2) << (int)data[i] << " ";
	}
	cout << "\nCRC8 = 0x" << hex << crc << "\n";
	*/
	return crc;
}

//
// Compute the SPI CRC16 (CRC16-CCITT) with the polynom below:
//  x16 +x12 +x5 +1 
//
// The CRC is the remainder you get by dividing each
// data byte with the polynom above.
//
uint16_t SDCard::crc16(vector <uint8_t>& data, int startPos, int len)
{

	uint16_t polynom = 0x1021;
	uint16_t crc = 0xffff; // init value
	for (int i = startPos; i < startPos + len; i++) {
		uint8_t d = data[i];
		for (int i = 0; i < 8; i++) {
			if (((crc & 0x8000) >> 8) ^ (d & 0x80)) {
				crc = (crc << 1) ^ polynom;
			}
			else {
				crc = (crc << 1);
			}
			d <<= 1;
		}
	}
	/*
	for (int i = startPos; i < startPos + len; i++) {
		if ((i - startPos )% 16 == 0)
			cout << "\n";
		cout << hex << setfill('0') << setw(2) << (int)data[i] << " ";
	}
	cout << "\nCRC16 = 0x" << hex << crc << "\n";
	*/
	return crc;
}

string SDCard::bytes2str(vector <uint8_t> data)
{
	string s;
	for (int i = 0; i < data.size(); i++)
		s += Utility::int2hexStr(data[i],2);

	return s;
}