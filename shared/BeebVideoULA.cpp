#include "BeebVideoULA.h"


BeebVideoULA::BeebVideoULA(
	string name, uint16_t adr, double clockSpeed, ALLEGRO_BITMAP* disp, uint16_t videoMemAdr, DebugInfo debugInfo, ConnectionManager* connectionManager
) : VideoDisplayUnit(name, BEEB_VDU_DEV, adr, 0x100, disp, videoMemAdr, debugInfo, connectionManager)
{
}

BeebVideoULA::~BeebVideoULA()
{
}


// Reset device
bool BeebVideoULA::reset()
{
	return true;
}

// Advance until clock cycle stopcycle has been reached
bool BeebVideoULA::advance(uint64_t stopCycle)
{
	return true;
}

bool BeebVideoULA::advanceLine(uint64_t& endCycle)
{
	return true;
}


bool BeebVideoULA::read(uint16_t adr, uint8_t& data)
{
	return true;
}

bool BeebVideoULA::write(uint16_t adr, uint8_t data)
{
	return true;
}

bool BeebVideoULA::readGraphicsMem(uint16_t adr, uint8_t& data)
{
	return true;
}

void BeebVideoULA::lockDisplay()
{

}

void BeebVideoULA::unlockDisplay()
{

}

bool BeebVideoULA::getVisibleArea(int& w, int& h)
{
	return true;
}

double BeebVideoULA::getScanLineDuration()
{
	return 64.0;
}

int BeebVideoULA::getScanLinesPerFrame()
{
	return 312;
}

int BeebVideoULA::getFrameRate()
{
	return 50;
}