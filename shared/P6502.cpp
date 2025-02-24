
#include "P6502.h"
#include "MemoryMappedDevice.h"
#include <chrono>
#include <thread>
#include <iostream>
#include <sstream>
#include <filesystem>
#include <bitset>
#include <cmath>


P6502::P6502(string name, double clockSpeed, DebugInfo debugInfo, ConnectionManager *connectionManager):
	Device(name, P6502_DEV, MICROROCESSOR_DEVICE, debugInfo, connectionManager)
{
	cPeriod = (int) round(1000 / clockSpeed);

	// Specify ports that can be connected to other devices
	registerPort("RESET", IN_PORT, 0x01, RESET, &mRESET);
	registerPort("IRQ", IN_PORT, 0x01, RESET, &mIRQ);
	registerPort("NMI", IN_PORT, 0x01, RESET, &mNMI);


}

P6502::~P6502()
{
	for (int i = 0; i < mDevices.size(); i++)
		delete mDevices[i];
}

bool P6502::serveNMI()
{
	
	// Disable IRQ interrupts
	mStatusRegister |= I_set_mask;

	// Save PC & Status to stack
	uint16_t PC_push_val = mProgramCounter + 2;
	writeDevice(0x100 + (uint16_t)mStackPointer--, PC_push_val / 256);
	writeDevice(0x100 + (uint16_t)mStackPointer--, PC_push_val % 256);
	uint8_t SR_push_val = mStatusRegister | B_set_mask;
	writeDevice(0x100 + (uint16_t)mStackPointer--, SR_push_val);

	// Fetch break vector
	uint8_t adr_L, adr_H;
	if (!readProgramMem(0xfffa, adr_L) || !readProgramMem(0xfffb, adr_H))
		return false;
	mProgramCounter = adr_H * 256 + adr_L;
	mStatusRegister |= I_set_mask;
	return true;
	return true;
}

bool P6502::serveIRQ()
{

	if (mStatusRegister & I_set_mask)
		return false;

	

	// Disable IRQ interrupts
	mStatusRegister |= I_set_mask;

	// Save PC & Status to stack
	uint16_t PC_push_val = mProgramCounter + 2;
	writeDevice(0x100 + (uint16_t)mStackPointer--, PC_push_val / 256);
	writeDevice(0x100 + (uint16_t)mStackPointer--, PC_push_val % 256);
	uint8_t SR_push_val = mStatusRegister;
	writeDevice(0x100 + (uint16_t)mStackPointer--, SR_push_val);

	// Fetch break vector
	uint8_t adr_L, adr_H;
	if (!readProgramMem(0xfffe, adr_L) || !readProgramMem(0xffff, adr_H))
		return false;
	mProgramCounter = adr_H * 256 + adr_L;

	//cout << "SERVING IRQ -  handler at 0x" << hex << mProgramCounter << " will be executed!\n";
	

	return true;
}

//
// Emulate RESET sequence which should take 7 clock cycles
//
bool P6502::reset()
{
	Device::reset();

	mLastPgmDevice = NULL;

	mCycleCount = 0;

	// Fetch RESET vector
	uint8_t adr_L, adr_H;
	if (!readProgramMem(0xfffc, adr_L) || !readProgramMem(0xfffd, adr_H))
		return false;

	//out << "RESET succesful!\n";

	mProgramCounter = adr_H * 256 + adr_L;

	// Increase time by 7 clock cycles for the RESET
	tick(7);

	return true;
}

// 
// Advance until stop cycle has been reached
bool P6502::advance(uint64_t stopCycle)
{

	while (mCycleCount < stopCycle) {
		if (!advanceInstr(stopCycle))
			return false;
	}

	return true;
}


