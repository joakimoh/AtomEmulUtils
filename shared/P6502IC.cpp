
#include "P6502IC.h"
#include "MemoryMappedDevice.h"
#include <chrono>
#include <thread>
#include <iostream>
#include <sstream>
#include <filesystem>
#include <bitset>
#include <cmath>
#include "Utility.h"
#include "DeviceManager.h"
#include "ClockedDevice.h"


P6502IC::P6502IC(string name, double deviceClockRate, double tickRate, DebugTracing* debugTracing, ConnectionManager* connectionManager, DeviceManager* deviceManager) :
	P6502(name, deviceClockRate, tickRate, debugTracing, connectionManager, deviceManager)
{

}

P6502IC::~P6502IC()
{

}

bool P6502IC::serveNMI()
{
	// Save SP, SR & PC for logging later on
	uint8_t oStackPointer = mStackPointer;
	uint8_t oStatusRegister = mStatusRegister;
	uint16_t oProgramCounter = mProgramCounter;

	// Save PC & Status to stack
	pushWord(mProgramCounter);
	push(mStatusRegister);

	// Disable IRQ interrupts
	mStatusRegister |= I_set_mask;

	// Fetch break vector
	uint8_t adr_L, adr_H;
	if (!readProgramMem(0xfffa, adr_L) || !readProgramMem(0xfffb, adr_H))
		return false;
	mProgramCounter = adr_H * 256 + adr_L;
	mStatusRegister |= I_set_mask;

	if (DBG_LEVEL_DEV(this, DBG_INTERRUPTS | DBG_6502)) {
		DBG_LOG(this, DBG_INTERRUPTS, "Serving NMI at PC = 0x" + Utility::int2HexStr(oProgramCounter, 4) + "\n");
		DBG_LOG_COND(mIRQ == 0, this, DBG_INTERRUPTS, getInterruptStack(mStackPointer + 1, oStackPointer, oProgramCounter, oStatusRegister));
	}

	// Increase time by 7 clock cycles for the NMI
	tick(7);

	return true;
}

bool P6502IC::serveIRQ()
{
	// Exit if IRQ disabled
	if (mStatusRegister & I_set_mask) {
		DBG_LOG_COND(mIrqTransition, this, DBG_INTERRUPTS, "I flag set => IRQ ignored at 0x" + Utility::int2HexStr(mProgramCounter, 4) + "...\n");
		return false;
	}

	// Save SP, SR & PC for logging later on
	uint8_t oStackPointer = mStackPointer;
	uint8_t oStatusRegister = mStatusRegister;
	uint16_t oProgramCounter = mProgramCounter;


	// Save PC & Status to stack
	pushWord(mProgramCounter);
	push(mStatusRegister & ~B_set_mask);

	// Disable IRQ interrupts
	mStatusRegister |= I_set_mask;


	// Fetch break vector
	uint8_t adr_L, adr_H;
	if (!readProgramMem(0xfffe, adr_L) || !readProgramMem(0xffff, adr_H))
		return false;
	mProgramCounter = adr_H * 256 + adr_L;

	if (DBG_LEVEL_DEV(this, DBG_INTERRUPTS | DBG_6502)) {
		DBG_LOG(this, DBG_INTERRUPTS, "Serving IRQ at PC = 0x" + Utility::int2HexStr(oProgramCounter, 4) + "\n");
		DBG_LOG_COND(mIRQ == 0, this, DBG_INTERRUPTS, getInterruptStack(mStackPointer + 1, oStackPointer, oProgramCounter, oStatusRegister));
	}

	// Increase time by 7 clock cycles for the IRQ
	tick(7);

	return true;
}


//
// Emulate RESET sequence which should take 7 clock cycles
//
bool P6502IC::reset()
{
	Device::reset();

	// Fetch RESET vector
	uint8_t adr_L, adr_H;
	if (!readProgramMem(0xfffc, adr_L) || !readProgramMem(0xfffd, adr_H))
		return false;

	mProgramCounter = adr_H * 256 + adr_L;

	// Increase time by 7 clock cycles for the RESET
	tick(7);


	return true;
}

// 
// Advance until time stopTick
bool P6502IC::advanceUntil(uint64_t stopTick)
{

	while (mTicks < stopTick) {
		if (!advanceInstr(stopTick)) {
			return false;
		}
	}

	return true;
}


// Advance one instruction
bool P6502IC::advanceInstr(uint64_t& endTick)
{
	bool success = true;

	mResetTransition = mRESET != pRESET;
	pRESET = mRESET;

	mIrqTransition = mIRQ != pIRQ;
	pIRQ = mIRQ;

	mNmiTransition = mNMI != pNMI;
	pNMI = mNMI;

	mSOTransition = mSO != pSO;
	pSO = mSO;

	DBG_LOG_COND(mResetTransition, this, DBG_RESET, "RESET => " + to_string(mRESET) + "\n");

	DBG_LOG_COND(mIrqTransition, this, DBG_INTERRUPTS, "IRQ => " + to_string(mIRQ) + "\n");

	DBG_LOG_COND(mNmiTransition, this, DBG_INTERRUPTS, "NMI => " + to_string(mNMI) + "\n");

	// Serve RESET, NMI & IRQ in priority order
	if (!mRESET) {
		reset();
		// No meaning to continue execution before RESET line becomes HIGH again
		endTick = mTicks;
		return true;
	}
	else if (!mNMI && mNmiTransition)	// NMI is edge-triggered!
		serveNMI();
	else if (!mIRQ && !I_flag)	// IRQ is level-triggered!
		serveIRQ();
	else if (!mSO && mSOTransition) {
		mStatusRegister |= V_set_mask; // Set overflow flag on SO transition as per 6502 specification
		cout << "Negative edge on SO inout pin!\n";
	}
	else if (mRDY == 0) {
		// If RDY is low, the processor is paused and will not execute the next instruction until RDY goes high again.
		// However, time still advances and the processor can still respond to interrupts (NMI, IRQ) and RESET while paused.
		DBG_LOG(this, DBG_6502, "RDY low => CPU paused at address 0x" + Utility::int2HexStr(mProgramCounter, 4) + "...\n");
		endTick = mTicks;
		cout << "RDY pin is LOW!\n";
		return true;
	}

	// Save cycle count before fetching and executing the instruction
	uint64_t start_cycle = mCycle;

	// Get mOpcode of next instruction
	updatePort(SYNC, 0); // SYNC goes low at the start of the fetch phase of an instruction
	mOpcodePC = mProgramCounter;
	if (!readProgramMem(mProgramCounter++, mOpcode)) {
		success = false;
		DBG_LOG(this, DBG_ERROR, "Failed to read instruction!\n");
	}
	updatePort(SYNC, 1); // SYNC goes high at the end of the fetch phase of an instruction

	pInstructionData = &(pInstrDataTbl->data[mOpcode]); // Execution info
	pInstructionInfo = &(pInstructionData->info); // Opcode info only

	// Fetch the instruction operands and execute the instruction
	uint8_t oI_flag = I_flag;
	mExecSuccess = true;

	if (!executeInstr()) {
		mExecSuccess = false;
		DBG_LOG(this, DBG_ERROR, "Invalid instruction 0x" + Utility::int2HexStr(mOpcode, 2) + " at address 0x" + Utility::int2HexStr(mOpcodePC, 4) + "!\n");
	}

	mRAccAdr = mWAccAdr = -1;
	if (pInstructionInfo->readsMem)
		mRAccAdr = mOperandAddress;
	if (pInstructionInfo->writesMem)
		mWAccAdr = mOperandAddress;
	success = success && mExecSuccess;

	DBG_LOG_COND(false && I_flag != oI_flag, this, DBG_INTERRUPTS, "I disable flag " + string(I_flag ? "set" : "cleared") + " by instruction " + mCodec.instr2str[pInstructionInfo->instruction] + " at address 0x" + Utility::int2HexStr(mOpcodePC, 4) + "\n");

	// Increase time by the no of clock cycles specified for the instruction and mode.
	// This excludes extra cycle at page boundary as this is instead accounted for
	// in the methods for evaluating the operand part of the instruction.
	// In addition to this, a branch instruction's clock cycles need to be further adjusted also in the execution handler
	// as the cycle depends also on whether the branch was taaken or not.
	tick(pInstructionInfo->cycles);

	// Return time reached
	endTick = mTicks;
	uint64_t end_cycle = mCycle;

	// Calculate the no of cycles for the executed instruction
	mExecutedCycles = end_cycle - start_cycle;

	// Log executed instruction if enabled
	if (DBG_LEVEL_DEV(this, DBG_6502)) {
		InstrLogData instr_log_data;
		getInstrLogData(instr_log_data);
		stringstream sout;
		printInstrLogData(sout, instr_log_data);
		DBG_LOG(this, DBG_6502, sout.str());
	}



	// Always return true even if the instruction execution failed in some way in order
	// not to terminate the simulation prematurely...
	return true;
}

