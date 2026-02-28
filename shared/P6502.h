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
	virtual bool accAdrHdlr() { return true; }
	virtual bool impliedAdrHdlr() { return true; }
	virtual bool relativeAdrHdlr() { return true; }
	virtual bool immediateAdrHdlr() { return true; }
	virtual bool zeroPageAdrHdlr() { return true; }
	virtual bool zeroPageXAdrHdlr() { return true; }
	virtual bool zeroPageYAdrHdlr() { return true; }
	virtual bool absoluteAdrHdlr() { return true; }
	virtual bool absoluteXAdrHdlr() { return true; }
	virtual bool absoluteYAdrHdlr() { return true; }
	virtual bool indirectAdrHdlr() { return true; }
	virtual bool preIndXAdrHdlr() { return true; }
	virtual bool postIndYAdrHdlr() { return true; }
	virtual bool undefinedAdrHdlr() { return true; }



	// Instruction handlers (independent of the addressing mode)
	virtual bool ADCExecHdlr() { return true; }
	virtual bool ANDExecHdlr() { return true; }
	virtual bool ASLExecHdlr() { return true; }
	virtual bool BCCExecHdlr() { return true; }
	virtual bool BCSExecHdlr() { return true; }
	virtual bool BEQExecHdlr() { return true; }
	virtual bool BITExecHdlr() { return true; }
	virtual bool BMIExecHdlr() { return true; }
	virtual bool BNEExecHdlr() { return true; }
	virtual bool BPLExecHdlr() { return true; }
	virtual bool BRKExecHdlr() { return true; }
	virtual bool BVCExecHdlr() { return true; }
	virtual bool BVSExecHdlr() { return true; }
	virtual bool CLCExecHdlr() { return true; }
	virtual bool CLDExecHdlr() { return true; }
	virtual bool CLIExecHdlr() { return true; }
	virtual bool CLVExecHdlr() { return true; }
	virtual bool CMPExecHdlr() { return true; }
	virtual bool CPXExecHdlr() { return true; }
	virtual bool CPYExecHdlr() { return true; }
	virtual bool DECExecHdlr() { return true; }
	virtual bool DEXExecHdlr() { return true; }
	virtual bool DEYExecHdlr() { return true; }
	virtual bool EORExecHdlr() { return true; }
	virtual bool INCExecHdlr() { return true; }
	virtual bool INXExecHdlr() { return true; }
	virtual bool INYExecHdlr() { return true; }
	virtual bool JMPExecHdlr() { return true; }
	virtual bool JSRExecHdlr() { return true; }
	virtual bool LDAExecHdlr() { return true; }
	virtual bool LDXExecHdlr() { return true; }
	virtual bool LDYExecHdlr() { return true; }
	virtual bool LSRExecHdlr() { return true; }
	virtual bool NOPExecHdlr() { return true; }
	virtual bool ORAExecHdlr() { return true; }
	virtual bool PHAExecHdlr() { return true; }
	virtual bool PHPExecHdlr() { return true; }
	virtual bool PLAExecHdlr() { return true; }
	virtual bool PLPExecHdlr() { return true; }
	virtual bool ROLExecHdlr() { return true; }
	virtual bool RORExecHdlr() { return true; }
	virtual bool RTIExecHdlr() { return true; }
	virtual bool RTSExecHdlr() { return true; }
	virtual bool SBCExecHdlr() { return true; }
	virtual bool SECExecHdlr() { return true; }
	virtual bool SEDExecHdlr() { return true; }
	virtual bool SEIExecHdlr() { return true; }
	virtual bool STAExecHdlr() { return true; }
	virtual bool STXExecHdlr() { return true; }
	virtual bool STYExecHdlr() { return true; }
	virtual bool TAXExecHdlr() { return true; }
	virtual bool TAYExecHdlr() { return true; }
	virtual bool TSXExecHdlr() { return true; }
	virtual bool TXAExecHdlr() { return true; }
	virtual bool TXSExecHdlr() { return true; }
	virtual bool TYAExecHdlr() { return true; }
	virtual bool LAXExecHdlr() { return true; }
	virtual bool SBXExecHdlr() { return true; }
	virtual bool ISCExecHdlr() { return true; }
	virtual bool DCPExecHdlr() { return true; }
	virtual bool ANCExecHdlr() { return true; }
	virtual bool ALRExecHdlr() { return true; }
	virtual bool ARRExecHdlr() { return true; }
	virtual bool LASExecHdlr() { return true; }
	virtual bool RLAExecHdlr() { return true; }
	virtual bool RRAExecHdlr() { return true; }
	virtual bool SAXExecHdlr() { return true; }
	virtual bool SLOExecHdlr() { return true; }
	virtual bool SREExecHdlr() { return true; }
	virtual bool undefinedExecHdlr() { return true; }

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

	int mMemLogAdr = -1;

	virtual bool serveNMI() { return true; }
	virtual bool serveIRQ() { return true; }

	bool readMem(uint16_t address, uint8_t& data);
	bool writeMem(uint16_t address, uint8_t data);

	string stack2Str();
	string getInterruptStack(uint16_t stackStart, uint16_t oStackPointer, uint16_t oProgramCounter, uint16_t oStatusRegister);
	string getCallStack();

	bool setInstrLogData();

public:

	MemoryMappedDevice* mZPMemDev = NULL;
	MemoryMappedDevice* mStackMemDev = NULL;

	P6502(string name, double deviceClockRate, double tickRate, DebugTracing* debugTracing, ConnectionManager* connectionManager, DeviceManager* deviceManager);
	~P6502();

	// Reset device
	virtual bool reset() { return true; }

	// Device power on
	bool power() { return reset(); }

	// Advance one instruction if the stop cycle hasn't already been reached
	virtual bool advanceInstr(uint64_t& endCycle) { return true; }

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