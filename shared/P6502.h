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
	uint8_t mAcc;		// Accumulator
	uint8_t mRegisterX;			// Index Register mRegisterX
	uint8_t mRegisterY;			// Index Register mRegisterY
	uint8_t mStackPointer;		// Stack Pointer
	// Status Register
	// b7 = Negative, b6 = Overflow (V), b5 not used, b4 = break (B),
	// b3 = Decimal (D), b2 = Interrupt Disable (I),b1 = zero (Z), b0 = carry (C) 
	// The B flag is not stored in the register but only put on stack (as b4) together with
	// the Status Register content when a BRK instruction is executed.
	uint8_t mStatusRegister;
#define N_set_mask 0x80 // 1000 0000 - b7
#define V_set_mask 0x40 // 0100 0000 - b6
#define b5_set_mask 0x30// 0010 0000 - b5 (not used)
#define B_set_mask 0x10 // 0001 0000 - b4
#define D_set_mask 0x08 // 0000 1000 - b3
#define I_set_mask 0x04 // 0000 0100 - b2
#define Z_set_mask 0x02 // 0000 0010 - b1
#define C_set_mask 0x01 // 0000 0001 - b0
#define N_flag ((mStatusRegister >> 7) & 1)
#define V_flag ((mStatusRegister >> 6) & 1)
#define D_flag ((mStatusRegister >> 3) & 1)
#define I_flag ((mStatusRegister >> 2) & 1)
#define Z_flag ((mStatusRegister >> 1) & 1)
#define C_flag (mStatusRegister & 1)

	bool getOperand(Codec6502::InstructionInfo instr, uint16_t& operand, uint16_t &calc_op_adr, uint8_t& read_val, bool& op_mem);


	void tick(int cycles = 1);

	bool pageBoundaryCrossed(uint16_t before, uint16_t after);

	void setNZflags(uint8_t val_8_u);
	void setNZCflags(uint8_t val_16_u, uint8_t C);
	void setNZVflags(uint8_t N, uint8_t Z, uint8_t V);
	void setNZCVflags(uint8_t N, uint8_t Z, uint8_t C, uint8_t V);

	bool executeInstr(Codec6502::InstructionInfo instr, uint16_t opcode_PC, uint16_t operand, uint16_t calc_op_adr, uint8_t read_val, bool op_mem);

	string getState();

	bool mVerbose = false;

public:

	P6502(double clockSpeed, vector<Device*> &devices, bool verbose = false);

	void run();

	bool reset();
	void stop();

	bool read(uint16_t adr, uint8_t &data);
	bool write(uint16_t adr, uint8_t data);

};

#endif