bool P6502IC::executeInstr()
{
	return (this->*(pInstructionData->addrHdlr))() && (this->*(pInstructionData->execHdlr))();
}

//
// Methods for executing an instruction
// 
// There is one method per instruction - independent of the addressing mode
//

//
// ADC
// 
// Add Memory to Accumulator with Carry
// 
// If decimal flag is set, addition will be BCD (Binary-Coded Decimal) and only the C flag will have a useful value
// 
// A + M + C -> A, C
// 
// N	Z	C	I	D	V
// +	+	+	-	-	+
//
bool P6502IC::ADCExecHdlr()
{
	int16_t val_C, val_V;
	if (D_flag) {

		// Calculate zero - flag set as for non-BDC addition
		int16_t val_Z = mAcc + mReadVal + C_flag;
		int8_t set_Z = ((val_Z & 0xff) == 0);

		// BCD Addition and calculation of carry flag
		uint8_t low_digit = (mAcc & 0xf) + (mReadVal & 0xf) + C_flag;
		uint8_t carry = 0;
		if (low_digit >= 0xa) {
			low_digit += 0x6; // same as subtraction by 10 (modulo 16)
			carry = 0x10;
		}
		low_digit &= 0xf;
		int16_t val_NV = (int8_t)(mAcc & 0xf0) + (int8_t)(mReadVal & 0xf0) + carry + low_digit; // save for later use when setting N & V flags
		uint16_t high_digit = (mAcc & 0xf0) + (mReadVal & 0xf0) + carry; // result could be up to 0x130 (0x90+0x90+0x10)
		int8_t set_C = 0;
		if (high_digit >= 0xa0) {
			high_digit += 0x60; // same as subtraction by 10 (modulo 16)
			set_C = 1;
		}
		high_digit &= 0xf0;
		mAcc = high_digit | low_digit;

		// Calculate N & V flags - they are based on the sum before adjusting for overflow of the high digit
		setNZCVflags((val_NV & 0x80) != 0, set_Z, set_C, val_NV < -128 || val_NV > 127);
	}
	else {
		val_V = (int8_t)mAcc + (int8_t)mReadVal + C_flag; // Add as signed no to determine overflow (V)
		val_C = mAcc + mReadVal + C_flag; // Add as unsigned no to determine carry (C)
		mAcc = val_C & 0xff;
		setNZCVflags((mAcc & 0x80) != 0, mAcc == 0, (val_C & 0x100) != 0, val_V < -128 || val_V > 127);
	}

	return true;

}

//
// AND
// 
// AND Memory with Accumulator
// 
// A AND M -> A
// 
// N	Z	C	I	D	V
// +	+	-	-	-	-
//
bool P6502IC::ANDExecHdlr()
{
	mAcc &= mReadVal;
	setNZflags(mAcc);

	return true;
}

//
// ASL
// 
// Shift Left One Bit (Memory or Accumulator)
// 
// C < [76543210] <- 0
// 
// N	Z	C	I	D	V
// +	+	+	-	-	-
//
bool P6502IC::ASLExecHdlr()
{
	mStatusRegister &= ~C_set_mask;
	mStatusRegister |= ((mReadVal & 0x80) != 0 ? C_set_mask : 0);
	uint8_t val_8_u = (mReadVal << 1) & 0xfe;
	if (pInstructionInfo->writesMem) {
		if (!writeDevice(mOperandAddress, mReadVal)) { // dummy write always made by NMOS 6502
			return false;
		}
		if (!writeDevice(mOperandAddress, val_8_u)) {
			return false;
		}
		mWrittenVal = val_8_u;
	}
	else
		mAcc = val_8_u;
	setNZflags(val_8_u);

	return true;
}

//
// BCC
// 
// Branch on Carry Clear
// 
// Branch on C = 0
// 
// N	Z	C	I	D	V
// -	-	-	-	-	-
//
bool P6502IC::BCCExecHdlr()
{
	if (!C_flag) {
		addBranchTakenCycles();
		mProgramCounter = (mOpcodePC + 2 + mOperand16) & 0xffff;
	}

	return true;
}

//
// BCS
// 
// Branch on Carry Set
// 
// Branch on C = 1
// 
// N	Z	C	I	D	V
// -	-	-	-	-	-
//
bool P6502IC::BCSExecHdlr()
{
	if (C_flag) {
		addBranchTakenCycles();
		mProgramCounter = (mOpcodePC + 2 + mOperand16) & 0xffff;
	}

	return true;
}

//
// BEQ
// 
// Branch on Result Zero
// 
// Branch on Z = 0
// 
// N	Z	C	I	D	V
// -	-	-	-	-	-
//
bool P6502IC::BEQExecHdlr()
{
	if (Z_flag) {
		addBranchTakenCycles();
		mProgramCounter = (mOpcodePC + 2 + mOperand16) & 0xffff;
	}

	return true;
}

//
// BIT
//
// Test Bits in Memory with Accumulator
// 
// A AND M -> Z, M7 -> N, M6 -> V
// 
// N	Z	C	I	D	V
// M7	+	-	-	-	M6
//
bool P6502IC::BITExecHdlr()
{

	uint8_t val_8_u = mAcc & mReadVal;
	mStatusRegister &= ~(Z_set_mask | N_set_mask | V_set_mask);
	if (val_8_u == 0)
		mStatusRegister |= Z_set_mask;
	if ((mReadVal & 0x80) != 0)
		mStatusRegister |= N_set_mask;
	if ((mReadVal & 0x40) != 0)
		mStatusRegister |= V_set_mask;

	return true;

}

//
// BMI
//
// Branch on Result Minus
// 
// Branch on N = 1
// 
// N	Z	C	I	D	V
// -	-	-	-	-	-
//
bool P6502IC::BMIExecHdlr()
{
	if (N_flag) {
		addBranchTakenCycles();
		mProgramCounter = (mOpcodePC + 2 + mOperand16) & 0xffff;
	}

	return true;
}

//
// BNE
//
// Branch on Result not Zero
// 
// Branch on Z = 0
// 
// N	Z	C	I	D	V
// -	-	-	-	-	-
//
bool P6502IC::BNEExecHdlr()
{
	if (!Z_flag) {
		addBranchTakenCycles();
		mProgramCounter = (mOpcodePC + 2 + mOperand16) & 0xffff;
	}

	return true;
}

