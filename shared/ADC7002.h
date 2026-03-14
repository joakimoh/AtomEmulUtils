#ifndef ADC_7002_H
#define ADC_7002_H

#include <cstdint>
#include "MemoryMappedDevice.h"
#include <vector>
#include "ClockedDevice.h"


using namespace std;

class ADC7002 : public MemoryMappedDevice, public ClockedDevice {
	// Selects one of the four sideways ROM sockets (IC52, IC88, IC100, IC101, left to right on the board after the OS socket)

private:

	// (Digital) Ports
	int EOC;
	PortVal mEOC = 0x0; // End of Conversion IRQ output

	int mSpeed12 = 10000; // 12-bit conversion time in CLK units (e.g., CLK = 1 MHz => 10 ms)
	int mSpeed8 = 4000;	// 8-bit conversion time in CLK units (e.g., CLK = 1 MHz => 4 ms)
	int mSpeed = mSpeed12;
	int mStartCycleCount = 0;

	double mVREF = 0.7 * 3;				// Reference voltage
	double mCHInput[4] = { 0.0 };		// Channel input voltages
	uint16_t mConversionData = 0x0;		// Conversion data
	int mConversionCount = 0;			// Conversion counter
	bool mConversion = false;			// Active conversion


	// Registers
	uint8_t mCR;	// Base address +  0x0		Control Register	Write-only	b1b0 = MPX adr, b2 = flag input, b3 = 8/12-bit conversion (Low:8,High:12)
	uint8_t mSR;	// Base address +  0x0		Status Register		Read-Only	b7=EOC, b6=Busy, b5=MSB, b4=2nd MSB, b3=8/12-bit, b2=flag output, b1b0=MPX adr
	uint8_t mDataH;	// Base address +  0x1		Data High			Read-only	b7:b0 = MSB - 8-bit
	uint8_t mDataL;	// Based address + 0x2/0x3	Data Low			Read-only	(12-bit only) b7:b4 = 9th-12th bit
	uint8_t mTest;	// Based address + 0x3		Test mode			Write-only

#define ADC_7002_CR_CH		(mCR & 0x3)
#define ADC_7002_CR_FLAG	((mCR >> 2) & 0x1)
#define ADC_7002_CR_12_BIT	((mCR >> 3) & 0x1)
#define ADC_7002_SR_EOC		((mCR >> 7) & 0x1)
#define ADC_7002_SR_BUSY	((mCR >> 6) & 0x1)
#define ADC_7002_SR_MSB		((mCR >> 4) & 0x3)
#define ADC_7002_SR_FLAG	((mCR >> 2) & 0x1)
#define ADC_7002_SR_CH		(mCR & 0x3)
#define ADC_7002_DATA_12_BIT		(ADC_7002_CR_12_BIT?(mDataH|(((uint16_t)mDataL<<4) & 0x300)):mDataH)
#define ADC_7002_CR_EOC_BIT_MASK	(0x1 << 7)



public:


	ADC7002(string name, double tickRate, double deviceClockRate, uint16_t adr, uint16_t sz, int waitStates, DebugTracing* debugTracing, ConnectionManager* connectionManager,
		DeviceManager *deviceManager);

	bool advanceUntil(uint64_t stopTick);

	bool read(uint16_t adr, uint8_t& data);

	bool dump(uint16_t adr, uint8_t& data) override;

	bool write(uint16_t adr, uint8_t data);

	bool setChannelVoltage(int channel, double voltage);

	// Outputs the internal state of the device
	bool outputState(ostream& sout) override;


};

#endif