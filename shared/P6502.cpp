
#include "P6502.h"
#include <chrono>
#include <thread>
#include <iostream>
#include <sstream>
#include <filesystem>
#include <bitset>
#include<conio.h>

P6502::P6502(double clockSpeed, vector<Device*>& devices, bool verbose) : mDevices(devices), mVerbose(verbose)
{
	cPeriod = (int) round(1000 / clockSpeed);
	reset();
}

// Execution method
// Will continue to execute instructions until the stop()
// method is called.
// Must be started as a thread.
//
void P6502::run()
{
	bool cont = true;

	while (cont) {

		// Check for stop condition
		mMutex.lock();
		if (mStop)
			cont = false;
		mMutex.unlock();

		// Get opcode of next instruction
		uint8_t opcode;
		if (!read(mProgramCounter, opcode)) {
			cont = false;
			std::cout << "Failed to read instruction!\n";
		}
		uint16_t opcode_PC = mProgramCounter;
		mProgramCounter++;

		// Decode the opcode
		Codec6502::InstructionInfo instr;
		if (!mCodec.decodeInstruction(opcode, instr)) {
			cont = false;
			std::cout << "Invalid instruction 0x" << hex << (int) opcode << " encountered at address 0x" << hex << mProgramCounter << "!\n";
		}

		// Get the operand
		uint16_t operand = 0x0;
		uint16_t calc_op_adr = 0x0;
		uint8_t read_val = 0x0;
		bool op_mem;
		if (!getOperand(instr, operand, calc_op_adr, read_val, op_mem)) {
			cont = false;
			std::cout << "Failed to get operand for instruction 0x" << hex << (int)opcode << " at address 0x" << opcode_PC << "!\n";
		}

		// Execute the instruction
		if (!executeInstr(instr, opcode_PC, operand, calc_op_adr, read_val, op_mem)) {
			cont = false;
			std::cout << "Failed to execute instruction!\n";
		}

		if (mVerbose && cont) {
			string instr_s = mCodec.decode(opcode_PC, opcode, operand);
			cout << setfill(' ') << setw(30) << left << instr_s << right <<
				" " << getState() << "\n";
		}

		// Increase time by the no of clock cycles specified for the instruction and mode.
		// This excludes extra cycle at page boundary as this is instead address in getOperand().
		tick(instr.cycles);
	}
}

