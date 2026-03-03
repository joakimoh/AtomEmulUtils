#ifndef P6502_H
#define P6502_H

#include "Codec6502.h"
#include "Device.h"
#include <vector>
#include <mutex>
#include "DebugTracing.h"
#include <cstdint>
#include <cmath>
#include <fstream>
#include "ClockedDevice.h"

using namespace std;

class P6502 : public Device, public ClockedDevice {

protected:

	int cPeriod = 1000; // clock period in ns

	uint64_t cycleCount = 0; // #clock cycles passed since RESET

	bool mStop = false;

	bool mResetTransition = false;
	bool mIrqTransition = false;
	bool mNmiTransition = false;
	bool mSOTransition = false;

	Codec6502 mCodec;

	// Registers
	uint16_t mProgramCounter = 0x0;	// Program Counter
	uint8_t mAcc = 0x0;				// Accumulator
	uint8_t mRegisterX = 0x0;		// Index Register mRegisterX
	uint8_t mRegisterY = 0x0;		// Index Register mRegisterY
	uint8_t mStackPointer = 0x0;	// Stack Pointer
	// Status Register
	// b7 = Negative, b6 = Overflow (V), b5 not used, b4 = break (B),
	// b3 = Decimal (D), b2 = Interrupt Disable (I),b1 = zero (Z), b0 = carry (C) 
	// The B flag is not stored in the register but only put on stack (as b4) together with
	// the Status Register content when a BRK instruction is executed.
	uint8_t mStatusRegister = 0x0;
#define N_set_mask 0x80 // 1000 0000 - b7
#define V_set_mask 0x40 // 0100 0000 - b6
#define b5_set_mask 0x30// 0010 0000 - b5 (not used)
#define B_set_mask 0x10 // 0001 0000 - b4
#define D_set_mask 0x08 // 0000 1000 - b3
#define I_set_mask 0x04 // 0000 0100 - b2
#define Z_set_mask 0x02 // 0000 0010 - b1
#define C_set_mask 0x01 // 0000 0001 - b0
#define N_flag ((mStatusRegister >> 7) & 1)
#define V_flag ((mStatusRegister >> 6) & 1)
#define D_flag ((mStatusRegister >> 3) & 1)
#define I_flag ((mStatusRegister >> 2) & 1)
#define Z_flag ((mStatusRegister >> 1) & 1)
#define C_flag (mStatusRegister & 1)

	void tick(int cycles = 1);

	bool pageBoundaryCrossed(uint16_t before, uint16_t after);

	void setNZflags(uint8_t val_8_u);
	void setNZCflags(uint8_t val_16_u, uint8_t C);
	void setNZVflags(uint8_t N, uint8_t Z, uint8_t V);
	void setNZCVflags(uint8_t N, uint8_t Z, uint8_t C, uint8_t V);

	// Information about an instructon required to execute it
	typedef struct struct_InstructionData {
		Codec6502::InstructionInfo info;
		bool (P6502::* addrHdlr)();
		bool (P6502::* execHdlr)();
	} InstructionData;
	InstructionData* pInstructionData = nullptr;
	Codec6502::InstructionInfo* pInstructionInfo = nullptr;


	// Information about all possible instructions (including unofficial undocumented ones and illegal ones)
	typedef struct InstrDataTable_struct { InstructionData data[256]; } InstrDataTable;
	InstrDataTable* pInstrDataTbl = nullptr;


	// Initialise the instruction data above
	void initInstrTable();


	// Addressing Modes (independent of the instruction)
	virtual bool accAdrHdlr() = 0;
	virtual bool impliedAdrHdlr() = 0;
	virtual bool relativeAdrHdlr() = 0;
	virtual bool immediateAdrHdlr() = 0;
	virtual bool zeroPageAdrHdlr() = 0;
	virtual bool zeroPageXAdrHdlr() = 0;
	virtual bool zeroPageYAdrHdlr() = 0;
	virtual bool absoluteAdrHdlr() = 0;
	virtual bool absoluteXAdrHdlr() = 0;
	virtual bool absoluteYAdrHdlr() = 0;
	virtual bool indirectAdrHdlr() = 0;
	virtual bool preIndXAdrHdlr() = 0;
	virtual bool postIndYAdrHdlr() = 0;
	virtual bool undefinedAdrHdlr() = 0;



