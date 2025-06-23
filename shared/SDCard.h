#ifndef SD_CARD_H
#define SD_CARD_H

#include <cstdint>
#include "Device.h"
#include <vector>


using namespace std;

class SDCard : public Device {
	// Standard SD Card - SPI interface#// Standard SD Card - SPI interface

private:

	int	CLK, MISO, MOSI, SEL;

	uint8_t mSEL = 0x0;		// Select#// Select
	uint8_t mCLK = 0x0;		// Clock#// Clock
	uint8_t mMISO = 0x1;	// Data Out#// Data Out
	uint8_t mMOSI = 0x1;	// Data In#// Data In
	uint8_t pMOSI = 0x1;

	uint8_t pCLK = 0x0;

	// Data sent by the slace
	vector <uint8_t> mSlaveResponse;
	vector <uint8_t> mSlaveDataToken;
	int mSlaveResponseBits = 8;
	int mSlaveDataTokenBits = 0;

	// Data sent by the master
	vector <uint8_t> mMasterRequest = { 0x00, 0x00, 0x00 , 0x00 , 0x00 , 0x00 };
	vector <uint8_t> mMasterDataToken;
	int mMasterDataTokenBits = 0;

	int mReceivedBits = 0;
	int mReceivedBytes = 0;
	int mSentBits = 0;
	int mSentBytes = 0;
	bool mFirstBit = true;

	bool mInitialised = false;
	bool mResetCmdReceived = false;
	bool mDataStartTokenReceived = false;

	

	// SD Card parameters
	uint8_t mCIDRegister[16] = {
		0x0
	};

	// SD Card memory parameters
	uint32_t mBlockLen = 0;
	int mBlockWriteAdr = 0x0;
	int mBlockReadAdr = 0x0;

	enum SPITxMode {SPI_Tx_RSP_WAIT, SPI_Tx_RSP_DATA_WAIT, SPI_Tx_DATA_WAIT, SPI_Tx_BLOCK_WRITE_WAIT, SPI_Tx_IDLE};
#define _SPI_TX_MODE(x) (x==SPI_Tx_RSP_WAIT?"SPI_Tx_RSP_WAIT":(x==SPI_Tx_RSP_DATA_WAIT?"SPI_Tx_RSP_DATA_WAIT":(x==SPI_Tx_DATA_WAIT?"SPI_Tx_DATA_WAIT":(x==SPI_Tx_BLOCK_WRITE_WAIT?"SPI_Tx_BLOCK_WRITE_WAIT":(x==SPI_Tx_IDLE?"SPI_Tx_IDLE":"???")))))
	enum SPIRxMode {SPI_Rx_IDLE, SPI_Rx_PREAMBLE_WAIT, SPI_Rx_CMD_WAIT, SPI_Tx_CMD_DATA_WAIT, SPI_Rx_DATA_WAIT};
#define _SPI_RX_MODE(x) (x==SPI_Rx_IDLE?"SPI_Rx_IDLE":(x==SPI_Rx_PREAMBLE_WAIT?"SPI_Rx_PREAMBLE_WAIT":(x==SPI_Rx_CMD_WAIT?"SPI_Rx_CMD_WAIT":(x==SPI_Tx_CMD_DATA_WAIT?"SPI_Tx_CMD_DATA_WAIT":(x==SPI_Rx_DATA_WAIT?"SPI_Rx_DATA_WAIT":"???")))))
	SPIRxMode mSPIRxMode = SPI_Rx_IDLE;
	SPITxMode mSPITxMode = SPI_Tx_IDLE;

	SPIRxMode pSPIRxMode = SPI_Rx_IDLE;
	SPITxMode pSPITxMode = SPI_Tx_IDLE;


	enum SPICmdEnum {
							
		SPI_CMD_0 = 0,		
		SPI_CMD_1 = 1,		
		SPI_CMD_6 = 6,		
		SPI_CMD_8 = 8,		
		SPI_CMD_9 = 9,		
		SPI_CMD_10 = 10,	
		SPI_CMD_12 = 12,	
		SPI_CMD_13 = 13,	
		SPI_CMD_16 = 16,	
		SPI_CMD_17 = 17,	
		SPI_CMD_18 = 18,	
		SPI_CMD_24 = 24,	
		SPI_CMD_25 = 25,	
		SPI_CMD_27 = 27,	
		SPI_CMD_28 = 28,	
		SPI_CMD_29 = 29,	
		SPI_CMD_30 = 30,	
		SPI_CMD_32 = 32,	
		SPI_CMD_33 = 33,	
		SPI_CMD_38 = 38,	
		SPI_CMD_42 = 42,	
		SPI_CMD_55 = 55,	
		SPI_CMD_56 = 56,	
		SPI_CMD_58 = 58,	
		SPI_CMD_59 = 59,		
		SPI_CMD_ILLEGAL = 255
	};

