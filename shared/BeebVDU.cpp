#include "BeebVDU.h"


BeebVDU::BeebVDU(
	string name, uint16_t adr, double clockSpeed, ALLEGRO_BITMAP* disp, uint16_t videoMemAdr, DebugInfo debugInfo, ConnectionManager* connectionManager
) : VideoDisplayUnit(name, BEEB_VDU_DEV, adr, 0x100, disp, videoMemAdr, debugInfo, connectionManager)
{
}

BeebVDU::~BeebVDU()
{
}


// Reset device
bool BeebVDU::reset()
{
	return true;
}

// Advance until clock cycle stopcycle has been reached
bool BeebVDU::advance(uint64_t stopCycle)
{
	return true;
}

bool BeebVDU::advanceLine(uint64_t& endCycle)
{
	return true;
}


bool BeebVDU::read(uint16_t adr, uint8_t& data)
{
	return true;
}

bool BeebVDU::write(uint16_t adr, uint8_t data)
{
	return true;
}

bool BeebVDU::readGraphicsMem(uint16_t adr, uint8_t& data)
{
	return true;
}

void BeebVDU::lockDisplay()
{

}

void BeebVDU::unlockDisplay()
{

}

bool BeebVDU::getVisibleArea(int& w, int& h)
{
	return true;
}

double BeebVDU::getScanLineDuration()
{
	return 64.0;
}

int BeebVDU::getScanLinesPerFrame()
{
	return 312;
}

int BeebVDU::getFrameRate()
{
	return 50;
}