
#include "P6502.h"
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

void P6502::initInstrTable()
{
	pInstrDataTbl = new InstrDataTable();
	auto& mInstrData = pInstrDataTbl->data;
	for (int opcode = 0; opcode < 256; opcode++) {

		mInstrData[opcode].info = mCodec.getInstrInfo((uint8_t)opcode);

		// Get pointer to method for handling the address mode
		switch (mInstrData[opcode].info.mode) {
		case Codec6502::Mode::Accumulator:
			mInstrData[opcode].addrHdlr = &P6502::accAdrHdlr;
			break;
		case Codec6502::Mode::Implied:
			mInstrData[opcode].addrHdlr = &P6502::impliedAdrHdlr;
			break;
		case Codec6502::Mode::Relative:
			mInstrData[opcode].addrHdlr = &P6502::relativeAdrHdlr;
			break;
		case Codec6502::Mode::Immediate:
			mInstrData[opcode].addrHdlr = &P6502::immediateAdrHdlr;
			break;
		case Codec6502::Mode::ZeroPage:
			mInstrData[opcode].addrHdlr = &P6502::zeroPageAdrHdlr;
			break;
		case Codec6502::Mode::ZeroPage_X:
			mInstrData[opcode].addrHdlr = &P6502::zeroPageXAdrHdlr;
			break;
		case Codec6502::Mode::ZeroPage_Y:
			mInstrData[opcode].addrHdlr = &P6502::zeroPageYAdrHdlr;
			break;
		case Codec6502::Mode::Absolute:
			mInstrData[opcode].addrHdlr = &P6502::absoluteAdrHdlr;
			break;
		case Codec6502::Mode::Absolute_X:
			mInstrData[opcode].addrHdlr = &P6502::absoluteXAdrHdlr;
			break;
		case Codec6502::Mode::Absolute_Y:
			mInstrData[opcode].addrHdlr = &P6502::absoluteYAdrHdlr;
			break;
		case Codec6502::Mode::Indirect:
			mInstrData[opcode].addrHdlr = &P6502::indirectAdrHdlr;
			break;
		case Codec6502::Mode::PreInd_X:
			mInstrData[opcode].addrHdlr = &P6502::preIndXAdrHdlr;
			break;
		case Codec6502::Mode::PostInd_Y:
			mInstrData[opcode].addrHdlr = &P6502::postIndYAdrHdlr;
			break;
		default:
			mInstrData[opcode].addrHdlr = &P6502::undefinedAdrHdlr;
			break;
		}

		// Get pointer to method to handle the instruction
		switch (mInstrData[opcode].info.instruction) {
		case Codec6502::ADC:
			mInstrData[opcode].execHdlr = &P6502::ADCExecHdlr;
			break;
		case Codec6502::AND:
			mInstrData[opcode].execHdlr = &P6502::ANDExecHdlr;
			break;
		case Codec6502::ASL:
			mInstrData[opcode].execHdlr = &P6502::ASLExecHdlr;
			break;
		case Codec6502::BCC:
			mInstrData[opcode].execHdlr = &P6502::BCCExecHdlr;
			break;
		case Codec6502::BCS:
			mInstrData[opcode].execHdlr = &P6502::BCSExecHdlr;
			break;
		case Codec6502::BEQ:
			mInstrData[opcode].execHdlr = &P6502::BEQExecHdlr;
			break;
		case Codec6502::BIT:
			mInstrData[opcode].execHdlr = &P6502::BITExecHdlr;
			break;
		case Codec6502::BMI:
			mInstrData[opcode].execHdlr = &P6502::BMIExecHdlr;
			break;
		case Codec6502::BNE:
			mInstrData[opcode].execHdlr = &P6502::BNEExecHdlr;
			break;
		case Codec6502::BPL:
			mInstrData[opcode].execHdlr = &P6502::BPLExecHdlr;
			break;
		case Codec6502::BRK:
			mInstrData[opcode].execHdlr = &P6502::BRKExecHdlr;
			break;
		case Codec6502::BVC:
			mInstrData[opcode].execHdlr = &P6502::BVCExecHdlr;
			break;
		case Codec6502::BVS:
			mInstrData[opcode].execHdlr = &P6502::BVSExecHdlr;
			break;
		case Codec6502::CLC:
			mInstrData[opcode].execHdlr = &P6502::CLCExecHdlr;
			break;
		case Codec6502::CLD:
			mInstrData[opcode].execHdlr = &P6502::CLDExecHdlr;
			break;
		case Codec6502::CLI:
			mInstrData[opcode].execHdlr = &P6502::CLIExecHdlr;
			break;
		case Codec6502::CLV:
			mInstrData[opcode].execHdlr = &P6502::CLVExecHdlr;
			break;
		case Codec6502::CMP:
			mInstrData[opcode].execHdlr = &P6502::CMPExecHdlr;
			break;
		case Codec6502::CPX:
			mInstrData[opcode].execHdlr = &P6502::CPXExecHdlr;
			break;
		case Codec6502::CPY:
			mInstrData[opcode].execHdlr = &P6502::CPYExecHdlr;
			break;
		case Codec6502::DEC:
			mInstrData[opcode].execHdlr = &P6502::DECExecHdlr;
			break;
		case Codec6502::DEX:
			mInstrData[opcode].execHdlr = &P6502::DEXExecHdlr;
			break;
		case Codec6502::DEY:
			mInstrData[opcode].execHdlr = &P6502::DEYExecHdlr;
			break;
		case Codec6502::EOR:
			mInstrData[opcode].execHdlr = &P6502::EORExecHdlr;
			break;
		case Codec6502::INC:
			mInstrData[opcode].execHdlr = &P6502::INCExecHdlr;
			break;
		case Codec6502::INX:
			mInstrData[opcode].execHdlr = &P6502::INXExecHdlr;
			break;
		case Codec6502::INY:
			mInstrData[opcode].execHdlr = &P6502::INYExecHdlr;
			break;
		case Codec6502::JMP:
			mInstrData[opcode].execHdlr = &P6502::JMPExecHdlr;
			break;
		case Codec6502::JSR:
			mInstrData[opcode].execHdlr = &P6502::JSRExecHdlr;
			break;
		case Codec6502::LDA:
			mInstrData[opcode].execHdlr = &P6502::LDAExecHdlr;
			break;
		case Codec6502::LDX:
			mInstrData[opcode].execHdlr = &P6502::LDXExecHdlr;
			break;
		case Codec6502::LDY:
			mInstrData[opcode].execHdlr = &P6502::LDYExecHdlr;
			break;
		case Codec6502::LSR:
			mInstrData[opcode].execHdlr = &P6502::LSRExecHdlr;
			break;
		case Codec6502::NOP:
			mInstrData[opcode].execHdlr = &P6502::NOPExecHdlr;
			break;
		case Codec6502::ORA:
			mInstrData[opcode].execHdlr = &P6502::ORAExecHdlr;
			break;
		case Codec6502::PHA:
			mInstrData[opcode].execHdlr = &P6502::PHAExecHdlr;
			break;
		case Codec6502::PHP:
			mInstrData[opcode].execHdlr = &P6502::PHPExecHdlr;
			break;
		case Codec6502::PLA:
			mInstrData[opcode].execHdlr = &P6502::PLAExecHdlr;
			break;
		case Codec6502::PLP:
			mInstrData[opcode].execHdlr = &P6502::PLPExecHdlr;
			break;
		case Codec6502::ROL:
			mInstrData[opcode].execHdlr = &P6502::ROLExecHdlr;
			break;
		case Codec6502::ROR:
			mInstrData[opcode].execHdlr = &P6502::RORExecHdlr;
			break;
		case Codec6502::RTI:
			mInstrData[opcode].execHdlr = &P6502::RTIExecHdlr;
			break;
		case Codec6502::RTS:
			mInstrData[opcode].execHdlr = &P6502::RTSExecHdlr;
			break;
		case Codec6502::SBC:
			mInstrData[opcode].execHdlr = &P6502::SBCExecHdlr;
			break;
		case Codec6502::SEC:
			mInstrData[opcode].execHdlr = &P6502::SECExecHdlr;
			break;
		case Codec6502::SED:
			mInstrData[opcode].execHdlr = &P6502::SEDExecHdlr;
			break;
		case Codec6502::SEI:
			mInstrData[opcode].execHdlr = &P6502::SEIExecHdlr;
			break;
		case Codec6502::STA:
			mInstrData[opcode].execHdlr = &P6502::STAExecHdlr;
			break;
		case Codec6502::STX:
			mInstrData[opcode].execHdlr = &P6502::STXExecHdlr;
			break;
		case Codec6502::STY:
			mInstrData[opcode].execHdlr = &P6502::STYExecHdlr;
			break;
		case Codec6502::TAX:
			mInstrData[opcode].execHdlr = &P6502::TAXExecHdlr;
			break;
		case Codec6502::TAY:
			mInstrData[opcode].execHdlr = &P6502::TAYExecHdlr;
			break;
		case Codec6502::TSX:
			mInstrData[opcode].execHdlr = &P6502::TSXExecHdlr;
			break;
		case Codec6502::TXA:
			mInstrData[opcode].execHdlr = &P6502::TXAExecHdlr;
			break;
		case Codec6502::TXS:
			mInstrData[opcode].execHdlr = &P6502::TXSExecHdlr;
			break;
		case Codec6502::TYA:
			mInstrData[opcode].execHdlr = &P6502::TYAExecHdlr;
			break;
		case Codec6502::LAX:
			mInstrData[opcode].execHdlr = &P6502::LAXExecHdlr;
			break;
		case Codec6502::SBX:
			mInstrData[opcode].execHdlr = &P6502::SBXExecHdlr;
			break;
		case Codec6502::ISC:
			mInstrData[opcode].execHdlr = &P6502::ISCExecHdlr;
			break;
		case Codec6502::DCP:
			mInstrData[opcode].execHdlr = &P6502::DCPExecHdlr;
			break;
		case Codec6502::ANC:
			mInstrData[opcode].execHdlr = &P6502::ANCExecHdlr;
			break;
		case Codec6502::ALR:
			mInstrData[opcode].execHdlr = &P6502::ALRExecHdlr;
			break;
		case Codec6502::ARR:
			mInstrData[opcode].execHdlr = &P6502::ARRExecHdlr;
			break;
		case Codec6502::LAS:
			mInstrData[opcode].execHdlr = &P6502::LASExecHdlr;
			break;
		case Codec6502::RLA:
			mInstrData[opcode].execHdlr = &P6502::RLAExecHdlr;
			break;
		case Codec6502::RRA:
			mInstrData[opcode].execHdlr = &P6502::RRAExecHdlr;
			break;
		case Codec6502::SAX:
			mInstrData[opcode].execHdlr = &P6502::SAXExecHdlr;
			break;
		case Codec6502::SLO:
			mInstrData[opcode].execHdlr = &P6502::SLOExecHdlr;
			break;
		case Codec6502::SRE:
			mInstrData[opcode].execHdlr = &P6502::SREExecHdlr;
			break;
		default:
			mInstrData[opcode].execHdlr = &P6502::undefinedExecHdlr;
			break;
		}

	}

}

