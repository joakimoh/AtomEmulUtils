#include "ADC7002.h"
#include "Utility.h"
#include <cmath>

ADC7002::ADC7002(string name, double tickRate, double deviceClockRate, uint16_t adr, uint16_t sz, int waitStates, DebugTracing* debugTracing, ConnectionManager* connectionManager,
	DeviceManager* deviceManager) :
	MemoryMappedDevice(name, ADC_7002_DEV, OTHER_DEVICE, waitStates, adr, sz, debugTracing, connectionManager, deviceManager),
	ClockedDevice(tickRate, deviceClockRate)
{
	registerPort("EOC", OUT_PORT, 0x01, EOC, &mEOC);	// EOC output

	mSpeed12 = (int)round(mSpeed12 * tickRate / mDeviceClockRate);
	mSpeed8 = (int)round(mSpeed8 * tickRate / mDeviceClockRate);
}

bool ADC7002::advanceUntil(uint64_t stopTick)
{
	while (mTicks < stopTick) {

		if (mConversion) {

			if (mTicks - mStartCycleCount >= mSpeed) {
				mConversion = false;
				uint16_t range = (ADC_7002_CR_12_BIT ? 0x3ff : 0xff);
				uint16_t data = mCHInput[ADC_7002_CR_CH]  * range;
				mDataH = data & 0xff;
				mDataL = (ADC_7002_CR_12_BIT ? (data >> 4) & 0xf0 : 0x00);

				// Set the EOC bit and assert the EOC output
				mSR |= ADC_7002_CR_EOC_BIT_MASK;
				updatePort(EOC, 0x0);

				DBG_LOG(this, DBG_ADC, (ADC_7002_CR_12_BIT ? "12-bit"s : "8-bit"s) + " conversion for channel " + to_string(ADC_7002_CR_CH) + 
					" completed; input voltage " + to_string(mCHInput[ADC_7002_CR_CH]) + " gave value " + Utility::int2HexStr(data,3));
			}

		}

		timeTick(1);
	}

	return true;
}

bool ADC7002::readByte(BusAddress adr, BusByte& data) {

	// Call parent class to trigger scheduling of other devices when applicable
	if (!MemoryMappedDevice::triggerBeforeRead(adr, data))
		return false;

	switch (adr & 0x3) {
	case 0x0:
		// Status Register
	{
		data = mSR;
	}
	case 0x1:
	{
		// Data High (b7:b0 of conversion data)
		data = mDataH;

		// Clear EOC bit and deassert the EOC output
		mSR &= ~ADC_7002_CR_EOC_BIT_MASK;
		uint8_t p_EOC = mEOC;
		updatePort(EOC, 0x1);

		DBG_LOG_COND(mEOC != p_EOC, this, DBG_ADC, "Clear EOC bit and deassert EOC output\n");

		break;
	}
	case 0x2:
	case 0x3:
	{
		// Data Low (b11:b8 of conversion data)
		data = mDataL;
		break;
	}
	default:
		break;
	}

	return true;
}

bool ADC7002::dump(BusAddress adr, uint8_t& data)
{
	data = 0xff;
	switch (adr & 0x3) {
	case 0x0:
		// Status Register
		data = mSR;
		break;
	case 0x1:
		// Data High (b7:b0 of conversion data)
		data = mDataH;
		break;
	case 0x2:
	case 0x3:
	{
		// Data Low (b11:b8 of conversion data)
		data = mDataL;
		break;
	}
	default:
		return false;
	}

	return true;
}

bool ADC7002::writeByte(BusAddress adr, BusByte data)
{
	switch (adr & 0x3) {
	case 0x0:
		// Control Register
	{
		mCR = data;
		mConversionCount = 0;
		mConversion = true;
		mSpeed = (ADC_7002_CR_12_BIT ? mSpeed12 : mSpeed8);
		mStartCycleCount = mTicks;

		DBG_LOG(this, DBG_ADC, "Start "s + (ADC_7002_CR_12_BIT?"12-bit"s:"8-bit"s) + " conversion for channel " + to_string(ADC_7002_CR_CH));
	}
	case 0x3:
	{
		// Test Mode
		break;
	}
	default:
		break;
	}

	// Call parent class to trigger scheduling of other devices when applicable
	return MemoryMappedDevice::triggerAfterWrite(adr, data);
}

bool ADC7002::setChannelVoltage(int channel, double voltage)
{
	if (channel >= 0 && channel < 4 && voltage >= 0 && voltage <= mVREF) {
		mCHInput[channel] = voltage;
		return true;
	}

	return false;
}

// Outputs the internal state of the device
bool ADC7002::outputState(ostream& sout)
{
	sout << "Control Register = 0x" << Utility::int2HexStr(mCR, 2) << " <=>\n";
	sout << "\tConversion bits =  " << (ADC_7002_CR_12_BIT ? 12 : 8) << "\n";
	sout << "\tSelected channel = " << (int)ADC_7002_CR_CH << "\n";
	sout << "\tFLAG =             " << (int)ADC_7002_CR_FLAG << "\n";
	sout << "Status register = 0x" << Utility::int2HexStr(mSR, 2) << " <=>\n";
	sout << "\tBUSY =             " << (int)ADC_7002_SR_BUSY << "\n";
	sout << "\tChannel =          " << (int)ADC_7002_SR_CH << "\n";
	sout << "\tEOC =              " << (int)ADC_7002_SR_EOC << "\n";
	sout << "\tFLAG =             " << (int)ADC_7002_SR_FLAG << "\n";
	sout << "\tMSB =              " << (int)ADC_7002_SR_MSB << "\n";
	for (int i = 0; i < 4; i++)
		sout << "Channel " << i << " Voltage =         " << mCHInput[i] << " V\n";

	return true;
}