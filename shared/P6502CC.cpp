
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
	mPageBoundaryCrossed =(((before >> 8) ^ (after >> 8)) != 0);
	return mPageBoundaryCrossed;
}


P6502CC::P6502CC(string name, double deviceClockRate, double tickRate, DebugTracing* debugTracing, ConnectionManager* connectionManager, DeviceManager* deviceManager) :
	P6502(name, P6502_DEV, MICROROCESSOR_DEVICE, debugTracing, connectionManager, deviceManager)
{
	

	// Modify address handler entry for JSR to point to the execution handler instead
	// - needed as JSR doesn't follow the normal micro cycle pattern for absolute mode
	pInstrDataTbl->data[0x20].addrHdlr = static_cast<bool (P6502::*)()>(&P6502CC::JSRExecHdlr);
	
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
	mPageBoundaryCrossed = false;
	MemoryMappedDevice* dev = nullptr;
	mBranchTaken = false;

	while (prev_state != FETCH_OPCODE && effective_cycles++ < 100) {
		
		

		// Keep executing microcycles of the current instruction until the next instruction's opcode needs to be fetched

		if (mPendingWaitStates == 0 && !mPendingAccess) {
			// New memory acess - check if the address is mapped to a device and if so add wait states if applicable
			if ((dev = mDeviceManager->getSelectedMemoryMappedDevice(mADDRESS)) != NULL)
				mPendingWaitStates = getWaitStates(dev);
			else {
				mPendingWaitStates = 0; // No device mapped to the accessed address - no wait states
			}
			mPendingAccess = mPendingWaitStates > 0;

		}
		if (mPendingWaitStates == 0) {
			mPendingAccess = false;
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
						if (mReadingOperandByte && mOperandByteCount < 2)
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

	endTick = mTicks;

	return true;
}


// Initialise the CPU state for fetching the next instruction (fetching the opcode of the next instruction will be done in the next cycle by executeInstrMicroCycle)
bool P6502CC::initFetch()
{
	// Advance device time and cycle count by the number of microcycles executed for the instruction
	mExecutedCycles = mOPerandMicroCycle + mExecMicroCycle;
	tick(mExecutedCycles);

	// Save information about the executed instruction for debugging and tracing purposes
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
	setInstrLogData();



	// Log the previously executed instruction if enabled
	if (DBG_LEVEL_DEV(this, DBG_6502)) {
		stringstream sout;
		printInstrLogData(sout, mInstrLogData);
		DBG_LOG(this, DBG_6502, sout.str());
	}

	// Uppdate the RW, SYNC & ADDRESS ports for the opcode fetch of the next instruction
	updatePort(RW, 1); // Set R/W port to read mode
	updatePort(SYNC, 1); // Set SYNC port to indicate an opcode read operation
	update16BitPort(ADDRESS, mProgramCounter); // Set the address bus to the read address

	// Reset the CPU execution state and microcycle counters for the next instruction
	mCPUExecState = FETCH_OPCODE; // Set the CPU execution state to (in the next cycle) fetch the opcode of the next instruction
	pOpcodePC = mOpcodePC;
	mOpcodePC = mProgramCounter;


	// Advance the program counter to point either to the first operand byte or (/if there are no operands) the next instruction's opcode 
	mProgramCounter++;

	return true;
}


bool P6502CC::completeFetch()
{
	mOpcode = mDATA;
	pInstructionData = &(pInstrDataTbl->data[mOpcode]); // Execution info
	pInstructionInfo = &(pInstructionData->info); // Opcode info only

	mOPerandMicroCycle = 0; // Reset the operand microcycle counter for the next instruction
	mExecMicroCycle = 0; // Reset the execution microcycle counter for the next instruction

	// Reset operand data (used for tracing only)
	mOperandByteCount = 0;
	mReadingOperandByte = false;

	return true;
}

void P6502CC::fakeBRKFetch()
{
	mOpcode = 0x00;
	pInstructionData = &(pInstrDataTbl->data[mOpcode]); // Execution info
	pInstructionInfo = &(pInstructionData->info); // Opcode info only

	mOPerandMicroCycle = 0; // Reset the operand microcycle counter for the next instruction
	mExecMicroCycle = 0; // Reset the execution microcycle counter for the next instruction

	// Reset operand data (used for tracing only)
	mOperandByteCount = 0;
	mReadingOperandByte = false;
}

bool P6502CC::prepMemRead(uint16_t adr)
{
	updatePort(RW, 1); // Set R/W port to read mode
	updatePort(SYNC, 0); // Clear SYNC port to indicate a non-opcode read operation
	update16BitPort(ADDRESS, adr); // Set the address bus to the read address

	return true;
}


bool P6502CC::prepMemWrite(uint16_t adr, uint8_t data)
{
	updatePort(RW, 0); // Set R/W port to read mode
	updatePort(SYNC, 0); // Set SYNC port to indicate a non-opcode read operation
	update16BitPort(ADDRESS, adr); // Set the address bus to the read address
	updatePort(DATA, data); // Set the data bus to the value to be written

	return true;
}


bool P6502CC::initMemRead(uint16_t adr)
{
	mReadingOperandByte = false;
	return prepMemRead(adr);
	return true;
}


bool P6502CC::initOperandByteRead()
{
	mReadingOperandByte = true;
	return prepMemRead(mProgramCounter++);
}


bool P6502CC::initDummyOperandRead()
{
	mReadingOperandByte = false;
	return prepMemRead(mProgramCounter);
}


bool P6502CC::initDummyRead(uint16_t adr)
{
	mReadingOperandByte = false;
	return prepMemRead(adr);
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

		if (pRESET != mRESET /* positive edge */) {
			mCPUExecState = IN_RESET; // Set the CPU execution state to (in the next cycle) be in reset processing
			mOPerandMicroCycle = 0; // Reset the operand microcycle counter for the reset sequence
			mExecMicroCycle = -1; // Reset the execution microcycle counter (even if not used for the reset sequence)
			fakeBRKFetch(); // Make execution of the RESET sequency appear as that of a BRK instruction when logging
		}

		if (!mIRQ && !I_flag && !mPendingIRQ)
			mPendingIRQ = true; // Set pending IRQ to allow waiting for the current instruction to be completed before starting the interrupt sequence; reset when the interrupt serquence later on has completed
		if (mPendingIRQ && mCPUExecState == FETCH_OPCODE) {
			mCPUExecState = IN_IRQ; // Set the CPU execution state to (in the next cycle) be in IRQ processing
			mOPerandMicroCycle = 0; // Reset the operand microcycle counter for the reset sequence
			mExecMicroCycle = -1; // Reset the execution microcycle counter (even if not used for the reset sequence)
			fakeBRKFetch(); // Make execution of the IRQ sequency appear as that of a BRK instruction when logging
		}

		if (!mNMI && mNmiTransition  && !mPendingNMI)
			mPendingNMI = true; // Set pending NMI to allow waiting for the current instruction to be completed before starting the interrupt sequence; reset when the interrupt serquence later on has completed
		if (mPendingNMI && mCPUExecState == FETCH_OPCODE) {
			mNmiTransition = true;
			mCPUExecState = IN_NMI; // Set the CPU execution state to (in the next cycle) be in NMI processing
			mOPerandMicroCycle = 0; // Reset the operand microcycle counter for the reset sequence
			mExecMicroCycle = -1; // Reset the execution microcycle counter (even if not used for the reset sequence)
			fakeBRKFetch(); // Make execution of the NMI sequency appear as that of a BRK instruction when logging
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
		CPUExecState adjusted_CPU_exec_state = mCPUExecState;
		if (adjusted_CPU_exec_state == FETCH_OPERAND)
			return (this->*(pInstructionData->addrHdlr))(); // Fetch the operand bytes and perform any related read operations (or prepare fetching of the next opcode if there were no operand bytes)
		
		if (mCPUExecState == EXECUTE_INSTRUCTION_DIRECTLY) {
			// The last operand micro cycle shall be interleaved with the first execution micro cycle (a trick to be able to share common micro cycle parts)
			mOPerandMicroCycle--; // As the last operand and the first execution micro cycle is one and the same, the no of operand micro cycles is decremented to make the sum of these cycles correct
			mCPUExecState = EXECUTE_INSTRUCTION;
			adjusted_CPU_exec_state = EXECUTE_INSTRUCTION;
		}

		if (adjusted_CPU_exec_state == EXECUTE_INSTRUCTION)
			return (this->*(pInstructionData->execHdlr))(); // Execute the instruction - takes at least one cycle and the last cycle includes preparing for the fetching of the next instruction's opcode
	}

	return true;
}

//
// Faking the detection of a RESET pulse by setting the CPU execution state to IN_RESET. This will
// lead to the execution of the RESET handler in the following micro cycle.
// 
// This method can be used instead of asserting the RESET input port itself if desired.
//
bool P6502CC::reset()
{
	mCPUExecState = IN_RESET; // Set the CPU execution state to (in the next cycle) be in reset
	mOPerandMicroCycle = 0; // Reset the operand microcycle counter for the reset sequence
	mExecMicroCycle = -1; // Reset the execution microcycle counter (even if not used for the reset sequence);
	fakeBRKFetch(); // Make execution of the RESET sequency appear as that of a BRK instruction when logging

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
	case 0:initDummyRead(0x00ff); return true; // dummy read always made by NMOS 6502
	case 1:initDummyRead(0x0100); return true; // dummy read always made by NMOS 6502
	case 2:initDummyRead(0x01ff); return true; // dummy read always made by NMOS 6502
	case 3:initDummyRead(0x01fe); return true; // dummy read always made by NMOS 6502
	case 4:initMemRead(0xfffc); return true; // init reading of the low byte of the reset vector
	case 5:mVecLow = mDATA; initMemRead(0xfffd); return true; // get the low byte of the reset vector
	case 6:mProgramCounter = (mDATA << 8) | mVecLow; initFetch();; return true; // PC = reset vector, prepare for fetching the next instruction
	default:
		return false;
	}

}

// Servicing of NMI and IRQ
bool P6502CC::serveNMI() { mInterruptState = NMI_PENDING;  return BRKExecHdlr(); }
bool P6502CC::serveIRQ() { mInterruptState = IRQ_PENDING;  return BRKExecHdlr(); }





//
// Addressing modes
//

//
// Accumulator addressing
//
// Mnemonic
// 
// E.g., ASL A, TAX
// 
//	Operand Syntax		Bytes	Cycles
//	==============		=====	=============================
//	-					0		1
//
bool P6502CC::accAdrHdlr()
{
	return impliedAdrHdlr();
}

//
// Implied addressing
//
// Mnemonic
// 
// E.g., BRK, NOP or CLC
// 
//	Operand Syntax		Bytes	Cycles
//	==============		=====	=============================
//	-					0		1
//
bool P6502CC::impliedAdrHdlr()
{
	switch (mOPerandMicroCycle++) {
	
	case 0:

		initMemRead(mProgramCounter); // dummy read always made by NMOS 6502 - program counter NOT updated  (as it would be for a reading of an operand)
		mCPUExecState = EXECUTE_INSTRUCTION;
		return true; 
	
	default:
		return false;
	}

}

//
// Immediate addressing
//
// Mnemonic #<value>
// 
// E.g., LDA #12
// 
//	Operand Syntax		Bytes	Cycles
//	==============		=====	=============================
//	#oper				1		2 
//
bool P6502CC::immediateAdrHdlr()
{
	switch (mOPerandMicroCycle++) {

	case 0:

		initOperandByteRead(); // iniitialize the read of immediate operand byte (the read will be performed in the next cycle by executeInstrMicroCycle)
		mCPUExecState = EXECUTE_INSTRUCTION;
		return true;

	case 1:
		mReadVal = mDATA;
		mCPUExecState = EXECUTE_INSTRUCTION_DIRECTLY;
		return true;

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
// Operand Syntax		Bytes	Cycles
//	==============		=====	=============================
//	branch adr			1		1 (RO)
//
bool P6502CC::relativeAdrHdlr()
{
	switch (mOPerandMicroCycle++) {

	case 0:

		initOperandByteRead();
		mBranchTaken = false;
		return true;

	case 1:
		mReadVal = mDATA;
		mOperandAddress = ((int8_t)mReadVal + mProgramCounter) & 0xffff;
		mCPUExecState = EXECUTE_INSTRUCTION_DIRECTLY;
		return true;
	
	default:

		return false;
	}
}

//
// Zero-page addressing
//
// Mnemonic <zero page address>
// 
// Get zero-page address (and for read and read-modify-write instructions, also make a read at the address)
// 
// E.g., LDA $12 and STA &12
//
//	Operand Syntax		Bytes	Cycles
//	==============		=====	=============================
//	zeropage			1		2 (WO) 3 (R,RW)
//
bool P6502CC::zeroPageAdrHdlr()
{
	switch (mOPerandMicroCycle++) {

	case 0:

		// Initialise reading of the zero page address
		initOperandByteRead();
		return true;

	case 1:

		// Complete reading of the zeropage address
		mOperandAddress = mDATA;

		// For Read and Read-modify-Write instructions, prepare a read at the address
		if (pInstructionData->info.readsMem) {
			initMemRead(mOperandAddress);
			return true;
		}

		// For Write-only instructions, hand over to instruction execution handler directly
		mCPUExecState = EXECUTE_INSTRUCTION_DIRECTLY;
		return true;

	case 2:

		// Read and Read-Modify-Write instructions only

		mReadVal = mDATA;
		mCPUExecState = EXECUTE_INSTRUCTION_DIRECTLY;
		return true;

	default:
		return false;
	}
}

//
// Zero-page X-indexed addressing
//
// Mnemonic <zero page address>,X
// 
// Calculate zero-page address + X (and for read and read-modify-write instructions, also make a read at the calculated address)
// 
// E.g., LDA $12,X
//
// Operand Syntax		Bytes	Cycles
//	==============		=====	=============================
//	zeropage,X			1		3 (WO) 4 (R,RW)
//
bool P6502CC::zeroPageXAdrHdlr()
{
	switch (mOPerandMicroCycle++) {

	case 0:

		// Initialise reading of the zero page address
		initOperandByteRead();

		return true;

	case 1:

		// Complete reading of the zeropage address
		mOperandAddress = mDATA;

		// Make dummy read at  the zero page address
		initMemRead(mOperandAddress);

		return true;

	case 2:

		// Calculate zero page address + X
		mOperandAddress = (mOperandAddress + mRegisterX) & 0xff;

		// For Read and Read-modify-Write instructions, add one micro cycle for reading at the calculated address
		if (pInstructionData->info.readsMem) {
			initMemRead(mOperandAddress);
			return true;
		}

		// For Write-only instructions, leave to instruction execution handler to make use of the calculated address
		mCPUExecState = EXECUTE_INSTRUCTION_DIRECTLY;
		return true;

	case 3:

		// (Read and Read-Modify-Write instructions only) complete reading at the calculated address
		mReadVal = mDATA;
		mCPUExecState = EXECUTE_INSTRUCTION_DIRECTLY;
		return true;

	default:
		return false;
	}
}

//
// Zero-page Y-indexed addressing
//
// Mnemonic <zero page address>,Y
// 
// Calculate zero-page address + Y (and for read and read-modify-write instructions, initiate a read at the calculated address)
// 
// E.g., LDX $12,Y
//
// Operand Syntax		Bytes	Cycles
//	==============		=====	=============================
//	zeropage,Y			1		3 (WO) 4 (R,RW)
//
bool P6502CC::zeroPageYAdrHdlr()
{
	switch (mOPerandMicroCycle++) {

	case 0:

		// Initialise reading of the zero page address
		initOperandByteRead();

		return true;

	case 1:

		// Complete reading of the zeropage address
		mOperandAddress = mDATA;

		// Make dummy read at  the zero page address
		initMemRead(mOperandAddress);

		return true;

	case 2:

		// Calculate zero page address + Y
		mOperandAddress = (mOperandAddress + mRegisterY) & 0xff;

		// For Read and Read-modify-Write instructions, add one micro cycle for reading at the calculated address
		if (pInstructionData->info.readsMem) {
			initMemRead(mOperandAddress);
			return true;
		}

		// For Write-only instructions, leave to instruction execution handler to make use of the calculated address
		mCPUExecState = EXECUTE_INSTRUCTION_DIRECTLY;
		return true;

	case 3:

		// (Read and Read-Modify-Write instructions only) complete reading at the calculated address
		mReadVal = mDATA;
		mCPUExecState = EXECUTE_INSTRUCTION_DIRECTLY;
		return true;

	default:
		return false;
	}
}

//
// Absolute addressing
// 
// Mnemonic <address>
// 
// Get address  (and for read and read-modify-write instructions, initiate a read at the address)
// 
// E.g., LDX $1234
//
//	Operand Syntax		Bytes	Cycles
//	==============		=====	=============================
//	address				2		3 (WO) 4 (R,RW)
//
bool P6502CC::absoluteAdrHdlr()
{

	switch (mOPerandMicroCycle++) {

	case 0:

		// Initialise reading of the low address byte
		initOperandByteRead();
		return true;

	case 1:

		// Complete reading of the low address byte
		mOperandAddress = mDATA;

		// Initialise reading of the high address byte
		initOperandByteRead();

		return true;

	case 2:

		// Complete reading of the address 
		mOperandAddress = (mDATA << 8) | mOperandAddress;

		// For Read and Read-modify-Write instructions, add one micro cycle for reading at the calculated address
		if (pInstructionData->info.readsMem) {
			initMemRead(mOperandAddress);
			return true;
		}

		// For Write-only instructions, leave to instruction execution handler to make use of the calculated address
		mCPUExecState = EXECUTE_INSTRUCTION_DIRECTLY;
		return true;

	case 3:

		// (Read and Read-Modify-Write instructions only) complete reading at the calculated address
		mReadVal = mDATA;
		mCPUExecState = EXECUTE_INSTRUCTION_DIRECTLY;
		return true;

	default:
		return false;
	}

}


//
// X-indexed addressing
//
// Mnemonic <address>,X
// 
// Calculate address + X (and for read and read-modify-write instructions, initiate a read at the calculated address)
// 
// E.g., LDA $1234,X
//
//	Operand Syntax		Bytes	Cycles
//	==============		=====	=============================
//	absolute,X			2		4 (WO) 4+ (R) 3 (RW)
//
// One micro cycle is added if the calculated address passes a page boundary
//
bool P6502CC::absoluteXAdrHdlr()
{
	switch (mOPerandMicroCycle++) {

	case 0:

		// Initialise reading of the low address byte
		initOperandByteRead();
		return true;

	case 1:

		// Complete reading of the low address byte
		mOperandAddress = mDATA;

		// Initialise reading of the high address byte
		initOperandByteRead();

		return true;

	case 2:
	{
		// Calculate address + X
		mOperand16 = (mDATA << 8) | mOperandAddress;
		mOperandAddress = mOperand16 + mRegisterX;
		uint16_t calc_adr_raw = (mOperand16 & 0xff00) | (mOperandAddress & 0xff);

		// Initialise reading at the calculated address (page boundary cross ignored)
		// Done even for a write-onlu instruction like STA abs,X...
		initMemRead(calc_adr_raw);
		return true;
	}
	case 3:

		// All types of accesses (read-only, write-only and read-modify-write)

		// Read-only access
		if (pInstructionData->info.readsMem && !pInstructionData->info.writesMem) {
			// Make a re-read if page boundary crossed for read-only accesses
			if (pageBoundaryCrossed(mOperandAddress, mOperand16)) {
				initMemRead(mOperandAddress);
				return true;
			}
			mReadVal = mDATA; // only used if the instruction actually reads data...
			mCPUExecState = EXECUTE_INSTRUCTION_DIRECTLY;
			return true;
		}

		// Hrite-only access
		if (!pInstructionData->info.readsMem && pInstructionData->info.writesMem) {
			mReadVal = mDATA; // only used if the instruction actually reads data...
			mCPUExecState = EXECUTE_INSTRUCTION_DIRECTLY;
			return true;
		}

		// Read-modify-Write access
		initMemRead(mOperandAddress);
		return true;



	case 4:

		// Read-only access when page boundary crossed OR Read-modify-Write access

		// Hand over to instruction execution directly 
		mReadVal = mDATA; // only used if the instruction actually reads data...
		mCPUExecState = EXECUTE_INSTRUCTION_DIRECTLY;
		return true;

	default:
		return false;
	}
}


//
// Y-indexed addressing
//
// Mnemonic <address>,X
// 
// Calculate address + X (and for read [and read-modify-write instructions]*, initiate a read at the calculated address)
// 
// E.g., LDX $1234,Y
//
//	Operand Syntax		Bytes	Cycles
//	==============		=====	=============================
//	absolute,Y			2		4 (WO) 3+ (R) [3 (RW)]*
//
// One micro cycle is added if the calculated address passes a page boundary
//
// * There doesn't seem to exist any read-modify-write instructions using Y-indexed addressing. Kept just as the code
//   can be identical as for the X-indexed addressing.
//
bool P6502CC::absoluteYAdrHdlr()
{
	switch (mOPerandMicroCycle++) {

	case 0:

		// Initialise reading of the low address byte
		initOperandByteRead();
		return true;

	case 1:

		// Complete reading of the low address byte
		mOperandAddress = mDATA;

		// Initialise reading of the high address byte
		initOperandByteRead();

		return true;

	case 2:
	{
		// Calculate address + Y
		mOperand16 = (mDATA << 8) | mOperandAddress;
		mOperandAddress = mOperand16 + mRegisterY;
		uint16_t calc_adr_raw = (mOperand16 & 0xff00) | (mOperandAddress & 0xff);

		// Initialise reading at the calculated address (page boundary cross ignored)
		// Done even for a write-onlu instruction like STA abs,X...
		initMemRead(calc_adr_raw);
		return true;
	}
	case 3:

		// All types of accesses (read-only, write-only and read-modify-write)

		// Read-only access
		if (pInstructionData->info.readsMem && !pInstructionData->info.writesMem) {
			// Make a re-read if page boundary crossed for read-only accesses
			if (pageBoundaryCrossed(mOperandAddress, mOperand16)) {
				initMemRead(mOperandAddress);
				return true;
			}
			mReadVal = mDATA; // only used if the instruction actually reads data...
			mCPUExecState = EXECUTE_INSTRUCTION_DIRECTLY;
			return true;
		}

		// Hrite-only access
		if (!pInstructionData->info.readsMem && pInstructionData->info.writesMem) {
			mReadVal = mDATA; // only used if the instruction actually reads data...
			mCPUExecState = EXECUTE_INSTRUCTION_DIRECTLY;
			return true;
		}

		// Read-modify-Write access
		initMemRead(mOperandAddress);
		return true;



	case 4:

		// Read-only access when page boundary crossed OR Read-modify-Write access

		// Hand over to instruction execution directly 
		mReadVal = mDATA; // only used if the instruction actually reads data...
		mCPUExecState = EXECUTE_INSTRUCTION_DIRECTLY;
		return true;

	default:
		return false;
	}
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
//	Operand Syntax		Bytes	Cycles
//	==============		=====	=============================
//	(absolute)			2		5 (NMOS) 5+ (CMOS)
// 
// Only used for the JMP instruction
//
bool P6502CC::indirectAdrHdlr()
{
	switch (mOPerandMicroCycle++) {

	case 0:

		// Initialise reading of the low address byte
		initOperandByteRead();
		return true;

	case 1:

		// Complete reading of the low address byte
		mOperandAddress = mDATA;

		// Initialise reading of the high address byte
		initOperandByteRead();
		return true;

	case 2:

		mOperandAddress = (mDATA << 8) | mOperandAddress;

		// Initialise reading at the lookup address
		initMemRead(mOperandAddress);
		return true;

	case 3:

		// Initialise reading at the lookup address + 1 (ignoring crossing of page boundary)
		initMemRead((mOperandAddress & 0xff00) | ((mOperandAddress + 1) & 0xff));

		mOperandAddress = mDATA;

		return true;

	case 4:

		// Complete calculation of the effective address
		mOperandAddress = (mDATA << 8) | mOperandAddress;
		mCPUExecState = EXECUTE_INSTRUCTION_DIRECTLY;
		return true;

	default:

		return false;
	}

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
//	Operand Syntax		Bytes	Cycles
//	==============		=====	=============================
//	(indirect,X)		1		5 (write-only) 6 (read-only)
//
// Read-only and write-only use only.
//
bool P6502CC::preIndXAdrHdlr()
{
	switch (mOPerandMicroCycle++) {

	case 0:

		// Initialise reading of the zero page address
		initOperandByteRead();

		return true;

	case 1:

		// Complete reading of the zeropage address
		mLookupAddress = mDATA;

		// Make dummy read at  the zero page address
		initMemRead(mLookupAddress);

		return true;

	case 2:

		// Calculate the lookup address <=> zero page address + X
		mLookupAddress = (mLookupAddress + mRegisterX) & 0xff;

		// Initialise reading at the lookup address
		initMemRead(mLookupAddress);
		return true;

	case 3:

		// Initialise reading at the lookup address + 1 (ignoring crossing of page boundary)
		initMemRead((mLookupAddress + 1) & 0xff);

		mEffectiveAddress = mDATA;

		return true;

	case 4:

		// Complete calculation of the effective address
		mEffectiveAddress = (mDATA << 8) | mEffectiveAddress;

		// Save calculated address for use by the instruction execution handler (and logging)
		mOperandAddress = mEffectiveAddress;

		// (For read-only instructions) Initialise reading at the effective address
		if (pInstructionData->info.readsMem) {
			initMemRead(mEffectiveAddress);
			return true;
		}

		// (For write-only instructions) hand over to instruction execution directly
		mCPUExecState = EXECUTE_INSTRUCTION_DIRECTLY;
		return true;

	case 5:

		mReadVal = mDATA;
		mCPUExecState = EXECUTE_INSTRUCTION_DIRECTLY;
		return true;

	default:

		return false;
	}
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
//	Operand Syntax		Bytes	Cycles
//	==============		=====	=============================
//	(indirect),Y		1		5 (write-only) 5+ (read-only)
//
// Read-only and write-only use only.
//
bool P6502CC::postIndYAdrHdlr()
{
	switch (mOPerandMicroCycle++) {

	case 0:

		// Initialise reading of the zero page address
		initOperandByteRead();

		return true;

	case 1:

		// Complete reading of the zeropage address <=> lookup address
		mLookupAddress = mDATA;

		// Initialise reading at the lookup address
		initMemRead(mLookupAddress);

		return true;

	case 2:

		// Complete reading of the low effective address byte
		mEffectiveAddress = mDATA;

		// Initialise reading of the high effective address byte
		initMemRead((mLookupAddress + 1) & 0xff);

		return true;

	case 3:

		// Complete reading of the high effective address byte
		mEffectiveAddress = (mDATA << 8) | mEffectiveAddress;

		// Preliminary calculated address <=> mEffectiveAddress + Y, ignoring crossing of page boundary
		mTmpAddress = (mEffectiveAddress & 0xff00) | (mEffectiveAddress + mRegisterY) & 0xff;

		// Save (final) calculated address for use by the instruction execution handler (and logging)
		mOperandAddress = mEffectiveAddress + mRegisterY;

		// Make a read (even if it is a write-only instruction => dummy read!)
		initMemRead(mTmpAddress);
		return true;

	case 4:

		// For read-only instructions - add an extra read cycle (at the corrected address) if page boundary is crossed
		if (pInstructionData->info.readsMem && pageBoundaryCrossed(mOperandAddress, mTmpAddress)) {
			initMemRead(mOperandAddress);
			return true;
		}

		// Complete reading of the data and then hand over directly to the instruction execution handler
		mReadVal = mDATA;
		mCPUExecState = EXECUTE_INSTRUCTION_DIRECTLY;
		return true;

	case 5:

		// Complete reading of the data and then hand over directly to the instruction execution handler
		mReadVal = mDATA;
		mCPUExecState = EXECUTE_INSTRUCTION_DIRECTLY;
		return true;

	default:

		return false;
	}
}

bool P6502CC::undefinedAdrHdlr()
{
	return true;
}







/////////////////////////////////////////////////////////////////////
// 
// Instruction Execution
// 
/////////////////////////////////////////////////////////////////////


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
bool P6502CC::ADCExecHdlr() {

	switch (mExecMicroCycle++) {

	case 0:
	{
		if (ADCCalc()) {
			initFetch();
			return true;
		}
		return false;
	}

	default:
		return false;
	}

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
bool P6502CC::ANDExecHdlr() {

	switch (mExecMicroCycle++) {

	case 0:
	{
		mAcc &= mReadVal;
		setNZflags(mAcc);
		initFetch();
		return true;
	}

	default:
		return false;
	}
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
bool P6502CC::ASLExecHdlr()
{

	switch (mExecMicroCycle++) {

	case 0:

		if (!pInstructionInfo->writesMem)
			mReadVal = mAcc;

		mStatusRegister &= ~C_set_mask;
		mStatusRegister |= ((mReadVal & 0x80) != 0 ? C_set_mask : 0);
		mCalcVal = (mReadVal << 1) & 0xfe;
		setNZflags(mCalcVal);

		if (!pInstructionInfo->writesMem) {
			mAcc = mCalcVal;
			initFetch();
		}
		else
			prepMemWrite(mOperandAddress, mReadVal); // dummy write always made by NMOS 6502

		return true;

	case 1:

		prepMemWrite(mOperandAddress, mCalcVal);
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
//	Mode			Syntax		Opcode	Bytes	Cycles
//	===========		==========	======	=====	======
//	relative		BCC oper	90		2		2+
// 
// The branch is taken if the carry flag is clear (C = 0).
// If the branch is taken, the program counter is set to the address of the branch target and the next instruction is fetched from there.
// If the branch is not taken, the program counter is not modified and the next instruction is fetched from the next address after the branch instruction as usual.
// The number of cycles taken by a branch instruction depends on whether the branch is taken (+1) or not (+0) and on whether a page boundary is crossed when branching (+1).
//
bool P6502CC::BCCExecHdlr()
{
	if (!C_flag) {

		mBranchTaken = true;

		// Branch taken, so set the program counter to the branch target address and fetch the next instruction in one or two cycles (depending on if a page boundary is crossed or not)
		switch (mExecMicroCycle++) {

		case 0:
			return true;

		case 1:	

			if (pageBoundaryCrossed(mProgramCounter, mOperandAddress)) {
				initDummyOperandRead(); // A dummy read is always made
				return true;
			}
			mProgramCounter = mOperandAddress;
			initFetch();
			return true;

		case 2:

			mProgramCounter = mOperandAddress;
			initFetch();
			return true;

		default:
			return false;
		}
	}

	// No branch taken, so just fetch the next instruction
	mExecMicroCycle++;
	initFetch();
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
bool P6502CC::BCSExecHdlr() {

	if (C_flag) {

		mBranchTaken = true;

		// Branch taken, so set the program counter to the branch target address and fetch the next instruction in one or two cycles (depending on if a page boundary is crossed or not)
		switch (mExecMicroCycle++) {

		case 0:
			return true;

		case 1:

			if (pageBoundaryCrossed(mProgramCounter, mOperandAddress)) {
				initDummyOperandRead(); // A dummy read is always made
				return true;
			}
			mProgramCounter = mOperandAddress;
			initFetch();
			return true;

		case 2:

			mProgramCounter = mOperandAddress;
			initFetch();
			return true;

		default:
			return false;
		}
	}

	// No branch taken, so just fetch the next instruction
	mExecMicroCycle++;
	initFetch();
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
bool P6502CC::BEQExecHdlr() {

	if (Z_flag) {

		mBranchTaken = true;

		// Branch taken, so set the program counter to the branch target address and fetch the next instruction in one or two cycles (depending on if a page boundary is crossed or not)
		switch (mExecMicroCycle++) {

		case 0:
			return true;

		case 1:

			if (pageBoundaryCrossed(mProgramCounter, mOperandAddress)) {
				initDummyOperandRead(); // A dummy read is always made
				return true;
			}
			mProgramCounter = mOperandAddress;
			initFetch();
			return true;

		case 2:

			mProgramCounter = mOperandAddress;
			initFetch();
			return true;

		default:
			return false;
		}
	}

	// No branch taken, so just fetch the next instruction
	mExecMicroCycle++;
	initFetch();
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
bool P6502CC::BITExecHdlr()
{

	switch (mExecMicroCycle++) {

	case 0:
	{
		uint8_t val_8_u = mAcc & mReadVal;
		mStatusRegister &= ~(Z_set_mask | N_set_mask | V_set_mask);
		if (val_8_u == 0)
			mStatusRegister |= Z_set_mask;
		if ((mReadVal & 0x80) != 0)
			mStatusRegister |= N_set_mask;
		if ((mReadVal & 0x40) != 0)
			mStatusRegister |= V_set_mask;
		initFetch();
		return true;
	}

	default:
		return false;
	}

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
bool P6502CC::BMIExecHdlr() {

	if (N_flag) {

		mBranchTaken = true;

		// Branch taken, so set the program counter to the branch target address and fetch the next instruction in one or two cycles (depending on if a page boundary is crossed or not)
		switch (mExecMicroCycle++) {

		case 0:
			return true;

		case 1:

			if (pageBoundaryCrossed(mProgramCounter, mOperandAddress)) {
				initDummyOperandRead(); // A dummy read is always made
				return true;
			}
			mProgramCounter = mOperandAddress;
			initFetch();
			return true;

		case 2:

			mProgramCounter = mOperandAddress;
			initFetch();
			return true;

		default:
			return false;
		}
	}

	// No branch taken, so just fetch the next instruction
	mExecMicroCycle++;
	initFetch();
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
bool P6502CC::BNEExecHdlr() {

	if (!Z_flag) {

		mBranchTaken = true;

		// Branch taken, so set the program counter to the branch target address and fetch the next instruction in one or two cycles (depending on if a page boundary is crossed or not)
		switch (mExecMicroCycle++) {

			mBranchTaken = true;

		case 0:
			return true;

		case 1:

			if (pageBoundaryCrossed(mProgramCounter, mOperandAddress)) {
				initDummyOperandRead(); // A dummy read is always made
				return true;
			}
			mProgramCounter = mOperandAddress;
			initFetch();
			return true;

		case 2:

			mProgramCounter = mOperandAddress;
			initFetch();
			return true;

		default:
			return false;
		}
	}

	// No branch taken, so just fetch the next instruction
	mExecMicroCycle++;
	initFetch();
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
bool P6502CC::BPLExecHdlr() {

	if (!N_flag) {

		mBranchTaken = true;

		// Branch taken, so set the program counter to the branch target address and fetch the next instruction in one or two cycles (depending on if a page boundary is crossed or not)
		switch (mExecMicroCycle++) {

		case 0:
			return true;

		case 1:

			if (pageBoundaryCrossed(mProgramCounter, mOperandAddress)) {
				initDummyOperandRead(); // A dummy read is always made
				return true;
			}
			mProgramCounter = mOperandAddress;
			initFetch();
			return true;

		case 2:

			mProgramCounter = mOperandAddress;
			initFetch();
			return true;

		default:
			return false;
		}
	}

	// No branch taken, so just fetch the next instruction
	mExecMicroCycle++;
	initFetch();
	return true;
}


//
// BRK (as well as handling an IRQ and an NMI)
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
bool P6502CC::BRKExecHdlr()
{
	// The first micro cycle has already been executed for a BRK instruction when this handler
	// is called (the speculative - and dummy - read of a potentially next instruction's opcode),
	// so then we start with the second micro cycle. When servicing an NMI or an IRQ, there has
	// been no opcode fetch, so we start with the first micro cycle instead.
	int exec_cycle = (mInterruptState == NONE ? mExecMicroCycle++ + 1 : mExecMicroCycle++);

	switch (exec_cycle) {

	case 0:

		initDummyOperandRead();  // dummy read at the current PC
		mProgramCounter++;
		return true;

	case 1:

		// Push high byte of the program counter to the stack (the low byte of the program counter is not pushed to the stack until the next cycle, so that the PC is stored in little endian format in the memory)
		prepMemWrite(0x100 | (uint16_t)mStackPointer--, mProgramCounter >> 8);
		return true;

	case 2:

		// Push the low byte of the program counter to the stack
		prepMemWrite(0x100 | (uint16_t)mStackPointer--, mProgramCounter & 0xff);

		return true;

	case 3:

		// Push the status register to the stack with the break flag (B) set
		prepMemWrite(0x100 + (uint16_t)mStackPointer--, mStatusRegister | B_set_mask);
		return true;

	case 4:

		if (mInterruptState != NONE)
			mStatusRegister |= I_set_mask; // (For an NMI and an IRQ) set the interrupt disable flag to prevent further interrupts until the RTI instruction is executed

		// Choose interrupt vector based on the type of interrupt (NMI or IRQ/BRK)
		if (mInterruptState == NMI_PENDING)
			mInterruptVector = 0xfffa; // NMI
		else 
			mInterruptVector = 0xfffe; // IRQ or BRK

		initMemRead(mInterruptVector); // init reading of the low byte of the BRK/IRQ/NMI vector

		return true; 

	case 5:

		// Get the low byte of the interrupt vector; prepare reading of the high byte of interrupt vector
		mVecLow = mDATA;
		initMemRead(mInterruptVector + 1);
		return true; 

	case 6:
		
		// Finish setting ut the service routine address by getting the high byte of the interrupt vector and combining it
		// with the low byte to set the program counter to the address of the interrupt vector; reset the interrupt state
		mProgramCounter = (mDATA << 8) | mVecLow;

		// If the completion of servicing an NMI or an IRQ, then clear the associated pending status
		if (mInterruptState == IRQ_PENDING)
			mPendingIRQ = false; // reset pending IRQ (if it was an IRQ that is being serviced)
		else if (mInterruptState == NMI_PENDING)
			mPendingNMI = false; // reset pending NMI (if it was an NMI that is being serviced)
		mInterruptState = NONE; // reset the interrupt state

		// Prepare fetching of the first instruction of the BRK/IRQ/NMI service routine at the address of the interrupt vector
		initFetch();

		return true;

	default:
		return false;
	}
}

//
// BVC
// 
// Branch on oVerflow Clear
// 
// Branch on V = 1
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
bool P6502CC::BVCExecHdlr() {
	
	if (!V_flag) {

		mBranchTaken = true;

		// Branch taken, so set the program counter to the branch target address and fetch the next instruction in one or two cycles (depending on if a page boundary is crossed or not)
		switch (mExecMicroCycle++) {

		case 0:
			return true;

		case 1:

			if (pageBoundaryCrossed(mProgramCounter, mOperandAddress)) {
				initDummyOperandRead(); // A dummy read is always made
				return true;
			}
			mProgramCounter = mOperandAddress;
			initFetch();
			return true;

		case 2:

			mProgramCounter = mOperandAddress;
			initFetch();
			return true;

		default:
			return false;
		}
	}

	// No branch taken, so just fetch the next instruction
	mExecMicroCycle++;
	initFetch();
	return true;

}


//
// BVS
// 
// Branch on oVerflow Set
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
bool P6502CC::BVSExecHdlr() {

	if (V_flag) {

		mBranchTaken = true;

		// Branch taken, so set the program counter to the branch target address and fetch the next instruction in one or two cycles (depending on if a page boundary is crossed or not)
		switch (mExecMicroCycle++) {

		case 0:
			return true;

		case 1:

			if (pageBoundaryCrossed(mProgramCounter, mOperandAddress)) {
				initDummyOperandRead(); // A dummy read is always made
				return true;
			}
			mProgramCounter = mOperandAddress;
			initFetch();
			return true;

		case 2:

			mProgramCounter = mOperandAddress;
			initFetch();
			return true;

		default:
			return false;
		}
	}

	// No branch taken, so just fetch the next instruction
	mExecMicroCycle++;
	initFetch();
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
bool P6502CC::CLCExecHdlr()
{
	mStatusRegister &= ~C_set_mask;

	mExecMicroCycle++;
	initFetch();
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
bool P6502CC::CLDExecHdlr()
{
	mStatusRegister &= ~D_set_mask;

	mExecMicroCycle++;
	initFetch();
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
bool P6502CC::CLIExecHdlr()
{

	mStatusRegister &= ~I_set_mask;

	mExecMicroCycle++;
	initFetch();
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
bool P6502CC::CLVExecHdlr()
{

	mStatusRegister &= ~V_set_mask;

	mExecMicroCycle++;
	initFetch();
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
bool P6502CC::CMPExecHdlr()
{

	switch (mExecMicroCycle++) {

	case 0:
	{
		uint8_t val_8_u = mAcc - mReadVal;
		setNZCflags(val_8_u, mAcc >= mReadVal);
		initFetch();
		return true;
	}

	default:
		return false;
	}

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
bool P6502CC::CPXExecHdlr() {

	switch (mExecMicroCycle++) {

	case 0:
	{
		uint8_t val_8_u = mRegisterX - mReadVal;
		setNZCflags(val_8_u, mRegisterX >= mReadVal);
		initFetch();
		return true;
	}

	default:
		return false;
	}

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
bool P6502CC::CPYExecHdlr()
{

	switch (mExecMicroCycle++) {

	case 0:
	{
		uint8_t val_8_u = mRegisterY - mReadVal;
		setNZCflags(val_8_u, mRegisterY >= mReadVal);
		initFetch();
		return true;
	}

	default:
		return false;
	}

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
bool P6502CC::DECExecHdlr() 
{
	switch (mExecMicroCycle++) {

	case 0:

		// Dummy write of previous value
		prepMemWrite(mOperandAddress, mReadVal);
		return true;

	case 1:

		// Write of the new value
		mWrittenVal = mReadVal - 1;
		prepMemWrite(mOperandAddress, mWrittenVal);	
		return true;

	case 2:

		setNZflags(mWrittenVal);
		initFetch();
		return true;

	default:

		return false;
	}

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
bool P6502CC::DEXExecHdlr() {

	switch (mExecMicroCycle++) {

	case 0:

		mRegisterX = mRegisterX - 1;
		setNZflags(mRegisterX);
		initFetch();
		return true;

	default:

		return false;
	}

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
bool P6502CC::DEYExecHdlr() {

	switch (mExecMicroCycle++) {

	case 0:

		mRegisterY = mRegisterY - 1;
		setNZflags(mRegisterY);
		initFetch();
		return true;

	default:

		return false;
	}

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
bool P6502CC::EORExecHdlr() {

	switch (mExecMicroCycle++) {

	case 0:

		mAcc ^= mReadVal;
		setNZflags(mAcc);
		initFetch();
		return true;

	default:

		return false;
	}

	return true;
}


//
// INC
//
// Decrement Memory by One
// 
// M  -> M + 1
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
bool P6502CC::INCExecHdlr()
{
	switch (mExecMicroCycle++) {

	case 0:

		// Dummy write of previous value
		prepMemWrite(mOperandAddress, mReadVal);
		return true;

	case 1:

		// Write of the new value
		mWrittenVal = mReadVal + 1;
		prepMemWrite(mOperandAddress, mWrittenVal);
		return true;

	case 2:

		setNZflags(mWrittenVal);
		initFetch();
		return true;

	default:

		return false;
	}

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
bool P6502CC::INXExecHdlr() {

	switch (mExecMicroCycle++) {

	case 0:

	mRegisterX = mRegisterX + 1;
	setNZflags(mRegisterX);
	initFetch();
	return true;

	default:

		return false;
	}

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
bool P6502CC::INYExecHdlr() {

	switch (mExecMicroCycle++) {

	case 0:

		mRegisterY = mRegisterY + 1;
		setNZflags(mRegisterY);
		initFetch();
		return true;

	default:

		return false;
	}

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
bool P6502CC::JMPExecHdlr() {

	switch (mExecMicroCycle++) {

	case 0:

		mProgramCounter = mOperandAddress;
		initFetch();
		return true;

	default:

		return false;
	}
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
bool P6502CC::JSRExecHdlr()
{
	switch (mExecMicroCycle++) {

	case 0:

		// Initialise reading of the low byte of the jump address
		initOperandByteRead();
		return true;

	case 1:

		// Complete reading of of the low byte of the jump address
		mOperandAddress = mDATA;

		// Initialise dummy reading at the stack pointer address
		initMemRead(0x100 | mStackPointer);
		return true;

	case 2:

		// Push high byte of the program counter to the stack (the low byte of the program counter is not pushed to the stack until the next cycle, so that the PC is stored in little endian format in the memory)
		prepMemWrite(0x100 | (uint16_t)mStackPointer--, mProgramCounter >> 8);
		return true;

	case 3:

		// Push the low byte of the program counter to the stack
		prepMemWrite(0x100 | (uint16_t)mStackPointer--, mProgramCounter & 0xff);
		return true;

	case 4:

		// Initialise reading of the high byte of the jump address
		initOperandByteRead();
		return true;

	case 5:

		// Complete reading of of the high byte of the jump address
		mOperandAddress = (mDATA << 8) | mOperandAddress;
		mProgramCounter = mOperandAddress;
		initFetch();
		return true;

	default:

		return false;
	}

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

bool P6502CC::LDAExecHdlr()
{
	switch (mExecMicroCycle++) {

	case 0:

		mAcc = mReadVal;
		setNZflags(mAcc);
		initFetch();
		return true;

	default:
		return false;
	}

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
bool P6502CC::LDXExecHdlr()
{
	switch (mExecMicroCycle++) {

	case 0:

		mRegisterX = mReadVal;
		setNZflags(mRegisterX);
		initFetch();
		return true;

	default:
		return false;
	}
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
bool P6502CC::LDYExecHdlr()
{
	switch (mExecMicroCycle++) {

	case 0:

		mRegisterY = mReadVal;
		setNZflags(mRegisterY);
		initFetch();
		return true;

	default:
		return false;
	}

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
bool P6502CC::LSRExecHdlr()
{
	switch (mExecMicroCycle++) {

	case 0:

		if (!pInstructionInfo->writesMem)
			mReadVal = mAcc;
		else
			mReadVal = mDATA;

		mStatusRegister &= ~C_set_mask;
		mStatusRegister |= ((mReadVal & 0x1) != 0 ? C_set_mask : 0);
		mCalcVal = (mReadVal >> 1) & 0x7f;
		setNZflags(mCalcVal);

		if (!pInstructionInfo->writesMem) {
			mAcc = mCalcVal;
			initFetch();
		}
		else
			prepMemWrite(mOperandAddress, mReadVal); // dummy write always made by NMOS 6502

		return true;

	case 1:

		prepMemWrite(mOperandAddress, mCalcVal);
		return true;

	case 2:

		initFetch();
		return true;

	default:
		return false;
	}

}


//
// NOP
//
// No Operation
// 
// N	Z	C	I	D	V
// -	-	-	-	-	-
//
bool P6502CC::NOPExecHdlr() {

	switch (mExecMicroCycle++) {

	case 0:
	{
		initFetch();
		return true;
	}

	default:
		return false;
	}
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
bool P6502CC::ORAExecHdlr() {

	switch (mExecMicroCycle++) {

	case 0:
	{
		mAcc |= mReadVal;
		setNZflags(mAcc);
		initFetch();
		return true;
	}

	default:
		return false;
	}

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
bool P6502CC::PHAExecHdlr()
{
	switch (mExecMicroCycle++) {

	case 0:
	{
		prepMemWrite(0x100 | (uint16_t)mStackPointer--, mAcc);
		return true;
	}

	case 1:
		initFetch();
		return true;

	default:
		return false;
	}

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
bool P6502CC::PHPExecHdlr()
{
	switch (mExecMicroCycle++) {

	case 0:
	{
		prepMemWrite(0x100 | (uint16_t)mStackPointer--, mStatusRegister | B_set_mask | b5_set_mask);
		return true;
	}

	case 1:
		initFetch();
		return true;

	default:
		return false;
	}
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
bool P6502CC::PLAExecHdlr() {

	switch (mExecMicroCycle++) {
		
	case 0:

		initMemRead(0x100 | (uint16_t)mStackPointer++);
		return true;

	case 1:

		initMemRead(0x100 | (uint16_t)mStackPointer);
		return true;

	case 2:

		mAcc = mDATA;
		setNZflags(mAcc);
		initFetch();
		return true;

	default:
		return false;
	}

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
bool P6502CC::PLPExecHdlr() {

	switch (mExecMicroCycle++) {

	case 0:

		initMemRead(0x100 | (uint16_t)mStackPointer++);
		return true;

	case 1:

		initMemRead(0x100 | (uint16_t)mStackPointer);
		return true;


	case 2:
		mStatusRegister &= ~(N_set_mask | Z_set_mask | C_set_mask | I_set_mask | D_set_mask | V_set_mask);
		mStatusRegister |= mDATA & ~(B_set_mask | b5_set_mask);
		initFetch();
		return true;

	default:
		return false;
	}

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
bool P6502CC::ROLExecHdlr() {

	switch (mExecMicroCycle++) {

	case 0:
	{
		if (!pInstructionInfo->writesMem)
			mReadVal = mAcc;
		else
			mReadVal = mDATA;

		mCalcVal = ((mReadVal << 1) & 0xfe) | C_flag;
		mStatusRegister &= ~C_set_mask;
		mStatusRegister |= ((mReadVal & 0x80) != 0 ? C_set_mask : 0);
		setNZflags(mCalcVal);

		if (!pInstructionInfo->writesMem) {
			mAcc = mCalcVal;
			initFetch();
		}
		else
			prepMemWrite(mOperandAddress, mReadVal); // dummy write always made by NMOS 6502

		return true;
	}
	case 1:

		prepMemWrite(mOperandAddress, mCalcVal);
		return true;

	case 2:

		initFetch();
		return true;

	default:
		return false;
	}

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
bool P6502CC::RORExecHdlr() {

	switch (mExecMicroCycle++) {

	case 0:
	{
		if (!pInstructionInfo->writesMem)
			mReadVal = mAcc;
		else
			mReadVal = mDATA;

		mCalcVal = ((mReadVal >> 1) & 0x7f) | ((C_flag << 7) & 0x80);
		mStatusRegister &= ~C_set_mask;
		mStatusRegister |= ((mReadVal & 0x1) ? C_set_mask : 0);
		setNZflags(mCalcVal);

		if (!pInstructionInfo->writesMem) {
			mAcc = mCalcVal;
			initFetch();
		}
		else
			prepMemWrite(mOperandAddress, mReadVal); // dummy write always made by NMOS 6502

		return true;
	}
	case 1:

		prepMemWrite(mOperandAddress, mCalcVal);
		return true;

	case 2:

		initFetch();
		return true;

	default:
		return false;
	}
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
bool P6502CC::RTIExecHdlr()
{

	switch(mExecMicroCycle++) {

	case 0:

		// Initialise dummy stack read =>  stack pointer increased
		initMemRead(0x100 + (uint16_t)mStackPointer++);
		return true;

	case 1:

		// Initialise the reading of the status register
		initMemRead(0x100 + (uint16_t)mStackPointer++);
		return true;

	case 2:

		// Pull status register while ignoring b5 and B flag
		mStatusRegister = (mStatusRegister & ~(N_set_mask | Z_set_mask | C_set_mask | I_set_mask | D_set_mask | V_set_mask)) | (mDATA & ~(B_set_mask | b5_set_mask));

		// Initialise the reading of the low program counter	
		initMemRead(0x100 + (uint16_t)mStackPointer++);
		return true;

	case 3:

		// Complete the reading of the low program counter
		mTmpReadByte = mDATA;

		// Initialise the reading of the high program counter	
		initMemRead(0x100 + (uint16_t)mStackPointer);

		return true;

	case 4:

		// Complete the reading of the high program counter
		mProgramCounter = (mDATA << 8) | mTmpReadByte;

		initFetch();
		return true;

	default:
		return false;
	}
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
bool P6502CC::RTSExecHdlr()
{
	switch (mExecMicroCycle++) {

	case 0:

		// Initialise dummy stack read =>  stack pointer increased
		initMemRead(0x100 + (uint16_t)mStackPointer++);
		return true;

	case 1:

		// Initialise the reading of the low program counter
		initMemRead(0x100 + (uint16_t)mStackPointer++);
		return true;

	case 2:

		// Initialise the reading of the high program counter
		mTmpReadByte = mDATA;
		initMemRead(0x100 + (uint16_t)mStackPointer);
		return true;

	case 3:

		// Complete the reading of the high program counter
		mProgramCounter = (mDATA << 8) | mTmpReadByte;

		// Initialise dummy operand read (needed as program counter points at return address - 1)
		initOperandByteRead();
		return true;

	case 4:
		
		initFetch();
		return true;

	default:
		return false;
	}

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
bool P6502CC::SBCExecHdlr() {
	switch (mExecMicroCycle++) {

	case 0:
	{
		if (SBCCalc()) {
			initFetch();
			return true;
		}
		return false;
	}

	default:
		return false;
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
bool P6502CC::SECExecHdlr()
{
	mStatusRegister |= C_set_mask;

	mExecMicroCycle++;
	initFetch();
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
bool P6502CC::SEDExecHdlr()
{
	mStatusRegister |= D_set_mask;

	mExecMicroCycle++;
	initFetch();
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
bool P6502CC::SEIExecHdlr()
{
	mStatusRegister |= I_set_mask;

	mExecMicroCycle++;
	initFetch();
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
bool P6502CC::STAExecHdlr()
{
	switch (mExecMicroCycle++) {

	case 0:

		prepMemWrite(mOperandAddress, mAcc);
		return true;

	case 1:
		initFetch();
		return true;

	default:
		return false;
	}

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
bool P6502CC::STXExecHdlr() {

	switch (mExecMicroCycle++) {

	case 0:

		prepMemWrite(mOperandAddress, mRegisterX);
		return true;

	case 1:
		initFetch();
		return true;

	default:
		return false;
	}
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
bool P6502CC::STYExecHdlr() {

	switch (mExecMicroCycle++) {

	case 0:

		prepMemWrite(mOperandAddress, mRegisterY);
		return true;

	case 1:
		initFetch();
		return true;

	default:
		return false;
	}

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
bool P6502CC::TAXExecHdlr()
{

	switch (mExecMicroCycle++) {

	case 0:

		mRegisterX = mAcc;
		setNZflags(mRegisterX);
		initFetch();
		return true;

	default:
		return false;
	}

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
bool P6502CC::TAYExecHdlr()
{

	switch (mExecMicroCycle++) {

	case 0:

		mRegisterY = mAcc;
		setNZflags(mRegisterY);
		initFetch();
		return true;

	default:
		return false;
	}

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
bool P6502CC::TSXExecHdlr()
{

	switch (mExecMicroCycle++) {

	case 0:

		mRegisterX = mStackPointer;
		setNZflags(mRegisterX);
		initFetch();
		return true;

	default:
		return false;
	}
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
bool P6502CC::TXAExecHdlr()
{

	switch (mExecMicroCycle++) {

	case 0:

		mAcc = mRegisterX;
		setNZflags(mAcc);
		initFetch();
		return true;

	default:
		return false;
	}
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
bool P6502CC::TXSExecHdlr()
{

	switch (mExecMicroCycle++) {

	case 0:

		mStackPointer = mRegisterX;
		initFetch();
		return true;

	default:
		return false;
	}
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
bool P6502CC::TYAExecHdlr() {

	switch (mExecMicroCycle++) {

	case 0:

		mAcc = mRegisterY;
		setNZflags(mAcc);
		initFetch();
		return true;

	default:
		return false;
	}
}




/////////////////////////////////////////////////////////////////////////
//
// Undocumented (inoffical) instructions
//
/////////////////////////////////////////////////////////////////////////



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


bool P6502CC::undefinedExecHdlr()
{
	return false;
}