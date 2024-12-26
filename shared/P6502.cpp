
#include "P6502.h"
#include <chrono>
#include <thread>
#include <iostream>

P6502::P6502(double clockSpeed, vector<Device*>& devices) : mDevices(devices)
{
	cPeriod = (int) round(1000 / clockSpeed);
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
		if (!read(pc, opcode)) {
			cont = false;
			std::cout << "Failed to read instruction!\n";
		}

		// Decode the opcode
		Codec6502::InstructionInfo instr;
		if (!mCodec.decodeInstruction(opcode, instr) && instr.type != Codec6502::InstructionType::Doc) {
			cont = false;
			std::cout << "Invalid instruction 0x" << hex << opcode << " encountered at address 0x" << hex << pc << "!\n";
		}

		// Get the operand

		// Execute the instruction
		switch (instr.instruction) {
		case Codec6502::Instruction::ADC:
		{
			// Add with Carry - impacts N Z V C
		}
		case Codec6502::Instruction::AND:
		case Codec6502::Instruction::ASL:
		case Codec6502::Instruction::BCC:
		case Codec6502::Instruction::BCS:
		case Codec6502::Instruction::BEQ:
		case Codec6502::Instruction::BIT:
		case Codec6502::Instruction::BMI:
		case Codec6502::Instruction::BNE:
		case Codec6502::Instruction::BPL:
		case Codec6502::Instruction::BRK:
		case Codec6502::Instruction::BVC:
		case Codec6502::Instruction::BVS:
		case Codec6502::Instruction::CLC:
		case Codec6502::Instruction::CLD:
		case Codec6502::Instruction::CLI:
		case Codec6502::Instruction::CLV:
		case Codec6502::Instruction::CMP:
		case Codec6502::Instruction::CPX:
		case Codec6502::Instruction::CPY:
		case Codec6502::Instruction::DEC:
		case Codec6502::Instruction::DEX:
		case Codec6502::Instruction::DEY:
		case Codec6502::Instruction::EOR:
		case Codec6502::Instruction::INC:
		case Codec6502::Instruction::INX:
		case Codec6502::Instruction::INY:
		case Codec6502::Instruction::JMP:
		case Codec6502::Instruction::JSR:
		case Codec6502::Instruction::LDA:
		case Codec6502::Instruction::LDX:
		case Codec6502::Instruction::LDY:
		case Codec6502::Instruction::LSR:
		case Codec6502::Instruction::NOP:
		case Codec6502::Instruction::ORA:
		case Codec6502::Instruction::PHA:
		case Codec6502::Instruction::PHP:
		case Codec6502::Instruction::PLA:
		case Codec6502::Instruction::PLP:
		case Codec6502::Instruction::ROL:
		case Codec6502::Instruction::ROR:
		case Codec6502::Instruction::RTI:
		case Codec6502::Instruction::RTS:
		case Codec6502::Instruction::SBC:
		case Codec6502::Instruction::SEC:
		case Codec6502::Instruction::SED:
		case Codec6502::Instruction::SEI:
		case Codec6502::Instruction::STA:
		case Codec6502::Instruction::STX:
		case Codec6502::Instruction::STY:
		case Codec6502::Instruction::TAX:
		case Codec6502::Instruction::TAY:
		case Codec6502::Instruction::TSX:
		case Codec6502::Instruction::TXA:
		case Codec6502::Instruction::TXS:
		case Codec6502::Instruction::TYA:
		default:
			break;
		}

		// Wait one clock period
		tick();
	}
}

//
// Emulate RESET sequence which should take 7 clock cycles
//
bool P6502::reset()
{
	// Five internal cycles
	for(int i = 0; i << 5; i++)
		tick();

	// Two cycles to fetch RESET vector
	uint8_t adr_L, adr_H;
	if (!read(0xffc, adr_L) || !read(0xffd, adr_H))
		return false;

	PC = adr_H * 256 + adr_L;

	return true;
}

