
#include "P6502CC.h"

int  P6502CC::getWaitStates(MemoryMappedDevice* dev)
{
	// Add wait states if applicable
	int wait_states = dev->getWaitStates();
	if (wait_states > 0)
		return (mCycle % 2) + wait_states; // synchronise with CPU Clock phase and add extra memory access cycles
	return 0;
}

// Check if a page boundary is crossed between two addresses (used for some instructions to determine if an extra cycle is required)
bool P6502CC::pageBoundaryCrossed(uint16_t before, uint16_t after)
{
	return (((before >> 8) ^ (after >> 8)) != 0);
}

P6502CC::P6502CC(string name, double deviceClockRate, double tickRate, DebugTracing* debugTracing, ConnectionManager* connectionManager, DeviceManager* deviceManager) :
	P6502(name, P6502_DEV, MICROROCESSOR_DEVICE, debugTracing, connectionManager, deviceManager)
{

}

P6502CC::~P6502CC()
{

}

// Execute a complete instruction (including all its micro cycles) and advance the device time accordingly. Returns false if an error occurs.
// The endTick parameter is set to the device time after the execution of the instruction
bool P6502CC::advanceInstr(uint64_t& endTick)
{
	int micro_cycles = 0;
	int effective_cycles = 0; // safeguard to put an upper limit on the number of microcycles that can be executed for an instruction to prevent infinite loops in case of a bug in the code
	CPUExecState prev_state = UNDEFINED;
	mExecSuccess = true;
	while (prev_state != FETCH_OPCODE && effective_cycles++ < 100) {
		// Keep executing microcycles of the current instruction until the next instruction's opcode needs to be fetched
		MemoryMappedDevice* dev;
		if (mPendingWaitStates == 0) {
			// New memory acess - check if the address is mapped to a device and if so add wait states if applicable
			if ((dev = mDeviceManager->getSelectedMemoryMappedDevice(mADDRESS)) != NULL)
				mPendingWaitStates = getWaitStates(dev);
			else {
				mPendingWaitStates = 0; // No device mapped to the accessed address - no wait states
			}
		}
		if (mPendingWaitStates == 0) {
			if (mRW == 0) {
				// Write operation - write mDATA to the device at mADDRESS
				if (dev != nullptr) {
					dev->write(mADDRESS, mDATA);
					mWrittenVal = mDATA;
				}
			}
			else {
				// Read operation - read from the device at mADDRESS into mDATA
				if (dev != nullptr) {
					dev->read(mADDRESS, mDATA);
					if (!mSYNC) {
						mReadVal = mDATA;
						if (mReadingOperandByte && mOperandByteCount < 1)
							mOperandBytes[mOperandByteCount++] = mDATA;
					}
				}
				else
					mDATA = 0xff;
			}
			if (!executeInstrMicroCycle()) {
				mExecSuccess = false;
			}; // Execute one microcycle
			micro_cycles++;
		}
		else {
			--mPendingWaitStates; // Decrease pending wait states until the memory access can be executed
			advanceTimeOnly(1); // Advance the device time (but not the cycle count!) while waiting for the memory access to complete
		}
		prev_state = mCPUExecState;
	}

	if (micro_cycles > 10) {
		cout << "Error: instruction at PC " << hex << mOpcodePC << " took more than 10 microcycles (" << micro_cycles << ") to execute. Possible infinite loop in code.\n";
	}

	// Advance device time and cycle count by the number of microcycles executed for the instruction
	tick(micro_cycles); 

	// Save information about the executed instruction for debugging and tracing purposes
	mExecutedCycles = micro_cycles;
	if (mOperandByteCount == 1)
		mOperand16 = mOperandBytes[0];
	else if (mOperandByteCount == 2)
		mOperand16 = (mOperandBytes[1] << 8) | mOperandBytes[0];
	else
		mOperand16 = 0;
	mRAccAdr = mWAccAdr = -1;
	if (pInstructionInfo != nullptr) {
		if (pInstructionInfo->readsMem)
			mRAccAdr = mOperandAddress;
		if (pInstructionInfo->writesMem)
			mWAccAdr = mOperandAddress;
	}


	endTick = mTicks;

	return true;
}