// Advance one instruction if the stop cycle hasn't already been reached
bool P6502::advanceInstr(uint64_t& endCycle)
{
	bool success = true;

	// Serve RESET, NMI & IRQ in priority order
	if (!mRESET) {
		reset();
		// No meaning to continue execution before RESET line becomes HIGH again
		endCycle = mCycleCount;
		return true;
	}
	else if (!mNMI && pNMI)	// NMI is edge-triggered!
		serveNMI();
	else if (!mIRQ)	// IRQ is level-triggered!
		serveIRQ();

	if ((true || (mDebugInfo.dbgLevel & DBG_6502)) && mIRQ != pIRQ) {
		cout << "IRQ => " << dec << (int)mIRQ << ", I flag = " << (int)I_flag << dec << "\n";
		if (!mIRQ && !I_flag)
			mDebugInfo.dbgLevel = DBG_6502;
	}

	if ((mDebugInfo.dbgLevel & DBG_6502) && mNMI != pNMI)
		cout << "NMI => " << dec << (int)mNMI << "\n";

	pNMI = mNMI;
	pIRQ = mIRQ;


	if (mDebugInfo.traceAdr > 0) {
		if (mProgramCounter == mDebugInfo.traceAdr && !mStopDebugBuffering) {
			mDebugInfo.dbgLevel |= DBG_6502;
			mTraceCount = 0;
			for (int i = 0; i < mBufferedTraceLines.size(); i++)
				cout << mBufferedTraceLines[i];
			mBufferedTraceLines.clear();
			mStopDebugBuffering = true;
		}
		if (mStopDebugBuffering && mTraceCount > mDebugInfo.postTraceLen) {
			mDebugInfo.dbgLevel &= ~DBG_6502;
			mEndOfTracingReached = true;
		}
		mTraceCount++;
	}

	if (mDebugInfo.stopAdr > 0 && mProgramCounter == mDebugInfo.stopAdr) {
		std::cout << "Execution stop triggered!\n";
		if (mDebugInfo.dumpAdr > 0) {
			// Output pre post tracing
			for (int a = mDebugInfo.dumpAdr; a < mDebugInfo.dumpAdr + mDebugInfo.dumpSz; a++) {
				uint8_t d;
				readProgramMem(a, d);
				std::cout << "0x" << hex << setw(4) << setfill('0') << a <<
					" 0x" << setw(2) << (int) d << "\n";
			}
		}
	}

	// Get opcode of next instruction
	uint8_t opcode;
	uint16_t opcode_PC = mProgramCounter;
	if (!readProgramMem(mProgramCounter++, opcode)) {
		success = false;
		if (mDebugInfo.dbgLevel & DBG_ERROR)
			std::cout << "Failed to read instruction!\n";
	}

	// Decode the opcode
	Codec6502::InstructionInfo instr;
	if (!mCodec.decodeInstruction(opcode, instr)) {
		success = false;
		if (mDebugInfo.dbgLevel & DBG_ERROR)
			std::cout << "Invalid instruction 0x" << hex << (int) opcode << " encountered at address 0x" << hex << opcode_PC << "!\n";
	}

	// Get the operand
	uint16_t operand = 0x0;
	uint16_t calc_op_adr = 0x0;
	uint8_t read_val = 0x0;
	if (!getOperand(instr, operand, calc_op_adr, read_val)) {
		success = false;
		if (mDebugInfo.dbgLevel & DBG_ERROR)
			std::cout << "Failed to get operand for instruction 0x" << hex << (int)opcode << " at address 0x" << opcode_PC << "!\n";
	}

	// Execute the instruction
	uint8_t written_val;
	if (!executeInstr(instr, opcode_PC, operand, calc_op_adr, read_val, written_val)) {
		success = false;
		if (mDebugInfo.dbgLevel & DBG_ERROR)
			std::cout << "Failed to execute instruction!\n";
	}

	if ((mDebugInfo.dbgLevel & DBG_6502)  || (mDebugInfo.traceAdr > 0 && !mEndOfTracingReached)) {
		string instr_s = mCodec.decode(opcode_PC, opcode, operand);
		stringstream sout;
		sout << setfill(' ') << setw(30) << left << instr_s << right <<
			" " << getState();
		if (instr.readsMem || instr.writesMem)
			sout << " ACCESSED 0x" << hex << setfill('0') << setw(4) << calc_op_adr;
		if (instr.readsMem)
			sout << " READ 0x" << setw(2) << (int)read_val;
		if (instr.writesMem)
			sout << " WROTE 0x" << setw(2) << (int)written_val;
		sout << "\n";
		if (mDebugInfo.dbgLevel & DBG_6502)
			cout << sout.str();
		if (mDebugInfo.traceAdr > 0 && !mEndOfTracingReached && !mStopDebugBuffering) {
			mBufferedTraceLines.push_back(sout.str());
			if (mBufferedTraceLines.size() > mDebugInfo.preTraceLen)
				mBufferedTraceLines.erase(mBufferedTraceLines.begin(), mBufferedTraceLines.end() - mDebugInfo.preTraceLen);

		}
	}

		
	// Increase time by the no of clock cycles specified for the instruction and mode.
	// This excludes extra cycle at page boundary as this is instead address in getOperand().
	tick(instr.cycles);

	// Return time reached
	endCycle = mCycleCount;

	return success;
}

