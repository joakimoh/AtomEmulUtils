#ifndef DEBUG_MANAGER_H
#define DEBUG_MANAGER_H

#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <cstdint>
#include <array>
#include "Codec6502.h"

class Device;


using namespace std;

typedef int DebugLevel;
#define	DBG_NONE			0x00000
#define	DBG_ERROR			0x00001
#define	DBG_VERB_DEV		0x00002
#define	DBG_WARNING			0x00004
#define	DBG_6502			0x00008
#define DBG_PORT			0x00010
#define DBG_INTERRUPTS		0x00020	// Interrupts & reset
#define DBG_KEYBOARD		0x00040
#define DBG_GRAPHICS		0x00080
#define DBG_IO_PERIPHERAL	0x00100
#define	DBG_DEVICE			0x00200
#define DBG_TRGGERING		0x00400	// TRIGGERING
#define DBG_TIME			0x00800
#define DBG_AUDIO			0x01000
#define DBG_TAPE			0x02000
#define DBG_RESET			0x04000	// Only reset
#define DBG_SPI				0x08000
#define DBG_ADC				0x10000
#define	DBG_VERBOSE			0x20000
#define DBG_EXTENSIVE		0x80000 // Make the extensive type of debugging for the selected debugging scope
#define	DBG_ALL				0x9ffff

#define	DBG_VERB_EXT_DEV		(DBG_VERB_DEV|DBG_EXTENSIVE)
#define DGB_VERBOSE_EXTENSIVE	(DBG_VERBOSE|DBG_EXTENSIVE)


#define VERBOSE_OUTPUT				mDM->verboseOutput(false)
#define VERBOSE_EXT_OUTPUT			mDM->verboseOutput(true)

//#define DBG_ON

#ifdef DBG_ON

#define DBG_LOG(...)				mDM->log(__VA_ARGS__)
#define DBG_LOG_COND(cond,...)		if(cond) mDM->log(__VA_ARGS__)
#define DBG_LEVEL_DEV(...)			mDM->debugLevelIs(__VA_ARGS__)


#else

#define DBG_LOG(...)
#define DBG_LOG_COND(...)
#define DBG_LEVEL_DEV(...)			false

#endif

// Logging of devices' serial states
#define ENGINE_SERIALISED_STATE_MAX 32
#define ENGINE_SERIALISED_STATES_MAX 32
typedef array<uint8_t, ENGINE_SERIALISED_STATE_MAX> SerialisedState;
typedef array<SerialisedState, ENGINE_SERIALISED_STATES_MAX> SerialisedStates;



typedef struct InstrLogData_struct {
	double logTime = 0;
	Codec6502::InstructionInfo *instr = nullptr;
	uint8_t A = 0x0;
	uint8_t X = 0x0;
	uint8_t Y = 0x0;
	uint8_t SP = 0x0;
	uint8_t SR = 0x0;
	uint16_t PC = 0x0;
	uint16_t opcodePC = 0x0;
	uint16_t opcode = 0x0;
	uint16_t operand = 0x0;
	int accessAdr = -1;
	bool execFailure = false;
	bool activeIRQ = false;
	bool activeNMI = false;
	uint16_t stack = 0x0;
	uint8_t readVal = 0;
	uint8_t writtenVal = 0;
	int memContent = -1;
	int cycles = -1; // No of cycles the instruction took to execute
	Codec6502::InterruptState BRKType = Codec6502::NONE_PENDING;
} InstrLogData;


class DebugTracing {

private:

	DebugLevel mDbgLevel = DBG_NONE;

	bool string2debugLevel(string debugLevelS, DebugLevel &debugLevel);
	static string levels2str(DebugLevel debugLevel);

public:

	bool debugLevelIs(Device* dev, DebugLevel level);
	bool debugLevelIs(DebugLevel level);

	bool verboseOutput(bool ext) { return (mDbgLevel & DBG_VERBOSE) != 0 && (!ext || ext && (mDbgLevel & DBG_EXTENSIVE)); }
	

	bool setDebugLevel(DebugLevel level);
	bool setDebugLevel(string level);
	bool clearDebugLevel(DebugLevel level);
	bool clearDebugLevel(string level);
	void clearDebugLevel();
	DebugLevel getDebugLevel();
	string getDebugLevelStr() { return levels2str(mDbgLevel); }

	// Logging of all the types of device data that are currently enabled
	void log(Device * dev, DebugLevel level, string line);



};




#endif