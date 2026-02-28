#ifndef P6502CC_H
#define P6502CC_H

#include "Codec6502.h"
#include "Device.h"
#include <vector>
#include <mutex>
#include "DebugTracing.h"
#include <cstdint>
#include <cmath>
#include <fstream>
#include "ClockedDevice.h"
#include "MemoryMappedDevice.h"
#include "DeviceManager.h"
#include "P6502.h"

using namespace std;

class P6502CC : public P6502 {

private:


	// Addressing Modes (independent of the instruction)
	bool accAdrHdlr() override;
	bool impliedAdrHdlr() override;
	bool relativeAdrHdlr() override;
	bool immediateAdrHdlr() override;
	bool zeroPageAdrHdlr() override;
	bool zeroPageXAdrHdlr() override;
	bool zeroPageYAdrHdlr() override;
	bool absoluteAdrHdlr() override;
	bool absoluteXAdrHdlr() override;
	bool absoluteYAdrHdlr() override;
	bool indirectAdrHdlr() override;
	bool preIndXAdrHdlr() override;
	bool postIndYAdrHdlr() override;
	bool undefinedAdrHdlr() override;


	// Instruction handlers (independent of the addressing mode)
	bool ADCExecHdlr() override;
	bool ANDExecHdlr() override;
	bool ASLExecHdlr() override;
	bool BCCExecHdlr() override;
	bool BCSExecHdlr() override;
	bool BEQExecHdlr() override;
	bool BITExecHdlr() override;
	bool BMIExecHdlr() override;
	bool BNEExecHdlr() override;
	bool BPLExecHdlr() override;
	bool BRKExecHdlr() override;
	bool BVCExecHdlr() override;
	bool BVSExecHdlr() override;
	bool CLCExecHdlr() override;
	bool CLDExecHdlr() override;
	bool CLIExecHdlr() override;
	bool CLVExecHdlr() override;
	bool CMPExecHdlr() override;
	bool CPXExecHdlr() override;
	bool CPYExecHdlr() override;
	bool DECExecHdlr() override;
	bool DEXExecHdlr() override;
	bool DEYExecHdlr() override;
	bool EORExecHdlr() override;
	bool INCExecHdlr() override;
	bool INXExecHdlr() override;
	bool INYExecHdlr() override;
	bool JMPExecHdlr() override;
	bool JSRExecHdlr() override;
	bool LDAExecHdlr() override;
	bool LDXExecHdlr() override;
	bool LDYExecHdlr() override;
	bool LSRExecHdlr() override;
	bool NOPExecHdlr() override;
	bool ORAExecHdlr() override;
	bool PHAExecHdlr() override;
	bool PHPExecHdlr() override;
	bool PLAExecHdlr() override;
	bool PLPExecHdlr() override;
	bool ROLExecHdlr() override;
	bool RORExecHdlr() override;
	bool RTIExecHdlr() override;
	bool RTSExecHdlr() override;
	bool SBCExecHdlr() override;
	bool SECExecHdlr() override;
	bool SEDExecHdlr() override;
	bool SEIExecHdlr() override;
	bool STAExecHdlr() override;
	bool STXExecHdlr() override;
	bool STYExecHdlr() override;
	bool TAXExecHdlr() override;
	bool TAYExecHdlr() override;
	bool TSXExecHdlr() override;
	bool TXAExecHdlr() override;
	bool TXSExecHdlr() override;
	bool TYAExecHdlr() override;
	bool LAXExecHdlr() override;
	bool SBXExecHdlr() override;
	bool ISCExecHdlr() override;
	bool DCPExecHdlr() override;
	bool ANCExecHdlr() override;
	bool ALRExecHdlr() override;
	bool ARRExecHdlr() override;
	bool LASExecHdlr() override;
	bool RLAExecHdlr() override;
	bool RRAExecHdlr() override;
	bool SAXExecHdlr() override;
	bool SLOExecHdlr() override;
	bool SREExecHdlr() override;
	bool undefinedExecHdlr() override;

