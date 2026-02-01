#include "SDCard.h"
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <filesystem>
#include "Utility.h"

namespace fs = std::filesystem;

SDCard::SDCard(string name,string cardImageFile, DebugTracing* debugTracing, ConnectionManager* connectionManager):
	Device(name, SD_CARD, OTHER_DEVICE, debugTracing, connectionManager)
{
	registerPort("CLK",		IN_PORT,	0x1,	CLK,	&mCLK);		// Clock
	registerPort("MOSI",	IN_PORT,	0x1,	MOSI,	&mMOSI);	// Data In
	registerPort("SEL",		IN_PORT,	0x1,	SEL,	&mSEL);		// Select
	registerPort("MISO",	OUT_PORT,	0x1,	MISO,	&mMISO);	// Data Out

	mSlaveDataToken.resize(512);

}

SDCard::~SDCard()
{
	if (mCardImage != NULL) {
		mCardImage->close();
		delete mCardImage;
	}
}

bool SDCard::cardInserted()
{
	return mCardImage != NULL;
}

bool SDCard::ejectCard()
{
	mResetCmdReceived = false;

	if (mCardImage != NULL) {
		if (mCardImage->is_open())
			mCardImage->close();
		delete mCardImage;
		mCardImage = NULL;
		return true;
	}
	return false;
}

