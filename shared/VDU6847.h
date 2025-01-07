#ifndef VDU6847_H
#define VDU6847_H

#include <cstdint>
#include "Device.h"
#include "RAM.h"

class VDU6847 : public Device {

private:
	
	uint8_t mMajorMode = 0x0;
	uint8_t mGraphicMode = 0x0;
	RAM* mVideoMem = NULL;
	uint16_t mVideoMemAdr = 0x0;

public:

	VDU6847(uint16_t adr, uint16_t videoMemAdr, DebugInfo debugInfo);

	bool read(uint16_t adr, uint8_t& data);
	bool write(uint16_t adr, uint8_t data);

	bool setGraphicMode(uint8_t mode);
	bool setVideoRam(RAM* ram);

	// Reset device
	bool reset();

	// Advance until clock cycle stopcycle has been reached
	bool advance(uint64_t stopCycle);

	uint16_t getVideoMemAdr() {
		return mVideoMemAdr;
	}

};

#endif