bool P6502::executeInstr(
	Codec6502::InstructionInfo instr, uint16_t opcode_PC, uint16_t operand, uint16_t calc_op_adr, uint8_t read_val, uint8_t& written_val
	)
{
	uint8_t val_8_u;
	written_val = 0x0;

	switch (instr.instruction) {

	case Codec6502::Instruction::ADC:
		// Add Memory to Accumulator with Carry
		// If decimal flag is set, addition will be BCD (Binary-Coded Decimal) and only the C flag will have a useful value
		// A + M + C -> A, C
		// N	Z	C	I	D	V
		// +	+	+	-	-	+
	{
		int16_t val_C, val_V;
		if (D_flag) {

			
			// Calculate zero - flag set as for non-BDC addition
			int16_t val_Z = mAcc + read_val + C_flag;
			int8_t set_Z = ((val_Z & 0xff) == 0);

			// BCD Addition and calculation of carry flag
			uint8_t low_digit = (mAcc & 0xf) + (read_val & 0xf) + C_flag;
			uint8_t carry = 0;
			if (low_digit >= 0xa) {
				low_digit += 0x6; // same as subtraction by 10 (modulo 16)
				carry = 0x10;
			}
			low_digit &= 0xf;
			int16_t val_NV = (int8_t)(mAcc & 0xf0) + (int8_t)(read_val & 0xf0) + carry + low_digit; // save for later use when setting N & V flags
			uint16_t high_digit = (mAcc & 0xf0) + (read_val & 0xf0) + carry; // result could be up to 0x130 (0x90+0x90+0x10)
			int8_t set_C  = 0;
			if (high_digit >= 0xa0) {
				high_digit += 0x60; // same as subtraction by 10 (modulo 16)
				set_C = 1;
			}
			high_digit &= 0xf0;
			mAcc = high_digit | low_digit;

			// Calculate N & V flags - they are based on the sum before adjusting for overflow of the high digit
			setNZCVflags((val_NV & 0x80) !=0, set_Z, set_C, val_NV < -128 || val_NV > 127);
		}
		else {
			val_V = (int8_t)mAcc + (int8_t)read_val + C_flag; // Add as signed no to determine overflow (V)
			val_C = mAcc + read_val + C_flag; // Add as unsigned no to determine carry (C)
			mAcc = val_C & 0xff;
			setNZCVflags((mAcc & 0x80) != 0, mAcc == 0, (val_C & 0x100) != 0, val_V < -128 || val_V > 127);
		}
		
		break;
	}

	case Codec6502::Instruction::AND:
		// AND Memory with Accumulator
		// A AND M -> A
		// N	Z	C	I	D	V
		// +	+	-	-	-	-
	{
		mAcc &= read_val;
		setNZflags(mAcc);
		break;
	}

	case Codec6502::Instruction::ASL:
		// Shift Left One Bit (Memory or Accumulator)
		// C < [76543210] <- 0
		// N	Z	C	I	D	V
		// +	+	+	-	-	-
	{
		mStatusRegister &= ~C_set_mask;
		mStatusRegister |= ((read_val & 0x80) != 0? C_set_mask : 0);
		val_8_u = (read_val << 1) & 0xfe;
		if (instr.writesMem) {
			if (!writeDevice(calc_op_adr, read_val)) { // dummy write always made by NMOS 6502
				return false;
			}
			if (!writeDevice(calc_op_adr, val_8_u)) {
				return false;
			}
			written_val = val_8_u;
		}
		else
			mAcc = val_8_u;
		setNZflags(val_8_u);
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
			val_8_u = mAcc & read_val;
			mStatusRegister &= ~(Z_set_mask | N_set_mask | V_set_mask);
			if (val_8_u == 0)
				mStatusRegister |= Z_set_mask;
			if ((read_val & 0x80) != 0)
				mStatusRegister |= N_set_mask;
			if ((read_val & 0x40) != 0)
				mStatusRegister |= V_set_mask;
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
		// push PC+2, push SR (together with a set b4 = B)
		// The low PC byte is pushed first (so that the PC is stored in little endian format in the memory)
		// N	Z	C	I	D	V
		// -	-	-	1	-	-
	{
		// Force Break
		
		// Save PC & Status to stack
		uint16_t PC_push_val = opcode_PC + 2;
		writeDevice(0x100 + (uint16_t) mStackPointer--, PC_push_val / 256);
		writeDevice(0x100 + (uint16_t)mStackPointer--, PC_push_val % 256);
		uint8_t SR_push_val = mStatusRegister | B_set_mask;
		writeDevice(0x100 + (uint16_t)mStackPointer--, SR_push_val);

		// Fetch break vector
		uint8_t adr_L, adr_H;
		if (!readProgramMem(0xfffe, adr_L) || !readProgramMem(0xffff, adr_H))
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
		mStatusRegister &= ~C_set_mask;
		break;
	}

	case Codec6502::Instruction::CLD:
		// Clear Decimal Mode
		// N	Z	C	I	D	V
		// -	-	-	-	0	-
	{
		mStatusRegister &= ~D_set_mask;
		break;
	}

	case Codec6502::Instruction::CLI:
		// Clear Interrupt Disable Bit
		// N	Z	C	I	D	V
		// -	-	-	0	-	-
	{
		mStatusRegister &= ~I_set_mask;
		break;
	}

	case Codec6502::Instruction::CLV:
		// Clear Overflow Flag
		// N	Z	C	I	D	V
		// -	-	-	-	-	0
	{
		mStatusRegister &= ~V_set_mask;
		break;
	}

	case Codec6502::Instruction::CMP:
		// Compare Memory with Accumulator
		// A - M
		// N	Z	C	I	D	V
		// +	+	+	-	-	-

	{
		
		val_8_u = mAcc - read_val;
		setNZCflags(val_8_u, mAcc >= read_val);
		break;
	}

	case Codec6502::Instruction::CPX:
		// Compare Memory and X
		// X - M
		// N	Z	C	I	D	V
		// +	+	+	-	-	-
	{
		
		val_8_u = mRegisterX - read_val;
		setNZCflags(val_8_u, mRegisterX >= read_val);
		break;
	}

	case Codec6502::Instruction::CPY:
		// Compare Memory and Y
		// Y - M
		// N	Z	C	I	D	V
		// +	+	+	-	-	-
	{
		val_8_u = mRegisterY - read_val;
		setNZCflags(val_8_u, mRegisterY >= read_val);
		break;
	}

	case Codec6502::Instruction::DEC:
		// Decrement Memory by One
		// M - 1 -> M
		// N	Z	C	I	D	V
		// +	+	-	-	-	-
	{
		val_8_u = read_val - 1;
		if (!writeDevice(calc_op_adr, read_val)) { // dummy write always made by NMOS 6502
			return false;
		}
		if (!writeDevice(calc_op_adr, val_8_u)) {
			return false;
		}
		written_val = val_8_u;
		setNZflags(val_8_u);
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
		setNZflags(mRegisterY);
		break;
	}

	case Codec6502::Instruction::EOR:
		// Exclusive-OR Memory with Accumulator
		// A EOR M -> A
		// N	Z	C	I	D	V
		// +	+	-	-	-	-

	{
		mAcc ^= read_val;
		setNZflags(mAcc);
		break;
	}

	case Codec6502::Instruction::INC:
		// Increment Memory by One
		// M + 1 -> M
		// N	Z	C	I	D	V
		// +	+	-	-	-	-
	{
		val_8_u = read_val + 1;
		if (!writeDevice(calc_op_adr, read_val)) { // dummy write always made by NMOS 6502
			return false;
		}
		if (!writeDevice(calc_op_adr, val_8_u)) {
			return false;
		}
		written_val = val_8_u;
		setNZflags(val_8_u);
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
		setNZflags(mRegisterY);
		break;
	}

	case Codec6502::Instruction::JMP:
		// Jump to New Location
		// OP2:OP1 -> PC
		// N	Z	C	I	D	V
		// -	-	-	-	-	-
	{
		
		mProgramCounter = calc_op_adr;

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
		writeDevice(0x100 + (uint16_t)mStackPointer--, PC_push_val / 256);
		writeDevice(0x100 + (uint16_t)mStackPointer--, PC_push_val % 256);
		mProgramCounter = calc_op_adr;

		break;
	}

	case Codec6502::Instruction::LDA:
		// Load Accumulator with Memory
		// M -> A
		// N	Z	C	I	D	V
		// +	+	-	-	-	-
	{
		mAcc = read_val;
		setNZflags(mAcc);
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
		mStatusRegister &= ~C_set_mask;
		mStatusRegister |= ((read_val & 0x1) != 0 ? C_set_mask : 0);
		val_8_u = (read_val >> 1) & 0x7f;
		if (instr.writesMem) {
			if (!writeDevice(calc_op_adr, read_val)) { // dummy write always made by NMOS 6502
				return false;
			}
			if (!writeDevice(calc_op_adr, val_8_u)) {
				return false;
				written_val = val_8_u;
			}
		}
		else
			mAcc = val_8_u;
		setNZflags(val_8_u);
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
		mAcc |=  read_val;
		setNZflags(mAcc);
		break;
	}

	case Codec6502::Instruction::PHA:
		// Push Accumulator on Stack
		// push A
		// N	Z	C	I	D	V
		// -	-	-	-	-	-
	{
		writeDevice(0x100 + (uint16_t)mStackPointer--, mAcc);
		break;
	}

	case Codec6502::Instruction::PHP:
		// Push Status on Stack
		// push SR.
		// The status register will be pushed with the B
		// flag and b5 set to 1
		// N	Z	C	I	D	V
		// -	-	-	-	-	-
	{
		writeDevice(0x100 + (uint16_t)mStackPointer--, mStatusRegister | B_set_mask | b5_set_mask);
		break;
	}

	case Codec6502::Instruction::PLA:
		// Pull Accumulator from Stack
		// pull A
		// N	Z	C	I	D	V
		// +	+	-	-	-	-
	{
		readProgramMem(0x100 + (uint16_t)++mStackPointer, mAcc);
		setNZflags(mAcc);
		break;
	}
	case Codec6502::Instruction::PLP:
		// Pull Status register from Stack
		// Pull SR (B flag and b5 ignored)
		// N	Z	C	I	D	V
		// +	+	+	+	+	+
	{
		uint8_t stack_val;
		readProgramMem(0x100 + (uint16_t)++mStackPointer, stack_val);
		stack_val &= ~(B_set_mask | b5_set_mask);
		mStatusRegister &= ~(N_set_mask | Z_set_mask | C_set_mask | I_set_mask | D_set_mask | V_set_mask);
		mStatusRegister |= stack_val;
		break;
	}
	case Codec6502::Instruction::ROL:
		// Rotate One Bit Left (Memory or Accumulator)
		// C <- [76543210] <- C
		// N	Z	C	I	D	V
		// +	+	+	-	-	-
	{
		val_8_u = ((read_val << 1) & 0xfe) | C_flag;
		mStatusRegister &= ~C_set_mask;
		mStatusRegister |= ((read_val & 0x80)!=0? C_set_mask : 0);	
		if (instr.writesMem) {
			if (!writeDevice(calc_op_adr, read_val)) { // dummy write always made by NMOS 6502
				return false;
			}
			if (!writeDevice(calc_op_adr, val_8_u)) {
				return false;
			}
			written_val = val_8_u;
		}
		else
			mAcc = val_8_u;
		setNZflags(val_8_u);
		break;
	}

	case Codec6502::Instruction::ROR:
		// Rotate One Bit Right (Memory or Accumulator)
		// C -> [76543210] -> C
		// N	Z	C	I	D	V
		// +	+	+	-	-	-
	{
		val_8_u = ((read_val >> 1) & 0x7f) | ((C_flag << 7) & 0x80);
		mStatusRegister &= ~C_set_mask;
		mStatusRegister |= ((read_val & 0x1) ? C_set_mask : 0);		
		if (instr.writesMem) {
			if (!writeDevice(calc_op_adr, read_val)) { // dummy write always made by NMOS 6502
				return false;
			}
			if (!writeDevice(calc_op_adr, val_8_u)) {
				return false;
			}
			written_val = val_8_u;
		}
		else
			mAcc = val_8_u;
		setNZflags(val_8_u);
		break;
	}

	case Codec6502::Instruction::RTI:
		// Return from Interrupt
		// Pull SR (b5 and B flag ignored) and then pull PC
		// The low byte is pulled first.
		// N	Z	C	I	D	V
		// +	+	+	+	+	+
	{
		// Pull Status Register
		uint8_t stack_val;
		readProgramMem(0x100 + (uint16_t)++mStackPointer, stack_val);
		stack_val &= ~(B_set_mask | b5_set_mask);
		mStatusRegister &= ~(N_set_mask | Z_set_mask | C_set_mask | I_set_mask | D_set_mask | V_set_mask);
		mStatusRegister |= stack_val;

		// Pull PC
		uint8_t PC_l, PC_h;
		readProgramMem(0x100 + (uint16_t)++mStackPointer, PC_l);
		readProgramMem(0x100 + (uint16_t)++mStackPointer, PC_h);
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
		readProgramMem(0x100 + (uint16_t)++mStackPointer, PC_l);
		readProgramMem(0x100 + (uint16_t)++mStackPointer, PC_h);
		mProgramCounter = PC_h * 256 + PC_l + 1;
		break;
	}

	case Codec6502::Instruction::SBC:
		// Subtract Memory from Accumulator with Borrow
		// If decimal flag is set, substraction will be BCD (Binary-Coded Decimal) and only the C flag will have a useful value
		// A - M - C* -> A
		// N	Z	C	I	D	V
		// +	+	+	-	-	+
		//
		// V is set if sign bit is incorrect
		// C is cleared if overflow in b7
	{
		{
			int16_t val_C, val_V;
			if (D_flag) {

				// Calculate zero, negative & overflow flag as for non-BDC subtraction
				int16_t val_ZNV = (int8_t)mAcc - (int8_t)read_val - (1 - C_flag);
				setNZVflags((val_ZNV & 0x80) != 0, (val_ZNV & 0xff) == 0, val_ZNV < -128 || val_ZNV > 127);

				// BCD Subtraction and calculation of carry flag
				int8_t low_digit = (mAcc & 0xf) - (read_val & 0xf) - (1-C_flag); // can become -10 (0-9-1) to 9 (9-0-0)
				int8_t borrow = 0;
				if (low_digit < 0) {
					low_digit += 0x0a;
					borrow = 0x10;
				}
				low_digit &= 0x0f;
				int16_t high_digit = (mAcc & 0xf0) - (read_val & 0xf0) - borrow; // can become -0xa0 (0-0x90-0x10) to 0x90 (0x90-0-0) 
				int8_t set_C = 1;
				if (high_digit < 0) {
					high_digit += 0xa0;
					set_C = 0;
				}
				mAcc = (high_digit | low_digit) & 0xff;
				mStatusRegister &= ~C_set_mask;
				mStatusRegister |= set_C;

			}
			else {
				val_V = (int8_t)mAcc - (int8_t)read_val - (1-C_flag);
				val_C = mAcc - read_val - (1 - C_flag);
				mAcc = val_C & 0xff;
				setNZCVflags((mAcc & 0x80) != 0, mAcc == 0, val_C >= 0, val_V < -128 || val_V > 127);
			}
			

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
		writeDevice(calc_op_adr, mAcc);
		written_val = mAcc;
		break;
	}

	case Codec6502::Instruction::STX:
		// Store X in Memory
		// X -> M
		// N	Z	C	I	D	V
		// -	-	-	-	-	-
	{
		writeDevice(calc_op_adr, mRegisterX);
		written_val = mRegisterX;
		break;
	}

	case Codec6502::Instruction::STY:
		// Store Y in Memory
		// Y -> M
		// N	Z	C	I	D	V
		// -	-	-	-	-	-
	{
		writeDevice(calc_op_adr, mRegisterY);
		written_val = mRegisterY;
		break;
	}

	case Codec6502::Instruction::TAX:
		// Transfer Accumulator to X
		// A -> X
		// N	Z	C	I	D	V
		// +	+	-	-	-	-

	{
		mRegisterX = mAcc;
		setNZflags(mRegisterX);
		break;
	}

	case Codec6502::Instruction::TAY:
		// Transfer Accumulator to Y
		// A -> Y
		// N	Z	C	I	D	V
		// +	+	-	-	-	-
	{
		mRegisterY = mAcc;
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
		mAcc = mRegisterX;
		setNZflags(mAcc);
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
		mAcc = mRegisterY;
		setNZflags(mAcc);
		break;
	}

	default:
		break;
	}

	return true;
}

//
// Evaluates the operand part of an instruction
//
// Doesn't care about which specific instruction it is (e.g., LDA, STA etc.). It only
// looks at the addressing mode independent of the specific instruction.
// 
// Returns:
// 
// operand_16_u:	the constant numeric part of the specific instruction's operand (when applicable)
// calc_op_adr:		calculated memory access address for the specific instruction (when applicable)
// read_val_8_u:	value read from memory or from internal register (A,X,Y,SR,PC) by the specific instruction (when applicable)
//
bool P6502::getOperand(Codec6502::InstructionInfo instr, uint16_t& operand_16_u, uint16_t &calc_op_adr, uint8_t& read_val_8_u)
{
	operand_16_u = 0x0; // needs to have a default value as e.g. the Implied addressing mode doesn't define any operand
	calc_op_adr = 0x0; // needs to have a default value as e.g. the Accumulator/Implied addressing modes don't define any addresses
	read_val_8_u = 0x0; // needs to have a default value as e.g. the Implied addressing mode doesn't define any result value

	switch (instr.mode) {

	case Codec6502::Mode::Accumulator: // e.g., LSR A

		// OPC mAcc
		// Read A (no numeric operand)

		// Save the value resulting from the evaluation of the operand
		read_val_8_u = mAcc;

		break;

	case Codec6502::Mode::Implied: // e.g., SEC

		// implicit (no explicit operand)
		break;

	case Codec6502::Mode::Relative: // e.g., BNE

		// OPC <branch target>
		// Read relative branch address
	{
		uint8_t rel_a;
		if (!readProgramMem(mProgramCounter++, rel_a))
			return false;

		// Save the constant numeric part of the operand for later use when executing the specific instruction
		operand_16_u = rel_a;

		// Save the calculated address for use when executing the specific instruction later on
		calc_op_adr = (rel_a + mProgramCounter) & 0xffff;

		// Add one cycle if branch to other page
		if (instr.addCycleAtPageBoundary && pageBoundaryCrossed(mProgramCounter, calc_op_adr))
			tick();

		break;
	}

	case Codec6502::Mode::Immediate: // e.g., LDA @$12

		// OPC #$12
		// Read value $12
	{
		uint8_t op8;
		if (!readProgramMem(mProgramCounter++, op8))
			return false;

		// Save the constant numeric part of the operand for later use when executing the specific instruction
		operand_16_u = op8;

		// Save the value resulting from the evaluation of the operand
		read_val_8_u = op8;

		break;
	}

	case Codec6502::Mode::ZeroPage: // e.g., LDA $12

		// OPC $12
		// Read/Write value Mem[$12]
	{

		// Read address operand
		uint8_t zp_a;
		if (!readProgramMem(mProgramCounter++, zp_a))
			return false;

		// Save the constant numeric part of the operand for later use when executing the specific instruction
		operand_16_u = zp_a;

		// Save the calculated address for use when executing the specific instruction later on
		calc_op_adr = zp_a;

		// If the instruction reads from the calculated memory address (e.g., LDA, INC but not STA), then pre-read it
		// to make it available as 'read_val_8_u' later on when executing the instruction
		if (instr.readsMem && !readZP(zp_a, read_val_8_u))
			return false;

		break;
	}

	case Codec6502::Mode::ZeroPage_X: // e.g., LDA $12,X

		// OPC $12,X
		// Read/Write Mem[$12+X]
	{

		// Read address operand
		uint8_t zp_a;
		if (!readProgramMem(mProgramCounter++, zp_a))
			return false;

		// Save the constant numeric part of the operand for later use when executing the specific instruction
		operand_16_u = zp_a;

		// Calculate address and save it for use when executing the specific instruction later on
		calc_op_adr = (uint8_t) (zp_a + mRegisterX);

		// If the instruction reads from the calculated memory address (e.g., LDA, INC but not STA), then pre-read it
		// to make it available as 'read_val_8_u' later on when executing the instruction
		if (instr.readsMem && !readZP(calc_op_adr, read_val_8_u))
			return false;

		break;
	}

	case Codec6502::Mode::ZeroPage_Y: // e.g., LDA $12,Y

		// OPC $12,Y
		// Read/Write Mem[$12+Y]
	{

		// Read address operand
		uint8_t zp_a;
		if (!readProgramMem(mProgramCounter++, zp_a))
			return false;

		// Save the constant numeric part of the operand for later use when executing the specific instruction
		operand_16_u = zp_a;

		// Calculate address and save it for use when executing the specific instruction later on
		calc_op_adr = (uint8_t) (zp_a + mRegisterY);

		// If the instruction reads from the calculated memory address (e.g., LDA, INC but not STA), then pre-read it
		// to make it available as 'read_val_8_u' later on when executing the instruction
		if (instr.readsMem && !readZP(calc_op_adr, read_val_8_u))
			return false;


		break;
	}

	case Codec6502::Mode::Absolute: // e.g., LDA $1234

		// OPC $1234
		// Read/Write Mem[$1234]
	{

		// Read address operand
		uint8_t a_L, a_H;
		if (!readProgramMem(mProgramCounter++, a_L) || !readProgramMem(mProgramCounter++, a_H))
			return false;

		// Save the constant numeric part of the operand for later use when executing the specific instruction
		operand_16_u = (a_H << 8) | a_L;

		// Save the calculated address for use when executing the specific instruction later on
		calc_op_adr = operand_16_u;

		// If the instruction reads from the calculated memory address (e.g., LDA, INC but not STA), then pre-read it
		// to make it available as 'read_val_8_u' later on when executing the instruction
		if (instr.readsMem && !readDevice(calc_op_adr, read_val_8_u))
			return false;

		break;

	}

	case Codec6502::Mode::Absolute_X: // e.g., LDA $1234,X

		// OPC $1234,X
		// Read/Write Mem[$1234+X]
	{

		// Read address operand
		uint8_t a_L, a_H;
		if (!readProgramMem(mProgramCounter++, a_L) || !readProgramMem(mProgramCounter++, a_H))
			return false;

		// Save the constant numeric part of the operand for later use when executing the specific instruction
		operand_16_u = (a_H << 8) | a_L;

		// Calculate address and save it for use when executing the specific instruction later on
		calc_op_adr = operand_16_u + mRegisterX;

		// If the instruction reads from the calculated memory address (e.g., LDA, INC but not STA), then pre-read it
		// to make it available as 'read_val_8_u' later on when executing the instruction
		if (instr.readsMem && !readDevice(calc_op_adr, read_val_8_u))
			return false;

		// Add one cycle if page boundary crossed
		if (instr.addCycleAtPageBoundary && pageBoundaryCrossed(calc_op_adr, operand_16_u))
			tick();

		break;
	}

	case Codec6502::Mode::Absolute_Y: // e.g., LDA $1234,X

		// OPC $1234,Y
		// Read/Write Mem[$1234+Y]
	{

		// Read address operand
		uint8_t a_L, a_H;
		if (!readProgramMem(mProgramCounter++, a_L))
			return false;
		if (!readProgramMem(mProgramCounter++, a_H))
			return false;

		// Save the constant numeric part of the operand for later use when executing the specific instruction
		operand_16_u = (a_H << 8) | a_L;

		// Calculate address and save it for use when executing the specific instruction later on
		calc_op_adr = operand_16_u + mRegisterY;

		// If the instruction reads from the calculated memory address (e.g., LDA, INC but not STA), then pre-read it
		// to make it available as 'read_val_8_u' later on when executing the instruction
		if (instr.readsMem && !readDevice(calc_op_adr, read_val_8_u))
			return false;

		// Add one cycle if page boundary crossed
		if (instr.addCycleAtPageBoundary && pageBoundaryCrossed(calc_op_adr, operand_16_u))
			tick();

		break;
	}

	case Codec6502::Mode::Indirect: // e.g., JMP ($1234)

		// OPC ($1234)
		// Read 16-bit little-endian word from Mem[Mem[$1234]]
	{

		// Read address operand
		uint8_t a_L, a_H;
		if (!readProgramMem(mProgramCounter++, a_L))
			return false;
		if (!readProgramMem(mProgramCounter++, a_H))
			return false;

		// Save the constant numeric part of the operand for later use when executing the specific instruction
		operand_16_u = (a_H << 8) | a_L;

		// Read indirect address
		uint16_t adr_i = operand_16_u;
		if (!readDevice(adr_i, a_L) || !readDevice(adr_i+1, a_H))
			return false;

		// Calculate address and save it for use when executing the specific instruction later on
		calc_op_adr = (a_H << 8) | a_L;

		// If the instruction reads from the calculated memory address (e.g., LDA, INC but not STA), then pre-read it
		// to make it available as 'read_val_8_u' later on when executing the instruction
		if (instr.readsMem && !readDevice(calc_op_adr, read_val_8_u))
			return false;

		break;
	}

	case Codec6502::Mode::PreInd_X: // pre-indexed indirect -  // e.g., LDA ($12,X)

		// OPC ($12,X)
		// Read/Write Mem[Mem[$12+X]]
	{

		// Read zero page address operand
		uint8_t zp_a;
		if (!readProgramMem(mProgramCounter++, zp_a))
			return false;

		// Save the constant numeric part of the operand for later use when executing the specific instruction
		operand_16_u = zp_a;

		// Read indirect address
		uint8_t mem_a = zp_a + mRegisterX;
		uint8_t mem_a_1 = mem_a + 1; // allow value to wrap around as for an actual NMOS 6502
		uint8_t a_L, a_H;
		if (!readZP(mem_a, a_L) || !readZP(mem_a_1, a_H))
			return false;

		// Calculate address and save it for use when executing the specific instruction later on
		calc_op_adr = (a_H << 8) | a_L;

		// If the instruction reads from the calculated memory address (e.g., LDA, INC but not STA), then pre-read it
		// to make it available as 'read_val_8_u' later on when executing the instruction
		if (instr.readsMem && !readDevice(calc_op_adr, read_val_8_u))
			return false;

		break;
	}

	case Codec6502::Mode::PostInd_Y: // post-indexed indirect -  // e.g., LDA ($12),Y

		// OPC ($12),Y
		// Read/Write Mem[Mem[$12]+Y]
	{

		// Read zero page address operand (e.g. $12)
		uint8_t zp_a;
		if (!readProgramMem(mProgramCounter++, zp_a))
			return false;

		// Save the constant numeric part of the operand for later use when executing the specific instruction
		operand_16_u = zp_a;

		// Read Indirect address -  e.g. ($12)
		uint8_t a_L, a_H;
		uint8_t zp_a_1 = zp_a + 1; // allow value to wrap around as for an actual NMOS 6502
		if (!readZP((uint16_t)zp_a, a_L) || !readZP((uint16_t) zp_a_1, a_H))
			return false;
		uint16_t mem_a = (a_H << 8) | a_L;

		// Calculate address and save it for use when executing the specific instruction later on
		calc_op_adr = mem_a + mRegisterY;

		// If the instruction reads from the calculated memory address (e.g., LDA, INC but not STA), then pre-read it
		// to make it available as 'read_val_8_u' later on when executing the instruction
		if (instr.readsMem && !readDevice(calc_op_adr, read_val_8_u))
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
		"A:" << setw(2) << (int) mAcc <<
		" X:" << setw(2) << (int) mRegisterX <<
		" Y:" << setw(2) << (int) mRegisterY <<
		" SP:" << setw(2) << (int) mStackPointer <<
		" NV--DIZC:" << setw(8) << bitset<8>(mStatusRegister & 0xdf) <<
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
	mCycleCount += cycles;
}

void P6502::setNZflags(uint8_t val_8_u)
{
	mStatusRegister &= ~(N_set_mask | Z_set_mask);

	// Zero flag (Z): Set if the result is zero
	if (val_8_u == 0)
		mStatusRegister |= Z_set_mask;

	// Negative flag (N): Set if the result is negative
	if ((val_8_u & 0x80) != 0)
		mStatusRegister |= N_set_mask;
}

void P6502::setNZCflags(uint8_t val_8_u, uint8_t C)
{
	setNZflags(val_8_u);

	mStatusRegister &= ~C_set_mask;
	if (C)
		mStatusRegister |= C_set_mask;

}

void P6502::setNZVflags(uint8_t N, uint8_t Z, uint8_t V)
{
	mStatusRegister &= ~(N_set_mask | Z_set_mask | V_set_mask);
	if (N)
		mStatusRegister |= N_set_mask;
	if (Z)
		mStatusRegister |= Z_set_mask;
	if (V)
		mStatusRegister |= V_set_mask;
}

void P6502::setNZCVflags(uint8_t N, uint8_t Z, uint8_t C, uint8_t V)
{
	mStatusRegister &= ~(N_set_mask | Z_set_mask | C_set_mask | V_set_mask);
	if (N)
		mStatusRegister |= N_set_mask;
	if (Z)
		mStatusRegister |= Z_set_mask;
	if (C)
		mStatusRegister |= C_set_mask;
	if (V)
		mStatusRegister |= V_set_mask;
}

//
//
bool P6502::readDevice(uint16_t adr, uint8_t& data)
{
	for (int i = 0; i < mDevices.size(); i++) {
		MemoryMappedDevice* dev = mDevices[i];
		if (dev->selected(adr)) {
			bool success = dev->read(adr, data);
			//if (mDebugInfo.dbgLevel & DBG_6502)
			//	cout << "READ DEVICE AT 0x" << hex << adr << " => " << (int)data << "\n";
			return success;
		}
	}

	data = 0xff;
	return true;
}

bool P6502::readZP(uint8_t adr, uint8_t &data)
{
	data = 0xff;
	if (mZPMemDev != NULL && mZPMemDev->read(adr, data)) {
		//if (mDebugInfo.dbgLevel & DBG_6502)
		//	cout << "READ ZERO PAGE ADR 0x" << hex << (int) adr << " => " << (int)data << "\n";
		return true;
	}
	return false;
}

bool P6502::readProgramMem(uint16_t adr, uint8_t& data)
{
	if (mLastPgmDevice != NULL && mLastPgmDevice->read(adr, data))
		return true;

	mLastPgmDevice = NULL;
	for (int i = 0; i < mMemories.size(); i++) {
		MemoryMappedDevice* dev = mMemories[i];
		if (dev->selected(adr)) {
			bool success = dev->read(adr, data);
			//if (mDebugInfo.dbgLevel & DBG_6502)
			//	cout << "READ PROGRAM MEMORY AT 0x" << hex << adr << " => " << (int)data << " (" << (success ? "OK" : "NOK") << ")\n";
			mLastPgmDevice = dev;
			return success;
		}
	}

	data = 0xff;
	return true;
}

//
//
//
bool P6502::writeDevice(uint16_t adr, uint8_t data)
{

	for (int i = 0; i < mDevices.size(); i++) {
		MemoryMappedDevice* dev = mDevices[i];
		if (dev->selected(adr)) {
			//if (mDebugInfo.dbgLevel & DBG_6502)
			//	cout << "WRITE DEVICE 0x" << hex << (int)data << " to 0x" << adr << "\n";
			return dev->write(adr, data);
		}
	}
	return false;
}