//
// BPL
//
// Branch on Result Plus
// 
// Branch on N = 0
// 
// N	Z	C	I	D	V
// -	-	-	-	-	-
//
bool P6502IC::BPLExecHdlr()
{
	if (!N_flag) {
		addBranchTakenCycles();
		mProgramCounter = (mOpcodePC + 2 + mOperand16) & 0xffff;
	}

	return true;
}

//
// BRK
//
// Force Break
// 
// Initiates a software interrupt
// 
// push PC+2, push SR (together with a set b4 <=> B)
// 
// The high PC byte is pushed first (so that the PC is stored in little endian format in the memory)
// 
// N	Z	C	I	D	V
// -	-	-	1	-	-
//
bool P6502IC::BRKExecHdlr()
{

	// Save SP, SR & PC for logging later on
	uint8_t oStackPointer = mStackPointer;
	uint8_t oStatusRegister = mStatusRegister;
	uint16_t oProgramCounter = mProgramCounter;

	// Save PC & Status to stack
	pushWord(mOpcodePC + 2); // this is the same as PC after the opcode has been read + 2
	push(mStatusRegister | B_set_mask);

	// Fetch return true vector
	uint8_t adr_L, adr_H;
	if (!readProgramMem(0xfffe, adr_L) || !readProgramMem(0xffff, adr_H))
		return false;
	mProgramCounter = adr_H * 256 + adr_L;

	if (DBG_LEVEL_DEV(this, DBG_INTERRUPTS)) {
		DBG_LOG(this, DBG_INTERRUPTS, "BRK executed at PC = 0x" + Utility::int2HexStr(mOpcodePC, 4) + "\n");
		DBG_LOG(this, DBG_INTERRUPTS, getInterruptStack(mStackPointer + 1, oStackPointer, oProgramCounter, oStatusRegister));
	}

	return true;
}

//
// BVC
//
// Branch on Overflow Clear
// 
// Branch on V = 0
// 
// N	Z	C	I	D	V
// -	-	-	-	-	-
//
bool P6502IC::BVCExecHdlr()
{
	if (!V_flag) {
		addBranchTakenCycles();
		mProgramCounter = (mOpcodePC + 2 + mOperand16) & 0xffff;
	}

	return true;
}

//
// BVS
//
// Branch on Overflow Set
// 
// Branch on V = 1
// 
// N	Z	C	I	D	V
// -	-	-	-	-	-
//
bool P6502IC::BVSExecHdlr()
{
	if (V_flag) {
		addBranchTakenCycles();
		mProgramCounter = (mOpcodePC + 2 + mOperand16) & 0xffff;
	}

	return true;
}

//
// CLC
//
// Clear Carry Flag
// 
// N	Z	C	I	D	V
// -	-	0	-	-	-
//
bool P6502IC::CLCExecHdlr()
{
	mStatusRegister &= ~C_set_mask;

	return true;
}

//
// CLD
//
// Clear Decimal Mode
// 
// N	Z	C	I	D	V
// -	-	-	-	0	-
//
bool P6502IC::CLDExecHdlr()
{
	mStatusRegister &= ~D_set_mask;

	return true;
}

//
// CLI
//
// Clear Interrupt Disable Bit
// 
// N	Z	C	I	D	V
// -	-	-	0	-	-
//
bool P6502IC::CLIExecHdlr()
{
	uint8_t oStatusRegister = mStatusRegister;
	mStatusRegister &= ~I_set_mask;

	return true;
}

//
// CLV
//
// Clear Overflow Flag
// 
// N	Z	C	I	D	V
// -	-	-	-	-	0
//
bool P6502IC::CLVExecHdlr()
{
	mStatusRegister &= ~V_set_mask;

	return true;
}

//
// CMP
//
// Compare Memory with Accumulator
// 
// A - M
// 
// N	Z	C	I	D	V
// +	+	+	-	-	-
//
bool P6502IC::CMPExecHdlr()
{
	uint8_t val_8_u = mAcc - mReadVal;
	setNZCflags(val_8_u, mAcc >= mReadVal);

	return true;
}

//
// CPX
//
// Compare Memory and X
// 
// X - M
// 
// N	Z	C	I	D	V
// +	+	+	-	-	-
//
bool P6502IC::CPXExecHdlr()
{
	uint8_t val_8_u = mRegisterX - mReadVal;
	setNZCflags(val_8_u, mRegisterX >= mReadVal);

	return true;
}

//
// CPY
//
// Compare Memory and Y
// 
// Y - M
// 
// N	Z	C	I	D	V
// +	+	+	-	-	-
//
bool P6502IC::CPYExecHdlr()
{
	uint8_t val_8_u = mRegisterY - mReadVal;
	setNZCflags(val_8_u, mRegisterY >= mReadVal);

	return true;
}

//
// DEC
//
// Decrement Memory by One
// 
// M - 1 -> M
// 
// N	Z	C	I	D	V
// +	+	-	-	-	-
//
bool P6502IC::DECExecHdlr()
{
	uint8_t val_8_u = mReadVal - 1;
	if (!writeDevice(mOperandAddress, mReadVal)) { // dummy write always made by NMOS 6502
		return false;
	}
	if (!writeDevice(mOperandAddress, val_8_u)) {
		return false;
	}
	mWrittenVal = val_8_u;
	setNZflags(val_8_u);

	return true;
}

//
// DEX
//
// Decrement X by One
// 
// X - 1 -> X
// 
// N	Z	C	I	D	V
// +	+	-	-	-	-
//
bool P6502IC::DEXExecHdlr()

{
	mRegisterX = mRegisterX - 1;
	setNZflags(mRegisterX);

	return true;
}

//
// DEY
//
// Decrement Y by One
// 
// Y - 1 -> Y
// 
// N	Z	C	I	D	V
// +	+	-	-	-	-
//
bool P6502IC::DEYExecHdlr()
{
	mRegisterY = mRegisterY - 1;
	setNZflags(mRegisterY);

	return true;
}

//
// EOR
//
// Exclusive-OR Memory with Accumulator
// 
// A EOR M -> A
// 
// N	Z	C	I	D	V
// +	+	-	-	-	-
//
bool P6502IC::EORExecHdlr()
{
	mAcc ^= mReadVal;
	setNZflags(mAcc);

	return true;
}

//
// INC 
//
// Increment Memory by One
// 
// M + 1 -> M
// 
// N	Z	C	I	D	V
// +	+	-	-	-	-
//
bool P6502IC::INCExecHdlr()
{
	mWrittenVal = mReadVal + 1;
	if (!writeDevice(mOperandAddress, mReadVal)) { // dummy write always made by NMOS 6502
		return false;
	}
	if (!writeDevice(mOperandAddress, mWrittenVal)) {
		return false;
	}
	setNZflags(mWrittenVal);

	return true;
}

//
// INX
//
// Increment X by One
// 
// X + 1 -> X
// 
// N	Z	C	I	D	V
// +	+	-	-	-	-
//
bool P6502IC::INXExecHdlr()

{
	mRegisterX = mRegisterX + 1;
	setNZflags(mRegisterX);

	return true;
}

//
// INY
//
// Increment Y by One
// 
// Y + 1 -> Y
// 
// N	Z	C	I	D	V
// +	+	-	-	-	-
//
bool P6502IC::INYExecHdlr()
{
	mRegisterY = mRegisterY + 1;
	setNZflags(mRegisterY);

	return true;
}

//
// JMP
//
// Jump to New Location
// 
// OP2:OP1 -> PC
// 
// N	Z	C	I	D	V
// -	-	-	-	-	-
//
bool P6502IC::JMPExecHdlr()
{
	mProgramCounter = mOperandAddress;

	return true;
}

