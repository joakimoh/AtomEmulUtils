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

	uint64_t mResponse = 0x0;
	int mReceviedBits = 0;
	int mSentBits = 0;
	uint64_t mCommand = 0x0;
	bool mFirstBit = true;

	int nResponseBits = 8;

	bool mInitialised = false;
	bool mResetCmdReceived = false;

	enum SPIMode {SPI_IDLE_WAIT, SPI_CMD_PREAMBLE_WAIT, SPI_CMD_WAIT, SPI_RESP_WAIT};

	SPIMode mSPIMode = SPI_IDLE_WAIT;

	enum SPICmdEnum {
							
		SPI_CMD0 = 0,		
		SPI_CMD1 = 1,		
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
		SPI_CMD_59 = 59		

	};

	enum SPIACmdEnum {
		SPI_A_CMD_13 = 13,	// SD_STATUS -					R2	Send the cards status register.
		SPI_A_CMD_22 = 22,	// SEND_NUM_WR_BLOCKS -			R1	Send the numbers of the well written (without errors) blocks
		SPI_A_CMD_23 = 23,	// SET_WR_BLK_ERASE_COUNT -		R1	Set the number of write blocks to be pre-erased before writing.
		SPI_A_CMD_41 = 41,	// SD_SEND_OP_COND -			R1	Sends host capacity support information and activates the card's initialization process.
		SPI_A_CMD_42 = 42,	// SET_CLR_CARD_DETECT -		R1	Connect ('1')/Disconnect ('0') the 50 KOhm pull-up resistor on CS (pin 1) of the card.
		SPI_A_CMD_51 = 51	// SEND_SCR -					R1	Reads the SD Configuration Register (SCR).
	};

	enum SPIRspEnum {
		SPI_RSP_R1 = 0,
		SPI_RSP_R1b = 1,
		SPI_RSP_R2 = 2,
		SPI_RSP_R3 = 3,
		SPI_RSP_R7 = 4
	};

	typedef struct SPICmdInfo_struct {
		uint64_t		request;
		SPIRspEnum		response;
	} SPICmdInfo;


	map<SPICmdEnum, SPICmdInfo> spiCmdInfo = {
		{SPI_CMD0,		{0x400000000095,	SPI_RSP_R1}},	// GO_IDLE_STATE -				Resets the SD Memory Card
		{SPI_CMD1,		{0x4100000000ff,	SPI_RSP_R1}},	// SEND_OP_COND -				Sends host capacity support information and activates the card's initialization process.
		{SPI_CMD_6,		{0x460000000000,	SPI_RSP_R1}},	// SWITCH_FUNC -				Checks switchable function (mode 0) and switches card function (mode 1)
		{SPI_CMD_8,		{0x48000001aa87,	SPI_RSP_R7}},	// SEND_IF_COND -				Sends SD Memory Card interface condition
		{SPI_CMD_9,		{0x490000000000,	SPI_RSP_R1}},	// SEND_CSD -					Asks the selected card to send its card-specific data (CSD)
		{SPI_CMD_10,	{0x4a0000000000,	SPI_RSP_R1}},	// SEND_CID	 -					Asks the selected card to send its card identification (CID)
		{SPI_CMD_12,	{0x4c0000000000,	SPI_RSP_R1b}},	// STOP_TRANSMISSION -			Forces the card to stop transmission in Multiple Block Read Operation
		{SPI_CMD_13,	{0x4d0000000000,	SPI_RSP_R2}},	// SEND_STATUS	-				Asks the selected card to send its status register.
		{SPI_CMD_16,	{0x500000000000,	SPI_RSP_R1}},	// SET_BLOCKLEN -				Sets a block length (in bytes) for all following block commands
		{SPI_CMD_17,	{0x510000000000,	SPI_RSP_R1}},	// READ_SINGLE_BLOCK -			Reads a block of the size selected by SET_BLOCKLEN command
		{SPI_CMD_18,	{0x520000000000,	SPI_RSP_R1}},	// READ_MULTIPLE_BLOCK -		Continuously transfers data blocks from card to host until interrupted by a STOP_TRANSMISSION command.
		{SPI_CMD_24,	{0x180000000000,	SPI_RSP_R1}},	// WRITE_BLOCK -				Writes a block of the size selected by the SET_BLOCKLEN command.
		{SPI_CMD_25,	{0x190000000000,	SPI_RSP_R1}},	// WRITE_MULTIPLE_BLOCK -		Continuously writes blocks of data until 'Stop Tran' token is sent (instead of 'Start Block').
		{SPI_CMD_27,	{0x1b0000000000,	SPI_RSP_R1}},	// PROGRAM_CSD -				Programming of the programmable bits of the CSD.
		{SPI_CMD_28,	{0x1c0000000000,	SPI_RSP_R1b}},	// SET_WRITE_PROT -				If the card has write protection features, this command sets the write protection bit of the addressed group.
		{SPI_CMD_29,	{0x1d0000000000,	SPI_RSP_R1b}},	// CLR_WRITE_PROT -				If the card has write protection features, this command clears the write protection bit of the addressed group.
		{SPI_CMD_30,	{0x1e0000000000,	SPI_RSP_R1}},	// SEND_WRITE_PROT -			If the card has write protection features, this command asks the card to send the status of the write protection bits
		{SPI_CMD_32,	{0x200000000000,	SPI_RSP_R1}},	// ERASE_WR_BLK_START_ADDR -	Sets the address of the first write block to be erased.
		{SPI_CMD_33,	{0x210000000000,	SPI_RSP_R1}},	// ERASE_WR_BLK_END_ADDR -		Sets the address of the last write block of the continuous range to be erased.
		{SPI_CMD_38,	{0x260000000000,	SPI_RSP_R1b}},	// ERASE -						Erases all previously selected write blocks
		{SPI_CMD_42,	{0x2a0000000000,	SPI_RSP_R1}},	// LOCK_UNLOCK -				Used to Set/Reset the Password or lock/unlock the card.
		{SPI_CMD_55,	{0x370000000000,	SPI_RSP_R1}},	// APP_CMD -					Defines to the card that the next command is an application specific command rather than a standard command.
		{SPI_CMD_56,	{0x380000000000,	SPI_RSP_R1}},	// GEN_CMD -					Used either to transfer a Data Block to the card or to get a Data Block from the card.
		{SPI_CMD_58,	{0x3a0000000000,	SPI_RSP_R3}},	// READ_OCR -					Reads the OCR register of a card.
		{SPI_CMD_59,	{0x3b0000000000,	SPI_RSP_R1}}		// CRC_ON_OFF -					Turns the CRC option on or off.
	};

	typedef struct SPIRspInfo_struct {
		int			nBytes;
	} SPIRspInfo;

	map< SPIRspEnum,int> spiRspInfo = {
		{SPI_RSP_R1,	1},
		{SPI_RSP_R1b,	1}, // one byte, could be followed by zero bytes
		{SPI_RSP_R2,	2},
		{SPI_RSP_R3,	5},
		{SPI_RSP_R7,	5}
	};
		
#define SPI_RESP_OK 0b10000000

	ifstream* mCardImage = NULL;

public:


	SDCard(string name, double cpuClock, string cardFile, DebugManager* debugManager, ConnectionManager* connectionManager);

	~SDCard();

	void processPortUpdate(int port) override;

	bool execCmd(uint64_t cmd);


};

#endif