// Initialise the CPU state for fetching the next instruction (fetching the opcode of the next instruction will be done in the next cycle by executeInstrMicroCycle)
bool P6502CC::initFetch()
{
	// Log previously executed instruction if enabled
	if (DBG_LEVEL_DEV(this, DBG_6502)) {
		InstrLogData instr_log_data;
		getInstrLogData(instr_log_data);
		stringstream sout;
		printInstrLogData(sout, instr_log_data);
		DBG_LOG(this, DBG_6502, sout.str());
	}

	int observed_cycles = mOPerandMicroCycle + mExecMicroCycle + 1;
	if (mExpectedCycles != observed_cycles) {
		cout << "Expected " << mExpectedCycles << " cycles for instruction at PC " << hex << mOpcodePC << " but got " << dec << observed_cycles << " cycles\n";
		cout << "operand micro cycles = " << mOPerandMicroCycle << " and execution micro cycles =  " << mExecMicroCycle + 1 << "\n";
	}

	// Uppdate the RW, SYNC & ADDRESS ports for the opcode fetch of the next instruction
	updatePort(RW, 1); // Set R/W port to read mode
	updatePort(SYNC, 1); // Set SYNC port to indicate opcode fetch
	update16BitPort(ADDRESS,mProgramCounter); // Set the address bus to the program counter for opcode fetch

	// Reset the CPU execution state and microcycle counters for the next instruction
	mCPUExecState = FETCH_OPCODE; // Set the CPU execution state to (in the next cycle) fetch the opcode of the next instruction
	mOpcodePC = mProgramCounter;
	mOPerandMicroCycle = 0; // Reset the operand microcycle counter for the next instruction
	mExecMicroCycle = 0; // Reset the execution microcycle counter for the next instruction
	mExpectedCycles = 0;

	// Advance the program counter to point either to the first operand byte or (/if there are no operands) the next instruction's opcode 
	mProgramCounter++;

	return true;
}

bool P6502CC::completeFetch()
{
	mOpcode = mDATA;
	pInstructionData = &(pInstrDataTbl->data[mOpcode]); // Execution info
	pInstructionInfo = &(pInstructionData->info); // Opcode info only

	mExpectedCycles = pInstructionInfo->cycles;

	return true;
}

bool P6502CC::prepMemRead(uint16_t adr)
{
	updatePort(RW, 1); // Set R/W port to read mode
	updatePort(SYNC, 0); // Set SYNC port to indicate a non-opcode read operation
	update16BitPort(ADDRESS, adr); // Set the address bus to the read address

	return true;
}

bool P6502CC::initMemRead(uint16_t adr)
{
	mReadingOperandByte = false;
	return prepMemRead(adr);
	return true;
}

bool P6502CC::initMemWrite(uint16_t adr, uint8_t data)
{
	updatePort(RW, 0); // Set R/W port to read mode
	updatePort(SYNC, 0); // Set SYNC port to indicate a non-opcode read operation
	update16BitPort(ADDRESS, adr); // Set the address bus to the read address
	updatePort(DATA, data); // Set the data bus to the value to be written

	return true;
}

bool P6502CC::initOperandByteRead()
{
	mReadingOperandByte = true;
	return initMemRead(mProgramCounter++);
}