//
// JSR
//
// Jump to New Location Saving Return Address
// 
// Push PC+2; OP2:OP1 -> PC
// 
// The stack byte contains the program count high first, followed by program count low
// 
// N	Z	C	I	D	V
// -	-	-	-	-	-
//
bool P6502IC::JSRExecHdlr()
{
	pushWord(mOpcodePC + 2); // this is the same as PC after the opcode has been read + 2

	mProgramCounter = mOperandAddress;

	return true;
}

//
// LDA
//
// Load Accumulator with Memory
// 
// M -> A
// 
// N	Z	C	I	D	V
// +	+	-	-	-	-
//
bool P6502IC::LDAExecHdlr()
{
	mAcc = mReadVal;
	setNZflags(mAcc);

	return true;
}

//
// LDX
//
// Load X with Memory
// 
// M -> X
// 
// N	Z	C	I	D	V
// +	+	-	-	-	-
//
bool P6502IC::LDXExecHdlr()

{
	mRegisterX = mReadVal;
	setNZflags(mRegisterX);

	return true;
}

//
// LDY
//
// Load Y with Memory
// 
// M -> Y
// 
// N	Z	C	I	D	V
// +	+	-	-	-	-
//
bool P6502IC::LDYExecHdlr()
{
	mRegisterY = mReadVal;
	setNZflags(mRegisterY);

	return true;
}

//
// LSR
//
// Shift One Bit Right (Memory or Accumulator)
// 
// 0 -> [76543210] -> C
// 
// N	Z	C	I	D	V
// 0	+	+	-	-	-
//
bool P6502IC::LSRExecHdlr()
{
	mStatusRegister &= ~C_set_mask;
	mStatusRegister |= ((mReadVal & 0x1) != 0 ? C_set_mask : 0);
	uint8_t val_8_u = (mReadVal >> 1) & 0x7f;
	if (pInstructionInfo->writesMem) {
		if (!writeDevice(mOperandAddress, mReadVal)) { // dummy write always made by NMOS 6502
			return false;
		}
		if (!writeDevice(mOperandAddress, val_8_u)) {
			return false;
			mWrittenVal = val_8_u;
		}
	}
	else
		mAcc = val_8_u;
	setNZflags(val_8_u);

	return true;
}

//
// NOP
//
// No Operation
// 
// N	Z	C	I	D	V
// -	-	-	-	-	-
//
bool P6502IC::NOPExecHdlr()
{
	return true;
}

//
// ORA
//
// OR Memory with Accumulator
// 
// A OR M -> A
// 
// N	Z	C	I	D	V
// +	+	-	-	-	-
//
bool P6502IC::ORAExecHdlr()
{
	mAcc |= mReadVal;
	setNZflags(mAcc);

	return true;
}

//
// PHA
//
// Push Accumulator on Stack
// 
// push A
// 
// N	Z	C	I	D	V
// -	-	-	-	-	-
//
bool P6502IC::PHAExecHdlr()

{
	push(mAcc);

	return true;
}

//
// PHP
//
// Push Status on Stack
// 
// push SR.
// 
// The status register will be pushed with the B
// flag and b5 set to 1
// 
// N	Z	C	I	D	V
// -	-	-	-	-	-
//
bool P6502IC::PHPExecHdlr()
{
	push(mStatusRegister | B_set_mask | b5_set_mask);

	return true;
}

//
// PLA
//
// Pull Accumulator from Stack
// 
// pull A
// 
// N	Z	C	I	D	V
// +	+	-	-	-	-
//
bool P6502IC::PLAExecHdlr()
{
	pull(mAcc);
	setNZflags(mAcc);

	return true;
}

//
// PLP
//
// Pull Status register from Stack
// 
// Pull SR (B flag and b5 ignored)
// 
// N	Z	C	I	D	V
// +	+	+	+	+	+
//
bool P6502IC::PLPExecHdlr()
{
	uint8_t stack_val;
	pull(stack_val);
	stack_val &= ~(B_set_mask | b5_set_mask);
	mStatusRegister &= ~(N_set_mask | Z_set_mask | C_set_mask | I_set_mask | D_set_mask | V_set_mask);
	mStatusRegister |= stack_val;

	return true;
}

//
// ROL
//
// Rotate One Bit Left (Memory or Accumulator)
// 
// C <- [76543210] <- C
// 
// N	Z	C	I	D	V
// +	+	+	-	-	-
//
bool P6502IC::ROLExecHdlr()
{
	uint8_t val_8_u = ((mReadVal << 1) & 0xfe) | C_flag;
	mStatusRegister &= ~C_set_mask;
	mStatusRegister |= ((mReadVal & 0x80) != 0 ? C_set_mask : 0);
	if (pInstructionInfo->writesMem) {
		if (!writeDevice(mOperandAddress, mReadVal)) { // dummy write always made by NMOS 6502
			return false;
		}
		if (!writeDevice(mOperandAddress, val_8_u)) {
			return false;
		}
		mWrittenVal = val_8_u;
	}
	else
		mAcc = val_8_u;
	setNZflags(val_8_u);

	return true;
}

//
// ROR
//
// Rotate One Bit Right (Memory or Accumulator)
// 
// C -> [76543210] -> C
// 
// N	Z	C	I	D	V
// +	+	+	-	-	-
//
bool P6502IC::RORExecHdlr()
{
	uint8_t val_8_u = ((mReadVal >> 1) & 0x7f) | ((C_flag << 7) & 0x80);
	mStatusRegister &= ~C_set_mask;
	mStatusRegister |= ((mReadVal & 0x1) ? C_set_mask : 0);
	if (pInstructionInfo->writesMem) {
		if (!writeDevice(mOperandAddress, mReadVal)) { // dummy write always made by NMOS 6502
			return false;
		}
		if (!writeDevice(mOperandAddress, val_8_u)) {
			return false;
		}
		mWrittenVal = val_8_u;
	}
	else
		mAcc = val_8_u;
	setNZflags(val_8_u);

	return true;
}

//
// RTI
//
// Return from Interrupt
// 
// Pull SR (b5 and B flag ignored) and then pull PC
// 
// The low byte is pulled first.
// 
// N	Z	C	I	D	V
// +	+	+	+	+	+
//
bool P6502IC::RTIExecHdlr()
{
	uint8_t oStackPointer = mStackPointer;
	uint8_t oStatusRegister = mStatusRegister;
	uint16_t oProgramCounter = mProgramCounter;

	// Pull Status Register
	uint8_t stack_val;
	pull(stack_val);
	stack_val &= ~(B_set_mask | b5_set_mask);
	mStatusRegister &= ~(N_set_mask | Z_set_mask | C_set_mask | I_set_mask | D_set_mask | V_set_mask);
	mStatusRegister |= stack_val;

	// Pull PC
	uint16_t oPC = mProgramCounter;
	pullWord(mProgramCounter);

	if (DBG_LEVEL_DEV(this, DBG_INTERRUPTS)) {
		DBG_LOG(this, DBG_INTERRUPTS, "RTI executed at 0x" + Utility::int2HexStr(oPC, 4) + "; execution resumed at 0x" + Utility::int2HexStr(mProgramCounter, 4) + "!\n");
		DBG_LOG(this, DBG_INTERRUPTS, getInterruptStack(mStackPointer - 2, oStackPointer, oProgramCounter, oStatusRegister));
	}

	return true;
}

//
// RTS
//
// Return from Subroutine
// 
// Pull PC, PC+1 -> PC
// 
// The low byte is pulled first.
// 
// N	Z	C	I	D	V
// -	-	-	-	-	-
//
bool P6502IC::RTSExecHdlr()
{
	uint16_t oPC = mProgramCounter;
	pullWord(mProgramCounter);
	mProgramCounter++;

	return true;
}

