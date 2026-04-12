#include "ADC7002.h"
#include "Utility.h"
#include <cmath>

ADC7002::ADC7002(string name, double tickRate, double deviceClockRate, uint16_t adr, uint16_t sz, int accessSpeed, DebugTracing* debugTracing, ConnectionManager* connectionManager,
	DeviceManager* deviceManager) :
	MemoryMappedDevice(name, ADC_7002_DEV, OTHER_DEVICE, accessSpeed, adr, sz, debugTracing, connectionManager, deviceManager),
	ClockedDevice(tickRate, deviceClockRate)
{
	registerPort("EOC", OUT_PORT, 0x01, EOC, &mEOC);	// EOC output

	registerAnaloguePort("CH0", IN_PORT, CH0, &mCHInput[0]);
	registerAnaloguePort("CH1", IN_PORT, CH1, &mCHInput[1]);
	registerAnaloguePort("CH2", IN_PORT, CH2, &mCHInput[2]);
	registerAnaloguePort("CH3", IN_PORT, CH3, &mCHInput[3]);
	registerAnaloguePort("VREF", IN_PORT, VREF, &mVREF);

	mSpeed12 = (int)round(mSpeed12 * tickRate / mDeviceClockRate);
	mSpeed8 = (int)round(mSpeed8 * tickRate / mDeviceClockRate);
}

bool ADC7002::advanceUntil(uint64_t stopTick)
{
	while (mTicks < stopTick) {

		if (mConversion) {

			if (mTicks - mStartCycleCount >= mSpeed) {
				mConversion = false;
				double a_in = mCHInput[ADC_7002_CR_CH];
				if (a_in < 0)
					a_in = 0.0;
				else if (a_in > mVREF)
					a_in = mVREF;
				uint16_t range = (ADC_7002_CR_12_BIT ? 0xfff : 0xff);
				uint16_t data = a_in / mVREF * range;
				mDataH = (ADC_7002_CR_12_BIT ? (data >> 4) & 0xff : data & 0xff);
				mDataL = (ADC_7002_CR_12_BIT ? (data << 4) & 0xf0 : 0x00);

				// Set the EOC bit and BUSY bits (BUSY set <=> not busy) and assert the EOC output
				uint8_t pSR = mSR;
				mSR |= ADC_7002_CR_EOC_BIT_MASK | ADC_7002_SR_BUSY_MASK;
				updatePort(EOC, 0x0);
				//cout << "Conversion for channel " << (int)ADC_7002_CR_CH << " completed at cycle " << mTicks << "; input voltage " << a_in <<
				//	" gave value 0x" << Utility::int2HexStr(data, 3) << " <=> " << (data * mVREF / range) << "V for VREF = " << mVREF << "V and range = 0x" << Utility::int2HexStr(range, 3) <<
				//	", Status Register 0x" << Utility::int2HexStr(pSR, 2) << " -> 0x" << Utility::int2HexStr(mSR, 2) << "\n";
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
		break;
	}
	case 0x1:
	{
		// Data High (b11:b8 or b7:b0 of conversion data depending on 12-bit or 8-bit mode)
		data = mDataH;

		// Clear EOC bit and deassert the EOC output
		uint8_t pSR = mSR;
		mSR &= ~ADC_7002_CR_EOC_BIT_MASK;
		uint8_t p_EOC = mEOC;
		updatePort(EOC, 0x1);
		uint16_t d = (ADC_7002_CR_12_BIT ? (mDataH << 4) | (mDataL >> 4) : mDataH & 0xff);
		//if (mEOC != p_EOC)
		//	cout << "Read conversion data for channel " << (int)ADC_7002_CR_CH << " at cycle " << mTicks << " => 0x" << Utility::int2HexStr(d, 3) << " <=> " << (mVREF * d / 0x3ff) << " V for VREF = " <<
		//	mVREF << " V" << ", Status Register 0x" << Utility::int2HexStr(pSR, 2) << " -> 0x" << Utility::int2HexStr(mSR,2) << "\n";
		DBG_LOG_COND(mEOC != p_EOC, this, DBG_ADC, "Clear EOC bit and deassert EOC output\n");

		break;
	}
	case 0x2:
	case 0x3:
	{
		// Data Low (b3:b0 of conversion data for 12-bit conversion; otherwise zero)
		data = mDataL;
		break;
	}
	default:
		data = 0xff;
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
		uint8_t pSR = mSR;
		mSR &= ~ADC_7002_SR_BUSY_MASK; // Clear BUSY bit to indicate that conversion is in progress
		mSR = (mSR & ~ADC_7002_SR_MPX_MASK) | ADC_7002_CR_CH; // Update channel (MPX) bits in the status register
		//cout << "Start conversion for channel " << (int)ADC_7002_CR_CH << " at cycle " << mStartCycleCount << ", Status Register 0x" << Utility::int2HexStr(pSR, 2) << " -> 0x" << Utility::int2HexStr(mSR,2) << "\n";
		DBG_LOG(this, DBG_ADC, "Start "s + (ADC_7002_CR_12_BIT?"12-bit"s:"8-bit"s) + " conversion for channel " + to_string(ADC_7002_CR_CH));
		break;
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
	double v = voltage;
	if (channel >= 0 && channel < 4) {
		if (voltage < 0)
			v = 0;
		else if (voltage > mVREF)
			v = mVREF;
		mCHInput[channel] = v;
		return true;
	}

	return false;
}

// Outputs the internal state of the device
bool ADC7002::outputState(ostream& sout)
{
	sout << "Control Register = 0x" << Utility::int2HexStr(mCR, 2) << dec << " <=>\n";
	sout << "\tConversion bits =  " << (int)(ADC_7002_CR_12_BIT ? 12 : 8) << "\n";
	sout << "\tSelected channel = " << (int)ADC_7002_CR_CH << "\n";
	sout << "\tFLAG =             " << (int)ADC_7002_CR_FLAG << "\n";
	sout << "Status register = 0x" << Utility::int2HexStr(mSR, 2) << " <=>\n";
	sout << "\tBUSY =             " << (int)ADC_7002_SR_BUSY << "\n";
	sout << "\tChannel =          " << (int)ADC_7002_SR_CH << "\n";
	sout << "\tEOC =              " << (int)ADC_7002_SR_EOC << "\n";
	sout << "\tFLAG =             " << (int)ADC_7002_SR_FLAG << "\n";
	sout << "\tMSB =              " << (int)ADC_7002_SR_MSB << "\n";
	cout << "EOC (the port) =     " << (int)mEOC << "\n";
	for (int i = 0; i < 4; i++)
		sout << "Channel " << i << " Voltage =         " << mCHInput[i] << " V\n";
	sout << "Reference Voltage =  " << mVREF << " V\n";

	return true;
}