bool P6502CC::initDummyByteRead()
{
	mReadingOperandByte = false;
	return initMemRead(mProgramCounter++);
}
bool P6502CC::executeInstrMicroCycle()
{
	mResetTransition = mRESET != pRESET;
	pRESET = mRESET;

	mIrqTransition = mIRQ != pIRQ;
	pIRQ = mIRQ;

	mNmiTransition = mNMI != pNMI;
	pNMI = mNMI;

	mSOTransition = mSO != pSO;
	pSO = mSO;

	if (mRESET != 0) {

		if (pRESET != mRESET) {
			mResetTransition = true;
			mCPUExecState = IN_RESET; // Set the CPU execution state to (in the next cycle) be in reset processing
			mOPerandMicroCycle = 0; // Reset the operand microcycle counter for the reset sequence
			mExecMicroCycle = -1; // Reset the execution microcycle counter (even if not used for the reset sequence)
			mExpectedCycles = 7;
		}

		if (!mIRQ && !I_flag) {
			mCPUExecState = IN_IRQ; // Set the CPU execution state to (in the next cycle) be in IRQ processing
			mOPerandMicroCycle = 0; // Reset the operand microcycle counter for the reset sequence
			mExecMicroCycle = -1; // Reset the execution microcycle counter (even if not used for the reset sequence)
			mExpectedCycles = 7;
		}

		if (!mNMI && mNmiTransition) {
			mIrqTransition = true;
			mCPUExecState = IN_NMI; // Set the CPU execution state to (in the next cycle) be in NMI processing
			mOPerandMicroCycle = 0; // Reset the operand microcycle counter for the reset sequence
			mExecMicroCycle = -1; // Reset the execution microcycle counter (even if not used for the reset sequence)
			mExpectedCycles = 7;
		}
		else if (mNMI)
			mNmiTransition = false;
		
		// Execute RESET sequence if in reset state (state will be changed to FETCH_OPCODE by reset() when the reset sequence is completed)
		if (mCPUExecState == IN_RESET)
			return resetHdlr();

		// Execute NMI or IRQ sequence if in NMI or IRQ state (state will be changed to FETCH_OPCODE by serveNMI() or serveIRQ() when the NMI or IRQ sequence is completed)
		if (mCPUExecState == IN_NMI)
			return serveNMI();
		if (mCPUExecState == IN_IRQ)
			return serveIRQ();


		// Execute the instruction handler for the current instruction and addressing mode
		if (mCPUExecState == FETCH_OPCODE) {
			completeFetch(); // Fetch the opcode and instruction info for the current instruction
			mCPUExecState = FETCH_OPERAND; // Set the CPU execution state to (in the next cycle) fetch the operand of the current instruction
		}
		if (mCPUExecState == FETCH_OPERAND)
			return (this->*(pInstructionData->addrHdlr))(); // Fetch the operand bytes and perform any related read operations (or prepare fetching of the next opcode if there were no operand bytes)
		else
			return (this->*(pInstructionData->execHdlr))(); // Execute the instruction - takes at least one cycle and the last cycle includes preparing for the fetching of the next instruction's opcode
	}

	return true;
}


bool P6502CC::reset()
{
	mCPUExecState = IN_RESET; // Set the CPU execution state to (in the next cycle) be in reset
	mOPerandMicroCycle = 0; // Reset the operand microcycle counter for the reset sequence
	mExecMicroCycle = -1; // Reset the execution microcycle counter (even if not used for the reset sequence)
	mExpectedCycles = 7;

	return true;
}

//
// The 6502 takes 7 cycles to reset in a similar way as for an interrupt, but slightly modified in that it has dummy read accesses to the stack.
// During these 7 cycles, there are dummy stack read accesses at 00FF, 0100, 01FF, and 01FE followed by the read of the reset vector at FFFC.
//
bool P6502CC::resetHdlr()
{
	Device::reset();

	switch (mOPerandMicroCycle++) {
	case 0:initMemRead(0x00ff); return true; // dummy read always made by NMOS 6502
	case 1:initMemRead(0x0100); return true; // dummy read always made by NMOS 6502
	case 2:initMemRead(0x01ff); return true; // dummy read always made by NMOS 6502
	case 3:initMemRead(0x01fe); return true; // dummy read always made by NMOS 6502
	case 4:initMemRead(0xfffc); return true; // dummy read always made by NMOS 6502
	case 5:mResetVecLow = mDATA; initMemRead(0xfffd); return true; // read low byte of reset vector
	case 6:mProgramCounter = (mDATA << 8) | mResetVecLow; initFetch();; return true; // read high byte of reset vector
	default:
		return false;
	}

}

// Servicing of NMI and IRQ
bool P6502CC::serveNMI() { return true; }
bool P6502CC::serveIRQ() { return true; }





//
// Addressing modes
//

// Instructions with accumulator addressing mode (like ASL A)
bool P6502CC::accAdrHdlr()
{
	return impliedAdrHdlr();
}

// Instructions with implied or accumulator addressing mode (like ASL A, NOP or CLC)
bool P6502CC::impliedAdrHdlr()
{
	switch (mOPerandMicroCycle++) {
	
	case 0:
		initDummyByteRead(); mCPUExecState = EXECUTE_INSTRUCTION; return true; // dummy read always made by NMOS 6502
	
	default:
		return false;
	}

}

