#include "DebugTracing.h"
#include <iostream>
#include <iomanip>
#include "Device.h"
#include "Utility.h"
#include <bitset>


using namespace std;

string DebugTracing::levels2str(DebugLevel debugLevel)
{
	string s;

	if ((debugLevel &  DBG_ERROR) != 0)
		s += " ERROR";
	if ((debugLevel & DBG_VERB_DEV) != 0)
		s += " VERBOSE";
	if ((debugLevel & DBG_WARNING) != 0)
		s += " WARNING";
	if ((debugLevel & DBG_6502) != 0)
		s += " 6502";
	if ((debugLevel & DBG_PORT) != 0)
		s += " PORT";
	if ((debugLevel & DBG_INTERRUPTS) != 0)
		s += " INTERRUPTS";
	if ((debugLevel & DBG_KEYBOARD) != 0)
		s += " KEYBOARD";
	if ((debugLevel & DBG_VDU) != 0)
		s += " VDU";
	if ((debugLevel & DBG_IO_PERIPHERAL) != 0)
		s += " IO_PERIPHERAL";
	if ((debugLevel & DBG_DEVICE) != 0)
		s += " DEVICE";
	if ((debugLevel & DBG_TRGGERING) != 0)
		s += " TRGGERING";
	if ((debugLevel & DBG_TIME) != 0)
		s += " TIME";
	if ((debugLevel & DBG_AUDIO) != 0)
		s += " AUDIO";
	if ((debugLevel & DBG_TAPE) != 0)
		s += " TAPE";
	if ((debugLevel & DBG_RESET) != 0) 
		s += " RESET";
	if ((debugLevel & DBG_SPI) != 0)
		s += " SPI";
	if ((debugLevel & DBG_ADC) != 0)
		s += " ADC";

	return s;
}

//
// ewdupirkvstxacASC
bool DebugTracing::string2debugLevel(string level, DebugLevel& debugLevel)
{
	debugLevel = DBG_NONE;
	if (level.find("V") != string::npos)
		debugLevel |= DBG_VERB_DEV;
	if (level.find("e") != string::npos)
		debugLevel  |= DBG_ERROR;
	if (level.find("w") != string::npos)
		debugLevel  |= DBG_WARNING;
	if (level.find("d") != string::npos)
		debugLevel  |= DBG_DEVICE;
	if (level.find("u") != string::npos)
		debugLevel  |= DBG_6502;
	if (level.find("p") != string::npos)
		debugLevel  |= DBG_PORT;
	if (level.find("i") != string::npos)
		debugLevel  |= DBG_INTERRUPTS | DBG_RESET;
	if (level.find("r") != string::npos)
		debugLevel  |= DBG_RESET;
	if (level.find("k") != string::npos)
		debugLevel  |= DBG_KEYBOARD;
	if (level.find("v") != string::npos)
		debugLevel  |= DBG_VDU;
	if (level.find("s") != string::npos)
		debugLevel  |= DBG_IO_PERIPHERAL;
	if (level.find("t") != string::npos)
		debugLevel  |= DBG_TRGGERING;
	if (level.find("x") != string::npos)
		debugLevel  |= DBG_TIME;
	if (level.find("a") != string::npos)
		debugLevel  |= DBG_AUDIO;
	if (level.find("c") != string::npos)
		debugLevel  |= DBG_TAPE;
	if (level.find("A") != string::npos)
		debugLevel  |= DBG_ALL;
	if (level.find("S") != string::npos)
		debugLevel  |= DBG_SPI;
	if (level.find("C") != string::npos)
		debugLevel  |= DBG_ADC;

	if (debugLevel == DBG_NONE && level != "")
		return false;

	return true;
}


bool DebugTracing::setDebugLevel(DebugLevel level)
{
	mDbgLevel |= level;
	return true;
}

bool DebugTracing::setDebugLevel(string levelS)
{
	DebugLevel level;
	if (!string2debugLevel(levelS, level))
		return false;
	mDbgLevel |= level;
	return true;
}

bool DebugTracing::clearDebugLevel(DebugLevel level)
{
	mDbgLevel &= ~level;
	return true;
}

bool DebugTracing::clearDebugLevel(string levelS)
{
	DebugLevel level;
	if (!string2debugLevel(levelS, level))
		return false;
	mDbgLevel &= ~level;
	return true;
}

void DebugTracing::clearDebugLevel()
{
	mDbgLevel = DBG_NONE;
}

DebugLevel DebugTracing::getDebugLevel()
{
	return mDbgLevel;
}

bool DebugTracing::debugLevelIs(DebugLevel level)
{
	return  (mDbgLevel & level) != 0;
}

bool DebugTracing::debugLevelIs(Device *dev, DebugLevel level)
{
	return (mDbgLevel & level) != 0 && dev->tracingEnabled();
	
}

//
// Logging of all the types of device data that are currently enabled
//
void DebugTracing::log(Device * dev, DebugLevel level, string line)
{
	if (!debugLevelIs(dev, level) || !dev->tracingEnabled())
		return;

	double t = dev->getCycleCount() / (dev->mCPUClock * 1e6);
	string t_s = Utility::encodeCPUTime(t);
	string prefix = t_s + " " + dev->name + " ";

	
	// Log the data
	cout << prefix << line + "\n";

}