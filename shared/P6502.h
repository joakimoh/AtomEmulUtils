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
#define N_set_mask 0x80
#define N_clr_mask 0x7f
#define V_set_mask 0x40
#define V_clr_mask 0xb0
#define B_set_mask 0x10
#define B_clr_mask 0xe0
#define D_set_mask 0x08
#define D_clr_mask 0xf7
#define I_set_mask 0x04
#define I_clr_mask 0xfb
#define Z_set_mask 0x02
#define Z_clr_mask 0xfd
#define C_set_mask 0x01
#define C_clr_mask 0xfe
#define N_flag ((mStatusRegister >> 7) & 1)
#define V_flag ((mStatusRegister >> 6) & 1)
#define B_flag ((mStatusRegister >> 4) & 1)
#define D_flag ((mStatusRegister >> 3) & 1)
#define I_flag ((mStatusRegister >> 2) & 1)
#define Z_flag ((mStatusRegister >> 1) & 1)
#define C_flag (mStatusRegister & 1)

	bool getOperand(Codec6502::InstructionInfo instr, uint16_t& operand, uint16_t &calc_op_adr, uint8_t& read_val, bool& op_mem);


	void tick(int cycles = 1);

	bool pageBoundaryCrossed(uint16_t before, uint16_t after);

	void setNZflags(uint8_t val);
	void setNZCflags(uint16_t val);
	void setNZCVflags(uint16_t val);

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