bool SDCard::insertCard(string cardImageFile)
{
	if (mCardImage != NULL) {
		if (mCardImage->is_open())
			mCardImage->close();
		delete mCardImage;
	}

	mCardImage = new fstream(cardImageFile, ios::in | ios::out | ios::binary);

	if (mCardImage == NULL || !mCardImage->is_open()) {
		DBG_LOG(this, DBG_ERROR, "couldn't open SD Card Image " + cardImageFile + "\n");
		return false;
	}

	DBG_LOG(this, DBG_VERB_DEV, "SD Card Image '" + cardImageFile + " added...\n");

	// Get file size (should normally equal ROM size)
	mCardImage->seekg(0, ios::end);
	streamsize file_sz = mCardImage->tellg();
	mCardImage->seekg(0);

	return true;
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
// MMC & SDCards use SPI mode 0 in which data is sampled on rising edge and shifted out on the falling edge
// 
// To respond to a command, the SD card requires the CLK signal to toggle for at least 8 cycles while
// keeping MOSI high.
// 
// The SD card is placed into the SPI mode by setting the MOSI and SEL lines to '1' and toggle the CLK for at least 74 cycles.
// Then the SEL line shall be set to '0' and the reset command (CMD0) shall be sent.
// 
// A successful reception of a command will be replied with an 8-bit R1 respone of the format below:
// +---+---+---+---+---+---+---+---+
// | b7| b6| b5| b4| b3 |b2| b1| b0|		PE = Parameter Error, AE = Address Error, EE = Erase sequence Error
// +---+---+---+---+---+---+---+---+		CE = CRC Error, IC = Illegal Command, ER = Erase Reset
// | 1 | PE| AE| EE| CE| IC| ER| IS|		IS = In Idle State
// +---+---+---+---+---+---+---+---+ 
//
// To receive this message (data in MISO), the SD CLK signal is toggled while keeping the MOSI line high and the SEL line low.
// 
// There are five types of command responses (they all start with MSB '0' to ensure synchronisation)
//	R1		One byte
//	R1b		One byte identical to R1 but potentially followed by one or more zero bytes
//	R2		Two bytes (first byte = R1)
//	R3		Five bytes (first byte = R1)
//	R7		Six bytes (first byte = R1)
// 
// For some commands, a response could be followed by a data token. A data token is made up of:
//		start block (0xfe byte) + user data + crc16
// 
// There are 25 basic commmands: 
//	CMD0, CMD1, CMD6, CMD8, CMD9, CMD10, CMD12, CMD13, CMD16, CMD17, CMD18, CMD24, CMD25, CMD27, CMD28,
//	CMD29, CMD30, CMD32, CMD33, CMD38, CMD42, CMD55, CMD56, CMD58 and CMD59
// 
// If the basic command CMD55  has been received, then a application-specific command will follow.
// There are 6 application-specific command:
//	ACMD13, ACMD22, ACMD23, ACMD41, ACMD42 and ACMD51
//
void SDCard::processPortUpdate(int port)
{

	if (mCardImage == NULL)
		return;

	// Mode 0 assumed
	if (port == CLK) {

		if (mCLK != pCLK) {
			

			if (mCLK && !mInitialised) {

				if (mMOSI == 1) {
					mReceivedBits++;
				}
				else
					mReceivedBits = 0;
				if (mReceivedBits == 74) {
					mSPIRxMode = SPI_Rx_PREAMBLE_WAIT;
					mReceivedBits = 0;
					mInitialised = true;
					DBG_LOG(this, DBG_SPI, "SD Card: SPI Reset (74 x '1') completed  - Waiting for Reset command CMD0!\n");
				}
			}

			else if (mSEL == 0) {

				//
				// Rx processing - sample MOSI on positive CLK edge (as master outputs samples on the negative edge)
				//
				if (mCLK)
					processRxBits();

				//
				// Tx processing - shift out MISO on negative CLK edge (as master samples it on positive edge)
				//
				if (!mCLK)
					generateTxBits();
			}

		}

		pCLK = mCLK;
		pMOSI = mMOSI;
	}
}

//
// Process bit stream from the master
//
bool SDCard::processRxBits()
{
	if (mSPIRxMode != pSPIRxMode) {
		//DBG_LOG(this, DBG_SPI, "SPI Rx Mode "s + _SPI_RX_MODE(pSPIRxMode) + " => " + _SPI_RX_MODE(mSPIRxMode) + "\n");
	}

	pSPIRxMode = mSPIRxMode;
	
	switch (mSPIRxMode) {
	case SPI_Rx_IDLE:
	{
		mCurrentCmd = SPI_CMD_ILLEGAL;
		mCurrenAppCmd = SPI_A_CMD_ILLEGAL;
		if (mMOSI == 1) {
			mSPIRxMode = SPI_Rx_PREAMBLE_WAIT;
		}
		break;
	}

	case SPI_Rx_PREAMBLE_WAIT:
	{
		if (mMOSI == 0)
			mReceivedBits++;
		else { // mMOSI == 1
			if (mReceivedBits == 1) {
				mSPIRxMode = SPI_Rx_CMD_WAIT; // Preamble '01' detected
				mRxShiftRegister = 0b01; // update the command shift register with the first received '01' bits shifted left one step
				mReceivedBits = 2;
				mReceivedBytes = 0;
			}
			else {
				mReceivedBits = 0;// restart search for '01'
				mReceivedBytes = 0;
			}
		}
		break;
	}

	case SPI_Rx_CMD_WAIT:
	{
		mReceivedBits++;
		mRxShiftRegister = ((mRxShiftRegister << 1) & 0xfe) | mMOSI;
		if (mReceivedBits % 8 == 0) {
			mMasterRequest[mReceivedBytes++] = mRxShiftRegister;
		}
		if (mReceivedBits == 48) {
			if (!execCmd(mMasterRequest)) {
				// Invalid command (no action - just ignore the received bit sequence and start all over again)
				DBG_LOG(this, DBG_ERROR, "Illegal command bit sequence  '" + bytes2str(mMasterRequest) + "'!\n");
				recover();
				return false;
			}
		}
		break;
	}

	case SPI_Rx_DATA_WAIT:
	{
		mReceivedBits++;
		mRxShiftRegister = ((mRxShiftRegister << 1) & 0xfe) | mMOSI;
		if (!mDataStartTokenReceived && mRxShiftRegister == 0xfe) {
			mDataStartTokenReceived = true;
			mReceivedBits = 8;

		}
		if (mDataStartTokenReceived && mReceivedBits % 8 == 0) {
			mMasterDataToken[mReceivedBytes++] = mRxShiftRegister;
		}
		if (mReceivedBits == mMasterDataTokenBits) {
			mSPIRxMode = SPI_Rx_IDLE;
			mDataStartTokenReceived = false;
			if (!processMasterData()) {
				// Invalid command (no action - just ignore the received bit sequence and start all over again)
				DBG_LOG(this, DBG_ERROR, "Illegal data bit sequence  '" + bytes2str(mMasterDataToken) + "'!\n");
				recover();
				return false;
			}
		}
		break;
	}

	default:
	{
		// ERROR - unknown SPI Rx Mode!
		DBG_LOG(this, DBG_ERROR, "Unknown SPI Rx Mode " + to_string(mSPIRxMode) + "!\n");
		// start all over again as an attempt to recover...
		recover();
		return false;
	}
	}

	return true;
}

//
// Generate bit stream to the master
//
bool SDCard::generateTxBits()
{
	if (mSPITxMode != pSPITxMode) {
		//DBG_LOG(this, DBG_SPI, "SPI Tx Mode "s + _SPI_TX_MODE(pSPITxMode) + " => " + _SPI_TX_MODE(mSPITxMode) + "\n");
	}

	pSPITxMode = mSPITxMode;
	
	switch (mSPITxMode) {
	case SPI_Tx_IDLE:
	{
		// Nothing to respond to => do nothing
		mResponseType = SPI_RSP_ILLEGAL;
		break;
	}
	case SPI_Tx_RSP_WAIT:
	case SPI_Tx_RSP_DATA_WAIT:
	case SPI_Tx_DATA_WAIT:
	{
		int response_bits = (mSPITxMode != SPI_Tx_DATA_WAIT ? mSlaveResponseBits : mSlaveDataTokenBits);
		if (mSentBits % 8 == 0) {
			if (mSPITxMode != SPI_Tx_DATA_WAIT) {
				mTxShiftRegister = mSlaveResponse[mSentBytes++];
			}
			else {
				//DBG_LOG(this, DBG_SPI, "Shift out of byte #" + Utility::int2HexStr(mSentBytes,4) + " = 0x" + Utility::int2HexStr(mSlaveDataToken[mSentBytes], 2) + "\n");
				mTxShiftRegister = mSlaveDataToken[mSentBytes++];
			}
		}
		else
			mTxShiftRegister <<= 1;
		updatePort(MISO, (mTxShiftRegister >> 7) & 0x1);
		mSentBits++;

		if (mSentBits == response_bits) {
#ifdef DBG_ON
			if (DBG_LEVEL_DEV(this, DBG_SPI)) {
				if (mSPITxMode != SPI_Tx_DATA_WAIT)
					DBG_LOG(this, DBG_SPI, "Response of type R" + to_string(mResponseType) + " (response 0x" + bytes2str(mSlaveResponse) +
						") [" + to_string(response_bits) + " bits] sent\n");
				else // mSPITxMode == SPI_Tx_DATA_WAIT
					DBG_LOG(this, DBG_SPI, "Data token '" + bytes2str(mSlaveDataToken) + "' [" + to_string(response_bits) + " bits] sent\n");
			}
#endif
			mSentBits = 0;
			mSentBytes = 0;
			if (mSPITxMode == SPI_Tx_RSP_WAIT || mSPITxMode == SPI_Tx_DATA_WAIT) {
				mSPITxMode = SPI_Tx_IDLE;
			}
			else if (mSPITxMode == SPI_Tx_RSP_DATA_WAIT) {
				DBG_LOG(this, DBG_SPI, "Response sent - will now be followed by a data token '" + bytes2str(mSlaveDataToken) + "' of length " + to_string(mSlaveDataTokenBits) + " bits\n");
				mSPITxMode = SPI_Tx_DATA_WAIT;
			}
			else {
				// ERROR - unknown SPI Tx Mode
				DBG_LOG(this, DBG_ERROR, "Unknown SPI Tx Mode " + to_string(mSPITxMode) + "!\n");
				// start all over again as an attempt to recover...
				recover();
				return false;
			}
		}
		break;
	}
	default:
	{
		// ERROR - unknown SPI Tx Mode!
		DBG_LOG(this, DBG_ERROR, "Unknown SPI Tx Mode " + to_string(mSPITxMode) + "!\n");
		// start all over again as an attempt to recover...
		recover();
		return false;
	}
	}

	return true;
}

void SDCard::recover()
{
	// start all over again as an attempt to recover...
	mSPITxMode = SPI_Tx_IDLE;
	mSPIRxMode = SPI_Rx_IDLE;
	mReceivedBits = 0;
	mReceivedBytes = 0;
	mFirstBit = true;
	mDataStartTokenReceived = false;
	mMasterDataTokenBits = 0;
}

void SDCard::prepareNextCmd()
{
	mSPIRxMode = SPI_Rx_IDLE;
	mReceivedBits = 0;
	mReceivedBytes = 0;
	mFirstBit = true;

}

void SDCard::initResponse(vector <uint8_t>& request)
{
	uint8_t cmd = request[0] & 0x3f;

	SPICmdInfo cmd_info = spiCmdInfo[SPICmdEnum(cmd)];
	mResponseType = cmd_info.respType;
	SPIRspInfo rsp_info = spiRspInfo[mResponseType];
	uint8_t req_crc_byte = (crc7(request, 0, 5) << 1) | 0x1;
	mSlaveResponseBits = rsp_info.nBytes * 8;
	mSlaveResponse = rsp_info.okResponse;
	mSPITxMode = SPI_Tx_RSP_WAIT;
	mReceivedBits = 0;
	mReceivedBytes = 0;

#ifdef DBG_ON
	if (DBG_LEVEL_DEV(this,DBG_SPI)) {
		DBG_LOG(this, DBG_SPI, "Command CMD" + to_string(cmd) + " (request 0x" + bytes2str(request) + ") " + cmd_info.mnemonic + " with expected CRC byte 0x" +
			Utility::int2HexStr(req_crc_byte,2) +
			" received - will send a response of type R" + to_string(mResponseType) + " (response 0x" + bytes2str(mSlaveResponse) + ")" +
			" with " + to_string(mSlaveResponseBits) + " bits (" + to_string(rsp_info.nBytes) + " bytes)...\n");
	}
#endif
}

void SDCard::prepareFailedReponse()
{
	mSlaveResponseBits = 3 * 8;
	mSlaveResponse = NegR1Rsp;
	mSPITxMode = SPI_Tx_RSP_WAIT;
	mReceivedBits = 0;
	mReceivedBytes = 0;
}

bool SDCard::execCmd(vector <uint8_t> &request)
{

	bool valid_cmd = validCmd(request[0]);

	if (!valid_cmd) {
		DBG_LOG(this, DBG_ERROR, "invalid command CMD" + to_string(request[0]) + " (request 0x" + bytes2str(request) + ") received !\n");
		return false;
	}
		
	if (!mPendingAppCmd)
		return execBaseCmd(request);
	else
		return execAppCmd(request);			

}

bool SDCard::execBaseCmd(vector <uint8_t>& request)
{
	uint8_t cmd = request[0] & 0x3f;

	mCurrentCmd = cmd;

	// Setup response and prepare for reception of a new command
	initResponse(request);
	prepareNextCmd();

	switch (cmd) {

		case SPI_CMD_0:
			// GO_IDLE_STATE
			// Resets the SD Memory Card.
			// Response is R1.
			// 
			// Required by MMFS for the BBC Micro
		{
			mSlaveResponse[1] = 0x1; // set idle status bit
			mResetCmdReceived = true;
			break;
		}

		case SPI_CMD_1:
			// SEND_OP_COND
			// Sends host capacity support information and activates the card's initialization process.
			// Response is R1.
			// 
			// Required by MMFS for the BBC Micro
		{
			mSlaveResponse[1] = 0x0; // clear idle status bit
			break;
		}

		case SPI_CMD_6:
			// SWITCH_FUNC
			// Checks switchable function (mode 0) and switches card function (mode 1).
			// Response is R1.
		{
			// NOT YET IMPLEMENTED!!!
			prepareFailedReponse();
			DBG_LOG(this, DBG_SPI, "Non-supported SPI command CMD6 (SWITCH_FUNC)!\n");
			break;
		}

		case SPI_CMD_8:
			// SEND_IF_COND
			// Sends SD Memory Card interface condition.
			// Response is R8.
			//
			// Required by MMFS for the BBC Micro
		{
			uint8_t supply_voltage = request[3] & 0xf;
			uint8_t check_pattern = request[4];
			mSlaveResponse[4] |= supply_voltage;
			mSlaveResponse[5] = check_pattern;
			mSlaveResponse[6] = (crc7(mSlaveResponse, 1, 5) << 1) | 0x1;
			break;
		}

		case SPI_CMD_9:
			// SEND_CSD
			// Asks the selected card to send its card-specific data (CSD).
			// Response is R1.
		{
			prepareFailedReponse();
			DBG_LOG(this, DBG_SPI, "Non-supported SPI command CMD9 (SEND_CSD)!\n");
			break;
		}

		case SPI_CMD_10:
			// SEND_CID
			// Asks the selected card to send its card identification (CID).
			// Response is R1 followded by a data token response.
			// The data token is an 128-bit Card Identifier.
			//
			// The complete sequence related to this command is:
			//	1.	Master sends SEND_CID command
			//	2.	Card responds with R1
			//	3.	Card sends data token with CID
			// 
			// Required by MMFS for the BBC Micro
		{
			// Command response
			mSlaveResponse[1] = 0x0; // OK status

			// CID data token response

			// Define its size and make sure the response vector can hold it
			int content_bytes = 128 / 8;
			int data_response_bytes = content_bytes + 4;
			mSlaveDataTokenBits = data_response_bytes * 8;
			if (mSlaveDataToken.size() != data_response_bytes)
				mSlaveDataToken.resize(data_response_bytes);

			// Add Start of block id
			mSlaveDataToken[0] = 0xfe;							// start of data token block
			mSlaveDataToken[data_response_bytes - 1] = 0xff;	// always end with a high level

			// Add CID  (emulation only)
			for (int i = 1; i < data_response_bytes - 4; i++)
				mSlaveDataToken[i] = 0x0;
			mSlaveDataToken[content_bytes] = 0x1;			// b0 of the 128 CID bits is a stop bit and shall always be '1'

			// Add CRC
			uint16_t crc = crc16(mSlaveDataToken, 1, data_response_bytes - 4);
			mSlaveDataToken[content_bytes + 1] = crc >> 8;
			mSlaveDataToken[content_bytes + 2] = crc & 0xff;

			mSPITxMode = SPI_Tx_RSP_DATA_WAIT;

			break;
		}

		case SPI_CMD_12:
			// STOP_TRANSMISSION
			// Forces the card to stop transmission in Multiple Block Read Operation.
			// Response is R1b.
		{
			prepareFailedReponse();
			DBG_LOG(this, DBG_SPI, "Non-supported SPI command CMD12 (STOP_TRANSMISSION)!\n");
			break;
		}

		case SPI_CMD_13:
			// SEND_STATUS
			// Asks the selected card to send its status register.
			// Response is R2.
		{
			prepareFailedReponse();
			DBG_LOG(this, DBG_SPI, "Non-supported SPI command CMD12 (SEND_STATUS)!\n");
			break;
		}

		case SPI_CMD_16:
			// SET_BLOCKLEN
			// Sets a block length (in bytes) for all following block commands.
			// 
			// Required by MMFS for the BBC Micro
		{
			
			mBlockLen = (request[1] << 24) | (request[2] << 16) | (request[3] << 8) | request[1];
			DBG_LOG(this, DBG_SPI, "CMD16: set block length to " + to_string(mBlockLen) + "\n");
			mSlaveResponse[1] = 0x0; // OK status
			break;
		}

		case SPI_CMD_17:
		{
			// READ_SINGLE_BLOCK
			// Respond with R1 followed by a read block as a data token.
			// 
			// The complete sequence related to this command is:
			//	1.	Master sends READ_SINGLE_BLOCK command
			//	2.	Card responds with R1
			//	3.	Card sends data token with block data
			//
			// Required by MMFS for the BBC Micro


			// Command response
			mSlaveResponse[1] = 0x0; // OK status

			// Define its size and make sure the response vector can hold it
			int content_bytes = mBlockLen;
			int data_response_bytes = content_bytes + 4;
			mSlaveDataTokenBits = data_response_bytes * 8;
			if (mSlaveDataToken.size() != data_response_bytes)
				mSlaveDataToken.resize(data_response_bytes);

			// Add Start of block id
			mSlaveDataToken[0] = 0xfe;						// start of data token block
			mSlaveDataToken[data_response_bytes-1] = 0xff;	// always end with a high level

			// Read block into response (emulation only)
			mBlockReadAdr = (request[1] << 24) | (request[2] << 16) | (request[3] << 8) | request[4];
			mCardImage->seekg(mBlockReadAdr);
			mCardImage->read((char*)&mSlaveDataToken[1], content_bytes);

			// Add CRC
			uint16_t crc = crc16(mSlaveDataToken, 1, data_response_bytes - 4);
			mSlaveDataToken[content_bytes + 1] = crc >> 8;
			mSlaveDataToken[content_bytes + 2] = crc & 0xff;

			DBG_LOG(this, DBG_SPI, "CMD17: address = 0x" + Utility::int2HexStr(mBlockReadAdr, 6) + "\n");

			mSPITxMode = SPI_Tx_RSP_DATA_WAIT;
			break;
		}

		case SPI_CMD_18:
			prepareFailedReponse();
			DBG_LOG(this, DBG_SPI, "Non-supported SPI command CMD18 (READ_MULTIPLE_BLOCK)!\n");
			break;

		case SPI_CMD_24:
			// WRITE_BLOCK
			// Writes a block of the size selected by the SET_BLOCKLEN command.
			// 
			// The complete sequence related to this command is:
			//	1.	Master send WRITE_BLOCK command
			//	2.	Card responds with R1
			//	3.	Master sends data token containing block data
			//	4.	Card responds with status data token xxx0sss1 where
			//		sss = status (2: data accepted, 5: CRC error, 6: write error)
			//	5.	Card sends zeros until the write operation has finished
			//	6.	Card sends 0xff
			// 
			// 
			// Required by MMFS for the BBC Micro
		{

			// Command response
			mSlaveResponse[1] = 0x0; // OK status

			// Get block write address
			mBlockWriteAdr = (request[1] << 24) | (request[2] << 16) | (request[3] << 8) | request[1];
			DBG_LOG(this, DBG_SPI, "CMD24: address = 0x" + Utility::int2HexStr(mBlockWriteAdr, 6) + "\n");

			// Prepare block data buffer for reception of block to write
			int content_bytes = mBlockLen;
			int data_token_bytes = content_bytes + 3;
			mMasterDataTokenBits = data_token_bytes * 8;
			if (mMasterDataToken.size() != data_token_bytes)
				mMasterDataToken.resize(data_token_bytes);

			// Initiate wait for block data to write
			mSPIRxMode = SPI_Rx_DATA_WAIT;
			mReceivedBits = 0;
			mReceivedBytes = 0;

			break;
		}

		case SPI_CMD_25:
			prepareFailedReponse();
			DBG_LOG(this, DBG_SPI, "Non-supported SPI command CMD25 (WRITE_MULTIPLE_BLOCK)!\n");
			break;

		case SPI_CMD_27:
			prepareFailedReponse();
			DBG_LOG(this, DBG_SPI, "Non-supported SPI command CMD27 (PROGRAM_CSD)!\n");
			break;

		case SPI_CMD_28:
			prepareFailedReponse();
			DBG_LOG(this, DBG_SPI, "Non-supported SPI command CMD28 (SET_WRITE_PROT)!\n");
			break;

		case SPI_CMD_29:
			prepareFailedReponse();
			DBG_LOG(this, DBG_SPI, "Non-supported SPI command CMD29 (CLR_WRITE_PROT)!\n");
			break;

		case SPI_CMD_30:
			prepareFailedReponse();
			DBG_LOG(this, DBG_SPI, "Non-supported SPI command CMD30 (SEND_WRITE_PROT)!\n");
			break;

		case SPI_CMD_32:
			prepareFailedReponse();
			DBG_LOG(this, DBG_SPI, "Non-supported SPI command CMD32 (ERASE_WR_BLK_START_ADDR)!\n");
			break;

		case SPI_CMD_33:
			prepareFailedReponse();
			DBG_LOG(this, DBG_SPI, "Non-supported SPI command CMD33 (ERASE_WR_BLK_END_ADDR)!\n");
			break;

		case SPI_CMD_38:
			prepareFailedReponse();
			DBG_LOG(this, DBG_SPI, "Non-supported SPI command CMD38 (LOCK_UNLOCK)!\n");
			break;

		case SPI_CMD_42:
			prepareFailedReponse();
			DBG_LOG(this, DBG_SPI, "Non-supported SPI command CMD42 (SWITCH_FUNC)!\n");
			break;

		case SPI_CMD_55:
			// APP_CMD
			// Defines to the card that the next command is an application specific command rather than a standard command.
			//
			// Required by MMFS for the BBC Micro
		{
			prepareFailedReponse();
			DBG_LOG(this, DBG_SPI, "Non-supported SPI command CMD55 (APP_CMD)!\n");
			break;
		}

		case SPI_CMD_56:
			break;

		case SPI_CMD_58:
			// READ_OCR
			// Reads the OCR register of a card.
			// 
			// Required by MMFS for the BBC Micro
		{
			prepareFailedReponse();
			DBG_LOG(this, DBG_SPI, "Non-supported SPI command CMD58 (READ_OCR)!\n");
			break;
		}

		case SPI_CMD_59:
			prepareFailedReponse();
			DBG_LOG(this, DBG_SPI, "Non-supported SPI command CMD59 (CRC_ON_OFF)!\n");
			break;

		default:
			prepareFailedReponse();
			DBG_LOG(this, DBG_SPI, "Illegal SPI Basic command " + to_string(cmd) + "\n");
			return false;
			break;

		}
		
	return true;
}

bool SDCard::execAppCmd(vector <uint8_t> &request)
{
	uint8_t cmd = request[0] & 0x3f;

	switch (cmd) {
		case SPI_A_CMD_13:
			// SD_STATUS -					R2	Send the cards status register.
		{
			// NOT YET IMPLEMENTED!!!
			prepareFailedReponse();
			DBG_LOG(this, DBG_SPI, "Non-supported SPI command ACMD13 (SD_STATUS)!\n");

			break;
		}
		case SPI_A_CMD_22:
			// SEND_NUM_WR_BLOCKS -			R1	Send the numbers of the well written (without errors) blocks
		{
			// NOT YET IMPLEMENTED!!!
			prepareFailedReponse();
			DBG_LOG(this, DBG_SPI, "Non-supported SPI command ACMD22 (SEND_NUM_WR_BLOCKS)!\n");
			break;
		}
		case SPI_A_CMD_23:
			// SET_WR_BLK_ERASE_COUNT -		R1	Set the number of write blocks to be pre-erased before writing.
		{
			// NOT YET IMPLEMENTED!!!
			prepareFailedReponse();
			DBG_LOG(this, DBG_SPI, "Non-supported SPI command ACMD23 (SET_WR_BLK_ERASE_COUNT)!\n");
			break;
		}
		case SPI_A_CMD_41: // Required by MMFS for the BBC Micr
			// SD_SEND_OP_COND
			// Response is R1.
			// Sends host capacity support information and activates the card's initialization process.
			//
			// Required by MMFS for the BBC Micro
		{
			// NOT YET IMPLEMENTED!!!
			prepareFailedReponse();
			DBG_LOG(this, DBG_SPI, "Non-supported SPI command ACMD41 (SD_SEND_OP_COND)!\n");
			break;
		}
		case SPI_A_CMD_42:
			// SET_CLR_CARD_DETECT -		R1	Connect ('1')/Disconnect ('0') the 50 KOhm pull-up resistor on CS (pin 1) of the card.
		{
			// NOT YET IMPLEMENTED!!!
			prepareFailedReponse();
			DBG_LOG(this, DBG_SPI, "Non-supported SPI command ACMD42 (SET_CLR_CARD_DETECT)!\n");
			break;
		}
		case SPI_A_CMD_51:
			// SEND_SCR -					R1	Reads the SD Configuration Register (SCR).
		{
			// NOT YET IMPLEMENTED!!!
			prepareFailedReponse();
			DBG_LOG(this, DBG_SPI, "Non-supported SPI command ACMD51 (SEND_SCR)!\n");
			break;
		}
		default:
		{
			// ERROR !!!
			prepareFailedReponse();
			DBG_LOG(this, DBG_SPI, "Illegal SPI Application command " + to_string(cmd) + "\n");
			break;
		}
		}

	return true;
}

bool SDCard::validCmd(uint8_t cmdByte)
{
	
	bool valid_cmd = (cmdByte & 0xc0) == 0x40;

	uint8_t cmd = cmdByte & 0x3f;

	if (!mResetCmdReceived && cmd != SPI_CMD_0)
		return false;

	if (!mPendingAppCmd) {
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
	}
	else {
		switch (cmd) {
		case SPI_A_CMD_13:
		case SPI_A_CMD_22:
		case SPI_A_CMD_23:
		case SPI_A_CMD_41:
		case SPI_A_CMD_42:
		case SPI_A_CMD_51:
			break;
		default:
			valid_cmd = false;
			break;
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

	return crc;
}

string SDCard::bytes2str(vector <uint8_t> data)
{
	string s;
	for (int i = 0; i < data.size(); i++)
		s += Utility::int2HexStr(data[i],2);

	return s;
}

bool SDCard::writeBlockData(int adr, vector <uint8_t>& data, int start, int len)
{
	if (mCardImage == NULL) {
		DBG_LOG(this, DBG_ERROR, "ERROR - no MMC card inserted when attempting a write operation!\n");
		return false;
	}
	
	mCardImage->seekg(adr);
	mCardImage->write((char*)&mMasterDataToken[start], len);

	return true;
}


bool SDCard::processMasterData() {

	switch (mCurrentCmd) {

	case SPI_CMD_12:
		// STOP_TRANSMISSION
		// Forces the card to stop transmission in Multiple Block Read Operation.
		// Response is R1b.
	{
		break;
	}

	case SPI_CMD_24:
		// WRITE_BLOCK
		// Writes a block of the size selected by the SET_BLOCKLEN command.
		// 
		// The complete sequence related to this command is:
		//	1.	Master send WRITE_BLOCK command
		//	2.	Card responds with R1
		//	3.	Master sends data token containing block data
		//	=>	4.	Card responds with status data token xxx0sss1 where
		//		sss = status (2: data accepted, 5: CRC error, 6: write error)
		//		5.	Card sends zeros until the write operation has finished
		//		6.	Card sends 0xff
		// 
		// 
		// Required by MMFS for the BBC Micro
	{

		// Make sure the max size (11) data response token fits in slave token response
		int data_response_bytes = 11;
		mSlaveDataTokenBits = data_response_bytes * 8;
		if (mSlaveDataToken.size() < data_response_bytes)
			mSlaveDataToken.resize(data_response_bytes);

		if (!writeBlockData(mBlockWriteAdr, mMasterDataToken, 1, mBlockLen)) {
			// Write failure data response token
			data_response_bytes = 2;
			mSlaveDataTokenBits = data_response_bytes * 8;
			mSlaveDataToken[0] = (0x6 << 1) | 0x1;				// write errors
			mSlaveDataToken[1] = 0xff;							// end with high bits
		}
		else {
			// Success Data response token
			data_response_bytes = 11;
			mSlaveDataTokenBits = data_response_bytes * 8;
			mSlaveDataToken[0] = (0x2 << 1) | 0x1;					// data accepted, neither CRC nor write errors
			for (int i = 1; i < 10; mSlaveDataToken[i++] = 0x0);	// send 9 'busy' bytes
			mSlaveDataToken[10] = 0xff;								// end with high bits (indicating a completed write operation)

		}


		mSPITxMode = SPI_Tx_DATA_WAIT;
		mSPIRxMode = SPI_Rx_IDLE;
		mReceivedBits = 0;
		mReceivedBytes = 0;
		mFirstBit = true;

		break;
	}

	default:
		return false;
		break;

	}

	return true;
}

// Outputs the internal state of the device
bool SDCard::outputState(ostream& sout)
{
	SPICmdEnum cmd = SPICmdEnum(mCurrentCmd);
	if (spiCmdInfo.find(cmd) == spiCmdInfo.end())
		cmd = SPI_CMD_ILLEGAL;

	SPIACmdEnum app_cmd = SPIACmdEnum(mCurrentAppCmd);
	if (spiAppCmdInfo.find(app_cmd) == spiAppCmdInfo.end())
		app_cmd = SPI_A_CMD_ILLEGAL;

	SPICmdInfo cmd_info = spiCmdInfo[cmd];
	SPICmdInfo app_cmd_info = spiAppCmdInfo[app_cmd];

	if (cmd != SPI_CMD_ILLEGAL) {
		sout << "Current base command   = " << cmd_info.mnemonic << ": " << cmd_info.desc << "\n";
		sout << "Current request        = " << bytes2str(mMasterRequest) << "\n";
		if (mMasterDataTokenBits != 0)
			sout << "Current master data    = " << bytes2str(mMasterDataToken) << "\n";
		if (mSlaveDataTokenBits != 0)
			sout << "Current slave data     = " << bytes2str(mSlaveDataToken) << "\n";
	}
	if (mResponseType != SPI_RSP_ILLEGAL) {
		sout << "Response type          = " << "R" << mResponseType << "\n";
		sout << "Current response       = " << bytes2str(mSlaveResponse) << "\n";
	}
	if (app_cmd != SPI_A_CMD_ILLEGAL) {
		sout << "Current app command    = " << app_cmd_info.mnemonic << ": " << app_cmd_info.desc << "\n";
	}
	sout << "Block length           = " << dec << mBlockLen << "\n";
	sout << "Read address           = 0x" << Utility::int2HexStr(mBlockReadAdr, 8) << "\n";
	sout << "Write address          = 0x" << Utility::int2HexStr(mBlockWriteAdr, 8) << "\n";
	sout << "Rx Mode                = " << _SPI_RX_MODE(mSPIRxMode) << "\n";
	sout << "Tx Mode                = " << _SPI_TX_MODE(mSPITxMode) << "\n";
	sout << "Slave data token bits  = " << mSlaveDataTokenBits << " (" << mSlaveDataTokenBits / 8 << " bytes)\n";
	sout << "Master data token bits = " << mMasterDataTokenBits << " (" << mMasterDataTokenBits / 8 << " bytes)\n";
	sout << "Received bits          = " << mReceivedBits << " (" << mReceivedBytes << " bytes)\n";
	sout << "Sent bits              = " << mSentBits << " (" << mSentBytes << " bytes)\n";
	sout << "Rx shift register      = 0x" << Utility::int2HexStr(mRxShiftRegister, 2) << " (0b" << Utility::int2BinStr(mRxShiftRegister, 8) << ")\n";
	sout << "Tx shift register      = 0x" << Utility::int2HexStr(mTxShiftRegister, 2) << " (0b" << Utility::int2BinStr(mTxShiftRegister, 8) << ")\n";

	return true;
}