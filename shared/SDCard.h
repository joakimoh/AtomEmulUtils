#ifndef SD_CARD_H
#define SD_CARD_H

#include <cstdint>
#include "Device.h"
#include <vector>


using namespace std;

class SDCard : public Device {
	// Standard SD Card - SPI interface

private:

	int	CLK, MISO, MOSI, SEL;

	uint8_t mSEL = 0x0;		// Select
	uint8_t mCLK = 0x0;		// Clock
	uint8_t mMISO = 0x1;	// Data Out
	uint8_t mMOSI = 0x1;	// Data In

	uint8_t pCLK = 0x0;

	uint8_t mResponse = 0x0;
	int mReceviedBits = 0;
	int mSentBits = 0;
	uint64_t mCommand = 0x0;

	enum SPIMode {SPI_NOT_INIT, SPI_INIT_PENDING, SPI_INIT_CMD_WAIT, SPI_READY};

	SPIMode mSPIMode = SPI_NOT_INIT;

	//enum SPI_CMD {SPI_CMD0, SPI_CMD1};

#define SPI_CMD0 0b010000000000000000000000000000000000000010010101
	//uint64_t cmds[2][2] = {
	//	{SPI_CMD0, 0b010000000000000000000000000000000000000010010101},		// CMD0 - RESET
	//	{SPI_CMD1, 0x567890}
	//};

	ifstream* mCardImage = NULL;

public:


	SDCard(string name, double cpuClock, string cardFile, DebugManager* debugManager, ConnectionManager* connectionManager);

	~SDCard();

	void processPortUpdate(int port) override;

	bool execCmd(uint64_t cmd);


};

#endif