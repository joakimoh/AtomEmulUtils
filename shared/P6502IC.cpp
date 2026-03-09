
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


P6502IC::P6502IC(string name, bool clockStretchingEnabled, double deviceClockRate, double tickRate, DebugTracing* debugTracing, ConnectionManager* connectionManager, DeviceManager* deviceManager) :
	P6502(name, clockStretchingEnabled, deviceClockRate, tickRate, debugTracing, connectionManager, deviceManager)
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

	mPageBoundaryCrossed = false;
	mBranchTaken = false;

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
	//mOpcodePC = mProgramCounter;
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
	setInstrLogData();
	if (DBG_LEVEL_DEV(this, DBG_6502)) {
		stringstream sout;
		printInstrLogData(sout, mInstrLogData);
		DBG_LOG(this, DBG_6502, sout.str());
	}

	pOpcodePC = mOpcodePC;
	mOpcodePC = mProgramCounter;

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
//	Mode			Syntax		Opcode	Bytes	Cycles
//	===========		==========	======	=====	======
//	immediate		ADC #oper	69		2		2  
//	zeropage		ADC oper	65		2		3
//	zeropage,X		ADC oper,X	75		2		4
//	absolute		ADC oper	6D		3		4
//	absolute,X		ADC oper,X	7D		3		4+
//	absolute,Y		ADC oper,Y	79		3		4+
//	(indirect,X)	ADC(oper,X)	61		2		6
//	(indirect),Y	ADC(oper),Y	71		2		5+
//
bool P6502IC::ADCExecHdlr()
{
	return ADCCalc();
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
//	Mode			Syntax		Opcode	Bytes	Cycles
//	===========		==========	======	=====	======
//	immediate		AND #oper	29		2		2  
//	zeropage		AND oper	25		2		3
//	zeropage,X		AND oper,X	35		2		4
//	absolute		AND oper	2D		3		4
//	absolute,X		AND oper,X	3D		3		4+
//	absolute,Y		AND oper,Y	39		3		4+
//	(indirect,X)	AND(oper,X)	21		2		6
//	(indirect),Y	AND(oper),Y	31		2		5+
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
//	Mode			Syntax		Opcode	Bytes	Cycles
//	===========		==========	======	=====	======
//	accumulator		ASL A		0A		1		2
//	zeropage		ASL oper	06		2		5
//	zeropage,X		ASL oper,X	16		2		6
//	absolute		ASL oper	0E		3		6
//	absolute,X		ASL oper,X	1E		3		7
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
//	Mode			Syntax		Opcode	Bytes	Cycles
//	===========		==========	======	=====	======
//	relative		BCC oper	90		2		2+
// 
// The branch is taken if the carry flag is clear (C = 0).
// If the branch is taken, the program counter is set to the address of the branch target and the next instruction is fetched from there.
// If the branch is not taken, the program counter is not modified and the next instruction is fetched from the next address after the branch instruction as usual.
// The number of cycles taken by a branch instruction depends on whether the branch is taken (+1) or not (+0) and on whether a page boundary is crossed when branching (+1).
//
bool P6502IC::BCCExecHdlr()
{
	if (!C_flag) {
		mBranchTaken = true;
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
//	Mode			Syntax		Opcode	Bytes	Cycles
//	===========		==========	======	=====	======
//	relative		BVS oper	b0		2		2+
// 
// The branch is taken if the carry flag is set (C = 1).
// If the branch is taken, the program counter is set to the address of the branch target and the next instruction is fetched from there.
// If the branch is not taken, the program counter is not modified and the next instruction is fetched from the next address after the branch instruction as usual.
// The number of cycles taken by a branch instruction depends on whether the branch is taken (+1) or not (+0) and on whether a page boundary is crossed when branching (+1).
//
bool P6502IC::BCSExecHdlr()
{
	if (C_flag) {
		mBranchTaken = true;
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
//	Mode			Syntax		Opcode	Bytes	Cycles
//	===========		==========	======	=====	======
//	relative		BEQ oper	f0		2		2+
// 
// The branch is taken if the zero flag is set (Z = 1).
// If the branch is taken, the program counter is set to the address of the branch target and the next instruction is fetched from there.
// If the branch is not taken, the program counter is not modified and the next instruction is fetched from the next address after the branch instruction as usual.
// The number of cycles taken by a branch instruction depends on whether the branch is taken (+1) or not (+0) and on whether a page boundary is crossed when branching (+1).
//
bool P6502IC::BEQExecHdlr()
{
	if (Z_flag) {
		mBranchTaken = true;
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
//	Mode			Syntax		Opcode	Bytes	Cycles
//	===========		==========	======	=====	======
//	zeropage		BIT oper	24		2		3  
//	absolute		BIT oper	2C		3		4
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
//	Mode			Syntax		Opcode	Bytes	Cycles
//	===========		==========	======	=====	======
//	relative		BMI oper	30		2		2+
// 
// The branch is taken if the negative flag is set (N = 1).
// If the branch is taken, the program counter is set to the address of the branch target and the next instruction is fetched from there.
// If the branch is not taken, the program counter is not modified and the next instruction is fetched from the next address after the branch instruction as usual.
// The number of cycles taken by a branch instruction depends on whether the branch is taken (+1) or not (+0) and on whether a page boundary is crossed when branching (+1).
//
bool P6502IC::BMIExecHdlr()
{
	if (N_flag) {
		mBranchTaken = true;
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
//	Mode			Syntax		Opcode	Bytes	Cycles
//	===========		==========	======	=====	======
//	relative		BEQ oper	d0		2		2+
// 
// The branch is taken if the zero flag is clear (Z = 0).
// If the branch is taken, the program counter is set to the address of the branch target and the next instruction is fetched from there.
// If the branch is not taken, the program counter is not modified and the next instruction is fetched from the next address after the branch instruction as usual.
// The number of cycles taken by a branch instruction depends on whether the branch is taken (+1) or not (+0) and on whether a page boundary is crossed when branching (+1).
//
bool P6502IC::BNEExecHdlr()
{
	if (!Z_flag) {
		mBranchTaken = true;
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
//	Mode			Syntax		Opcode	Bytes	Cycles
//	===========		==========	======	=====	======
//	relative		BPL oper	10		2		2+
//
// The branch is taken if the negative flag is clear (N = 0).
// If the branch is taken, the program counter is set to the address of the branch target and the next instruction is fetched from there.
// If the branch is not taken, the program counter is not modified and the next instruction is fetched from the next address after the branch instruction as usual.
// The number of cycles taken by a branch instruction depends on whether the branch is taken (+1) or not (+0) and on whether a page boundary is crossed when branching (+1).
//
bool P6502IC::BPLExecHdlr()
{
	if (!N_flag) {
		mBranchTaken = true;
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
//	Mode			Syntax		Opcode	Bytes	Cycles
//	===========		==========	======	=====	======
//	implied			BRK			00		1		7
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
//	Mode			Syntax		Opcode	Bytes	Cycles
//	===========		==========	======	=====	======
//	relative		BVC oper	50		2		2+
// 
// The branch is taken if the overflow flag is clear (V = 0).
// If the branch is taken, the program counter is set to the address of the branch target and the next instruction is fetched from there.
// If the branch is not taken, the program counter is not modified and the next instruction is fetched from the next address after the branch instruction as usual.
// The number of cycles taken by a branch instruction depends on whether the branch is taken (+1) or not (+0) and on whether a page boundary is crossed when branching (+1).
//
bool P6502IC::BVCExecHdlr()
{
	if (!V_flag) {
		mBranchTaken = true;
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
//	Mode			Syntax		Opcode	Bytes	Cycles
//	===========		==========	======	=====	======
//	relative		BVS oper	70		2		2+
// 
// The branch is taken if the overflow flag is set (V = 1).
// If the branch is taken, the program counter is set to the address of the branch target and the next instruction is fetched from there.
// If the branch is not taken, the program counter is not modified and the next instruction is fetched from the next address after the branch instruction as usual.
// The number of cycles taken by a branch instruction depends on whether the branch is taken (+1) or not (+0) and on whether a page boundary is crossed when branching (+1).
//
bool P6502IC::BVSExecHdlr()
{
	if (V_flag) {
		mBranchTaken = true;
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
//	Mode			Syntax		Opcode	Bytes	Cycles
//	===========		==========	======	=====	======
//	implied			CLC			18		1		2  
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
//	Mode			Syntax		Opcode	Bytes	Cycles
//	===========		==========	======	=====	======
//	implied			CLD			D8		1		2  
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
//	Mode			Syntax		Opcode	Bytes	Cycles
//	===========		==========	======	=====	======
//	implied			CLI			58		1		2  
//
bool P6502IC::CLIExecHdlr()
{
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
//	Mode			Syntax		Opcode	Bytes	Cycles
//	===========		==========	======	=====	======
//	implied			CLV			b8		1		2  
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
//	Mode			Syntax		Opcode	Bytes	Cycles
//	===========		==========	======	=====	======
//	immediate		CMP #oper	C9		2	2
//	zeropage		CMP oper	C5		2	3
//	zeropage,X		CMP oper, X	D5		2	4
//	absolute		CMP oper	CD		3	4
//	absolute,X		CMP oper, X	DD		3	4+
//	absolute,Y		CMP oper, Y	D9		3	4+
//	(indirect,X)	CMP(oper,X)	C1		2	6
//	(indirect),Y	CMP(oper),Y	D1		2	5+
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
//	Mode			Syntax		Opcode	Bytes	Cycles
//	===========		==========	======	=====	======
//	immediate		CPX #oper	E0		2		2  
//	zeropage		CPX oper	E4		2		3
//	absolute		CPX oper	EC		3		4
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
//	Mode			Syntax		Opcode	Bytes	Cycles
//	===========		==========	======	=====	======
//	immediate		CPY #oper	C0		2		2  
//	zeropage		CPY oper	C4		2		3
//	absolute		CPY oper	CC		3		4
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
//	Mode			Syntax		Opcode	Bytes	Cycles
//	===========		==========	======	=====	======
//	zeropage		DEC oper	C6		2		5
//	zeropage,X		DEC oper,X	D6		2		6
//	absolute		DEC oper	CE		3		6
//	absolute,X		DEC oper,X	DE		3		7
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
//	Mode			Syntax		Opcode	Bytes	Cycles
//	===========		==========	======	=====	======
//	implied			DEX			CA		1		2 
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
//	Mode			Syntax		Opcode	Bytes	Cycles
//	===========		==========	======	=====	======
//	implied			DEY			88		1		2 
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
//	Mode			Syntax		Opcode	Bytes	Cycles
//	===========		==========	======	=====	======
//	immediate		EOR #oper	49		2		2  
//	zeropage		EOR oper	45		2		3
//	zeropage,X		EOR oper, X	55		2		4
//	absolute		EOR oper	4D		3		4
//	absolute,X		EOR oper, X	5D		3		4+
//	absolute,Y		EOR oper, Y	59		3		4+
//	(indirect,X)	EOR(oper,X)	41		2		6
//	(indirect),Y	EOR(oper),Y	51		2		5+
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
//	Mode			Syntax		Opcode	Bytes	Cycles
//	===========		==========	======	=====	======
//	zeropage		INC oper	E6		2		5  
//	zeropage,X		INC oper,X	F6		2		6
//	absolute		INC oper	EE		3		6
//	absolute,X		INC oper,X	FE		3		7
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
//	Mode			Syntax		Opcode	Bytes	Cycles
//	===========		==========	======	=====	======
//	implied			INX			E8		1		2
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
//	Mode			Syntax		Opcode	Bytes	Cycles
//	===========		==========	======	=====	======
//	implied			INY			C8		1		2
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
//	Mode			Syntax		Opcode	Bytes	Cycles
//	===========		==========	======	=====	======
//	absolute		JMP oper	4C		3		3  
//	indirect		JMP(oper)	6C		3		5
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
//	Mode			Syntax		Opcode	Bytes	Cycles
//	===========		==========	======	=====	======
//	absolute		JSR oper	20		3		6
//
bool P6502IC::JSRExecHdlr()
{
	// Make dummy reading at the stack pointer address
	uint8_t dummy_byte;
	if (!readMem(mStackPointer, dummy_byte))
		return false;

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
//	Mode			Syntax		Opcode	Bytes	Cycles
//	===========		==========	======	=====	======
//	immediate		LDA #oper	A9		2		2  
//	zeropage		LDA oper	A5		2		3
//	zeropage,X		LDA oper,X	B5		2		4
//	absolute		LDA oper	AD		3		4
//	absolute,X		LDA oper,X	BD		3		4+
//	absolute,Y		LDA oper,Y	B9		3		4+
//	(indirect,X)	LDA(oper,X)	A1		2		6
//	(indirect),Y	LDA(oper),Y	B1		2		5+
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
//	Mode			Syntax		Opcode	Bytes	Cycles
//	===========		==========	======	=====	======
//	immediate		LDX #oper	A2		2		2  
//	zeropage		LDX oper	A6		2		3
//	zeropage,Y		LDX oper,Y	B6		2		4
//	absolute		LDX oper	AE		3		4
//	absolute,Y		LDX oper,Y	BE		3		4+
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
//	Mode			Syntax		Opcode	Bytes	Cycles
//	===========		==========	======	=====	======
//	immediate		LDY #oper	A0		2		2  
//	zeropage		LDY oper	A4		2		3
//	zeropage,X		LDY oper,X	B4		2		4
//	absolute		LDY oper	AC		3		4
//	absolute,X		LDY oper,X	BC		3		4+
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
//	Mode			Syntax		Opcode	Bytes	Cycles
//	===========		==========	======	=====	======
//	accumulator		LSR A		4A		1		2  
//	zeropage		LSR oper	46		2		5
//	zeropage,X		LSR oper,X	56		2		6
//	absolute		LSR oper	4E		3		6
//	absolute,X		LSR oper,X	5E		3		7
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
//	Mode			Syntax		Opcode	Bytes	Cycles
//	===========		==========	======	=====	======
//	implied			NOP			EA		1		2
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
//	Mode			Syntax		Opcode	Bytes	Cycles
//	===========		==========	======	=====	======
//	immediate		ORA #oper	09		2		2  
//	zeropage		ORA oper	05		2		3
//	zeropage,X		ORA oper,X	15		2		4
//	absolute		ORA oper	0D		3		4
//	absolute,X		ORA oper,X	1D		3		4+
//	absolute,Y		ORA oper,Y	19		3		4+
//	(indirect,X)	ORA(oper,X)	01		2		6
//	(indirect),Y	ORA(oper),Y	11		2		5+
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
//	Mode			Syntax		Opcode	Bytes	Cycles
//	===========		==========	======	=====	======
//	implied			PHA			48		1		3
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
//	Mode			Syntax		Opcode	Bytes	Cycles
//	===========		==========	======	=====	======
//	implied			PHP			08		1		3
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
//	Mode			Syntax		Opcode	Bytes	Cycles
//	===========		==========	======	=====	======
//	implied			PLA			68		1		4 
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
//	Mode			Syntax		Opcode	Bytes	Cycles
//	===========		==========	======	=====	======
// implied			PLP			28		1		4 
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
//	Mode			Syntax		Opcode	Bytes	Cycles
//	===========		==========	======	=====	======
//	accumulator		ROL A		2A		1		2  
//	zeropage		ROL oper	26		2		5
//	zeropage,X		ROL oper,X	36		2		6
//	absolute		ROL oper	2E		3		6
//	absolute,X		ROL oper,X	3E		3		7
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
//	Mode			Syntax		Opcode	Bytes	Cycles
//	===========		==========	======	=====	======
//	accumulator		ROR A		6A		1		2  
//	zeropage		ROR oper	66		2		5
//	zeropage,X		ROR oper,X	76		2		6
//	absolute		ROR oper	6E		3		6
//	absolute,X		ROR oper,X	7E		3		7
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
//	Mode			Syntax		Opcode	Bytes	Cycles
//	===========		==========	======	=====	======
//	implied			RTI			40		1		6 
//
bool P6502IC::RTIExecHdlr()
{
	uint8_t oStackPointer = mStackPointer;
	uint8_t oStatusRegister = mStatusRegister;
	uint16_t oProgramCounter = mProgramCounter;

	// A dummy read at the stack pointer address is always made by NMOS 6502
	uint8_t dummy_byte;
	if (!readMem(mStackPointer, dummy_byte))
		return false;

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
//	Mode			Syntax		Opcode	Bytes	Cycles
//	===========		==========	======	=====	======
//	implied			RTS			60		1		6 
//
bool P6502IC::RTSExecHdlr()
{
	uint16_t oPC = mProgramCounter;

	// A dummy read at the stack pointer address is always made by NMOS 6502
	uint8_t dummy_byte;
	if (!readMem(mStackPointer, dummy_byte))
		return false;

	pullWord(mProgramCounter);

	// A dummy read at the stack pointer address is always made by NMOS 6502 (this will also
	// increase the program counter to point correctly)
	if (!readMem(mProgramCounter++, dummy_byte))
		return false;

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
//	Mode			Syntax		Opcode	Bytes	Cycles
//	===========		==========	======	=====	======
//	immediate		SBC #oper	E9		2		2  
//	zeropage		SBC oper	E5		2		3
//	zeropage,X		SBC oper,X	F5		2		4
//	absolute		SBC oper	ED		3		4
//	absolute,X		SBC oper,X	FD		3		4+
//	absolute,Y		SBC oper,Y	F9		3		4+
//	(indirect,X)	SBC(oper,X)	E1		2		6
//	(indirect),Y	SBC(oper),Y	F1		2		5+
//
bool P6502IC::SBCExecHdlr()
{
	return SBCCalc();
}

//
// SEC
//
// Set Carry Flag
// 
// N	Z	C	I	D	V
// -	-	1	-	-	-
//
//	Mode			Syntax		Opcode	Bytes	Cycles
//	===========		==========	======	=====	======
//	implied			SEC			38		1		2
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
//	Mode			Syntax		Opcode	Bytes	Cycles
//	===========		==========	======	=====	======
//	implied			SED			F8		1		2 
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
//	Mode			Syntax		Opcode	Bytes	Cycles
//	===========		==========	======	=====	======
//	implied			SEI			78		1		2
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
//	Mode			Syntax		Opcode	Bytes	Cycles
//	===========		==========	======	=====	======
//	zeropage		STA oper	85		2		3  
//	zeropage,X		STA oper,X	95		2		4
//	absolute		STA oper	8D		3		4
//	absolute,X		STA oper,X	9D		3		5
//	absolute,Y		STA oper,Y	99		3		5
//	(indirect,X)	STA(oper,X)	81		2		6
//	(indirect),Y	STA(oper),Y	91		2		6
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
//	Mode			Syntax		Opcode	Bytes	Cycles
//	===========		==========	======	=====	======
//	zeropage		STX oper	86		2		3  
//	zeropage,Y		STX oper,Y	96		2		4
//	absolute		STX oper	8E		3		4
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
//	Mode			Syntax		Opcode	Bytes	Cycles
//	===========		==========	======	=====	======
//	zeropage		STY oper	84		2		3  
//	zeropage,X		STY oper,X	94		2		4
//	absolute		STY oper	8C		3		4
//
bool P6502IC::STYExecHdlr()
{
	writeDevice(mOperandAddress, mRegisterY);
	mWrittenVal = mRegisterY;

	return true;
}

//
// TAX
//
// Transfer Accumulator to X
// 
// A -> X
// 
// N	Z	C	I	D	V
// +	+	-	-	-	-
//
//	Mode			Syntax		Opcode	Bytes	Cycles
//	===========		==========	======	=====	======
//	implied			TAX			AA		1		2 
//
bool P6502IC::TAXExecHdlr()
{
	mRegisterX = mAcc;
	setNZflags(mRegisterX);

	return true;
}

//
// TAY
//
// Transfer Accumulator to Y
// 
// A -> Y
// 
// N	Z	C	I	D	V
// +	+	-	-	-	-
//
//	Mode			Syntax		Opcode	Bytes	Cycles
//	===========		==========	======	=====	======
//	implied			TAY			A8		1		2
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
//	Mode			Syntax		Opcode	Bytes	Cycles
//	===========		==========	======	=====	======
//	implied			TSX			bA		1		2 
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
//	Mode			Syntax		Opcode	Bytes	Cycles
//	===========		==========	======	=====	======
//	implied			TAX			8a		1		2 
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
//	Mode			Syntax		Opcode	Bytes	Cycles
//	===========		==========	======	=====	======
//	implied			TXS			9A		1		2
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
//	Mode			Syntax		Opcode	Bytes	Cycles
//	===========		==========	======	=====	======
//	implied			TYA			98		1		2 
//
bool P6502IC::TYAExecHdlr()
{
	mAcc = mRegisterY;
	setNZflags(mAcc);

	return true;
}






////////////////////////////////////////////////////////////////////////////
// 
// Undocumented 6502 NMOS instructions that are used by some programs
//
////////////////////////////////////////////////////////////////////////////


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
//	Mode			Syntax		Opcode	Bytes	Cycles
//	===========		==========	======	=====	======
//	immediate		ALR #oper	4B		2		2
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
// ANC
//
// AND Memory with Carry
// 
// A AND oper, b7 -> C
// 
// N	Z	C	I	D	V
// +	+	+	-	-	-
//
//	Mode			Syntax		Opcode	Bytes	Cycles
//	===========		==========	======	=====	======
//	immediate		ANC #oper	0B		2		2
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
// ANC2
//
// AND Memory with Carry
// 
// A AND oper, b7 -> C
// 
// N	Z	C	I	D	V
// +	+	+	-	-	-
//
//	Mode			Syntax		Opcode	Bytes	Cycles
//	===========		==========	======	=====	======
//	immediate		ANC #oper	2B		2		2
//
// Identical to ANC
//
bool P6502IC::ANC2ExecHdlr() { return ANCExecHdlr(); }

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
//	Mode			Syntax		Opcode	Bytes	Cycles
//	===========		==========	======	=====	======
//	immediate		ARR #oper	6B		2		2
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
//	Mode			Syntax		Opcode	Bytes	Cycles
//	===========		==========	======	=====	======
//	zeropage		DCP oper	C7		2		5  	
//	zeropage,X		DCP oper,X	D7		2		6
//	absolute		DCP oper	CF		3		6
//	absolute,X		DCP oper,X	DF		3		7
//	absolute,Y		DCP oper,Y	DB		3		7
//	(indirect,X)	DCP(oper,X)	C3		2		8
//	(indirect),Y	DCP(oper),Y	D3		2		8
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
// ISC
//
// Increment and Subtract <=> INC <mem> + SBC <mem>
// 
// M + 1 -> M, A - M - C* -> A
// 
// N	Z	C	I	D	V
// +	+	+	-	-	+
//
//	Mode			Syntax		Opcode	Bytes	Cycles
//	===========		==========	======	=====	======
//	zeropage		ISC oper	E7		2		5  	
//	zeropage,X		ISC oper,X	F7		2		6
//	absolute		ISC oper	EF		3		6
//	absolute,X		ISC oper,X	FF		3		7
//	absolute,Y		ISC oper,Y	FB		3		7
//	(indirect,X)	ISC(oper,X)	E3		2		8
//	(indirect),Y	ISC(oper),Y	F3		2		8
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
// LAS
// 
// Load Memory ANDed with SP into A, X & SP
// 
// M AND SP -> A, X, SP
// 
// N	Z	C	I	D	V
// +	+	-	-	-	-
//
//	Mode			Syntax		Opcode	Bytes	Cycles
//	===========		==========	======	=====	======
//	absolute,Y		LAS oper,Y	BB		3		4+
//
bool P6502IC::LASExecHdlr()
{
	mAcc = mRegisterX = mStackPointer = mReadVal & mStackPointer;
	setNZflags(mAcc);

	return true;
}


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
//	Mode			Syntax		Opcode	Bytes	Cycles
//	===========		==========	======	=====	======
//	zeropage		LAX oper	A7		2		3  	
//	zeropage,Y		LAX oper,Y	B7		2		4
//	absolute		LAX oper	AF		3		4
//	absolute,Y		LAX oper,Y	BF		3		4+
//	(indirect,X)	LAX(oper,X)	A3		2		6
//	(indirect),Y	LAX(oper),Y	B3		2		5+
//
bool P6502IC::LAXExecHdlr()

{
	mAcc = mRegisterX = mReadVal;
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
//	Mode			Syntax		Opcode	Bytes	Cycles
//	===========		==========	======	=====	======
//	zeropage		RLA oper	27		2		5  	
//	zeropage,X		RLA oper,X	37		2		6
//	absolute		RLA oper	2F		3		6
//	absolute,X		RLA oper,X	3F		3		7
//	absolute,Y		RLA oper,Y	3B		3		7
//	(indirect,X)	RLA(oper,X)	23		2		8
//	(indirect),Y	RLA(oper),Y	33		2		8
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
//	Mode			Syntax		Opcode	Bytes	Cycles
//	===========		==========	======	=====	======
//	zeropage		RRA oper	67		2		5  	
//	zeropage,X		RRA oper,X	77		2		6
//	absolute		RRA oper	6F		3		6
//	absolute,X		RRA oper,X	7F		3		7
//	absolute,Y		RRA oper,Y	7B		3		7
//	(indirect,X)	RRA(oper,X)	63		2		8
//	(indirect),Y	RRA(oper),Y	73		2		8
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
//	Mode			Syntax		Opcode	Bytes	Cycles
//	===========		==========	======	=====	======
//	zeropage		SAX oper	87		2		3  	
//	zeropage,Y		SAX oper,Y	97		2		4
//	absolute		SAX oper	8F		3		4
//	(indirect,X)	SAX(oper,X)	83		2		6
//
bool P6502IC::SAXExecHdlr()
{
	mWrittenVal = mAcc & mRegisterX;
	writeDevice(mOperandAddress, mAcc);

	return true;
}

//
// SBX
//
// Subtract immnediate value from Accumulator AND X
// (A AND X) - oper -> X
// N	Z	C	I	D	V
// +	+	+	-	-	-
//
//	Mode			Syntax		Opcode	Bytes	Cycles
//	===========		==========	======	=====	======
//	immediate	S	SBX #oper	CB		2		2
//
bool P6502IC::SBXExecHdlr()

{
	mRegisterX = (mAcc & mRegisterX) - mReadVal;
	setNZCflags(mRegisterX, mRegisterX >= mReadVal);

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
//	Mode			Syntax		Opcode	Bytes	Cycles
//	===========		==========	======	=====	======
//	zeropage		SLO oper	07		2		5  	
//	zeropage,X		SLO oper,X	17		2		6
//	absolute		SLO oper	0F		3		6
//	absolute,X		SLO oper,X	1F		3		7
//	absolute,Y		SLO oper,Y	1B		3		7
//	(indirect,X)	SLO(oper,X)	03		2		8
//	(indirect),Y	SLO(oper),Y	13		2		8
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
//	Mode			Syntax		Opcode	Bytes	Cycles
//	===========		==========	======	=====	======
//	zeropage		SRE oper	47		2		5  	
//	zeropage,X		SRE oper,X	57		2		6
//	absolute		SRE oper	4F		3		6
//	absolute,X		SRE oper,X	5F		3		7
//	absolute,Y		SRE oper,Y	5B		3		7
//	(indirect,X)	SRE(oper,X)	43		2		8
//	(indirect),Y	SRE(oper),Y	53		2		8
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
	// A dummy read at the program counter location is always made by NMOS 6502
	uint8_t dummy_byte;
	readProgramMem(mProgramCounter, dummy_byte);

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

	mBranchTaken = false;

	return true;

}

bool P6502IC::addBranchTakenCycles()
{
	// Add two cycles if branch to other page; otherwise just one cycle
	if (pInstructionInfo->addCycleAtPageBoundary && pageBoundaryCrossed(mProgramCounter, mOperandAddress)) {

		// Make dummy read at the program location
		uint8_t dummy_byte;
		if (!readProgramMem(mProgramCounter, dummy_byte))
			return false;

		// Advance 2 cycles
		tick(2);
	}
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

	// A dummy read at the zero page address is always made by NMOS 6502
	uint8_t dummy_byte;
	readProgramMem(zp_a, dummy_byte);

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

	// A dummy read at the zero page address is always made by NMOS 6502
	uint8_t dummy_byte;
	readProgramMem(zp_a, dummy_byte);

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

	// A dummy read at the zero page address is always made by NMOS 6502
	uint8_t dummy_byte;
	readProgramMem(zp_a, dummy_byte);

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

	// Always make a read at the non-page boundary compensated address (even for e.g. a STA instruction)
	if (!readProgramMem(mOperandAddress & 0xff | mOperand16 & 0xff00, mReadVal))
		return false;

	// For read-only instruction like LDA, add one cycle if page boundary crossed and re-read the data at the corrected address
	if (pInstructionInfo->addCycleAtPageBoundary && pageBoundaryCrossed(mOperandAddress, mOperand16)) {
		if (!readProgramMem(mOperandAddress, mReadVal))
			return false;
		tick();
	}

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

	// Always make a read at the non-page boundary compensated address (even for e.g. a STA instruction)
	if (!readProgramMem(mOperandAddress & 0xff | mOperand16 & 0xff00, mReadVal))
		return false;

	// For read-only instruction like LDA, add one cycle if page boundary crossed and re-read the data at the corrected address
	if (pInstructionInfo->addCycleAtPageBoundary && pageBoundaryCrossed(mOperandAddress, mOperand16)) {
		if (!readProgramMem(mOperandAddress, mReadVal))
			return false;
		tick();
	}

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
// Only used for the JMP instruction
//
bool P6502IC::indirectAdrHdlr()
{

	// Read address operand
	uint8_t lookup_address_L, lookup_address_H;
	if (!readProgramMem(mProgramCounter++, lookup_address_L))
		return false;
	if (!readProgramMem(mProgramCounter++, lookup_address_H))
		return false;

	// Save the constant numeric part of the operand for later use when executing the specific instruction
	mOperand16 = (lookup_address_H << 8) | lookup_address_L;

	// Read indirect (effective) address; crossing page boundary for the lookup address + 1 is ignored
	uint16_t lookup_address = mOperand16;
	uint8_t effective_address_L, effective_address_H;
	if (!readDevice(lookup_address, effective_address_L) || !readDevice((mOperand16 & 0xff00) | (lookup_address + 1) & 0xff, effective_address_H))
		return false;

	// Calculate effective address
	mOperandAddress = (effective_address_H << 8) | effective_address_L;

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

	// Make dummy read at the zeropage address
	uint8_t dummy_byte;
	if (!readProgramMem(zp_a, dummy_byte))
		return false;

	// Save the constant numeric part of the operand for later use when executing the specific instruction
	mOperand16 = zp_a;

	// Read indirect address
	uint8_t lookup_address = zp_a + mRegisterX;
	uint8_t effective_address_L, effective_address_H;
	if (!readZP(lookup_address, effective_address_L) || !readZP((lookup_address + 1) & 0xff, effective_address_H))
		return false;

	// Calculate the effective address and save it for use when executing the specific instruction later on
	mOperandAddress = (effective_address_H << 8) | effective_address_L;

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
	uint8_t effective_address_L, effective_address_H;
	uint8_t zp_a_1 = zp_a + 1; // allow value to wrap around as for an actual NMOS 6502
	if (!readZP((uint16_t)zp_a, effective_address_L) || !readZP((uint16_t)zp_a_1, effective_address_H))
		return false;
	uint16_t effective_address = (effective_address_H << 8) | effective_address_L;

	// Calculate address and save it for use when executing the specific instruction later on
	mOperandAddress = effective_address + mRegisterY;

	// Read at the calculated address while ignoring crossing a page boundary (made also for write-only instructions like STA)
	if (!readProgramMem((effective_address + mRegisterY) & 0xff | effective_address & 0xff00, mReadVal))
		return false;


	// Add one cycle if page boundary crossed and re-read at the corrected address
	if (pInstructionInfo->addCycleAtPageBoundary && pageBoundaryCrossed(effective_address, mOperandAddress)) {
		if (pInstructionInfo->readsMem && !readDevice(mOperandAddress, mReadVal))
			return false;
		tick();
	}

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


void  P6502IC::adjustForClockStretching(MemoryMappedDevice* dev)
{
	if (!mClockStretchingEnabled)
		return;

	// Strech the CPU clock for slow devices
	int access_ratio = dev->getAccessRatio();
	if (access_ratio > 1) {
		advanceTimeOnly(mCycle % 2 + access_ratio); // synchronise with CPU Clock phase and add extra memory access cycles
	}

}

//
bool P6502IC::readDevice(uint16_t adr, uint8_t& data)
{

	MemoryMappedDevice* mem_dev;
	if ((mem_dev = mDeviceManager->getSelectedMemoryMappedDevice(adr)) != NULL) {
		adjustForClockStretching(mem_dev); // Strech CPU clock if applicable
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
		adjustForClockStretching(mZPMemDev);// Strech CPU clock if applicable
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
			adjustForClockStretching(dev);// Strech CPU clock if applicable
		return dev->read(adr, data);
	}

	data = 0xff;
	return true;
}

bool P6502IC::writeDevice(uint16_t adr, uint8_t data)
{

	MemoryMappedDevice* dev;
	if ((dev = mDeviceManager->getSelectedMemoryMappedDevice(adr)) != NULL) {
		adjustForClockStretching(dev);// Strech CPU clock if applicable
		return dev->write(adr, data);
	}


	return false;
}

void P6502IC::push(uint8_t v)
{
	uint16_t adr = 0x100 + (uint16_t)mStackPointer--;
	if (mStackMemDev != NULL && mStackMemDev->selected(adr)) {
		adjustForClockStretching(mStackMemDev);// Strech CPU clock if applicable
		mStackMemDev->write(adr, v);
	}
}

void P6502IC::pull(uint8_t& v)
{
	uint16_t adr = 0x100 + (uint16_t)++mStackPointer;
	if (mStackMemDev != NULL && mStackMemDev->selected(adr)) {
		adjustForClockStretching(mStackMemDev);// Strech CPU clock if applicable
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