bool P6502CC::immediateAdrHdlr()
{
	switch (mOPerandMicroCycle++) {

	case 0:
		initOperandByteRead(); mCPUExecState = EXECUTE_INSTRUCTION; return true; // iniitialize the read of immediate operand byte (the read will be performed in the next cycle by executeInstrMicroCycle)

	default:
		return false;
	}

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
bool P6502CC::relativeAdrHdlr()
{
	switch (mOPerandMicroCycle++) {

	case 0:
		initOperandByteRead(); mCPUExecState = EXECUTE_INSTRUCTION;
		return true;
	
	default:
		return false;
	}
}

bool P6502CC::zeroPageAdrHdlr()
{
	return true;
}

bool P6502CC::zeroPageXAdrHdlr()
{
	return true;
}

bool P6502CC::zeroPageYAdrHdlr()
{
	return true;
}

bool P6502CC::absoluteAdrHdlr()
{
	switch (mOPerandMicroCycle++) {

	case 0:
		initOperandByteRead();
		return true;

	case 1:
		mOperandAddress = mDATA; initOperandByteRead(); mCPUExecState = EXECUTE_INSTRUCTION;
		return true;
	
	default:
		return false;
	}

}

bool P6502CC::absoluteXAdrHdlr()
{
	return true;
}

bool P6502CC::absoluteYAdrHdlr()
{
	return true;
}

bool P6502CC::indirectAdrHdlr()
{
	return true;
}

bool P6502CC::preIndXAdrHdlr()
{
	return true;
}

bool P6502CC::postIndYAdrHdlr()
{
	return true;
}

bool P6502CC::undefinedAdrHdlr()
{
	return true;
}




//
// Instruction Execution
//



bool P6502CC::ADCExecHdlr() { return true; }
bool P6502CC::ANDExecHdlr() { return true; }

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
// Mode			Syntax		OPC	Bytes	Cycles
// accumulator	ASL A		0A	1		2
// zeropage		ASL oper	06	2		5
// zeropage,X	ASL oper,X	16	2		6
// absolute		ASL oper	0E	3		6
// absolute,X	ASL oper,X	1E	3		7
//
bool P6502CC::ASLExecHdlr()
{

	switch (mExecMicroCycle++) {

	case 0:
		if (!pInstructionInfo->writesMem)
			mReadVal = mAcc;
		else
			mReadVal = mDATA;

		mStatusRegister &= ~C_set_mask;
		mStatusRegister |= ((mReadVal & 0x80) != 0 ? C_set_mask : 0);
		mCalcVal = (mReadVal << 1) & 0xfe;
		setNZflags(mCalcVal);

		if (!pInstructionInfo->writesMem)
			initFetch();
		else
			initMemWrite(mOperandAddress, mReadVal); // dummy write always made by NMOS 6502

		return true;

	case 1:
		initMemWrite(mOperandAddress, mCalcVal);
		return true;

	case 2:
		initFetch();
		return true;

	default:
		return false;
	}

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
bool P6502CC::BCCExecHdlr()
{
	if (!C_flag) {
		// Branch taken, so set the program counter to the branch target address and fetch the next instruction in one or two cycles (depending on if a page boundary is crossed or not)
		switch (mExecMicroCycle++) {
		case 0:	
			mExpectedCycles++;
			mOperandAddress = ((int8_t)mDATA + mProgramCounter) & 0xffff;
			if (!pageBoundaryCrossed(mProgramCounter, mOperandAddress)) {
				mExpectedCycles++;
				initDummyByteRead(); // A dummy read is always made
				return true;
			}
			mProgramCounter = mOperandAddress;
			initFetch();
			return true;
		case 1:
			mProgramCounter = mOperandAddress;
			initFetch();
			return true;
		default:
			return false;
		}
	}
	// No branch taken, so just fetch the next instruction
	initFetch();
	return true;
}


bool P6502CC::BCSExecHdlr() { return true; }
bool P6502CC::BEQExecHdlr() { return true; }
bool P6502CC::BITExecHdlr() { return true; }
bool P6502CC::BMIExecHdlr() { return true; }
bool P6502CC::BNEExecHdlr() { return true; }
bool P6502CC::BPLExecHdlr() { return true; }
bool P6502CC::BRKExecHdlr() { return true; }
bool P6502CC::BVCExecHdlr() { return true; }
bool P6502CC::BVSExecHdlr() { return true; }
bool P6502CC::CLCExecHdlr() { return true; }



//
// CLD
//
// Clear Decimal Mode
// 
// N	Z	C	I	D	V
// -	-	-	-	0	-
//
bool P6502CC::CLDExecHdlr()
{
	mStatusRegister &= ~D_set_mask;

	initFetch();
	return true;
}

bool P6502CC::CLIExecHdlr() { return true; }
bool P6502CC::CLVExecHdlr() { return true; }
bool P6502CC::CMPExecHdlr() { return true; }
bool P6502CC::CPXExecHdlr() { return true; }
bool P6502CC::CPYExecHdlr() { return true; }
bool P6502CC::DECExecHdlr() { return true; }
bool P6502CC::DEXExecHdlr() { return true; }
bool P6502CC::DEYExecHdlr() { return true; }
bool P6502CC::EORExecHdlr() { return true; }
bool P6502CC::INCExecHdlr() { return true; }
bool P6502CC::INXExecHdlr() { return true; }
bool P6502CC::INYExecHdlr() { return true; }
bool P6502CC::JMPExecHdlr() { return true; }
bool P6502CC::JSRExecHdlr() { return true; }


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
//
// Mode			Syntax		OPC	Bytes	Cycles
// immediate	LDA #oper	A9	2		2  
// zeropage		LDA oper	A5	2		3
// zeropage,X	LDA oper,X	B5	2		4
// absolute		LDA oper	AD	3		4
// absolute,X	LDA oper,X	BD	3		4+
// absolute,Y	LDA oper,Y	B9	3		4+
// (indirect,X)	LDA(oper,X)	A1	2		6
// (indirect),Y	LDA(oper),Y	B1	2		5+

bool P6502CC::LDAExecHdlr()
{
	switch (mExecMicroCycle++) {

	case 0:
		mReadVal = mDATA;
		setNZflags(mAcc);
		initFetch();
		return true;

	default:
		return false;
	}

}



bool P6502CC::LDXExecHdlr() { return true; }
bool P6502CC::LDYExecHdlr() { return true; }
bool P6502CC::LSRExecHdlr() { return true; }
bool P6502CC::NOPExecHdlr() { return true; }
bool P6502CC::ORAExecHdlr() { return true; }
bool P6502CC::PHAExecHdlr() { return true; }
bool P6502CC::PHPExecHdlr() { return true; }
bool P6502CC::PLAExecHdlr() { return true; }
bool P6502CC::PLPExecHdlr() { return true; }
bool P6502CC::ROLExecHdlr() { return true; }
bool P6502CC::RORExecHdlr() { return true; }
bool P6502CC::RTIExecHdlr() { return true; }
bool P6502CC::RTSExecHdlr() { return true; }
bool P6502CC::SBCExecHdlr() { return true; }
bool P6502CC::SECExecHdlr() { return true; }
bool P6502CC::SEDExecHdlr() { return true; }
bool P6502CC::SEIExecHdlr() { return true; }


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
// Mode			Syntax		OPC	Bytes	Cycles
// zeropage		STA oper	85	2		3  
// zeropage,X	STA oper,X	95	2		4
// absolute		STA oper	8D	3		4
// absolute,X	STA oper,X	9D	3		5
// absolute,Y	STA oper,Y	99	3		5
// (indirect,X)	STA(oper,X)	81	2		6
// (indirect),Y	STA(oper),Y	91	2		6
//
bool P6502CC::STAExecHdlr()
{
	switch (mExecMicroCycle++) {

	case 0:
		mOperandAddress = mOperandAddress | (mDATA << 8);
		initMemWrite(mOperandAddress, mAcc);
		return true;
	case 1:
		initFetch();
		return true;

	default:
		return false;
	}

}


bool P6502CC::STXExecHdlr() { return true; }
bool P6502CC::STYExecHdlr() { return true; }
bool P6502CC::TAXExecHdlr() { return true; }
bool P6502CC::TAYExecHdlr() { return true; }
bool P6502CC::TSXExecHdlr() { return true; }
bool P6502CC::TXAExecHdlr() { return true; }
bool P6502CC::TXSExecHdlr() { return true; }
bool P6502CC::TYAExecHdlr() { return true; }
bool P6502CC::LAXExecHdlr() { return true; }
bool P6502CC::SBXExecHdlr() { return true; }
bool P6502CC::ISCExecHdlr() { return true; }
bool P6502CC::DCPExecHdlr() { return true; }
bool P6502CC::ANCExecHdlr() { return true; }
bool P6502CC::ALRExecHdlr() { return true; }
bool P6502CC::ARRExecHdlr() { return true; }
bool P6502CC::LASExecHdlr() { return true; }
bool P6502CC::RLAExecHdlr() { return true; }
bool P6502CC::RRAExecHdlr() { return true; }
bool P6502CC::SAXExecHdlr() { return true; }
bool P6502CC::SLOExecHdlr() { return true; }
bool P6502CC::SREExecHdlr() { return true; }
bool P6502CC::undefinedExecHdlr() { return true; }