//
//
//
bool P6502::read(uint16_t adr, uint8_t &data)
{
	tick();

	for (int i = 0; i < mDevices.size(); i++) {
		Device* dev = mDevices[i];
		if (dev->selected(adr)) {
			return dev->read(adr, data);
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
	tick();

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

bool P6502::getOperand(Codec6502::InstructionInfo instr, uint8_t &operand)
{
	operand = 0x0;
	switch (instr.mode) {
	case Codec6502::Mode::ac:		// OPC A
		operand = A;
		break;

	case Codec6502::Mode::ip:		// implicit (no operand)
		// cycles depends on actual instruction - BRK, RTI, RTS, PHP, CLC, PLP, SEC, PHA, CLI, PLA, SEI, DEY, TYA, TAY, CLV, INY, CLD, INX, SED, TXA, TXS, TAX, TSX, NOP
		TBD
		break;

	case Codec6502::Mode::re:		// OPC <branch target>
	{
		if (!read(PC, operand))
			return false;
		PC++;

		tick();

		// Add one cycle if branch to other page
		if ((PC ^ (PC+operand)) & 0x80)
			tick();

		break;
	}

	case Codec6502::Mode::im:		// OPC #$12
	{
		if (!read(PC, operand))
			return false;
		PC++;

		break;
	}

	case Codec6502::Mode::zp:		// OPC $12
	{
	
		uint8_t zp_a;
		if (!read(PC+1, zp_a))
			return false;
		PC++;

		if (!read((uint16_t) zp_a, operand))
			return false;

		break;
	}

	case Codec6502::Mode::zX:		// OPC $12,X
	{
		uint8_t zp_a;
		if (!read(PC+1, zp_a))
			return false;
		PC++;

		uint8_t data;
		if (!read((uint16_t)zp_a, data))
			return false;

		uint16_t mem_a = (uint16_t)zp_a + X;
		if (!read(mem_a, operand))
			return false;

		break;
	}

	case Codec6502::Mode::zY:		// OPC $12,Y
	{

		uint8_t zp_a;
		if (!read(PC+1, zp_a))
			return false;
		PC++;

		uint8_t data;
		if (!read((uint16_t)zp_a, data))
			return false;

		uint16_t mem_a = (uint16_t)zp_a + Y;
		if (!read(mem_a, operand))
			return false;

		break;
	}

	case Codec6502::Mode::ab:		// OPC $1234
	{
		PC++;
		uint8_t a_L, a_H;
		if (!read(PC, a_L))
			return false;
		PC++;
		if (!read(PC, a_H))
			return false;
		PC++;

		uint16_t adr = (uint16_t)a_H * 256 + a_L;
		if (!read(adr, operand))
			return false;

		break;

	}

	case Codec6502::Mode::aX:		// OPC $1234,X
	{
		PC++;
		uint8_t a_L, a_H;
		if (!read(PC, a_L))
			return false;
		PC++;
		if (!read(PC, a_H))
			return false;
		PC++;

		uint16_t adr = (uint16_t)a_H * 256 + a_L + X;
		if (!read(adr, operand))
			return false;

		// Add one cycle if page boundary crossed
		if ((((uint16_t)a_H * 256 + a_L) ^ adr) & 0x80)
			tick();

		break;
	}

	case Codec6502::Mode::aY:		// OPC $1234,Y
	{
		PC++;
		uint8_t a_L, a_H;
		if (!read(PC, a_L))
			return false;
		PC++;
		if (!read(PC, a_H))
			return false;
		PC++;

		uint16_t adr = (uint16_t)a_H * 256 + a_L + Y;
		if (!read(adr, operand))
			return false;

		// Add one cycle if page boundary crossed
		if ((((uint16_t)a_H * 256 + a_L) ^ adr) & 0x80)
			tick();

		break;
	}
	case Codec6502::Mode::id:		// OPC ($1234)
	{
		PC++;
		uint8_t a_L, a_H;
		if (!read(PC, a_L))
			return false;
		PC++;
		if (!read(PC, a_H))
			return false;
		PC++;

		uint16_t adr_i = (uint16_t) a_H * 256 + a_L;
		if (!read(adr_i, a_L))
			return false;
		if (!read(adr_i, a_H))
			return false;
		uint16_t adr = (uint16_t) a_H * 256 + a_L;

		if (!read(adr, operand))
			return false;

		break;
	}
	case Codec6502::Mode::iX:		// OPC ($12,X)
	{
		uint8_t zp_a;
		if (!read(PC + 1, zp_a))
			return false;
		PC++;

		uint8_t data;
		if (!read((uint16_t)zp_a, data))
			return false;

		uint16_t mem_a = (uint16_t)zp_a + X;
		uint8_t a_L, a_H;
		if (!read(mem_a, a_L) || !read(mem_a, a_H))
			return false;
		uint16_t adr_i = (uint16_t)a_H * 256 + a_L;
		if (!read(adr_i, operand))
			return false;

		break;
	}
	case Codec6502::Mode::iY:		// OPC ($12),Y
	{
		// Read zero page address (e.g. $12)
		uint8_t zp_a;
		if (!read(PC + 1, zp_a))
			return false;
		PC++;

		// Read indirect address -  e.g. ($12)
		uint16_t mem_a = (uint16_t)zp_a;
		uint8_t a_L, a_H;
		if (!read(mem_a, a_L) || !read(mem_a, a_H))
			return false;
		uint16_t adr_i = (uint16_t)a_H * 256 + a_L + Y;
		if (!read(adr_i, operand))
			return false;

		// Add one cycle if page boundary crossed
		if ((((uint16_t)a_H * 256 + a_L) ^ adr_i) & 0x80)
			tick();

		break;
	}
	default:
		return false;
		break;
	}

	return true;
}

void P6502::tick()
{
	std::this_thread::sleep_for(std::chrono::nanoseconds(cPeriod));
}