//
// SBC
//
// Subtract Memory from Accumulator with Borrow
// 
// If decimal flag is set, substraction will be BCD (Binary-Coded Decimal) and only the C flag will have a useful value
// 
// A - M - C* -> A
// 
// N	Z	C	I	D	V
// +	+	+	-	-	+
//
// V is set if sign bit is incorrect
// C is cleared if overflow in b7
//
bool P6502IC::SBCExecHdlr()
{
	{
		int16_t val_C, val_V;
		if (D_flag) {

			// Calculate zero, negative & overflow flag as for non-BDC subtraction
			int16_t val_ZNV = (int8_t)mAcc - (int8_t)mReadVal - (1 - C_flag);
			setNZVflags((val_ZNV & 0x80) != 0, (val_ZNV & 0xff) == 0, val_ZNV < -128 || val_ZNV > 127);

			// BCD Subtraction and calculation of carry flag
			int8_t low_digit = (mAcc & 0xf) - (mReadVal & 0xf) - (1 - C_flag); // can become -10 (0-9-1) to 9 (9-0-0)
			int8_t borrow = 0;
			if (low_digit < 0) {
				low_digit += 0x0a;
				borrow = 0x10;
			}
			low_digit &= 0x0f;
			int16_t high_digit = (mAcc & 0xf0) - (mReadVal & 0xf0) - borrow; // can become -0xa0 (0-0x90-0x10) to 0x90 (0x90-0-0) 
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
			val_V = (int8_t)mAcc - (int8_t)mReadVal - (1 - C_flag);
			val_C = mAcc - mReadVal - (1 - C_flag);
			mAcc = val_C & 0xff;
			setNZCVflags((mAcc & 0x80) != 0, mAcc == 0, val_C >= 0, val_V < -128 || val_V > 127);
		}


		return true;
	}
}

//
// SEC
//
// Set Carry Flag
// 
// N	Z	C	I	D	V
// -	-	1	-	-	-
//
bool P6502IC::SECExecHdlr()
{
	mStatusRegister |= C_set_mask;

	return true;
}

//
// SED
//
// Set Decimal Flag
// 
// N	Z	C	I	D	V
// -	-	-	-	1	-
//
bool P6502IC::SEDExecHdlr()
{
	mStatusRegister |= D_set_mask;

	return true;
}

//
// SEI
//
// Set Interrupt Disable Status
// 
// N	Z	C	I	D	V
// -	-	-	1	-	-
//
bool P6502IC::SEIExecHdlr()
{
	uint8_t oStatusRegister = mStatusRegister;
	mStatusRegister |= I_set_mask;

	return true;
}

//
// STA
//
// Store Accumulator in Memory
// 
// A -> M
// 
// N	Z	C	I	D	V
// -	-	-	-	-	-
//
bool P6502IC::STAExecHdlr()
{
	writeDevice(mOperandAddress, mAcc);
	mWrittenVal = mAcc;

	return true;
}

//
// STX
//
// Store X in Memory
// 
// X -> M
// 
// N	Z	C	I	D	V
// -	-	-	-	-	-
//
bool P6502IC::STXExecHdlr()
{
	writeDevice(mOperandAddress, mRegisterX);
	mWrittenVal = mRegisterX;

	return true;
}

//
// STY
//
// Store Y in Memory
// 
// Y -> M
// 
// N	Z	C	I	D	V
// -	-	-	-	-	-
//
bool P6502IC::STYExecHdlr()
{
	writeDevice(mOperandAddress, mRegisterY);
	mWrittenVal = mRegisterY;

	return true;
}

//
// TSX
//
// Transfer Accumulator to X
// 
// A -> X
// 
// N	Z	C	I	D	V
// +	+	-	-	-	-
//
bool P6502IC::TAXExecHdlr()
{
	mRegisterX = mAcc;
	setNZflags(mRegisterX);

	return true;
}

//
// TSX
//
// Transfer Accumulator to Y
// 
// A -> Y
// 
// N	Z	C	I	D	V
// +	+	-	-	-	-
//
bool P6502IC::TAYExecHdlr()
{
	mRegisterY = mAcc;
	setNZflags(mRegisterY);

	return true;
}

//
// TSX
//
// Transfer Stack Pointer to X
// 
// SP -> X
// 
// N	Z	C	I	D	V
// +	+	-	-	-	-
//
bool P6502IC::TSXExecHdlr()
{
	mRegisterX = mStackPointer;
	setNZflags(mRegisterX);

	return true;
}

//
// TXA
//
// Transfer X to Accumulator
// 
// X -> A
// 
// N	Z	C	I	D	V
// +	+	-	-	-	-
//
bool P6502IC::TXAExecHdlr()
{
	mAcc = mRegisterX;
	setNZflags(mAcc);

	return true;
}

//
// TXS
//
// Transfer X to Stack Register: X -> SP
// 
// X -> SP
// 
// N	Z	C	I	D	V
// -	-	-	-	-	-
//
bool P6502IC::TXSExecHdlr()
{
	mStackPointer = mRegisterX;

	return true;
}

//
// TYA
//
// Transfer Y to Accumulator: Y -> A
// 
// Y -> A
// 
// N	Z	C	I	D	V
// +	+	-	-	-	-
//
bool P6502IC::TYAExecHdlr()
{
	mAcc = mRegisterY;
	setNZflags(mAcc);

	return true;
}



//
// Undocumented 6502 NMOS instructions that are used by some programs
//

//
// LAX
//
// Load Accumulator and X with Memory
// 
// M -> A -> X
// 
// N	Z	C	I	D	V
// +	+	-	-	-	-
//
bool P6502IC::LAXExecHdlr()

{
	mAcc = mRegisterX = mReadVal;
	setNZflags(mAcc);

	return true;
}

//
// SBX
//
// Subtract Memory from Accumulator AND X
// (A AND X) - oper -> X
// N	Z	C	I	D	V
// +	+	+	-	-	-
//
bool P6502IC::SBXExecHdlr()

{
	mRegisterX = (mAcc & mRegisterX) - mReadVal;
	setNZCflags(mRegisterX, mRegisterX >= mReadVal);

	return true;
}

//
// ISC
//
// Increment and Subtract <=> INC <mem> + SBC <mem>
// 
// M + 1 -> M, A - M - C* -> A
// 
// N	Z	C	I	D	V
// +	+	+	-	-	+
//
bool P6502IC::ISCExecHdlr()
{
	uint8_t val_8_u = mReadVal + 1;
	int16_t val_C, val_V;

	//
	// M + 1 -> M
	//
	if (!writeDevice(mOperandAddress, mReadVal)) { // dummy write always made by NMOS 6502
		return false;
	}

	if (!writeDevice(mOperandAddress, val_8_u)) {
		return false;
	}
	mWrittenVal = val_8_u;

	// 
	// A - M - C* -> A
	//
	val_V = (int8_t)mAcc - (int8_t)val_8_u - (1 - C_flag);
	val_C = mAcc - val_8_u - (1 - C_flag);
	mAcc = val_C & 0xff;
	setNZCVflags((mAcc & 0x80) != 0, mAcc == 0, val_C >= 0, val_V < -128 || val_V > 127);

	return true;
}