	// Instruction handlers (independent of the addressing mode)
	virtual bool ADCExecHdlr() = 0;
	virtual bool ANDExecHdlr() = 0;
	virtual bool ASLExecHdlr() = 0;
	virtual bool BCCExecHdlr() = 0;
	virtual bool BCSExecHdlr() = 0;
	virtual bool BEQExecHdlr() = 0;
	virtual bool BITExecHdlr() = 0;
	virtual bool BMIExecHdlr() = 0;
	virtual bool BNEExecHdlr() = 0;
	virtual bool BPLExecHdlr() = 0;
	virtual bool BRKExecHdlr() = 0;
	virtual bool BVCExecHdlr() = 0;
	virtual bool BVSExecHdlr() = 0;
	virtual bool CLCExecHdlr() = 0;
	virtual bool CLDExecHdlr() = 0;
	virtual bool CLIExecHdlr() = 0;
	virtual bool CLVExecHdlr() = 0;
	virtual bool CMPExecHdlr() = 0;
	virtual bool CPXExecHdlr() = 0;
	virtual bool CPYExecHdlr() = 0;
	virtual bool DECExecHdlr() = 0;
	virtual bool DEXExecHdlr() = 0;
	virtual bool DEYExecHdlr() = 0;
	virtual bool EORExecHdlr() = 0;
	virtual bool INCExecHdlr() = 0;
	virtual bool INXExecHdlr() = 0;
	virtual bool INYExecHdlr() = 0;
	virtual bool JMPExecHdlr() = 0;
	virtual bool JSRExecHdlr() = 0;
	virtual bool LDAExecHdlr() = 0;
	virtual bool LDXExecHdlr() = 0;
	virtual bool LDYExecHdlr() = 0;
	virtual bool LSRExecHdlr() = 0;
	virtual bool NOPExecHdlr() = 0;
	virtual bool ORAExecHdlr() = 0;
	virtual bool PHAExecHdlr() = 0;
	virtual bool PHPExecHdlr() = 0;
	virtual bool PLAExecHdlr() = 0;
	virtual bool PLPExecHdlr() = 0;
	virtual bool ROLExecHdlr() = 0;
	virtual bool RORExecHdlr() = 0;
	virtual bool RTIExecHdlr() = 0;
	virtual bool RTSExecHdlr() = 0;
	virtual bool SBCExecHdlr() = 0;
	virtual bool SECExecHdlr() = 0;
	virtual bool SEDExecHdlr() = 0;
	virtual bool SEIExecHdlr() = 0;
	virtual bool STAExecHdlr() = 0;
	virtual bool STXExecHdlr() = 0;
	virtual bool STYExecHdlr() = 0;
	virtual bool TAXExecHdlr() = 0;
	virtual bool TAYExecHdlr() = 0;
	virtual bool TSXExecHdlr() = 0;
	virtual bool TXAExecHdlr() = 0;
	virtual bool TXSExecHdlr() = 0;
	virtual bool TYAExecHdlr() = 0;
	virtual bool LAXExecHdlr() = 0;
	virtual bool SBXExecHdlr() = 0;
	virtual bool ISCExecHdlr() = 0;
	virtual bool DCPExecHdlr() = 0;
	virtual bool ANCExecHdlr() = 0;
	virtual bool ALRExecHdlr() = 0;
	virtual bool ARRExecHdlr() = 0;
	virtual bool LASExecHdlr() = 0;
	virtual bool RLAExecHdlr() = 0;
	virtual bool RRAExecHdlr() = 0;
	virtual bool SAXExecHdlr() = 0;
	virtual bool SLOExecHdlr() = 0;
	virtual bool SREExecHdlr() = 0;
	virtual bool undefinedExecHdlr() = 0;

	string getState();

