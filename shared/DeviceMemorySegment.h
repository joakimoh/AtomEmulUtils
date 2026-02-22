#ifndef DEVICE_MEMORY_SEGMENT_H
#define DEVICE_MEMORY_SEGMENT_H

#include <iostream>
#include <cstdint>


using namespace std;

class MemoryMappedDevice;

class DeviceMemorySegment {
public:

    // Address range of the memory segment
    uint16_t lowerAdr, upperAdr;

    // Device mapped into the memory segment
    MemoryMappedDevice* dev;

    // Constructor to initialize the node with a value
    DeviceMemorySegment(uint16_t lowerAdr, uint16_t upperAdr, MemoryMappedDevice* dev);

    // Overload < operator
    bool operator<(const DeviceMemorySegment& segment);

    // Overload > operator
    bool operator>(const DeviceMemorySegment& segment);

    friend ostream& operator<<(ostream& sout, const DeviceMemorySegment &segment);


    


};
#endif