//
// DCP
// 
// Decrement Memory by One
// 
// M - 1 -> M, A - M
// 
// N	Z	C	I	D	V
// +	+	+	-	-	-
//
//
bool P6502IC::DCPExecHdlr()
{
	// M - 1
	uint8_t val_8_u = mReadVal - 1;
	if (!writeDevice(mOperandAddress, mReadVal)) { // dummy write always made by NMOS 6502
		return false;
	}
	if (!writeDevice(mOperandAddress, val_8_u)) {
		return false;
	}
	mWrittenVal = val_8_u;

	//
	// A - M
	//
	val_8_u = mAcc - mReadVal;
	setNZCflags(val_8_u, mAcc >= mReadVal);

	return true;
}

//
// ANC
//
// AND Memory with Carry
// 
// A AND oper, b7 -> C
// 
// N	Z	C	I	D	V
// +	+	+	-	-	-
//
bool P6502IC::ANCExecHdlr()
{
	mAcc &= mReadVal;

	// N & Z flags set based on result
	setNZflags(mAcc);

	// C flag = b7 of A
	mStatusRegister &= ~C_set_mask | ((mAcc & 0x80) != 0 ? C_set_mask : 0x0);

	return true;
}

//
// ALR
// 
// AND followed by LSR
// 
// A AND oper, 0 -> [76543210] -> C
// 
// N	Z	C	I	D	V
// +	+	+	-	-	-
//
bool P6502IC::ALRExecHdlr()
{
	// AND
	uint8_t val_before_shift = mAcc & mReadVal;

	// N & Z flags set based on result
	setNZflags(val_before_shift);

	// LSR
	mAcc = (val_before_shift >> 1) & 0x7f;

	// Set C as for LSR
	mStatusRegister &= ~C_set_mask;
	mStatusRegister |= ((val_before_shift & 0x1) != 0 ? C_set_mask : 0);

	return true;
}

//
// ARR
// 
// AND followed by ROR
// 
// A AND oper, C -> [76543210] -> C
// 
// N	Z	C	I	D	V
// +	+	+	-	-	+
//
bool P6502IC::ARRExecHdlr()
{
	// AND
	uint8_t val_before_shift = mAcc & mReadVal;

	// ROR
	mAcc = ((val_before_shift >> 1) & 0x7f) | ((C_flag << 7) & 0x80);

	// N & Z flags set based on result
	setNZflags(val_before_shift);

	// Set C as for ROR
	mStatusRegister &= ~C_set_mask;
	mStatusRegister |= ((val_before_shift & 0x1) != 0 ? C_set_mask : 0);

	return true;
}

//
// LAS
// 
// Load Memory ANDed with SP into A, X & SP
// 
// M AND SP -> A, X, SP
// 
// N	Z	C	I	D	V
// +	+	-	-	-	-
//
bool P6502IC::LASExecHdlr()
{
	mAcc = mRegisterX = mStackPointer = mReadVal & mStackPointer;
	setNZflags(mAcc);

	return true;
}

//
// RLA
// 
// ROL followed by AND
// 
// M = C <- [76543210] <- C, A AND M -> A
// 
// N	Z	C	I	D	V
// +	+	+	-	-	-
//
bool P6502IC::RLAExecHdlr()
{
	// ROL
	uint8_t val_8_u = ((mReadVal << 1) & 0xfe) | C_flag;
	mStatusRegister &= ~C_set_mask;
	mStatusRegister |= ((mReadVal & 0x80) != 0 ? C_set_mask : 0);

	// Write back the result to memory
	if (!writeDevice(mOperandAddress, mReadVal)) { // dummy write always made by NMOS 6502
		return false;
	}
	if (!writeDevice(mOperandAddress, val_8_u)) {
		return false;
	}
	mWrittenVal = val_8_u;

	// AND
	mAcc &= val_8_u;

	// N & Z flags set based on result
	setNZflags(val_8_u);

	return true;
}

//
// RRA
// 
// ROR followed by ADC
// 
// M = C -> [76543210] -> C, A + M + C -> A, C
// 
// N	Z	C	I	D	V
// +	+	+	-	-	+
//
bool P6502IC::RRAExecHdlr()
{
	// ROR
	uint8_t rotated_val = ((mReadVal >> 1) & 0x7f) | ((C_flag << 7) & 0x80);
	mStatusRegister &= ~C_set_mask;
	mStatusRegister |= ((mReadVal & 0x1) ? C_set_mask : 0);
	if (!writeDevice(mOperandAddress, mReadVal)) { // dummy write always made by NMOS 6502
		return false;
	}
	if (!writeDevice(mOperandAddress, rotated_val)) {
		return false;
	}
	mWrittenVal = rotated_val;
	setNZflags(rotated_val);

	// ADC
	int16_t val_C, val_V;
	val_V = (int8_t)mAcc + (int8_t)rotated_val + C_flag;	// Add as signed no to determine overflow (V)
	val_C = mAcc + rotated_val + C_flag;					// Add as unsigned no to determine carry (C)
	mAcc = val_C & 0xff;
	setNZCVflags((mAcc & 0x80) != 0, mAcc == 0, (val_C & 0x100) != 0, val_V < -128 || val_V > 127);

	return true;
}

//
// SAX
// 
// Store A and X into Memory
// 
// A AND X -> M
// 
// N	Z	C	I	D	V
// -	-	-	-	-	-
//
bool P6502IC::SAXExecHdlr()
{
	mWrittenVal = mAcc & mRegisterX;
	writeDevice(mOperandAddress, mAcc);

	return true;
}

//
// SLO
// 
// ASL followed by OR
// 
// M = C <- [76543210] <- 0, A OR M -> A
// 
// N	Z	C	I	D	V
// +	+	+	-	-	-
//
bool P6502IC::SLOExecHdlr()
{
	// ASL
	mStatusRegister &= ~C_set_mask;
	mStatusRegister |= ((mReadVal & 0x80) != 0 ? C_set_mask : 0);
	uint8_t val_8_u = (mReadVal << 1) & 0xfe;
	if (!writeDevice(mOperandAddress, mReadVal)) { // dummy write always made by NMOS 6502
		return false;
	}
	if (!writeDevice(mOperandAddress, val_8_u)) {
		return false;
	}
	mWrittenVal = val_8_u;

	// OR
	mAcc |= val_8_u;

	// N & Z flags set based on result
	setNZflags(val_8_u);

	return true;
}

//
// SRE
// 
// LSR followed by EOR
// 
// M = 0 ->[76543210]->C, A EOR M->A
// 
// N	Z	C	I	D	V
// +	+	+	-	-	-
//
bool P6502IC::SREExecHdlr()
{
	// LSR
	mStatusRegister &= ~C_set_mask;
	mStatusRegister |= ((mReadVal & 0x1) != 0 ? C_set_mask : 0);
	uint8_t val_8_u = (mReadVal >> 1) & 0x7f;
	if (pInstructionInfo->writesMem) {
		if (!writeDevice(mOperandAddress, mReadVal)) { // dummy write always made by NMOS 6502
			return false;
		}
		if (!writeDevice(mOperandAddress, val_8_u)) {
			return false;
			mWrittenVal = val_8_u;
		}
	}
	else
		mAcc = val_8_u;
	setNZflags(val_8_u);

	// XOR
	mAcc ^= val_8_u;

	// N & Z flags set based on result
	setNZflags(val_8_u);

	return true;
}

//
// Each opcode that doesn't result in a predictable
// function is mapped to this method.
//
bool P6502IC::undefinedExecHdlr()
{
	//cout << "Undefined instruction 0x" << hex << (int)mOpcode << " encountered!\n";
	return true;
}

//
// Methods for evaluating the operand part of an instruction
// 
// There is one method per addressing mode
//
// Doesn't care about which specific instruction it is (e.g., LDA, STA etc.). It only
// looks at the addressing mode independent of the specific instruction.
// 
// Updates:
// 
// mOperand16:			the constant numeric part of the specific instruction's operand (when applicable)
// mOperandAddress:		calculated memory access address for the specific instruction (when applicable)
// mReadVal:			value read from memory or from internal register (A,X,Y,SR,PC) by the specific instruction (when applicable)
//