	// Ports that can be connected to other devices
	int NMI = 0, IRQ = 0, SO = 0, RDY = 0, SYNC = 0, RW = 0;
	int ADDRESS = 0, DATA = 0;
	uint8_t mNMI = 0x1;
	uint8_t mIRQ = 0x1;
	uint8_t mSO = 0x1;
	uint8_t mRDY = 0x1;
	uint8_t mSYNC = 0x1;
	uint8_t mRW = 0x1;// 1 for read, 0 for write
	uint16_t mADDRESS = 0x0;
	uint8_t mDATA = 0x0;

	uint8_t pNMI = 0x1;
	uint8_t pIRQ = 0x1;
	uint8_t pSO = 0x1;

	InstrLogData mInstrLogData; // Saved data about the last executed instruction - to be used for logging and debugging purposes
	
	// State of the current instruction execution
	uint8_t mOpcode = 0xff;			
	uint16_t mOperand16 = 0xffff;		// The operand bytes (up to two bytes) of the current instruction (if applicable)
	uint16_t mOpcodePC = 0xffff;		// The program counter value at which the opcode of the current instruction was fetched
	uint16_t mOperandAddress = 0;		// Calculated effective address of the instruction's operand (if applicable)
	uint8_t mReadVal = 0xff;			// Value read from memory as part of the operand evaluation (if applicable)
	uint8_t mWrittenVal = 0xff;			// Value written to memory as part of the instruction execution (if applicable)
	int mRAccAdr = -1;					// Memory address read by the instruction (if applicable) or -1 if no memory read is made or the instruction only reads from the accumulator
	int mWAccAdr = -1;					// Memory address written to by the instruction (if applicable) or -1 if no memory write is made or the instruction only writes to the accumulator


	DeviceManager* mDeviceManager = NULL;

	int mExecutedCycles = 0;
	bool mExecSuccess = true;

	bool mBranchTaken = false;
	bool mPageBoundaryCrossed = false;

	int mMemLogAdr = -1;

	virtual bool serveNMI() = 0;
	virtual bool serveIRQ() = 0;

	bool readMem(uint16_t address, uint8_t& data);
	bool writeMem(uint16_t address, uint8_t data);

	string stack2Str();
	string getInterruptStack(uint16_t stackStart, uint16_t oStackPointer, uint16_t oProgramCounter, uint16_t oStatusRegister);
	string getCallStack();

	bool setInstrLogData();

	bool ADCCalc();
	bool SBCCalc();

public:

	MemoryMappedDevice* mZPMemDev = NULL;
	MemoryMappedDevice* mStackMemDev = NULL;

	P6502(string name, double deviceClockRate, double tickRate, DebugTracing* debugTracing, ConnectionManager* connectionManager, DeviceManager* deviceManager);
	~P6502();

	// Reset device
	virtual bool reset() = 0;

	// Device power on
	bool power() { return reset(); }

	// Advance one instruction if the stop cycle hasn't already been reached
	virtual bool advanceInstr(uint64_t& endCycle) = 0;

	int getPC() { return (int)mProgramCounter; }

	int getOpcodePC() { return (int)mOpcodePC; }
	uint8_t getOpcode() { return mOpcode; }

	// Outputs the internal state of the device
	bool outputState(ostream& sout) override;

	int readAdr(uint8_t& data) { data = mReadVal;  return (int)mRAccAdr; }
	int writtenAdr(uint8_t& data) { data = mWrittenVal;  return (int)mWAccAdr; }
	int operandAddress() { return (int)mOperandAddress; }

	Codec6502::InstructionInfo getInstrInfo() {
		return *pInstructionInfo;
	}

	bool getInstrLogData(InstrLogData& instr_log_data);
	bool printInstrLogData(ostream& sout, InstrLogData& instr_log_data);
	void setMemLogging(int memLogAdr) { mMemLogAdr = memLogAdr; }

	bool setAcc(uint8_t val) { mAcc = val; return true; }
	bool setRegX(uint8_t val) { mRegisterX = val; return true;}
	bool setRegY(uint8_t val) { mRegisterY = val; return true; }
	bool setSP(uint8_t val) { mStackPointer = val; return true; }
	bool setSR(uint8_t val) { mStatusRegister = val; return true;}
	bool setPC(uint16_t val) { mProgramCounter = val; return true;}

};

#endif