bool P6502::executeInstr(Codec6502::InstructionInfo instr, uint16_t opcode_PC, uint16_t operand, uint16_t calc_op_adr, uint8_t read_val, bool op_mem)
{
	uint16_t val_16;
	uint8_t val_8;
	switch (instr.instruction) {

	case Codec6502::Instruction::ADC:
		// Add Memory to Accumulator with Carry
		// If decimal flag is set, addition will be BCD (Binary-Coded Decimal)
		// A + M + C -> A, C
		// N	Z	C	I	D	V
		// +	+	+	-	-	+
	{
		if (D_flag) {
			// Convert values to 2-complement numbers before addition
			uint8_t A_2c = (mAccumulator / 16) * 10 + mAccumulator & 16;
			uint8_t op_2c = (read_val / 16) * 10 + read_val & 16;
			uint16_t val_16_2c = (int8_t) A_2c + (int8_t)op_2c + (int8_t) C_flag;
			setNZCVflags(val_16_2c);
			mAccumulator = (val_16_2c % 100) / 10 * 16 + (val_16_2c % 100) % 10;
		}
		else {
			val_16 = (int8_t)mAccumulator + (int8_t)read_val + (int8_t)C_flag; // Add as 2-complement numbers to secure sign-extension
			mAccumulator = (uint8_t)(val_16 & 0xf);
			setNZCVflags(val_16);
		}
		break;
	}

	case Codec6502::Instruction::AND:
		// AND Memory with Accumulator
		// A AND M -> A
		// N	Z	C	I	D	V
		// +	+	-	-	-	-
	{
		mAccumulator = mAccumulator & read_val;
		setNZflags(mAccumulator);
		break;
	}

	case Codec6502::Instruction::ASL:
		// Shift Left One Bit (Memory or Accumulator)
		// C < [76543210] <- 0
		// N	Z	C	I	D	V
		// +	+	+	-	-	-
	{
		mStatusRegister &= C_clr_mask;
		mStatusRegister |= ((read_val & 0x80) ? C_set_mask : 0);
		val_16 = (read_val << 1) & 0x1fe;
		if (op_mem) {
			if (!write(calc_op_adr, val_16 & 0xff)) {
				return false;
			}
		}
		else
			mAccumulator = val_16 & 0xff;
		setNZflags(val_16);
		break;
	}

	case Codec6502::Instruction::BCC:
		// Branch on Carry Clear
		// Branch on C = 0
		// N	Z	C	I	D	V
		// -	-	-	-	-	-
	{
		if (!C_flag)
			mProgramCounter = (opcode_PC+2 + (int8_t) operand) & 0xffff;
		break;
	}

	case Codec6502::Instruction::BCS:
		// Branch on Carry Set
		// Branch on C = 1
		// N	Z	C	I	D	V
		// -	-	-	-	-	-
	{
		if (C_flag)
			mProgramCounter = (opcode_PC+2 + (int8_t) operand) & 0xffff;
		break;
	}

	case Codec6502::Instruction::BEQ:
		// Branch on Result Zero
		// Branch on Z = 0
		// N	Z	C	I	D	V
		// -	-	-	-	-	-
	{
		if (Z_flag)
			mProgramCounter = (opcode_PC+2 + (int8_t) operand) & 0xffff;
		break;
	}

	case Codec6502::Instruction::BIT:
	{
		// Test Bits in Memory with Accumulator
		// A AND M -> Z, M7 -> N, M6 -> V
		// N	Z	C	I	D	V
		// M7	+	-	-	-	M6
		{
			val_8 = mAccumulator & read_val;
			mStatusRegister &= Z_clr_mask & N_clr_mask & V_clr_mask;
			mStatusRegister |= (val_8 == 0? Z_set_mask: 0);
			mStatusRegister |= read_val & N_set_mask;
			mStatusRegister |= read_val &V_set_mask;
			break;
		}
	}

	case Codec6502::Instruction::BMI:
		// Branch on Result Minus
		// Branch on N = 1
		// N	Z	C	I	D	V
		// -	-	-	-	-	-
	{
		if (N_flag)
			mProgramCounter = (opcode_PC+2 + (int8_t) operand) & 0xffff;
		break;
	}

	case Codec6502::Instruction::BNE:
		// Branch on Result not Zero
		// Branch on Z = 0
		// N	Z	C	I	D	V
		// -	-	-	-	-	-
	{
		if (!Z_flag)
			mProgramCounter = (opcode_PC+2 + (int8_t) operand) & 0xffff;
		break;
	}

	case Codec6502::Instruction::BPL:
		// Branch on Result Plus
		// Branch on N = 0
		// N	Z	C	I	D	V
		// -	-	-	-	-	-
	{
		if (!N_flag)
			mProgramCounter = (opcode_PC+2 + (int8_t) operand) & 0xffff;
		break;
	}

	case Codec6502::Instruction::BRK:
		// Force Break
		// Initiates a software interrupt
		// push PC+2, push SR
		// The low PC byte is pushed first (so that the PC is stored in little endian format in the memory)
		// N	Z	C	I	D	V
		// -	-	-	1	-	-
	{
		// Force Break
		
		// Save PC & Status to stack
		uint16_t PC_push_val = opcode_PC + 2;
		write(0x100 + (uint16_t) mStackPointer--, PC_push_val / 256);
		write(0x100 + (uint16_t)mStackPointer--, PC_push_val % 256);
		write(0x100 + (uint16_t)mStackPointer--, mStatusRegister);

		// Fetch break vector
		uint8_t adr_L, adr_H;
		if (!read(0xfffe, adr_L) || !read(0xffff, adr_H))
			return false;
		mProgramCounter = adr_H * 256 + adr_L;

		break;
	}

	case Codec6502::Instruction::BVC:
		// Branch on Overflow Clear
		// Branch on V = 0
		// N	Z	C	I	D	V
		// -	-	-	-	-	-
	{
		if (!V_flag)
			mProgramCounter = (opcode_PC+2 + (int8_t) operand) & 0xffff;
		break;
	}

	case Codec6502::Instruction::BVS:
		// Branch on Overflow Set
		// Branch on V = 1
		// N	Z	C	I	D	V
		// -	-	-	-	-	-
	{
		if (V_flag)
			mProgramCounter = (opcode_PC+2 + (int8_t) operand) & 0xffff;
		break;
	}

	case Codec6502::Instruction::CLC:
		// Clear Carry Flag
		// N	Z	C	I	D	V
		// -	-	0	-	-	-
	{
		mStatusRegister &= C_clr_mask;
		break;
	}

	case Codec6502::Instruction::CLD:
		// Clear Decimal Mode
		// N	Z	C	I	D	V
		// -	-	-	-	0	-
	{
		mStatusRegister &= D_clr_mask;
		break;
	}

	case Codec6502::Instruction::CLI:
		// Clear Interrupt Disable Bit
		// N	Z	C	I	D	V
		// -	-	-	0	-	-
	{
		mStatusRegister &= I_clr_mask;
		break;
	}

	case Codec6502::Instruction::CLV:
		// Clear Overflow Flag
		// N	Z	C	I	D	V
		// -	-	-	-	-	0
	{
		mStatusRegister &= V_clr_mask;
		break;
	}

	case Codec6502::Instruction::CMP:
		// Compare Memory with Accumulator
		// A - M
		// N	Z	C	I	D	V
		// +	+	+	-	-	-

	{
		
		val_16 = mAccumulator - read_val;
		setNZCflags(val_16);
		mStatusRegister ^= C_set_mask; // invert carry as it was set based on overflow at addition
		break;
	}

	case Codec6502::Instruction::CPX:
		// Compare Memory and X
		// X - M
		// N	Z	C	I	D	V
		// +	+	+	-	-	-
	{
		
		val_16 = mRegisterX - read_val;
		setNZCflags(val_16);
		mStatusRegister ^= C_set_mask; // invert carry as it was set based on overflow at addition
		break;
	}

	case Codec6502::Instruction::CPY:
		// Compare Memory and Y
		// Y - M
		// N	Z	C	I	D	V
		// +	+	+	-	-	-
	{
		val_16 = mRegisterY - read_val;
		setNZCflags(val_16);
		mStatusRegister ^= C_set_mask; // invert carry as it was set based on overflow at addition
		break;
	}

	case Codec6502::Instruction::DEC:
		// Decrement Memory by One
		// M - 1 -> M
		// N	Z	C	I	D	V
		// +	+	-	-	-	-
	{
		val_8 = read_val - 1;
		if (!write(calc_op_adr, val_8)) {
			return false;
		}
		setNZflags(val_8);
		break;
	}

	case Codec6502::Instruction::DEX:
		// Decrement X by One
		// X - 1 -> X
		// N	Z	C	I	D	V
		// +	+	-	-	-	-
	{
		mRegisterX = mRegisterX - 1;
		setNZflags(mRegisterX);
		break;
	}

	case Codec6502::Instruction::DEY:
		// Decrement Y by One
		// Y - 1 -> Y
		// N	Z	C	I	D	V
		// +	+	-	-	-	-
	{
		mRegisterY = mRegisterY - 1;
		setNZflags(mRegisterX);
		break;
	}

	case Codec6502::Instruction::EOR:
		// Exclusive-OR Memory with Accumulator
		// A EOR M -> A
		// N	Z	C	I	D	V
		// +	+	-	-	-	-

	{
		mAccumulator = mAccumulator ^ read_val;
		setNZflags(mAccumulator);
		break;
	}

	case Codec6502::Instruction::INC:
		// Increment Memory by One
		// M + 1 -> M
		// N	Z	C	I	D	V
		// +	+	-	-	-	-
	{
		val_8 = read_val + 1;
		if (!write(calc_op_adr, val_8)) {
			return false;
		}
		setNZflags(val_8);
		break;
	}

	case Codec6502::Instruction::INX:
		// Increment X by One
		// X + 1 -> X
		// N	Z	C	I	D	V
		// +	+	-	-	-	-
	{
		mRegisterX = mRegisterX + 1;
		setNZflags(mRegisterX);
		break;
	}

	case Codec6502::Instruction::INY:
		// Increment Y by One
		// Y + 1 -> Y
		// N	Z	C	I	D	V
		// +	+	-	-	-	-
	{
		mRegisterY = mRegisterY + 1;
		setNZflags(mRegisterX);
		break;
	}

	case Codec6502::Instruction::JMP:
		// Jump to New Location
		// OP2:OP1 -> PC
		// N	Z	C	I	D	V
		// -	-	-	-	-	-
	{
		if (calc_op_adr == 0xfe52) { // OSWRCH is executed via JSR $fff4:JMP($208) where Mem[0x208] = 0xfe52
			// Output A content on screen
			if (mAccumulator >= 0x20 && mAccumulator < 0x7f)
				cout.put((char)mAccumulator).flush();
			else if (mAccumulator == 0xd)
				cout << "\n";

			// Emulate RTS instruction
			uint8_t PC_l, PC_h;
			read(0x100 + (uint16_t)++mStackPointer, PC_l);
			read(0x100 + (uint16_t)++mStackPointer, PC_h);
			mProgramCounter = PC_h * 256 + PC_l + 1;
			//cout << "OSWRCH RTS => PC = 0x" << mProgramCounter << "\n";
		}
		else if (calc_op_adr == 0xfe94) { // OSRDCH is executed via JSR $FFE3:JMP($20a) where Mem[0x20a] = 0xfe94
			// Read char into A
			//cout << "OSRDCH\n";
			mAccumulator = (uint8_t)getch();
			if (mAccumulator == '\n')
				mAccumulator = 0xd;
			else if (mAccumulator == 0x3)
				return false;
			//cout << "GOT " << hex << (int)mAccumulator << "\n";
			// Emulate RTS instruction
			uint8_t PC_l, PC_h;
			read(0x100 + (uint16_t)++mStackPointer, PC_l);
			read(0x100 + (uint16_t)++mStackPointer, PC_h);
			mProgramCounter = PC_h * 256 + PC_l + 1;
			//cout << "OSRDCH RTS => PC = 0x" << mProgramCounter << "\n";
		}
		else {
			mProgramCounter = calc_op_adr;
		}

		break;
	}

	case Codec6502::Instruction::JSR:
		// Jump to New Location Saving Return Address
		// Push PC+2; OP2:OP1 -> PC
		// The stack byte contains the program count high first, followed by program count low
		// N	Z	C	I	D	V
		// -	-	-	-	-	-
	{
		uint16_t PC_push_val = opcode_PC + 2;
		write(0x100 + (uint16_t)mStackPointer--, PC_push_val / 256);
		write(0x100 + (uint16_t)mStackPointer--, PC_push_val % 256);
		mProgramCounter = calc_op_adr;

		break;
	}

	case Codec6502::Instruction::LDA:
		// Load Accumulator with Memory
		// M -> A
		// N	Z	C	I	D	V
		// +	+	-	-	-	-
	{
		mAccumulator = read_val;
		setNZflags(mAccumulator);
		break;
	}

	case Codec6502::Instruction::LDX:
		// Load X with Memory
		// M -> X
		// N	Z	C	I	D	V
		// +	+	-	-	-	-
	{
		mRegisterX = read_val;
		setNZflags(mRegisterX);
		break;
	}

	case Codec6502::Instruction::LDY:
		// Load Y with Memory
		// M -> Y
		// N	Z	C	I	D	V
		// +	+	-	-	-	-
	{
		mRegisterY = read_val;
		setNZflags(mRegisterY);
		break;
	}

	case Codec6502::Instruction::LSR:
		// Shift One Bit Right (Memory or Accumulator)
		// 0 -> [76543210] -> C
		// N	Z	C	I	D	V
		// 0	+	+	-	-	-
	{
		mStatusRegister |= ((read_val & 0x1) ? C_set_mask : 0);
		val_8 = (read_val >> 1) & 0x7f;
		if (op_mem) {
			if (!write(calc_op_adr, val_8)) {
				return false;
			}
		}
		else
			mAccumulator = val_8;
		setNZflags(val_8);
		break;
	}
	case Codec6502::Instruction::NOP:
		// No Operation
		// N	Z	C	I	D	V
		// -	-	-	-	-	-
	{
		break;
	}
	case Codec6502::Instruction::ORA:
		// OR Memory with Accumulator
		// A OR M -> A
		// N	Z	C	I	D	V
		// +	+	-	-	-	-
	{
		mAccumulator = mAccumulator | read_val;
		setNZflags(mAccumulator);
		break;
	}

	case Codec6502::Instruction::PHA:
		// Push Accumulator on Stack
		// push A
		// N	Z	C	I	D	V
		// -	-	-	-	-	-
	{
		write(0x100 + (uint16_t)mStackPointer--, mAccumulator);
		break;
	}

	case Codec6502::Instruction::PHP:
		// Push Status on Stack
		// push SR
		// N	Z	C	I	D	V
		// -	-	-	-	-	-
	{
		write(0x100 + (uint16_t)mStackPointer--, mStatusRegister);
		break;
	}

	case Codec6502::Instruction::PLA:
		// Pull Accumulator from Stack
		// pull A
		// N	Z	C	I	D	V
		// +	+	-	-	-	-
	{
		read(0x100 + (uint16_t)++mStackPointer, mAccumulator);
		setNZflags(mAccumulator);
		break;
	}
	case Codec6502::Instruction::PLP:
		// Pull Status register from Stack
		// Pull SR (B flag and b5 ignored)
		// N	Z	C	I	D	V
		// +	+	+	+	+	+
	{
		uint8_t stack_val;
		read(0x100 + (uint16_t)++mStackPointer, stack_val);
		mStatusRegister &= N_flag & Z_flag & C_flag & I_flag & D_flag & V_flag;
		mStatusRegister |= (N_flag | Z_flag | C_flag | I_flag | D_flag | V_flag) & stack_val;
		break;
	}
	case Codec6502::Instruction::ROL:
		// Rotate One Bit Left (Memory or Accumulator)
		// C <- [76543210] <- C
		// N	Z	C	I	D	V
		// +	+	+	-	-	-
	{
		mStatusRegister |= ((read_val & 0x80)? C_set_mask : 0);
		val_8 = (read_val << 1) | C_flag;
		if (op_mem) {
			if (!write(calc_op_adr, val_8)) {
				return false;
			}
		}
		else
			mAccumulator = val_8;
		setNZflags(val_8);
		break;
	}

	case Codec6502::Instruction::ROR:
		// Rotate One Bit Right (Memory or Accumulator)
		// C -> [76543210] -> C
		// N	Z	C	I	D	V
		// +	+	+	-	-	-
	{
		mStatusRegister |= ((read_val & 0x1) ? C_set_mask : 0);
		val_8 = ((read_val >> 1) & 0x3f) | C_flag;
		if (op_mem) {
			if (!write(calc_op_adr, val_8)) {
				return false;
			}
		}
		else
			mAccumulator = val_8;
		setNZflags(val_8);
		break;
	}

	case Codec6502::Instruction::RTI:
		// Return from Interrupt
		// Pull SR and then pull PC
		// The low byte is pulled first.
		// N	Z	C	I	D	V
		// +	+	+	+	+	+
	{
		// Pull Status Register
		uint8_t stack_val;
		read(0x100 + (uint16_t)++mStackPointer, stack_val);
		mStatusRegister &= N_flag & Z_flag & C_flag & I_flag & D_flag & V_flag;
		mStatusRegister |= (N_flag | Z_flag | C_flag | I_flag | D_flag | V_flag) & stack_val;


		// Pull PC
		uint8_t PC_l, PC_h;
		read(0x100 + (uint16_t)++mStackPointer, PC_l);
		read(0x100 + (uint16_t)++mStackPointer, PC_h);
		mProgramCounter = PC_h * 256 + PC_l;

		break;
	}

	case Codec6502::Instruction::RTS:
		// Return from Subroutine
		// Pull PC, PC+1 -> PC
		// The low byte is pulled first .
		// N	Z	C	I	D	V
		// -	-	-	-	-	-

	{
		uint8_t PC_l, PC_h;
		read(0x100 + (uint16_t)++mStackPointer, PC_l);
		read(0x100 + (uint16_t)++mStackPointer, PC_h);
		mProgramCounter = PC_h * 256 + PC_l + 1;
		break;
	}

	case Codec6502::Instruction::SBC:
		// Subtract Memory from Accumulator with Borrow
		// If decimal flag is set, substraction will be BCD (Binary-Coded Decimal)
		// A - M - C* -> A
		// N	Z	C	I	D	V
		// +	+	+	-	-	+
		//
		// V is set if sign bit is incorrect
		// C is cleared if overflow in b7
	{
		{
			if (D_flag) {
				// Convert values to 2-complement numbers before addition
				uint8_t A_2c = (mAccumulator / 16) * 10 + mAccumulator & 16;
				uint8_t op_2c = (read_val / 16) * 10 + read_val & 16;
				uint16_t val_16_2c = (int8_t)A_2c - (int8_t)op_2c - (int8_t)(1-C_flag);
				setNZCVflags(val_16_2c);
				mAccumulator = (val_16_2c % 100) / 10 * 16 + (val_16_2c % 100) % 10;
			}
			else {
				val_16 = (int8_t)mAccumulator - (int8_t)read_val - (int8_t)(1-C_flag);// Subtract as 2-complement numbers to secure sign-extension
				mAccumulator = (uint8_t)(val_16 & 0xff);
				setNZCVflags(val_16);
				
			}
			mStatusRegister ^= C_set_mask; // invert carry as it was set based on overflow at addition
			break;
		}
	}

	case Codec6502::Instruction::SEC:
		// Set Carry Flag
		// N	Z	C	I	D	V
		// -	-	1	-	-	-
	{
		mStatusRegister |= C_set_mask;
		break;
	}

	case Codec6502::Instruction::SED:
		// Set Decimal Flag
		// N	Z	C	I	D	V
		// -	-	-	-	1	-
	{
		mStatusRegister |= D_set_mask;
		break;
	}
	case Codec6502::Instruction::SEI:
		// Set Interrupt Disable Status
		// N	Z	C	I	D	V
		// -	-	-	1	-	-
	{
		mStatusRegister |= I_set_mask;
		break;
	}

	case Codec6502::Instruction::STA:
		// Store Accumulator in Memory
		// A -> M
		// N	Z	C	I	D	V
		// -	-	-	-	-	-
	{
		write(calc_op_adr, mAccumulator);
		break;
	}

	case Codec6502::Instruction::STX:
		// Store X in Memory
		// X -> M
		// N	Z	C	I	D	V
		// -	-	-	-	-	-
	{
		write(calc_op_adr, mRegisterX);
		break;
	}

	case Codec6502::Instruction::STY:
		// Store Y in Memory
		// Y -> M
		// N	Z	C	I	D	V
		// -	-	-	-	-	-
	{
		write(calc_op_adr, mRegisterY);
		break;
	}

	case Codec6502::Instruction::TAX:
		// Transfer Accumulator to X
		// A -> X
		// N	Z	C	I	D	V
		// +	+	-	-	-	-

	{
		mRegisterX = mAccumulator;
		setNZflags(mRegisterX);
		break;
	}

	case Codec6502::Instruction::TAY:
		// Transfer Accumulator to Y
		// A -> Y
		// N	Z	C	I	D	V
		// +	+	-	-	-	-
	{
		mRegisterY = mAccumulator;
		setNZflags(mRegisterY);
		break;
	}

	case Codec6502::Instruction::TSX:
		// Transfer Stack Pointer to X
		// SP -> X
		// N	Z	C	I	D	V
		// +	+	-	-	-	-
	{
		mRegisterX = mStackPointer;
		setNZflags(mRegisterX);
		break;
	}

	case Codec6502::Instruction::TXA:
		// Transfer X to Accumulator
		// X -> A
		// N	Z	C	I	D	V
		// +	+	-	-	-	-
	{
		mAccumulator = mRegisterX;
		setNZflags(mAccumulator);
		break;
	}

	case Codec6502::Instruction::TXS:
		// Transfer X to Stack Register: X -> SP
		// X -> SP
		// N	Z	C	I	D	V
		// -	-	-	-	-	-
	{
		mStackPointer = mRegisterX;
		break;
	}

	case Codec6502::Instruction::TYA:
		// Transfer Y to Accumulator: Y -> A
		// Y -> A
		// N	Z	C	I	D	V
		// +	+	-	-	-	-
	{
		mAccumulator = mRegisterY;
		setNZflags(mAccumulator);
		break;
	}

	default:
		break;
	}

	return true;
}

