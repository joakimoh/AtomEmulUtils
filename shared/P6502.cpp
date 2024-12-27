
#include "P6502.h"
#include <chrono>
#include <thread>
#include <iostream>
#include <sstream>
#include <filesystem>
#include <bitset>

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
			std::cout << "Failed to get operand for instruction!\n";
		}

		// Execute the instruction
		if (!executeInstr(instr, operand, calc_op_adr, read_val, op_mem)) {
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

bool P6502::executeInstr(Codec6502::InstructionInfo instr, uint16_t operand, uint16_t calc_op_adr, uint8_t read_val, bool op_mem)
{
	uint16_t val_16;
	uint8_t val_8;
	switch (instr.instruction) {

	case Codec6502::Instruction::ADC:
		// Add Memory to Accumulator with Carry
		// If decimal flag is set, addition will be BCD (Binary-Coded Decimal)
	{
		if (getDecimalFlag()) {
			// Convert values to 2-complement numbers before addition
			uint8_t A_2c = (mAccumulator / 16) * 10 + mAccumulator % 16;
			uint8_t op_2c = (read_val / 16) * 10 + read_val % 16;
			uint16_t val_16_2c = (int8_t) A_2c + (int8_t)op_2c + (int8_t) getCarryFlag();
			setNZCVFlags(val_16_2c);
			mAccumulator = (val_16_2c % 100) / 10 * 16 + (val_16_2c % 100) % 10;
		}
		else {
			val_16 = (int8_t)mAccumulator + (int8_t)read_val + (int8_t)getCarryFlag(); // Add as 2-complement numbers to secure sign-extension
			mAccumulator = (uint8_t)(val_16 & 0xf);
			setNZCVFlags(val_16);
		}
		break;
	}

	case Codec6502::Instruction::AND:
		// AND Memory with Accumulator
	{
		mAccumulator = mAccumulator & read_val;
		setNZFlags(mAccumulator);
		break;
	}

	case Codec6502::Instruction::ASL:
		// Shift Left One Bit (Memory or Accumulator)
	{
		val_16 = (read_val << 1) & 0x1fe;
		if (op_mem) {
			if (!write(calc_op_adr, val_16 & 0xff)) {
				return false;
			}
		}
		else
			mAccumulator = val_16 & 0xff;
		setNZCFlags(val_16);
		break;
	}

	case Codec6502::Instruction::BCC:
		// Branch on Carry Clear
	{
		if (!getCarryFlag())
			mProgramCounter = (mProgramCounter + read_val) & 0xffff;
		break;
	}

	case Codec6502::Instruction::BCS:
		// Branch on Carry Set
	{
		if (getCarryFlag())
			mProgramCounter = (mProgramCounter + read_val) & 0xffff;
		break;
	}

	case Codec6502::Instruction::BEQ:
		// Branch on Result Zero
	{
		if (!getZeroFlag())
			mProgramCounter = (mProgramCounter + read_val) & 0xffff;
		break;
	}

	case Codec6502::Instruction::BIT:
	{
		// Test Bits in Memory with Accumulator
		{
			val_8 = mAccumulator & read_val;
			setZeroFlag(val_8 == 0 ? 1 : 0);
			setCarryFlag((read_val & 0x80) ? 1 : 0);
			setOverFlowFlag((read_val & 0x40) ? 1 : 0);
			break;
		}
	}

	case Codec6502::Instruction::BMI:
		// Branch on Result Minus
	{
		if (getNegativeFlag())
			mProgramCounter = (mProgramCounter + read_val) & 0xffff;
		break;
	}

	case Codec6502::Instruction::BNE:
		// Branch on Result not Zero
	{
		if (!getZeroFlag())
			mProgramCounter = (mProgramCounter + read_val) & 0xffff;
		break;
	}

	case Codec6502::Instruction::BPL:
		// Branch on Result Plus
	{
		if (!getNegativeFlag())
			mProgramCounter = (mProgramCounter + read_val) & 0xffff;
		break;
	}

	case Codec6502::Instruction::BRK:
	{
		// Force Break
		
		// Save PC & Status to stack
		write(0x100 + (uint16_t) mStackPointer--, mProgramCounter % 256);
		write(0x100 + (uint16_t)mStackPointer--, mProgramCounter / 256);
		write(0x100 + (uint16_t)mStackPointer--, mStatusRegister);

		// Fetch break vector
		uint8_t adr_L, adr_H;
		if (!read(0xffe, adr_L) || !read(0xfff, adr_H))
			return false;
		mProgramCounter = adr_H * 256 + adr_L;

		break;
	}

	case Codec6502::Instruction::BVC:
		// Branch on Overflow Clear
	{
		if (!getOverFlowFlag())
			mProgramCounter = (mProgramCounter + read_val) & 0xffff;
		break;
	}

	case Codec6502::Instruction::BVS:
		// Branch on Overflow Set
	{
		if (getOverFlowFlag())
			mProgramCounter = (mProgramCounter + read_val) & 0xffff;
		break;
	}

	case Codec6502::Instruction::CLC:
		// Clear Carry Flag
	{
		setCarryFlag(0);
		break;
	}

	case Codec6502::Instruction::CLD:
		// Clear Decimal Mode
	{
		setDecimalFlag(0);
		break;
	}

	case Codec6502::Instruction::CLI:
		// Clear Interrupt Disable Bit
	{
		setInterruptDisableFlag(0);
		break;
	}

	case Codec6502::Instruction::CLV:
		// Clear Overflow Flag
	{
		setOverFlowFlag(0);
		break;
	}

	case Codec6502::Instruction::CMP:
		// Compare Memory with Accumulator
	{
		
		val_16 = mAccumulator - read_val;
		setNZCFlags(val_16);
		break;
	}

	case Codec6502::Instruction::CPX:
		// Compare Memory and Index X
	{
		
		val_16 = mRegisterX - read_val;
		setNZCFlags(val_16);
		break;
	}

	case Codec6502::Instruction::CPY:
		// Compare Memory and Index Y
	{
		val_16 = mRegisterY - read_val;
		setNZCFlags(val_16);
		break;
	}

	case Codec6502::Instruction::DEC:
		// Decrement Memory by One
	{
		val_8 = read_val - 1;
		if (!write(calc_op_adr, val_8)) {
			return false;
		}
		setNZFlags(val_8);
		break;
	}

	case Codec6502::Instruction::DEX:
		// Decrement Index X by One
	{
		mRegisterX = mRegisterX - 1;
		setNZFlags(mRegisterX);
		break;
	}

	case Codec6502::Instruction::DEY:
		// Decrement Index Y by One
	{
		mRegisterX = mRegisterY - 1;
		setNZFlags(mRegisterX);
		break;
	}

	case Codec6502::Instruction::EOR:
		// Exclusive-OR Memory with Accumulator
	{
		mAccumulator = mAccumulator ^ read_val;
		setNZFlags(mAccumulator);
		break;
	}

	case Codec6502::Instruction::INC:
		// Increment Memory by One
	{
		val_8 = read_val + 1;
		if (!write(calc_op_adr, val_8)) {
			return false;
		}
		setNZFlags(val_8);
		break;
	}

	case Codec6502::Instruction::INX:
		// Increment X by One
	{
		mRegisterX = mRegisterX + 1;
		setNZFlags(mRegisterX);
		break;
	}

	case Codec6502::Instruction::INY:
		// Increment Y by One
	{
		mRegisterX = mRegisterY + 1;
		setNZFlags(mRegisterX);
		break;
	}

	case Codec6502::Instruction::JMP:
		// Jump to New Location
	{
		mProgramCounter = calc_op_adr;
		break;
	}

	case Codec6502::Instruction::JSR:
		// Jump to New Location Saving Return Address
	{
		write(0x100 + (uint16_t) mStackPointer--, mProgramCounter % 256);
		write(0x100 + (uint16_t)mStackPointer--, mProgramCounter / 256);
		mProgramCounter = calc_op_adr;
		break;
	}

	case Codec6502::Instruction::LDA:
		// Load Accumulator with Memory
	{
		mAccumulator = read_val;
		setNZFlags(mAccumulator);
		break;
	}

	case Codec6502::Instruction::LDX:
		// Load X with Memory
	{
		mRegisterX = read_val;
		setNZFlags(mRegisterX);
		break;
	}

	case Codec6502::Instruction::LDY:
		// Load Y with Memory
	{
		mRegisterY = read_val;
		setNZFlags(mRegisterY);
		break;
	}

	case Codec6502::Instruction::LSR:
		// Shift One Bit Right (Memory or Accumulator)
	{
		setCarryFlag(read_val & 0x1);
		val_8 = (read_val >> 1) & 0xff;
		if (op_mem) {
			if (!write(calc_op_adr, val_8)) {
				return false;
			}
		}
		else
			mAccumulator = val_8;
		setNZFlags(val_8);
		break;
	}
	case Codec6502::Instruction::NOP:
		// No Operation
	{
		break;
	}
	case Codec6502::Instruction::ORA:
		// OR Memory with Accumulator
	{
		mAccumulator = mAccumulator | read_val;
		setNZFlags(mAccumulator);
		break;
	}

	case Codec6502::Instruction::PHA:
		// Push Accumulator on Stack
	{
		write(0x100 + (uint16_t)mStackPointer--, mAccumulator);
		break;
	}

	case Codec6502::Instruction::PHP:
		// Push Status on Stack
	{
		write(0x100 + (uint16_t)mStackPointer--, mStatusRegister);
		break;
	}

	case Codec6502::Instruction::PLA:
		// Pull Accumulator from Stack
	{
		read(0x100 + (uint16_t)++mStackPointer, mAccumulator);
		break;
	}
	case Codec6502::Instruction::PLP:
		// Pull Status register from Stack
	{
		read(0x100 + (uint16_t)++mStackPointer, mStatusRegister);
		break;
	}
	case Codec6502::Instruction::ROL:
		// Rotate One Bit Left (Memory or Accumulator): C <- [76543210] <- C
	{
		setCarryFlag(read_val & 0x80);
		val_8 = (read_val << 1) | getCarryFlag();
		if (op_mem) {
			if (!write(calc_op_adr, val_8)) {
				return false;
			}
		}
		else
			mAccumulator = val_8;
		setNZFlags(val_8);
		break;
	}

	case Codec6502::Instruction::ROR:
		// Rotate One Bit Right (Memory or Accumulator): C -> [76543210] -> C
	{
		setCarryFlag(read_val & 0x1);
		val_8 = ((read_val >> 1) & 0x3f) | (getCarryFlag() << 7);
		if (op_mem) {
			if (!write(calc_op_adr, val_8)) {
				return false;
			}
		}
		else
			mAccumulator = val_8;
		setNZFlags(val_8);
		break;
	}

	case Codec6502::Instruction::RTI:
		// Return from Interrupt
	{
		uint8_t status;
		read(0x100 + (uint16_t)++mStackPointer, status);
		mStatusRegister = (status & B_inv_mask);
		uint8_t PC_l, PC_h;
		read(0x100 + (uint16_t)++mStackPointer, PC_l);
		read(0x100 + (uint16_t)++mStackPointer, PC_h);
		mProgramCounter = PC_h * 256 + PC_l;

		break;
	}

	case Codec6502::Instruction::RTS:
	{
		uint8_t PC_l, PC_h;
		read(0x100 + (uint16_t)++mStackPointer, PC_l);
		read(0x100 + (uint16_t)++mStackPointer, PC_h);
		mProgramCounter = PC_h * 256 + PC_l;
		break;
	}

	case Codec6502::Instruction::SBC:
		// Subtract Memory from Accumulator with Borrow: A - M - C* -> A
		// If decimal flag is set, substraction will be BCD (Binary-Coded Decimal)
	{
		{
			if (getDecimalFlag()) {
				// Convert values to 2-complement numbers before addition
				uint8_t A_2c = (mAccumulator / 16) * 10 + mAccumulator % 16;
				uint8_t op_2c = (read_val / 16) * 10 + read_val % 16;
				uint16_t val_16_2c = (int8_t) A_2c - (int8_t) op_2c - (int8_t) getCarryFlag();
				setNZCVFlags(val_16_2c);
				mAccumulator = (val_16_2c % 100) / 10 * 16 + (val_16_2c % 100) % 10;
			}
			else {
				val_16 = (int8_t)mAccumulator - (int8_t)read_val - (int8_t)getCarryFlag();// Subtract as 2-complement numbers to secure sign-extension
				mAccumulator = (uint8_t)(val_16 & 0xff);
				setNZCVFlags(val_16);
			}
			break;
		}
	}

	case Codec6502::Instruction::SEC:
		// Set Carry Flag
	{
		setCarryFlag(1);
		break;
	}

	case Codec6502::Instruction::SED:
		// Set Decimal Flag
	{
		setDecimalFlag(1);
		break;
	}
	case Codec6502::Instruction::SEI:
		// Set Interrupt Disable Status
	{
		setInterruptDisableFlag(1);
		break;
	}

	case Codec6502::Instruction::STA:
		// Store Accumulator in Memory: A -> M
	{
		write(calc_op_adr, mAccumulator);
		break;
	}

	case Codec6502::Instruction::STX:
		// Store Index X in Memory
	{
		write(calc_op_adr, mRegisterX);
		break;
	}

	case Codec6502::Instruction::STY:
		// Store Index Y in Memory
	{
		write(calc_op_adr, mRegisterY);
		break;
	}

	case Codec6502::Instruction::TAX:
		// Transfer Accumulator to Index X
	{
		mRegisterX = mAccumulator;
		setNZFlags(mRegisterX);
		break;
	}

	case Codec6502::Instruction::TAY:
		// Transfer Accumulator to Index Y
	{
		mRegisterY = mAccumulator;
		setNZFlags(mRegisterY);
		break;
	}

	case Codec6502::Instruction::TSX:
		// Transfer Stack Pointer to Index X
	{
		mRegisterX = mStackPointer;
		setNZFlags(mRegisterX);
		break;
	}

	case Codec6502::Instruction::TXA:
		// Transfer Index X to Accumulator
	{
		mAccumulator = mRegisterX;
		setNZFlags(mAccumulator);
		break;
	}

	case Codec6502::Instruction::TXS:
		// Transfer Index X to Stack Register: X -> SP
	{
		mStackPointer = mRegisterX;
		break;
	}

	case Codec6502::Instruction::TYA:
		// Transfer Index Y to Accumulator: Y -> A
	{
		mAccumulator = mRegisterY;
		setNZFlags(mAccumulator);
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

	mStackPointer = 0xff;

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

	data = 0x0;
	return false;
}

//
//
//
bool P6502::write(uint16_t adr, uint8_t data)
{

	for (int i = 0; i < mDevices.size(); i++) {
		Device* dev = mDevices[i];
		if (dev->selected(adr)) {
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

bool P6502::getOperand(Codec6502::InstructionInfo instr, uint16_t& operand, uint16_t calc_op_adr, uint8_t& read_val, bool& op_mem)
{
	operand = 0x0;
	calc_op_adr = 0x0;
	read_val = 0x0;
	op_mem = false;
	switch (instr.mode) {
	case Codec6502::Mode::Accumulator:		// OPC mAccumulator
		read_val = mAccumulator;
		break;

	case Codec6502::Mode::Implied:		// implicit (no operand)
		break;

	case Codec6502::Mode::Relative:		// OPC <branch target>
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

	case Codec6502::Mode::Immediate:		// OPC #$12
	{
		uint8_t op8;
		if (!read(mProgramCounter, op8))
			return false;
		operand = op8;
		mProgramCounter++;

		break;
	}

	case Codec6502::Mode::ZeroPage:		// OPC $12
	{
		op_mem = true;

		uint8_t zp_a;
		if (!read(mProgramCounter, zp_a))
			return false;
		operand = zp_a;
		calc_op_adr = zp_a;
		mProgramCounter++;

		if (!read((uint16_t) zp_a, read_val))
			return false;

		break;
	}

	case Codec6502::Mode::ZeroPage_X:		// OPC $12,mRegisterX
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

		operand = zp_a;
		read_val = (uint16_t)zp_a + mRegisterX;
		if (!read(operand, read_val))
			return false;

		break;
	}

	case Codec6502::Mode::ZeroPage_Y:		// OPC $12,mRegisterY
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
		uint16_t operand = mem_a + mRegisterY;

		if (!read(operand, read_val))
			return false;


		// Add one cycle if access on other page
		if (instr.addCycleAtPageBoundary && pageBoundaryCrossed((uint16_t)zp_a, operand))
			tick();

		break;
	}

	case Codec6502::Mode::Absolute:		// OPC $1234
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
		if (!read(operand, read_val))
			return false;

		break;

	}

	case Codec6502::Mode::Absolute_X:		// OPC $1234,X
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
		if (!read(operand, read_val))
			return false;


		// Add one cycle if page boundary crossed
		if (instr.addCycleAtPageBoundary && pageBoundaryCrossed(calc_op_adr, operand))
			tick();

		break;
	}

	case Codec6502::Mode::Absolute_Y:		// OPC $1234,Y
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
		if (!read(operand, read_val))
			return false;

		// Add one cycle if page boundary crossed
		if (instr.addCycleAtPageBoundary && pageBoundaryCrossed(calc_op_adr, operand))
			tick();

		break;
	}
	case Codec6502::Mode::Indirect:		// OPC ($1234)
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
		if (!read(adr_i, a_H))
			return false;
		calc_op_adr = (uint16_t) a_H * 256 + a_L;
		if (!read(operand, read_val))
			return false;

		break;
	}
	case Codec6502::Mode::Indirect_X:		// OPC ($12,X)
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
		if (!read(mem_a, a_L) || !read(mem_a, a_H))
			return false;
		calc_op_adr = (uint16_t)a_H * 256 + a_L;
		if (!read(operand, read_val))
			return false;

		break;
	}
	case Codec6502::Mode::Indirect_Y:		// OPC ($12),Y
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
		if (!read(operand, read_val))
			return false;

		// Add one cycle if page boundary crossed
		if (instr.addCycleAtPageBoundary && pageBoundaryCrossed(mem_a, operand))
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

	sout << hex << setfill('0') << setw(2) <<
		"A:" << (int) mAccumulator <<
		" X:" << (int) mRegisterX <<
		" Y:" << (int) mRegisterY <<
		" SP:" << (int) mStackPointer <<
		" NV-BDIZC:" << bitset<8>(mStatusRegister & 0xdf) <<
		setw(4) <<
		" PC:" << mProgramCounter;

	return sout.str();
}


bool P6502::pageBoundaryCrossed(uint16_t before, uint16_t after)
{
	return (((before >> 8) ^ (after >> 8)) != 0);
}

void P6502::tick(int cycles)
{
	std::this_thread::sleep_for(std::chrono::nanoseconds(cycles * cPeriod));
}

void P6502::setNZFlags(uint8_t val)
{
	setZeroFlag(0);
	setNegativeFlag(0);

	// Zero flag (Z): Set if the result is zero
	if (val == 0)
		setZeroFlag(1);

	// Negative flag (N): Set of the result is negative
	if (val & 0x80)
		setNegativeFlag(1);
}

void P6502::setNZCFlags(uint16_t val)
{
	setNZFlags((uint8_t) (val & 0xff));

	// Carry flag (C)
	setCarryFlag(0);
	if (val & 0x100)
		setCarryFlag(1);

}

void P6502::setNZCVFlags(uint16_t val)
{

	setOverFlowFlag(0);

	// Overflow flag (V): Set if a carry occurred to b6 during an add operation; cleared if a borrow occurred to b6 in a subtract operation.
	int16_t v = val;
	if (v < -128 || v > 127)
		setOverFlowFlag(1);

	setNZCFlags(val);

}