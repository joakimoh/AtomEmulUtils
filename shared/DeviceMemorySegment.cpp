
#include "DeviceMemorySegment.h"
#include "MemoryMappedDevice.h"
#include "Utility.h"
#include <fstream>
#include <filesystem>
#include <iostream>
#include <cmath>

using namespace std;

DeviceMemorySegment::DeviceMemorySegment(uint16_t lowerAdr, uint16_t upperAdr, MemoryMappedDevice *dev) :
        lowerAdr(lowerAdr), upperAdr(upperAdr), dev(dev)
{
}

// Overload << operator
ostream& operator<<(ostream& sout, const DeviceMemorySegment &segment)
{

    sout << segment.dev->name << ":" << hex << segment.lowerAdr << ":" << segment.upperAdr;
    return  sout;
}

// Overload < operator
bool DeviceMemorySegment::operator<(const DeviceMemorySegment& segment)
{
    return this->lowerAdr < segment.upperAdr;
}

// Overload < operator
bool DeviceMemorySegment::operator>(const DeviceMemorySegment& segment)
{
    return this->lowerAdr > segment.upperAdr;
}