	// Temp variables for the reset()
	bool resetHdlr();
	uint8_t mVecLow = 0;

	// Servicing of NMI and IRQ
	bool serveNMI() override;
	bool serveIRQ() override;

	// Execute one micro cycle of an instruction which opcode has been fetched already 
	bool executeInstrMicroCycle();

	// Check if a page boundary is crossed between two addresses (used for some instructions to determine if an extra cycle is required)
	bool pageBoundaryCrossed(uint16_t before, uint16_t after);

	// Fetch the opcode of the next instruction to execute and set up the instruction data for it. Returns false if an error occurs.
	bool initFetch();
	bool completeFetch();

	// Read a byte from memory at the address specified by the current instruction and addressing mode. Returns false if an error occurs.
	bool initMemRead(uint16_t adr);
	bool prepMemRead(uint16_t adr);	
	bool initOperandByteRead();
	bool initDummyOperandRead();
	bool initDummyRead(uint16_t adr);

	// Write a byte to memory at the address specified by the current instruction and addressing mode. Returns false if an error occurs.
	bool prepMemWrite(uint16_t adr, uint8_t val);

	//
	// The 6502 executes instructions in up to 7 micro cycles (depending on the instruction and addressing mode) and
	// the internal state of the device can change in each of these micro cycles. The mOPerandMicroCycle and mExecMicroCycle are used to keep
	// track of which micro cycle of the instruction execution we are in.
	// In the last cycle, the next instruction will always be fetched.
	int mOPerandMicroCycle = 0; // For instructions with operand fetch cycles, this is used to keep track of which operand fetch cycle we are in (if more than one)
	int mExecMicroCycle = 0; // For instructions with execution cycles, this is used to keep track of which execution cycle we are in (if more than one)
	enum CPUExecState { IN_RESET, IN_IRQ, IN_NMI, FETCH_OPCODE, FETCH_OPERAND, EXECUTE_INSTRUCTION, UNDEFINED } mCPUExecState = UNDEFINED;

	int cPeriod = 1000; // clock period in ns

	// State of the current instruction execution (in addition to what the base class holds)
	uint8_t mCalcVal = 0;			// A value calculated as part of the instruction execution (e.g., the result of an addition in ADC) that will be written to memory or a register in a later micro cycle (if applicable)

	int getWaitStates(MemoryMappedDevice* dev);
	int mPendingWaitStates = 0; // Number of wait states that the CPU needs to wait to complete a memory access to a slow device

	bool mReadingOperandByte = true; // Whether the current memory access is for reading an operand byte (as opposed to for executing the instruction or for a dummy read)
	uint8_t mOperandBytes[2] = { 0 }; // For instructions with operand fetch cycles, this is used to store the operand byte(s) read during these cycles
	int mOperandByteCount = 0; // For instructions with operands, this is used to keep track of how many operand bytes have been read so far during these cycles

	enum InterruptState { NONE, NMI_PENDING, IRQ_PENDING, RESET_PENDING } mInterruptState = NONE; // Whether a RESET, an NMI or an IRQ is pending
	uint16_t mInterruptVector = 0; // The interrupt vector to jump to when the pending interrupt is executed
	bool mPendingNMI = false;
	bool mPendingIRQ = false;
	void fakeBRKFetch(); // Make IRQ/NMI appear as a BRK instruction during execution to support tracing also of their execution

	// Temp byte/word used when reading or there is a need to 'remember' a 16-bit value over two micro cycles (usually an address)
	uint8_t mTmpReadByte = 0x0;
	uint8_t mTmpReadWord = 0x0;

public:

	P6502CC(string name, double deviceClockRate, double tickRate, DebugTracing* debugTracing, ConnectionManager* connectionManager, DeviceManager* deviceManager);
	~P6502CC();

	// Reset device
	bool reset() override;

	// Advance one instruction if the stop cycle hasn't already been reached
	bool advanceInstr(uint64_t& endTick) override;

};

#endif