//
// Implied addressing - with a result
// 
// Implicit (no explicit operand)
// 
// e.g., LSR A
//
bool P6502IC::accAdrHdlr()
{
	// Save the value resulting from the evaluation of the operand
	mReadVal = mAcc;

	return true;
}

//
// Implied addressing - without a result
// 
// Implicit (no explicit operand)
// 
// e.g., SEC
//
bool P6502IC::impliedAdrHdlr()
{
	return true;
}

//
// Relative addressing
//
// Mnemonic <branch target>
// 
// Read relative branch address
// 
// E.g., BNE
//
bool P6502IC::relativeAdrHdlr()
{

	uint8_t rel_a;
	if (!readProgramMem(mProgramCounter++, rel_a))
		return false;

	// Save the constant numeric part of the operand for later use when executing the specific instruction
	mOperand16 = (int8_t)rel_a;

	// Save the calculated address for use when executing the specific instruction later on
	mOperandAddress = (mOperand16 + mProgramCounter) & 0xffff;

	return true;

}

bool P6502IC::addBranchTakenCycles()
{
	// Add two cycles if branch to other page; otherwise just one cycle
	if (pInstructionInfo->addCycleAtPageBoundary && pageBoundaryCrossed(mProgramCounter, mOperandAddress))
		tick(2);
	else
		tick();
	return true;
}

//
// Immediate Addressing
//
// Mnemonic #<value>
// 
// Read <value>
//
// e.g., LDA @$12
//
bool P6502IC::immediateAdrHdlr()
{

	if (!readProgramMem(mProgramCounter++, mReadVal))
		return false;

	// Save the constant numeric part of the operand for later use when executing the specific instruction
	mOperand16 = mReadVal;

	return true;
}

//
// Zero page addressing
//
// Mnemonic <zero-page address>
// Read/Write value Mem[<zero-page address>]
// 
// e.g., LDA $12
//
bool P6502IC::zeroPageAdrHdlr()
{

	// Read address operand
	uint8_t zp_a;
	if (!readProgramMem(mProgramCounter++, zp_a))
		return false;

	// Save the constant numeric part of the operand for later use when executing the specific instruction
	mOperand16 = zp_a;

	// Save the calculated address for use when executing the specific instruction later on
	mOperandAddress = zp_a;

	// If the instruction reads from the calculated memory address (e.g., LDA, INC but not STA), then pre-read it
	// to make it available as 'mReadVal' later on when executing the instruction
	if (pInstructionInfo->readsMem && !readZP(zp_a, mReadVal))
		return false;

	return true;
}

//
// Indexed addressing, Zero page - X
//
// Mnemonic <zero-page address>,X
// Read/Write Mem[<zero-page address>+X]
// 
// e.g., LDA $12,X
//
bool P6502IC::zeroPageXAdrHdlr()
{

	// Read address operand
	uint8_t zp_a;
	if (!readProgramMem(mProgramCounter++, zp_a))
		return false;

	// Save the constant numeric part of the operand for later use when executing the specific instruction
	mOperand16 = zp_a;

	// Calculate address and save it for use when executing the specific instruction later on
	mOperandAddress = (uint8_t)(zp_a + mRegisterX);

	// If the instruction reads from the calculated memory address (e.g., LDA, INC but not STA), then pre-read it
	// to make it available as 'mReadVal' later on when executing the instruction
	if (pInstructionInfo->readsMem && !readZP(mOperandAddress, mReadVal))
		return false;
	return true;
}

//
// Indexed addressing, Zero page - Y
//
// Mnemonic <zero-page address>,Y
// Read/Write Mem[<zero-page address>+Y]
// 
// e.g., LDA $12,Y
//
bool P6502IC::zeroPageYAdrHdlr()
{

	// Read address operand
	uint8_t zp_a;
	if (!readProgramMem(mProgramCounter++, zp_a))
		return false;

	// Save the constant numeric part of the operand for later use when executing the specific instruction
	mOperand16 = zp_a;

	// Calculate address and save it for use when executing the specific instruction later on
	mOperandAddress = (uint8_t)(zp_a + mRegisterY);

	// If the instruction reads from the calculated memory address (e.g., LDA, INC but not STA), then pre-read it
	// to make it available as 'mReadVal' later on when executing the instruction
	if (pInstructionInfo->readsMem && !readZP(mOperandAddress, mReadVal))
		return false;


	return true;
}

//
// Absolute addressing
//
// Mnemonic<absolute address>
// Read/Write Mem[<absolute address>]
//
// e.g., LDA $1234
bool P6502IC::absoluteAdrHdlr()
{

	// Read address operand
	uint8_t a_L, a_H;
	if (!readProgramMem(mProgramCounter++, a_L) || !readProgramMem(mProgramCounter++, a_H))
		return false;

	// Save the constant numeric part of the operand for later use when executing the specific instruction
	mOperand16 = (a_H << 8) | a_L;

	// Save the calculated address for use when executing the specific instruction later on
	mOperandAddress = mOperand16;

	// If the instruction reads from the calculated memory address (e.g., LDA, INC but not STA), then pre-read it
	// to make it available as 'mReadVal' later on when executing the instruction
	if (pInstructionInfo->readsMem && !readDevice(mOperandAddress, mReadVal))
		return false;

	return true;

}

//
// Indexed addressing, Absolute - X
//
// Mnemonic <absolute address>,X
// Read/Write Mem[<absolute address>+X]
//
// e.g., LDA $1234,X
//
bool P6502IC::absoluteXAdrHdlr()
{

	// Read address operand
	uint8_t a_L, a_H;
	if (!readProgramMem(mProgramCounter++, a_L) || !readProgramMem(mProgramCounter++, a_H))
		return false;

	// Save the constant numeric part of the operand for later use when executing the specific instruction
	mOperand16 = (a_H << 8) | a_L;

	// Calculate address and save it for use when executing the specific instruction later on
	mOperandAddress = mOperand16 + mRegisterX;

	// If the instruction reads from the calculated memory address (e.g., LDA, INC but not STA), then pre-read it
	// to make it available as 'mReadVal' later on when executing the instruction
	if (pInstructionInfo->readsMem && !readDevice(mOperandAddress, mReadVal))
		return false;

	// Add one cycle if page boundary crossed
	if (pInstructionInfo->addCycleAtPageBoundary && pageBoundaryCrossed(mOperandAddress, mOperand16))
		tick();

	return true;
}

//
// Indexed addressing, Absolute - Y
//
// Mnemonic <absolute address>,Y
// Read/Write Mem[<absolute address>+Y]
//
// e.g., LDA $1234,Y
//
bool P6502IC::absoluteYAdrHdlr()
{

	// Read address operand
	uint8_t a_L, a_H;
	if (!readProgramMem(mProgramCounter++, a_L))
		return false;
	if (!readProgramMem(mProgramCounter++, a_H))
		return false;

	// Save the constant numeric part of the operand for later use when executing the specific instruction
	mOperand16 = (a_H << 8) | a_L;

	// Calculate address and save it for use when executing the specific instruction later on
	mOperandAddress = mOperand16 + mRegisterY;

	// If the instruction reads from the calculated memory address (e.g., LDA, INC but not STA), then pre-read it
	// to make it available as 'mReadVal' later on when executing the instruction
	if (pInstructionInfo->readsMem && !readDevice(mOperandAddress, mReadVal))
		return false;

	// Add one cycle if page boundary crossed
	if (pInstructionInfo->addCycleAtPageBoundary && pageBoundaryCrossed(mOperandAddress, mOperand16))
		tick();

	return true;
}

