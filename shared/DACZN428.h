#ifndef DAC_ZN428_H
#define DAC_ZN428_H

#include <cstdint>
#include "MemoryMappedDevice.h"


using namespace std;


//
// Emulates an ZN428 8b-bit Digitial-to-Analogue Converter
// 
// The analogue output range is [0,VREF] where VREF is [2.475, 2.625]
//
class DACZN428 : public MemoryMappedDevice {

private:

	// (Digital) Ports
	int ENABLE;
	PortVal mENABLE = 0x0;


	// Analogue ports
	int AOut;
	int VREF; // Reference voltage input

	double mOS = 1e-3;				// 1mV offset voltage
	uint8_t mDIn = 0x0;
	double mAOut = 0.0;
	double mVREF = 2.5;				// Reference voltage [2.475 to 2.625]

#define ADC_7002_CR_EOC_BIT_MASK	(0x1 << 7)



public:


	DACZN428(string name, uint16_t adr, uint16_t sz, int accessSpeed, DebugTracing* debugTracing, ConnectionManager* connectionManager, DeviceManager* deviceManager);

	bool writeByte(BusAddress adr, BusByte data);


};

#endif