//
// Emulate RESET sequence which should take 7 clock cycles
//
bool P6502::reset()
{
	if (mVerbose)
		cout << "RESET\n";

	mStatusRegister &= V_clr_mask;

	// Fetch RESET vector
	uint8_t adr_L, adr_H;
	if (!read(0xfffc, adr_L) || !read(0xfffd, adr_H))
		return false;

	mProgramCounter = adr_H * 256 + adr_L;

	// Increase time by 7 clock cycles for the RESET
	tick(7);

	return true;
}

//
//
//
bool P6502::read(uint16_t adr, uint8_t &data)
{
	
	for (int i = 0; i < mDevices.size(); i++) {
		Device* dev = mDevices[i];
		if (dev->selected(adr)) {
			bool success = dev->read(adr, data);
			if (mVerbose)
				cout << "READ 0x" << hex << adr << " => " << (int)data << "\n";
			return success;
		}
	}

	data = 0xff;
	return true;
}

//
//
//
bool P6502::write(uint16_t adr, uint8_t data)
{

	for (int i = 0; i < mDevices.size(); i++) {
		Device* dev = mDevices[i];
		if (dev->selected(adr)) {
			if (mVerbose)
				cout << "WRITE 0x" << hex << (int)data << " to 0x" << adr << "\n";
			return dev->write(adr, data);			
		}
	}
	return false;
}