P6502::P6502(string name, double clockSpeed, DebugTracing  *debugTracing, ConnectionManager *connectionManager, DeviceManager* deviceManager):
	Device(name, P6502_DEV, MICROROCESSOR_DEVICE, clockSpeed, debugTracing, connectionManager), mDeviceManager(deviceManager)
{
	cPeriod = (int) round(1000 / clockSpeed);

	// Specify ports that can be connected to other devices
	registerPort("RESET", IN_PORT, 0x01, RESET, &mRESET);
	registerPort("IRQ", IN_PORT, 0x01, RESET, &mIRQ);
	registerPort("NMI", IN_PORT, 0x01, RESET, &mNMI);

	initInstrTable();


}

P6502::~P6502()
{
	if (pInstrDataTbl != nullptr)
		delete pInstrDataTbl;
}

bool P6502::serveNMI()
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
		DBG_LOG(this, DBG_INTERRUPTS, "Serving NMI at PC = 0x" + Utility::int2HexStr(oProgramCounter,4) + "\n");
		DBG_LOG_COND(mIRQ == 0, this, DBG_INTERRUPTS, getInterruptStack(mStackPointer + 1, oStackPointer, oProgramCounter, oStatusRegister));
	}

	return true;
}

bool P6502::serveIRQ()
{
	// Exit if IRQ disabled
	if (mStatusRegister & I_set_mask) {
		DBG_LOG_COND(mIrqTransition, this, DBG_INTERRUPTS, "I flag set => IRQ ignored at 0x" + Utility::int2HexStr(mProgramCounter,4) + "...\n");
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
		DBG_LOG(this, DBG_INTERRUPTS, "Serving IRQ at PC = 0x" + Utility::int2HexStr(oProgramCounter,4) + "\n");
		DBG_LOG_COND(mIRQ == 0, this, DBG_INTERRUPTS, getInterruptStack(mStackPointer+1, oStackPointer, oProgramCounter, oStatusRegister));
	}

	return true;
}