	uint8_t mCurrentCmd = SPI_CMD_ILLEGAL;

	enum SPIACmdEnum {
		SPI_A_CMD_13 = 13,	// SD_STATUS -					R2	Send the cards status register.
		SPI_A_CMD_22 = 22,	// SEND_NUM_WR_BLOCKS -			R1	Send the numbers of the well written (without errors) blocks
		SPI_A_CMD_23 = 23,	// SET_WR_BLK_ERASE_COUNT -		R1	Set the number of write blocks to be pre-erased before writing.
		SPI_A_CMD_41 = 41,	// SD_SEND_OP_COND -			R1	Sends host capacity support information and activates the card's initialization process.
		SPI_A_CMD_42 = 42,	// SET_CLR_CARD_DETECT -		R1	Connect ('1')/Disconnect ('0') the 50 KOhm pull-up resistor on CS (pin 1) of the card.
		SPI_A_CMD_51 = 51,	// SEND_SCR -					R1	Reads the SD Configuration Register (SCR).
		SPI_A_CMD_ILLEGAL = 255
	};

	uint8_t mCurrentAppCmd = SPI_A_CMD_ILLEGAL;

	enum SPIRspEnum {
		SPI_RSP_R1 = 1,
		SPI_RSP_R1b = 11,
		SPI_RSP_R2 = 2,
		SPI_RSP_R3 = 3,
		SPI_RSP_R7 = 7
	};

	typedef struct SPICmdInfo_struct {
		vector<uint8_t>		request;
		SPIRspEnum			respType;
		string				mnemonic;
		string				desc;
	} SPICmdInfo;


	map<SPICmdEnum, SPICmdInfo> spiCmdInfo = {
		{SPI_CMD_0,		{{0x40,0x00,0x00,0x00,0x00,0x95},	SPI_RSP_R1, "GO_IDLE_STATE",			"Resets the SD Memory Card"}},
		{SPI_CMD_1,		{{0x41,0x00,0x00,0x00,0x00,0xff},	SPI_RSP_R1, "SEND_OP_COND",				"Sends host capacity support information and activates the card's initialization process."}},
		{SPI_CMD_6,		{{0x46,0x00,0x00,0x00,0x00,0x00},	SPI_RSP_R1, "SWITCH_FUNC",				"Checks switchable function (mode 0) and switches card function (mode 1)"}},
		{SPI_CMD_8,		{{0x48,0x00,0x00,0x01,0xaa,0x87},	SPI_RSP_R7, "SEND_IF_COND",				"Sends SD Memory Card interface condition"}},
		{SPI_CMD_9,		{{0x49,0x00,0x00,0x00,0x00,0x00},	SPI_RSP_R1, "SEND_CSD",					"Asks the selected card to send its card-specific data (CSD)"}},
		{SPI_CMD_10,	{{0x4a,0x00,0x00,0x00,0x00,0x00},	SPI_RSP_R1, "SEND_CID",					"Asks the selected card to send its card identification (CID)"}},
		{SPI_CMD_12,	{{0x4c,0x00,0x00,0x00,0x00,0x00},	SPI_RSP_R1b, "STOP_TRANSMISSION",		"Forces the card to stop transmission in Multiple Block Read Operation"}},
		{SPI_CMD_13,	{{0x4d,0x00,0x00,0x00,0x00,0x00},	SPI_RSP_R2, "SEND_STATUS",				"Asks the selected card to send its status register."}},
		{SPI_CMD_16,	{{0x50,0x00,0x00,0x00,0x00,0x00},	SPI_RSP_R1, "SET_BLOCKLEN",				"Sets a block length (in bytes) for all following block commands"}},
		{SPI_CMD_17,	{{0x51,0x00,0x00,0x00,0x00,0x00},	SPI_RSP_R1, "READ_SINGLE_BLOCK",		"Reads a block of the size selected by SET_BLOCKLEN command"}},
		{SPI_CMD_18,	{{0x52,0x00,0x00,0x00,0x00,0x00},	SPI_RSP_R1, "READ_MULTIPLE_BLOCK",		"Continuously transfers data blocks from card to host until interrupted by a STOP_TRANSMISSION command."}},
		{SPI_CMD_24,	{{0x58,0x00,0x00,0x00,0x00,0x00},	SPI_RSP_R1, "WRITE_BLOCK",				"Writes a block of the size selected by the SET_BLOCKLEN command."}},
		{SPI_CMD_25,	{{0x59,0x00,0x00,0x00,0x00,0x00},	SPI_RSP_R1, "WRITE_MULTIPLE_BLOCK",		"Continuously writes blocks of data until 'Stop Tran' token is sent (instead of 'Start Block')."}},
		{SPI_CMD_27,	{{0x5b,0x00,0x00,0x00,0x00,0x00},	SPI_RSP_R1, "PROGRAM_CSD",				"Programming of the programmable bits of the CSD."}},
		{SPI_CMD_28,	{{0x5c,0x00,0x00,0x00,0x00,0x00},	SPI_RSP_R1b, "SET_WRITE_PROT",			"If the card has write protection features, this command sets the write protection bit of the addressed group."}},
		{SPI_CMD_29,	{{0x5d,0x00,0x00,0x00,0x00,0x00},	SPI_RSP_R1b, "CLR_WRITE_PROT",			"If the card has write protection features, this command clears the write protection bit of the addressed group."}},
		{SPI_CMD_30,	{{0x5e,0x00,0x00,0x00,0x00,0x00},	SPI_RSP_R1, "SEND_WRITE_PROT",			"If the card has write protection features, this command asks the card to send the status of the write protection bits"}},
		{SPI_CMD_32,	{{0x60,0x00,0x00,0x00,0x00,0x00},	SPI_RSP_R1, "ERASE_WR_BLK_START_ADDR",	"Sets the address of the first write block to be erased."}},
		{SPI_CMD_33,	{{0x61,0x00,0x00,0x00,0x00,0x00},	SPI_RSP_R1, "ERASE_WR_BLK_END_ADDR",	"Sets the address of the last write block of the continuous range to be erased."}},
		{SPI_CMD_38,	{{0x66,0x00,0x00,0x00,0x00,0x00},	SPI_RSP_R1b, "ERASE",					"Erases all previously selected write blocks"}},	
		{SPI_CMD_42,	{{0x6a,0x00,0x00,0x00,0x00,0x00},	SPI_RSP_R1, "LOCK_UNLOCK",				"Used to Set/Reset the Password or lock/unlock the card."}},
		{SPI_CMD_55,	{{0x77,0x00,0x00,0x00,0x00,0x00},	SPI_RSP_R1, "APP_CMD",					"Defines to the card that the next command is an application specific command rather than a standard command."}},
		{SPI_CMD_56,	{{0x78,0x00,0x00,0x00,0x00,0x00},	SPI_RSP_R1, "GEN_CMD",					"Used either to transfer a Data Block to the card or to get a Data Block from the card."}},
		{SPI_CMD_58,	{{0x7a,0x00,0x00,0x00,0x00,0x00},	SPI_RSP_R3, "READ_OCR",					"Reads the OCR register of a card."}},
		{SPI_CMD_59,	{{0x7b,0x00,0x00,0x00,0x00,0x00},	SPI_RSP_R1, "CRC_ON_OFF",				"Turns the CRC option on or off."}}
	};