// Stop run method()
void P6502::stop()
{
	mMutex.lock();

	mStop = true;

	mMutex.unlock();
}

bool P6502::getOperand(Codec6502::InstructionInfo instr, uint16_t& operand, uint16_t &calc_op_adr, uint8_t& read_val, bool& op_mem)
{
	operand = 0x0;
	calc_op_adr = 0x0;
	read_val = 0x0;
	op_mem = false;
	switch (instr.mode) {
	case Codec6502::Mode::Accumulator:
		// OPC mAccumulator
		// Read A
		read_val = mAccumulator;
		break;

	case Codec6502::Mode::Implied:
		// implicit (no operand)
		break;

	case Codec6502::Mode::Relative:
		// OPC <branch target>
		// Read relative branch address
	{
		uint8_t rel_a;
		if (!read(mProgramCounter, rel_a))
			return false;
		mProgramCounter++;
		operand = rel_a;
		calc_op_adr = (rel_a + mProgramCounter) & 0xffff;

		// Add one cycle if branch to other page
		if (instr.addCycleAtPageBoundary && pageBoundaryCrossed(mProgramCounter, calc_op_adr))
			tick();

		break;
	}

	case Codec6502::Mode::Immediate:
		// OPC #$12
		// Read value $12
	{
		uint8_t op8;
		if (!read(mProgramCounter, op8))
			return false;
		operand = op8;
		read_val = op8;
		mProgramCounter++;

		break;
	}

	case Codec6502::Mode::ZeroPage:
		// OPC $12
		// Read value Mem[$12]
	{
		op_mem = true;

		uint8_t zp_a;
		if (!read(mProgramCounter, zp_a))
			return false;
		operand = zp_a;
		calc_op_adr = zp_a;
		mProgramCounter++;

		if (instr.readsMem && !read((uint16_t) zp_a, read_val))
			return false;

		break;
	}

	case Codec6502::Mode::ZeroPage_X:
		// OPC $12,X
		// Read Mem[$12+X]
	{
		op_mem = true;

		uint8_t zp_a;
		if (!read(mProgramCounter, zp_a))
			return false;
		operand = zp_a;
		mProgramCounter++;

		uint8_t data;
		if (!read((uint16_t)zp_a, data))
			return false;

		calc_op_adr = zp_a;
		read_val = (uint16_t)zp_a + mRegisterX;
		if (instr.readsMem && !read(calc_op_adr, read_val))
			return false;

		break;
	}

	case Codec6502::Mode::ZeroPage_Y:
		// OPC $12,Y
		// Read Mem[$12+X]
	{
		op_mem = true;

		uint8_t zp_a;
		if (!read(mProgramCounter, zp_a))
			return false;
		operand = zp_a;
		mProgramCounter++;

		uint8_t data;
		if (!read((uint16_t)zp_a, data))
			return false;

		uint16_t mem_a = (uint16_t)zp_a;
		uint16_t calc_op_adr = mem_a + mRegisterY;

		if (instr.readsMem && !read(calc_op_adr, read_val))
			return false;


		// Add one cycle if access on other page
		if (instr.addCycleAtPageBoundary && pageBoundaryCrossed((uint16_t)zp_a, calc_op_adr))
			tick();

		break;
	}

	case Codec6502::Mode::Absolute:
		// OPC $1234
		// Read Mem[$1234]
	{
		op_mem = true;

		uint8_t a_L, a_H;
		if (!read(mProgramCounter, a_L))
			return false;
		mProgramCounter++;
		if (!read(mProgramCounter, a_H))
			return false;
		mProgramCounter++;

		operand = (uint16_t)a_H * 256 + a_L;
		calc_op_adr = operand;
		if (instr.readsMem && !read(calc_op_adr, read_val))
			return false;

		break;

	}

	case Codec6502::Mode::Absolute_X:
		// OPC $1234,X
		// Read Mem[$1234+X]
	{
		op_mem = true;

		uint8_t a_L, a_H;
		if (!read(mProgramCounter, a_L))
			return false;
		mProgramCounter++;
		if (!read(mProgramCounter, a_H))
			return false;
		mProgramCounter++;

		operand = (uint16_t)a_H * 256 + a_L;
		calc_op_adr = operand + mRegisterX;
		if (instr.readsMem && !read(calc_op_adr, read_val))
			return false;

		// Add one cycle if page boundary crossed
		if (instr.addCycleAtPageBoundary && pageBoundaryCrossed(calc_op_adr, operand))
			tick();

		break;
	}

	case Codec6502::Mode::Absolute_Y:
		// OPC $1234,Y
		// Read Mem[$1234+Y]
	{
		op_mem = true;

		uint8_t a_L, a_H;
		if (!read(mProgramCounter, a_L))
			return false;
		mProgramCounter++;
		if (!read(mProgramCounter, a_H))
			return false;
		mProgramCounter++;

		operand = (uint16_t)a_H * 256 + a_L;
		calc_op_adr = operand + mRegisterY;
		if (instr.readsMem && !read(calc_op_adr, read_val))
			return false;

		// Add one cycle if page boundary crossed
		if (instr.addCycleAtPageBoundary && pageBoundaryCrossed(calc_op_adr, operand))
			tick();

		break;
	}
	case Codec6502::Mode::Indirect:
		// OPC ($1234)
		// Read 16-bit little-endian word from Mem[Mem[$1234]]
	{
		op_mem = true;

		uint8_t a_L, a_H;
		if (!read(mProgramCounter, a_L))
			return false;
		mProgramCounter++;
		if (!read(mProgramCounter, a_H))
			return false;
		operand = (uint16_t)a_H * 256 + a_L;
		mProgramCounter++;

		uint16_t adr_i = (uint16_t) a_H * 256 + a_L;
		if (!read(adr_i, a_L))
			return false;
		if (!read(adr_i+1, a_H))
			return false;
		calc_op_adr = (uint16_t) a_H * 256 + a_L;
		if (instr.readsMem && !read(calc_op_adr, read_val))
			return false;

		break;
	}
	case Codec6502::Mode::Indirect_X:
		// OPC ($12,X)
		// Read from Mem[Mem[$12+X]]
	{
		op_mem = true;

		uint8_t zp_a;
		if (!read(mProgramCounter, zp_a))
			return false;
		operand = zp_a;
		mProgramCounter++;

		uint8_t data;
		if (!read((uint16_t)zp_a, data))
			return false;

		uint16_t mem_a = (uint16_t)zp_a + mRegisterX;
		uint8_t a_L, a_H;
		if (!read(mem_a, a_L) || !read(mem_a+1, a_H))
			return false;
		calc_op_adr = (uint16_t)a_H * 256 + a_L;
		if (instr.readsMem && !read(calc_op_adr, read_val))
			return false;

		break;
	}
	case Codec6502::Mode::Indirect_Y:
		// OPC ($12),Y
		// Read from Mem[Mem[$12]+Y]
	{
		op_mem = true;

		// Read zero page address (e.g. $12)
		uint8_t zp_a;
		if (!read(mProgramCounter, zp_a))
			return false;
		operand = zp_a;
		mProgramCounter++;

		// Read Indirect address -  e.g. ($12)
		uint8_t a_L, a_H;
		if (!read((uint16_t)zp_a, a_L) || !read((uint16_t)zp_a+1, a_H))
			return false;
		uint16_t mem_a = (uint16_t)a_H * 256 + a_L;
		calc_op_adr = mem_a + mRegisterY;
		if (instr.readsMem && !read(calc_op_adr, read_val))
			return false;

		// Add one cycle if page boundary crossed
		if (instr.addCycleAtPageBoundary && pageBoundaryCrossed(mem_a, calc_op_adr))
			tick();

		break;
	}
	default:
		return false;
		break;
	}

	return true;
}