//
// Indirect addressing
//
// Mnemonic (<absolute address>)
// 
// Read 16-bit little-endian word from Mem[Mem[<absolute address>]]
// 
// e.g., JMP ($1234)
//
bool P6502IC::indirectAdrHdlr()
{

	// Read address operand
	uint8_t a_L, a_H;
	if (!readProgramMem(mProgramCounter++, a_L))
		return false;
	if (!readProgramMem(mProgramCounter++, a_H))
		return false;

	// Save the constant numeric part of the operand for later use when executing the specific instruction
	mOperand16 = (a_H << 8) | a_L;

	// Read indirect address
	uint16_t adr_i = mOperand16;
	if (!readDevice(adr_i, a_L) || !readDevice(adr_i + 1, a_H))
		return false;

	// Calculate address and save it for use when executing the specific instruction later on
	mOperandAddress = (a_H << 8) | a_L;

	// If the instruction reads from the calculated memory address (e.g., LDA, INC but not STA), then pre-read it
	// to make it available as 'mReadVal' later on when executing the instruction
	if (pInstructionInfo->readsMem && !readDevice(mOperandAddress, mReadVal))
		return false;

	// Add one cycle if page boundary is crossed fpr the indirect address
	if (pInstructionInfo->addCycleAtPageBoundary && (adr_i ^ (adr_i + 1)) != 0)
		tick();

	return true;
}

//
// Pre-index indirect addressing  - X
//
// Mnemonic (<zero-page address>,X)
// 
// Read/Write Mem[Mem[<zero-page address>+X]]
// 
// E.g., LDA ($12,X)
//
bool P6502IC::preIndXAdrHdlr()
{

	// Read zero page address operand
	uint8_t zp_a;
	if (!readProgramMem(mProgramCounter++, zp_a))
		return false;

	// Save the constant numeric part of the operand for later use when executing the specific instruction
	mOperand16 = zp_a;

	// Read indirect address
	uint8_t mem_a = zp_a + mRegisterX;
	uint8_t mem_a_1 = mem_a + 1; // allow value to wrap around as for an actual NMOS 6502
	uint8_t a_L, a_H;
	if (!readZP(mem_a, a_L) || !readZP(mem_a_1, a_H))
		return false;

	// Calculate address and save it for use when executing the specific instruction later on
	mOperandAddress = (a_H << 8) | a_L;

	// If the instruction reads from the calculated memory address (e.g., LDA, INC but not STA), then pre-read it
	// to make it available as 'mReadVal' later on when executing the instruction
	if (pInstructionInfo->readsMem && !readDevice(mOperandAddress, mReadVal))
		return false;

	return true;
}

//
// Post-indexed indirect addressing
//
// Mnemonic (<zero-page address>),Y
// 
// Read/Write Mem[Mem[<zero-page address>]+Y]
// 
// e.g., LDA ($12),Y
//
bool P6502IC::postIndYAdrHdlr()
{

	// Read zero page address operand (e.g. $12)
	uint8_t zp_a;
	if (!readProgramMem(mProgramCounter++, zp_a))
		return false;

	// Save the constant numeric part of the operand for later use when executing the specific instruction
	mOperand16 = zp_a;

	// Read Indirect address -  e.g. ($12)
	uint8_t a_L, a_H;
	uint8_t zp_a_1 = zp_a + 1; // allow value to wrap around as for an actual NMOS 6502
	if (!readZP((uint16_t)zp_a, a_L) || !readZP((uint16_t)zp_a_1, a_H))
		return false;
	uint16_t mem_a = (a_H << 8) | a_L;

	// Calculate address and save it for use when executing the specific instruction later on
	mOperandAddress = mem_a + mRegisterY;

	// If the instruction reads from the calculated memory address (e.g., LDA, INC but not STA), then pre-read it
	// to make it available as 'mReadVal' later on when executing the instruction
	if (pInstructionInfo->readsMem && !readDevice(mOperandAddress, mReadVal))
		return false;

	// Add one cycle if page boundary crossed
	if (pInstructionInfo->addCycleAtPageBoundary && pageBoundaryCrossed(mem_a, mOperandAddress))
		tick();

	return true;
}

//
// Illegal/unknown addressing mode
//
// Should never be called unless there is a bug somewhere...
//
bool P6502IC::undefinedAdrHdlr()
{

	return false;

}


void  P6502IC::adjustForWaitStates(MemoryMappedDevice* dev)
{
	// Add wait states if applicable
	int wait_states = dev->getWaitStates();
	if (wait_states > 0) {
		deviceTick(mCycle % 2); // synchronise with CPU Clock phase
		deviceTick(wait_states); // add extra memory access cycles
	}
}

//
bool P6502IC::readDevice(uint16_t adr, uint8_t& data)
{

	MemoryMappedDevice* mem_dev;
	if ((mem_dev = mDeviceManager->getSelectedMemoryMappedDevice(adr)) != NULL) {
		adjustForWaitStates(mem_dev);// Add wait states if applicable
		bool success = mem_dev->read(adr, data);
		return success;
	}

	DBG_LOG(this, DBG_WARNING, "*Warning* Read at unmapped address 0x" + Utility::int2HexStr(adr, 4) + ". Returns 0x0 for all unmapped addresses...\n");

	data = 0x0;// Better to return 0x00 than 0xff for now when not all devices are implemented as peripheral status 0x00 usually means inactive/no event
	return true;
}

bool P6502IC::readZP(uint8_t adr, uint8_t& data)
{
	data = 0xff;
	if (mZPMemDev != NULL && mZPMemDev->selected(adr)) {
		adjustForWaitStates(mZPMemDev);// Add wait states if applicable
		return mZPMemDev->read(adr, data);
	}
	return false;
}

bool P6502IC::readProgramMem(uint16_t adr, uint8_t& data)
{
	return readProgramMem(adr, data, true);
}

bool P6502IC::readProgramMem(uint16_t adr, uint8_t& data, bool adjustTiming)
{
	MemoryMappedDevice* dev;
	if ((dev = mDeviceManager->getSelectedMemoryMappedDevice(adr)) != NULL) {
		if (adjustTiming)
			adjustForWaitStates(dev);// Add wait states if applicable
		return dev->read(adr, data);
	}

	data = 0xff;
	return true;
}

bool P6502IC::writeDevice(uint16_t adr, uint8_t data)
{

	MemoryMappedDevice* dev;
	if ((dev = mDeviceManager->getSelectedMemoryMappedDevice(adr)) != NULL) {
		adjustForWaitStates(dev);// Add wait states if applicable
		return dev->write(adr, data);
	}


	return false;
}

void P6502IC::push(uint8_t v)
{
	uint16_t adr = 0x100 + (uint16_t)mStackPointer--;
	if (mStackMemDev != NULL && mStackMemDev->selected(adr)) {
		adjustForWaitStates(mStackMemDev);// Add wait states if applicable
		mStackMemDev->write(adr, v);
	}
}

void P6502IC::pull(uint8_t& v)
{
	uint16_t adr = 0x100 + (uint16_t)++mStackPointer;
	if (mStackMemDev != NULL && mStackMemDev->selected(adr)) {
		adjustForWaitStates(mStackMemDev);// Add wait states if applicable
		mStackMemDev->read(adr, v);
	}
}

void P6502IC::pushWord(uint16_t word)
{
	push(word >> 8);
	push(word & 0xff);
}

void P6502IC::pullWord(uint16_t& word)
{
	uint8_t low, high;
	pull(low);
	pull(high);
	word = (high << 8) | low;
}