	typedef struct SPIRspInfo_struct {
		vector<uint8_t>	okResponse;
		int				nBytes;
	} SPIRspInfo;

	// Response types including default content (with header '1...1' and trailing '1...1')
	map< SPIRspEnum, SPIRspInfo> spiRspInfo = {
		{SPI_RSP_R1,	{{0xff, 0x01, 0xff},								3}},
		{SPI_RSP_R1b,	{{0xff, 0x00, 0xff},								3}}, // one byte, could be followed by zero bytes
		{SPI_RSP_R2,	{{0xff, 0x00, 0x00, 0xff},							4}},
		{SPI_RSP_R3,	{{0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff},		7}},
		{SPI_RSP_R7,	{{0xff, 0x08, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff},	8}}
	};

	SPIRspEnum mResponseType = SPI_RSP_R1;

	ifstream* mCardImage = NULL;
	streamsize mCardSz = 0;

	uint8_t crc7(vector <uint8_t> &data, int startPos, int len);

	uint16_t crc16(vector <uint8_t>& data, int startPos, int len);

	string bytes2str(vector <uint8_t> data);

	uint8_t mTxShiftRegister = 0x0;
	uint8_t mRxShiftRegister = 0x0;

	bool validCmd(uint8_t cmdByte);

	bool mPendingAppCmd = false;

	bool execCmd(vector <uint8_t>  &request);
	bool execBaseCmd(vector <uint8_t>& reques);
	bool execAppCmd(vector <uint8_t>& reques);


	bool writeBlockData(int adr, vector <uint8_t>& data, int start, int len);


	bool processRxBits();
	bool generateTxBits();
	void recover();
	void prepareNextCmd();
	void initResponse(vector <uint8_t>& request);
	bool processMasterData();

public:


	SDCard(string name, double cpuClock, string cardImageFile, DebugManager* debugManager, ConnectionManager* connectionManager);

	~SDCard();

	bool cardInserted();

	bool insertCard(string cardImageFile);
	bool ejectCard();

	void processPortUpdate(int port) override;



};

#endif