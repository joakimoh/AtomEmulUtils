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

	int cPeriod = 1000; // clock period in ns

	vector<Device*> & mDevices;

	mutex mMutex;

	bool mStop = false;

	Codec6502 mCodec;

	// Registers
	uint16_t mProgramCounter;	// Program Counter
	uint8_t mAccumulator;		// Accumulator
	uint8_t mRegisterX;			// Index Register mRegisterX
	uint8_t mRegisterY;			// Index Register mRegisterY
	uint8_t mStackPointer;		// Stack Pointer
	// Status Register
	// b7 = Negative, b6 = Overflow (V), b5 not used, b4 = break (B),
	// b3 = Decimal (D), b2 = Interrupt Disable (I),b1 = zero (Z), b0 = carry (C) 
	uint8_t mStatusRegister;
#define N_bit 7
#define V_bit 6
#define B_bit 4
#define D_bit 3
#define I_bit 2
#define Z_bit 1
#define C_bit 0
#define N_mask 0x80
#define V_mask 0x40
#define B_mask 0x10
#define D_mask 0x08
#define I_mask 0x04
#define Z_mask 0x02
#define C_mask 0x01
#define B_inv_mask 0xcf

	uint8_t getNegativeFlag() { return (mStatusRegister >> 7) & 0x1; }
	void setNegativeFlag(uint8_t v) { mStatusRegister = mStatusRegister | (v!=0?128:0); }

	uint8_t getOverFlowFlag() { return (mStatusRegister >> 6) & 0x1; }
	void setOverFlowFlag(uint8_t v) { mStatusRegister = mStatusRegister | (v!=0?64:0); }

	uint8_t getBreak() { return (mStatusRegister >> 4) & 0x1; }
	void setBreakFlag(uint8_t v) { mStatusRegister = mStatusRegister | (v!=0?16:0); }

	uint8_t getDecimalFlag() { return (mStatusRegister >> 3) & 0x1; }
	void setDecimalFlag(uint8_t v) { mStatusRegister = mStatusRegister | (v!=0?8:0); }

	uint8_t getInterruptDisableFlag() { return (mStatusRegister >> 2) & 0x1; }
	void setInterruptDisableFlag(uint8_t v) { mStatusRegister = mStatusRegister | (v!=0?4:0); }

	uint8_t getZeroFlag() { return (mStatusRegister >> 1) & 0x1; }
	void setZeroFlag(uint8_t v) { mStatusRegister = mStatusRegister | (v!=0?2:0); }

	uint8_t getCarryFlag() { return (mStatusRegister >> 0) & 0x1; }
	void setCarryFlag(uint8_t v) { mStatusRegister = mStatusRegister | (v!=0?1:0); }

	bool getOperand(Codec6502::InstructionInfo instr, uint16_t& op_adr, uint8_t &op_val, bool& op_mem);


	void tick(int cycles = 1);

	bool pageBoundaryCrossed(uint16_t before, uint16_t after);

	void setNZCVFlags(uint16_t val);
	void setNZFlags(uint8_t val);
	void setNZCFlags(uint16_t val);

	bool executeInstr(Codec6502::InstructionInfo instr, uint16_t op_adr, uint8_t op_val, bool op_mem);

public:

	P6502(double clockSpeed, vector<Device*> &devices);

	void run();

	bool reset();
	void stop();

	bool read(uint16_t adr, uint8_t &data);
	bool write(uint16_t adr, uint8_t data);

};

#endif