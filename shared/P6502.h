#ifndef P6052_H
#define P6502_H

#include "Codec6502.h"
#include "VIA6522.h"
#include "ROM.h"
#include "Device.h"
#include <vector>
#include <mutex>

class P6502 {

private:

	uint16_t pc = 0xfffc;
	uint16_t address = 0xfffc; // reset vector;
	uint8_t  data = 0x00;

	int cPeriod = 1000; // clock period in ns

	vector<Device*> & mDevices;

	mutex mMutex;

	bool mStop = false;

	Codec6502 mCodec;

	// Registers
	uint16_t PC;	// Program Counter
	uint8_t A;		// Accumulator
	uint8_t X;		// Index Register X
	uint8_t Y;		// Index Register Y
	uint8_t SP;		// Stack Pointer
	uint8_t S;		// Status Register;
					// b7 = Negative, b6 = Overflow (V), b5 not used, b4 = break (B),
					// b3 = Decimal (D), b2 = Interrupt Disable (I),b1 = zero (Z), b0 = carry (C) 


	bool getOperand(Codec6502::InstructionInfo instrInfo, uint8_t& operand);


	void tick();

public:

	P6502(double clockSpeed, vector<Device*> &devices);

	void run();

	bool reset();
	void stop();

	bool read(uint16_t adr, uint8_t &data);
	bool write(uint16_t adr, uint8_t data);

};

#endif