string P6502::getState()
{
	stringstream sout;

	sout << hex << setfill('0') <<
		"A:" << setw(2) << (int) mAccumulator <<
		" X:" << setw(2) << (int) mRegisterX <<
		" Y:" << setw(2) << (int) mRegisterY <<
		" SP:" << setw(2) << (int) mStackPointer <<
		" NV-BDIZC:" << setw(8) << bitset<8>(mStatusRegister & 0xdf) <<
		setw(4) <<
		" PC:" << setw(2) << mProgramCounter;

	return sout.str();
}


bool P6502::pageBoundaryCrossed(uint16_t before, uint16_t after)
{
	return (((before >> 8) ^ (after >> 8)) != 0);
}

void P6502::tick(int cycles)
{
	//std::this_thread::sleep_for(std::chrono::nanoseconds(cycles * cPeriod));
}

void P6502::setNZflags(uint8_t val)
{
	mStatusRegister &= N_clr_mask & Z_clr_mask;

	// Zero flag (Z): Set if the result is zero
	if (val == 0)
		mStatusRegister |= Z_set_mask;

	// Negative flag (N): Set if the result is negative
	if (val & 0x80)
		mStatusRegister |= N_set_mask;
}


void P6502::setNZCflags(uint16_t val)
{
	setNZflags((uint8_t)(val & 0xff));

	// Carry flag (C)
	mStatusRegister &= C_clr_mask;
	if (val & 0x100)
		mStatusRegister |= C_set_mask;

}

void P6502::setNZCVflags(uint16_t val)
{
	setNZCflags(val);

	mStatusRegister &= V_clr_mask;

	// Overflow flag (V): Set if a carry occurred to b6 during an add operation; cleared if a borrow occurred to b6 in a subtract operation.
	int16_t v = val;
	if (v < -128 || v > 127)
		mStatusRegister |= V_set_mask;

}