//
// Emulate RESET sequence which should take 7 clock cycles
//
bool P6502::reset()
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
// Advance until stop cycle has been reached
bool P6502::advanceUntil(uint64_t stopCycle)
{

	while (mCycleCount < stopCycle) {
		if (!advanceInstr(stopCycle))
			return false;
	}

	return true;
}


// Advance one instruction
bool P6502::advanceInstr(uint64_t& endCycle)
{
	bool success = true;

	mResetTransition = mRESET != pRESET;
	pRESET = mRESET;

	mIrqTransition = mIRQ != pIRQ;
	pIRQ = mIRQ;

	mNmiTransition = mNMI != pNMI;
	pNMI = mNMI;

	DBG_LOG_COND(mResetTransition, this, DBG_RESET, "RESET => " + to_string(mRESET) + "\n");

	DBG_LOG_COND(mIrqTransition, this, DBG_INTERRUPTS, "IRQ => " + to_string(mIRQ) + "\n");

	DBG_LOG_COND(mNmiTransition, this, DBG_INTERRUPTS, "NMI => " + to_string(mNMI) + "\n");

	// Serve RESET, NMI & IRQ in priority order
	if (!mRESET) {
		reset();
		// No meaning to continue execution before RESET line becomes HIGH again
		endCycle = mCycleCount;
		return true;
	}
	else if (!mNMI && mNmiTransition)	// NMI is edge-triggered!
		serveNMI();
	else if (!mIRQ)	// IRQ is level-triggered!
		serveIRQ();

	// Save cycle count before fetching and executing the instruction
	uint64_t start_cycle = mCycleCount;

	// Get mOpcode of next instruction
	mOpcodePC = mProgramCounter;
	if (!readProgramMem(mProgramCounter++, mOpcode)) {
		success = false;
		DBG_LOG(this, DBG_ERROR, "Failed to read instruction!\n");
	}

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

	DBG_LOG_COND(false && I_flag != oI_flag, this, DBG_INTERRUPTS, "I disable flag " + string(I_flag?"set":"cleared") + " by instruction " + mCodec.instr2str[pInstructionInfo->instruction] + " at address 0x" + Utility::int2HexStr(mOpcodePC,4) + "\n");

	// Increase time by the no of clock cycles specified for the instruction and mode.
	// This excludes extra cycle at page boundary as this is instead accounted for
	// in the methods for evaluating the operand part of the instruction.
	tick(pInstructionInfo->cycles);

	// Return time reached
	endCycle = mCycleCount;

	// Calculate the no of cycles for the executed instruction
	mExecutedCycles = endCycle - start_cycle;

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

bool P6502::executeInstr()
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
bool P6502::ADCExecHdlr()
{
	int16_t val_C, val_V;
	if (D_flag) {

		// Calculate zero - flag set as for non-BDC addition
		int16_t val_Z = mAcc + mReadVal8 + C_flag;
		int8_t set_Z = ((val_Z & 0xff) == 0);

		// BCD Addition and calculation of carry flag
		uint8_t low_digit = (mAcc & 0xf) + (mReadVal8 & 0xf) + C_flag;
		uint8_t carry = 0;
		if (low_digit >= 0xa) {
			low_digit += 0x6; // same as subtraction by 10 (modulo 16)
			carry = 0x10;
		}
		low_digit &= 0xf;
		int16_t val_NV = (int8_t)(mAcc & 0xf0) + (int8_t)(mReadVal8 & 0xf0) + carry + low_digit; // save for later use when setting N & V flags
		uint16_t high_digit = (mAcc & 0xf0) + (mReadVal8 & 0xf0) + carry; // result could be up to 0x130 (0x90+0x90+0x10)
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
		val_V = (int8_t)mAcc + (int8_t)mReadVal8 + C_flag; // Add as signed no to determine overflow (V)
		val_C = mAcc + mReadVal8 + C_flag; // Add as unsigned no to determine carry (C)
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
bool P6502::ANDExecHdlr()
{
	mAcc &= mReadVal8;
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
bool P6502::ASLExecHdlr()
{
	mStatusRegister &= ~C_set_mask;
	mStatusRegister |= ((mReadVal8 & 0x80) != 0 ? C_set_mask : 0);
	uint8_t val_8_u = (mReadVal8 << 1) & 0xfe;
	if (pInstructionInfo->writesMem) {
		if (!writeDevice(mOperandAddress, mReadVal8)) { // dummy write always made by NMOS 6502
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
bool P6502::BCCExecHdlr()
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
bool P6502::BCSExecHdlr()
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
bool P6502::BEQExecHdlr()	
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
bool P6502::BITExecHdlr()
{

	uint8_t val_8_u = mAcc & mReadVal8;
	mStatusRegister &= ~(Z_set_mask | N_set_mask | V_set_mask);
	if (val_8_u == 0)
		mStatusRegister |= Z_set_mask;
	if ((mReadVal8 & 0x80) != 0)
		mStatusRegister |= N_set_mask;
	if ((mReadVal8 & 0x40) != 0)
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
bool P6502::BMIExecHdlr()
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
bool P6502::BNEExecHdlr()
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
bool P6502::BPLExecHdlr()
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
bool P6502::BRKExecHdlr()
{

	// Save SP, SR & PC for logging later on
	uint8_t oStackPointer = mStackPointer;
	uint8_t oStatusRegister = mStatusRegister;
	uint16_t oProgramCounter = mProgramCounter;
		
	// Save PC & Status to stack
	pushWord(mOpcodePC+2); // this is the same as PC after the opcode has been read + 2
	push(mStatusRegister | B_set_mask);

	// Fetch return true vector
	uint8_t adr_L, adr_H;
	if (!readProgramMem(0xfffe, adr_L) || !readProgramMem(0xffff, adr_H))
		return false;
	mProgramCounter = adr_H * 256 + adr_L;

	if (DBG_LEVEL_DEV(this,DBG_INTERRUPTS)) {		
		DBG_LOG(this, DBG_INTERRUPTS, "BRK executed at PC = 0x" + Utility::int2HexStr(mOpcodePC,4) +"\n");
		DBG_LOG(this, DBG_INTERRUPTS, getInterruptStack(mStackPointer+1, oStackPointer, oProgramCounter, oStatusRegister));
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
bool P6502::BVCExecHdlr()
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
bool P6502::BVSExecHdlr()
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
bool P6502::CLCExecHdlr()
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
bool P6502::CLDExecHdlr()
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
bool P6502::CLIExecHdlr()
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
bool P6502::CLVExecHdlr()
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
bool P6502::CMPExecHdlr()
{	
	uint8_t val_8_u = mAcc - mReadVal8;
	setNZCflags(val_8_u, mAcc >= mReadVal8);

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
bool P6502::CPXExecHdlr()
{	
	uint8_t val_8_u = mRegisterX - mReadVal8;
	setNZCflags(val_8_u, mRegisterX >= mReadVal8);

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
bool P6502::CPYExecHdlr()
{
	uint8_t val_8_u = mRegisterY - mReadVal8;
	setNZCflags(val_8_u, mRegisterY >= mReadVal8);

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
bool P6502::DECExecHdlr()
{
	uint8_t val_8_u = mReadVal8 - 1;
	if (!writeDevice(mOperandAddress, mReadVal8)) { // dummy write always made by NMOS 6502
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
bool P6502::DEXExecHdlr()

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
bool P6502::DEYExecHdlr()
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
bool P6502::EORExecHdlr()
{
	mAcc ^= mReadVal8;
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
bool P6502::INCExecHdlr()
{
	mWrittenVal = mReadVal8 + 1;
	if (!writeDevice(mOperandAddress, mReadVal8)) { // dummy write always made by NMOS 6502
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
bool P6502::INXExecHdlr()

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
bool P6502::INYExecHdlr()
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
bool P6502::JMPExecHdlr()
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
bool P6502::JSRExecHdlr()
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
bool P6502::LDAExecHdlr()
{
	mAcc = mReadVal8;
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
bool P6502::LDXExecHdlr()

{
	mRegisterX = mReadVal8;
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
bool P6502::LDYExecHdlr()
{
	mRegisterY = mReadVal8;
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
bool P6502::LSRExecHdlr()
{
	mStatusRegister &= ~C_set_mask;
	mStatusRegister |= ((mReadVal8 & 0x1) != 0 ? C_set_mask : 0);
	uint8_t val_8_u = (mReadVal8 >> 1) & 0x7f;
	if (pInstructionInfo->writesMem) {
		if (!writeDevice(mOperandAddress, mReadVal8)) { // dummy write always made by NMOS 6502
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
bool P6502::NOPExecHdlr()
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
bool P6502::ORAExecHdlr()
{
	mAcc |=  mReadVal8;
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
bool P6502::PHAExecHdlr()

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
bool P6502::PHPExecHdlr()
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
bool P6502::PLAExecHdlr()
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
bool P6502::PLPExecHdlr()		
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
bool P6502::ROLExecHdlr()
{
	uint8_t val_8_u = ((mReadVal8 << 1) & 0xfe) | C_flag;
	mStatusRegister &= ~C_set_mask;
	mStatusRegister |= ((mReadVal8 & 0x80)!=0? C_set_mask : 0);	
	if (pInstructionInfo->writesMem) {
		if (!writeDevice(mOperandAddress, mReadVal8)) { // dummy write always made by NMOS 6502
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
bool P6502::RORExecHdlr()	
{
	uint8_t val_8_u = ((mReadVal8 >> 1) & 0x7f) | ((C_flag << 7) & 0x80);
	mStatusRegister &= ~C_set_mask;
	mStatusRegister |= ((mReadVal8 & 0x1) ? C_set_mask : 0);		
	if (pInstructionInfo->writesMem) {
		if (!writeDevice(mOperandAddress, mReadVal8)) { // dummy write always made by NMOS 6502
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
bool P6502::RTIExecHdlr()
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

	if (DBG_LEVEL_DEV(this,DBG_INTERRUPTS)) {
		DBG_LOG(this, DBG_INTERRUPTS, "RTI executed at 0x" + Utility::int2HexStr(oPC,4) + "; execution resumed at 0x" + Utility::int2HexStr(mProgramCounter,4) +"!\n");
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
bool P6502::RTSExecHdlr()
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
bool P6502::SBCExecHdlr()
{
	{
		int16_t val_C, val_V;
		if (D_flag) {

			// Calculate zero, negative & overflow flag as for non-BDC subtraction
			int16_t val_ZNV = (int8_t)mAcc - (int8_t)mReadVal8 - (1 - C_flag);
			setNZVflags((val_ZNV & 0x80) != 0, (val_ZNV & 0xff) == 0, val_ZNV < -128 || val_ZNV > 127);

			// BCD Subtraction and calculation of carry flag
			int8_t low_digit = (mAcc & 0xf) - (mReadVal8 & 0xf) - (1-C_flag); // can become -10 (0-9-1) to 9 (9-0-0)
			int8_t borrow = 0;
			if (low_digit < 0) {
				low_digit += 0x0a;
				borrow = 0x10;
			}
			low_digit &= 0x0f;
			int16_t high_digit = (mAcc & 0xf0) - (mReadVal8 & 0xf0) - borrow; // can become -0xa0 (0-0x90-0x10) to 0x90 (0x90-0-0) 
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
			val_V = (int8_t)mAcc - (int8_t)mReadVal8 - (1-C_flag);
			val_C = mAcc - mReadVal8 - (1 - C_flag);
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
bool P6502::SECExecHdlr()
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
bool P6502::SEDExecHdlr()
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
bool P6502::SEIExecHdlr()
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
bool P6502::STAExecHdlr()
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
bool P6502::STXExecHdlr()
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
bool P6502::STYExecHdlr()
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
bool P6502::TAXExecHdlr()
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
bool P6502::TAYExecHdlr()
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
bool P6502::TSXExecHdlr()
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
bool P6502::TXAExecHdlr()
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
bool P6502::TXSExecHdlr()
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
bool P6502::TYAExecHdlr()
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
bool P6502::LAXExecHdlr()

{
	mAcc = mRegisterX = mReadVal8;
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
bool P6502::SBXExecHdlr()

{
	mRegisterX = (mAcc & mRegisterX) - mReadVal8;
	setNZCflags(mRegisterX, mRegisterX >= mReadVal8);

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
bool P6502::ISCExecHdlr()
{
	uint8_t val_8_u = mReadVal8 + 1;
	int16_t val_C, val_V;

	//
	// M + 1 -> M
	//
	if (!writeDevice(mOperandAddress, mReadVal8)) { // dummy write always made by NMOS 6502
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
bool P6502::DCPExecHdlr()
{
	// M - 1
	uint8_t val_8_u = mReadVal8 - 1;
	if (!writeDevice(mOperandAddress, mReadVal8)) { // dummy write always made by NMOS 6502
		return false;
	}
	if (!writeDevice(mOperandAddress, val_8_u)) {
		return false;
	}
	mWrittenVal = val_8_u;

	//
	// A - M
	//
	val_8_u = mAcc - mReadVal8;
	setNZCflags(val_8_u, mAcc >= mReadVal8);

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
bool P6502::ANCExecHdlr()
{
	mAcc &= mReadVal8;

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
bool P6502::ALRExecHdlr()
{
	// AND
	uint8_t val_before_shift = mAcc & mReadVal8;

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
bool P6502::ARRExecHdlr()
{
	// AND
	uint8_t val_before_shift = mAcc & mReadVal8;

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
bool P6502::LASExecHdlr()
{
	mAcc = mRegisterX = mStackPointer = mReadVal8 & mStackPointer;
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
bool P6502::RLAExecHdlr()
{
	// ROL
	uint8_t val_8_u = ((mReadVal8 << 1) & 0xfe) | C_flag;
	mStatusRegister &= ~C_set_mask;
	mStatusRegister |= ((mReadVal8 & 0x80) != 0 ? C_set_mask : 0);

	// Write back the result to memory
	if (!writeDevice(mOperandAddress, mReadVal8)) { // dummy write always made by NMOS 6502
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
bool P6502::RRAExecHdlr()
{
	// ROR
	uint8_t rotated_val = ((mReadVal8 >> 1) & 0x7f) | ((C_flag << 7) & 0x80);
	mStatusRegister &= ~C_set_mask;
	mStatusRegister |= ((mReadVal8 & 0x1) ? C_set_mask : 0);
	if (!writeDevice(mOperandAddress, mReadVal8)) { // dummy write always made by NMOS 6502
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
bool P6502::SAXExecHdlr()
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
bool P6502::SLOExecHdlr()
{
	// ASL
	mStatusRegister &= ~C_set_mask;
	mStatusRegister |= ((mReadVal8 & 0x80) != 0 ? C_set_mask : 0);
	uint8_t val_8_u = (mReadVal8 << 1) & 0xfe;
	if (!writeDevice(mOperandAddress, mReadVal8)) { // dummy write always made by NMOS 6502
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
bool P6502::SREExecHdlr()
{
	// LSR
	mStatusRegister &= ~C_set_mask;
	mStatusRegister |= ((mReadVal8 & 0x1) != 0 ? C_set_mask : 0);
	uint8_t val_8_u = (mReadVal8 >> 1) & 0x7f;
	if (pInstructionInfo->writesMem) {
		if (!writeDevice(mOperandAddress, mReadVal8)) { // dummy write always made by NMOS 6502
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
bool P6502::undefinedExecHdlr()
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
// mReadVal8:			value read from memory or from internal register (A,X,Y,SR,PC) by the specific instruction (when applicable)
//



//
// Implied addressing - with a result
// 
// Implicit (no explicit operand)
// 
// e.g., LSR A
//
bool P6502::accAdrHdlr()
{
	// Save the value resulting from the evaluation of the operand
	mReadVal8 = mAcc;

	return true;
}

//
// Implied addressing - without a result
// 
// Implicit (no explicit operand)
// 
// e.g., SEC
//
bool P6502::impliedAdrHdlr()
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
bool P6502::relativeAdrHdlr()
{
	
	uint8_t rel_a;
	if (!readProgramMem(mProgramCounter++, rel_a))
		return false;

	// Save the constant numeric part of the operand for later use when executing the specific instruction
	mOperand16 = (int8_t) rel_a;

	// Save the calculated address for use when executing the specific instruction later on
	mOperandAddress = (mOperand16 + mProgramCounter) & 0xffff;

	return true;

}

bool P6502::addBranchTakenCycles()
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
bool P6502::immediateAdrHdlr()
{
	
	if (!readProgramMem(mProgramCounter++, mReadVal8))
		return false;

	// Save the constant numeric part of the operand for later use when executing the specific instruction
	mOperand16 = mReadVal8;

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
bool P6502::zeroPageAdrHdlr()
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
	// to make it available as 'mReadVal8' later on when executing the instruction
	if (pInstructionInfo->readsMem && !readZP(zp_a, mReadVal8))
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
bool P6502::zeroPageXAdrHdlr()
{

	// Read address operand
	uint8_t zp_a;
	if (!readProgramMem(mProgramCounter++, zp_a))
		return false;

	// Save the constant numeric part of the operand for later use when executing the specific instruction
	mOperand16 = zp_a;

	// Calculate address and save it for use when executing the specific instruction later on
	mOperandAddress = (uint8_t) (zp_a + mRegisterX);

	// If the instruction reads from the calculated memory address (e.g., LDA, INC but not STA), then pre-read it
	// to make it available as 'mReadVal8' later on when executing the instruction
	if (pInstructionInfo->readsMem && !readZP(mOperandAddress, mReadVal8))
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
bool P6502::zeroPageYAdrHdlr()
{
	
	// Read address operand
	uint8_t zp_a;
	if (!readProgramMem(mProgramCounter++, zp_a))
		return false;

	// Save the constant numeric part of the operand for later use when executing the specific instruction
	mOperand16 = zp_a;

	// Calculate address and save it for use when executing the specific instruction later on
	mOperandAddress = (uint8_t) (zp_a + mRegisterY);

	// If the instruction reads from the calculated memory address (e.g., LDA, INC but not STA), then pre-read it
	// to make it available as 'mReadVal8' later on when executing the instruction
	if (pInstructionInfo->readsMem && !readZP(mOperandAddress, mReadVal8))
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
bool P6502::absoluteAdrHdlr()
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
	// to make it available as 'mReadVal8' later on when executing the instruction
	if (pInstructionInfo->readsMem && !readDevice(mOperandAddress, mReadVal8))
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
bool P6502::absoluteXAdrHdlr()
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
	// to make it available as 'mReadVal8' later on when executing the instruction
	if (pInstructionInfo->readsMem && !readDevice(mOperandAddress, mReadVal8))
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
bool P6502::absoluteYAdrHdlr()
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
	// to make it available as 'mReadVal8' later on when executing the instruction
	if (pInstructionInfo->readsMem && !readDevice(mOperandAddress, mReadVal8))
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
bool P6502::indirectAdrHdlr()
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
	if (!readDevice(adr_i, a_L) || !readDevice(adr_i+1, a_H))
		return false;

	// Calculate address and save it for use when executing the specific instruction later on
	mOperandAddress = (a_H << 8) | a_L;

	// If the instruction reads from the calculated memory address (e.g., LDA, INC but not STA), then pre-read it
	// to make it available as 'mReadVal8' later on when executing the instruction
	if (pInstructionInfo->readsMem && !readDevice(mOperandAddress, mReadVal8))
		return false;

	// Add one cycle if page boundary is crossed fpr the indirect address
	if (pInstructionInfo->addCycleAtPageBoundary && (adr_i ^ (adr_i+1)) != 0)
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
bool P6502::preIndXAdrHdlr()
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
	// to make it available as 'mReadVal8' later on when executing the instruction
	if (pInstructionInfo->readsMem && !readDevice(mOperandAddress, mReadVal8))
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
bool P6502::postIndYAdrHdlr()
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
	if (!readZP((uint16_t)zp_a, a_L) || !readZP((uint16_t) zp_a_1, a_H))
		return false;
	uint16_t mem_a = (a_H << 8) | a_L;

	// Calculate address and save it for use when executing the specific instruction later on
	mOperandAddress = mem_a + mRegisterY;

	// If the instruction reads from the calculated memory address (e.g., LDA, INC but not STA), then pre-read it
	// to make it available as 'mReadVal8' later on when executing the instruction
	if (pInstructionInfo->readsMem && !readDevice(mOperandAddress, mReadVal8))
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
bool P6502::undefinedAdrHdlr()
{
	
	return false;
		
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


void  P6502::adjustForWaitStates(MemoryMappedDevice* dev)
{
	// Add wait states if applicable
	int wait_states = dev->getWaitStates();
	if (wait_states > 0) {
		mCycleCount += mCycleCount % 2; // synchronise with CPU Clock phase
		mCycleCount += wait_states; // add extra memory access cycles
	}
}
//
//
bool P6502::readDevice(uint16_t adr, uint8_t& data)
{
	
	MemoryMappedDevice* mem_dev;
	if ((mem_dev = mDeviceManager->getSelectedMemoryMappedDevice(adr)) != NULL) {
		adjustForWaitStates(mem_dev);// Add wait states if applicable
		bool success = mem_dev->read(adr, data);
		return success;
	}

	DBG_LOG(this, DBG_WARNING, "*Warning* Read at unmapped address 0x" + Utility::int2HexStr(adr,4) + ". Returns 0x0 for all unmapped addresses...\n");

	data = 0x0;// Better to return 0x00 than 0xff for now when not all devices are implemented as peripheral status 0x00 usually means inactive/no event
	return true;
}

bool P6502::readZP(uint8_t adr, uint8_t &data)
{
	data = 0xff;
	if (mZPMemDev != NULL && mZPMemDev->selected(adr)) {
		adjustForWaitStates(mZPMemDev);// Add wait states if applicable
		return mZPMemDev->read(adr, data);
	}
	return false;
}

bool P6502::readProgramMem(uint16_t adr, uint8_t& data)
{
	return readProgramMem(adr, data, true);
}

bool P6502::readProgramMem(uint16_t adr, uint8_t& data, bool adjustTiming)
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

bool P6502::writeDevice(uint16_t adr, uint8_t data)
{
	
	MemoryMappedDevice* dev;
	if ((dev = mDeviceManager->getSelectedMemoryMappedDevice(adr)) != NULL) {
		adjustForWaitStates(dev);// Add wait states if applicable
		return dev->write(adr, data);
	}


	return false;
}

void P6502::push(uint8_t v)
{
	uint16_t adr = 0x100 + (uint16_t)mStackPointer--;
	if (mStackMemDev != NULL && mStackMemDev->selected(adr)) {
		adjustForWaitStates(mStackMemDev);// Add wait states if applicable
		mStackMemDev->write(adr, v);
	}
}

void P6502::pull(uint8_t& v)
{
	uint16_t adr = 0x100 + (uint16_t)++mStackPointer;
	if (mStackMemDev != NULL && mStackMemDev->selected(adr)) {
		adjustForWaitStates(mStackMemDev);// Add wait states if applicable
		mStackMemDev->read(adr, v);
	}
}

void P6502::pushWord(uint16_t word)
{
	push(word >> 8);
	push(word & 0xff);
}

void P6502::pullWord(uint16_t& word)
{
	uint8_t low, high;
	pull(low);
	pull(high);
	word = (high << 8) | low;
}

string P6502::stack2Str()
{
	if (mStackPointer < 0xff) {
		stringstream sout;
		uint16_t a = (0x100 + mStackPointer + 1) & 0x1ff;
		uint8_t l, h;
		readProgramMem(a, l, false);
		readProgramMem(a + 1, h, false);
		uint16_t w = (h << 8) | l;
		sout << "mem[" << hex << setw(3) << setfill('0') << a << "]=" << setw(2) << setfill('0') << hex << w;
		return sout.str();
	}
	else
		return "";

}

string P6502::getInterruptStack(uint16_t stackStart, uint16_t oStackPointer, uint16_t oProgramCounter, uint16_t oStatusRegister)
{
	string s;
	s += "\n\tStackPointer 0x" + Utility::int2HexStr(0x100 + oStackPointer,4) + " => 0x" + Utility::int2HexStr(0x100 + mStackPointer,4) + "\n";
	s += "\tStatusregister NV--DIZC:" + Utility::int2BinStr(oStatusRegister & 0xdf,8) + " => 0x" + Utility::int2BinStr(mStatusRegister & 0xdf,8) + "\n";
	s += "\tProgramCounter 0x" + Utility::int2HexStr(oProgramCounter,4) + " => 0x" + Utility::int2HexStr(mProgramCounter,4) + "\n";
	s += "\tInterrupt Stack:\n";
	for (int a = 0x100 + stackStart; a < 0x100 + stackStart + 3; a++) {
		uint8_t d;
		readProgramMem(a, d, false);
		if (a == 0x100 + stackStart)
			s += "\tmem[0x" + Utility::int2HexStr(a,4) + "] = 0x"  + Utility::int2HexStr(d,2) + " <=> NV-(B)DIZC " + Utility::int2BinStr(d,8) + "\n";
		else
			s += "\tmem[0x" + Utility::int2HexStr(a, 4) + "] = 0x" + Utility::int2HexStr(d, 2) + "\n";
	}
	return s;
}

string P6502::getCallStack()
{
	string s = "Call Stack:\n";
	int start_adr = 0x100 + mStackPointer + 1;
	for (int a = start_adr; a < start_adr + 2; a++) {
		uint8_t d;
		readProgramMem(a, d, false);
		s += "\tmem[0x" + Utility::int2HexStr(a, 4) + "] = 0x" + Utility::int2HexStr(d, 2) + "\n";
	}
	return s;
}

// Outputs the internal state of the device
bool P6502::outputState(ostream& sout)
{
	uint16_t pc = mProgramCounter;
	uint16_t a = pc;
	string n_instr_s;

	// Read up to three bytes, if possible
	vector<uint8_t> bytes = {0, 0, 0};
	if (!readProgramMem(a++, bytes[0], false))
		return false;
	if (readProgramMem(a, bytes[1], false))
		a++;
	if (readProgramMem(a, bytes[2], false))
		a++;

	if (!mCodec.decodeInstrFromBytes(pc, bytes, n_instr_s, false)) {
		return false;
	}
	string x_instr_s = mCodec.decode(mOpcodePC, mOpcode, mOperand16);
	
	sout << "Executed instruction:    " << setfill(' ') << setw(30) << left << x_instr_s << "\n";
	sout << "Resulting State:         " << getState() << "\n";
	sout << "Next instruction:        " << setfill(' ') << setw(30) << left << n_instr_s << "\n";
	sout << "IRQ = " << (int)mIRQ << "\n";
	sout << "RESET = " << (int)mRESET << "\n";
	sout << "NMI = " << (int)mNMI << "\n";

	return true;
}

bool P6502::getInstrLogData(InstrLogData& instrLogData) {
	instrLogData.logTime = getCycleCount() / (mCPUClock * 1e6);
	instrLogData.instr = pInstructionInfo;
	instrLogData.A = mAcc;
	instrLogData.X = mRegisterX;
	instrLogData.Y = mRegisterY;
	instrLogData.SP = mStackPointer;
	instrLogData.SR = mStatusRegister;
	instrLogData.opcodePC = mOpcodePC;
	instrLogData.PC = mProgramCounter;
	instrLogData.opcode = mOpcode;
	instrLogData.operand = mOperand16;
	instrLogData.accessAdr = mOperandAddress;
	instrLogData.activeIRQ = (!mIRQ && mIrqTransition);
	instrLogData.activeNMI = (!mNMI && mNmiTransition);
	instrLogData.execFailure = !mExecSuccess;
	instrLogData.readVal = mReadVal8;
	instrLogData.writtenVal = mWrittenVal;
	uint16_t a = (0x100 + mStackPointer + 1) & 0x1ff;
	uint8_t l, h;
	readProgramMem(a, l);
	readProgramMem(a + 1, h);
	uint16_t w = (h << 8) | l;
	instrLogData.stack = w;

	instrLogData.cycles = mExecutedCycles;

	if (mMemLogAdr > 0 && mDeviceManager != NULL) {
		uint8_t data;
		mDeviceManager->dumpDeviceMemory(mMemLogAdr, data);
		instrLogData.memContent = (int)data;
	}

	return true;
}

bool P6502::printInstrLogData(ostream& sout, InstrLogData& instrLogData)
{
	string instr_s = mCodec.decode(instrLogData.opcodePC, instrLogData.opcode, instrLogData.operand);
	Codec6502::InstructionInfo instr = *instrLogData.instr;

	string t_s = Utility::encodeCPUTime(instrLogData.logTime);

	sout << t_s << " [" << instrLogData.cycles << "] " << setfill(' ') << setw(30) << left << instr_s << right <<
		" " << hex << setfill('0') <<
		"A:" << setw(2) << (int)instrLogData.A <<
		" X:" << setw(2) << (int)instrLogData.X <<
		" Y:" << setw(2) << (int)instrLogData.Y <<
		" SP:" << setw(2) << (int)instrLogData.SP <<
		" NV--DIZC:" << setw(8) << bitset<8>(instrLogData.SR & 0xdf) <<
		setw(4) <<
		" PC:" << setw(2) << instrLogData.PC;
	if (instr.readsMem || instr.writesMem)
		sout << " ACCESSED 0x" << hex << setfill('0') << setw(4) << instrLogData.accessAdr;
	if (instr.readsMem)
		sout << " READ 0x" << setw(2) << (int)instrLogData.readVal;
	if (instr.writesMem)
		sout << " WROTE 0x" << setw(2) << (int)instrLogData.writtenVal;

	sout << " ";

	uint16_t stack_adr = (0x100 + instrLogData.SP + 1) & 0x1ff;
	sout << "Mem[" << hex << setw(3) << setfill('0') << stack_adr << "]=" << setw(2) << setfill('0') << hex << instrLogData.stack;

	if (instrLogData.execFailure)
		sout << " EXEC FAILURE";
	if (instrLogData.activeIRQ)
		sout << " *IRQ";
	if (instrLogData.activeNMI)
		sout << " *NMI";

	if (instrLogData.memContent != -1)
		sout << " Mem[0x" << hex << mMemLogAdr << "]=0x" << instrLogData.memContent;


	return true;
}