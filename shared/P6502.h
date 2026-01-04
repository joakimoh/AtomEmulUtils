#ifndef P6052_H
#define P6502_H

#include "Codec6502.h"
#include "Device.h"
#include <vector>
#include <mutex>
#include "DebugManager.h"
#include <cstdint>
#include <cmath>
#include <fstream>

using namespace std;

class P6502 : public Device {

private:

	int cPeriod = 1000; // clock period in ns

	uint64_t cycleCount = 0; // #clock cycles passed since RESET

	mutex mMutex;

	bool mStop = false;

	bool mResetTransition = false;
	bool mIrqTransition = false;
	bool mNmiTransition = false;

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
		bool (P6502::*addrHdlr)();
		bool (P6502::*execHdlr)();
	} InstructionData;

	Codec6502::InstructionInfo mInstructionInfo;

	
	// Addressing Modes (independent of the instruction)
	bool accAdrHdlr();
	bool impliedAdrHdlr();
	bool relativeAdrHdlr();
	bool immediateAdrHdlr();
	bool zeroPageAdrHdlr();
	bool zeroPageXAdrHdlr();
	bool zeroPageYAdrHdlr();
	bool absoluteAdrHdlr();
	bool absoluteXAdrHdlr();
	bool absoluteYAdrHdlr();
	bool indirectAdrHdlr();
	bool preIndXAdrHdlr();
	bool postIndYAdrHdlr();
	bool undefinedAdrHdlr();



	// Instruction handlers (independent of the addressing mode)
	bool ADCExecHdlr();
	bool ANDExecHdlr();
	bool ASLExecHdlr();
	bool BCCExecHdlr();
	bool BCSExecHdlr();
	bool BEQExecHdlr();
	bool BITExecHdlr();
	bool BMIExecHdlr();
	bool BNEExecHdlr();
	bool BPLExecHdlr();
	bool BRKExecHdlr();
	bool BVCExecHdlr();
	bool BVSExecHdlr();
	bool CLCExecHdlr();
	bool CLDExecHdlr();
	bool CLIExecHdlr();
	bool CLVExecHdlr();
	bool CMPExecHdlr();
	bool CPXExecHdlr();
	bool CPYExecHdlr();
	bool DECExecHdlr();
	bool DEXExecHdlr();
	bool DEYExecHdlr();
	bool EORExecHdlr();
	bool INCExecHdlr();
	bool INXExecHdlr();
	bool INYExecHdlr();
	bool JMPExecHdlr();
	bool JSRExecHdlr();
	bool LDAExecHdlr();
	bool LDXExecHdlr();
	bool LDYExecHdlr();
	bool LSRExecHdlr();
	bool NOPExecHdlr();
	bool ORAExecHdlr();
	bool PHAExecHdlr();
	bool PHPExecHdlr();
	bool PLAExecHdlr();
	bool PLPExecHdlr();
	bool ROLExecHdlr();
	bool RORExecHdlr();
	bool RTIExecHdlr();
	bool RTSExecHdlr();
	bool SBCExecHdlr();
	bool SECExecHdlr();
	bool SEDExecHdlr();
	bool SEIExecHdlr();
	bool STAExecHdlr();
	bool STXExecHdlr();
	bool STYExecHdlr();
	bool TAXExecHdlr();
	bool TAYExecHdlr();
	bool TSXExecHdlr();
	bool TXAExecHdlr();
	bool TXSExecHdlr();
	bool TYAExecHdlr();
	bool LAXExecHdlr();
	bool SBXExecHdlr();
	bool ISCExecHdlr();
	bool DCPExecHdlr();
	bool ANCExecHdlr();
	bool ALRExecHdlr();
	bool ARRExecHdlr();
	bool LASExecHdlr();
	bool RLAExecHdlr();
	bool RRAExecHdlr();
	bool SAXExecHdlr();
	bool SLOExecHdlr();
	bool SREExecHdlr();
	bool undefinedExecHdlr();

	// Execute an instruction which opcode has been fetched already 
	bool executeInstr();

	// Information about all possible instructions (including unofficial undocumented ones and illegal ones)
	InstructionData mInstrData[256];

	// Initialise the instruction data above
	void initInstrTable();


	string getState();

	// Ports that can be connected to other devices
	int NMI = 0, IRQ = 0;
	uint8_t mNMI = 0x1;
	uint8_t mIRQ = 0x1;

	uint8_t pNMI = 0x1;
	uint8_t pIRQ = 0x1;

	int mRAccAdr = -1;
	int mWAccAdr = -1;
	uint8_t mReadVal8 = 0xff;
	uint8_t mWrittenVal = 0xff;
	uint16_t mOpcodePC = 0xffff;
	uint8_t mOpcode = 0xff;
	uint16_t mOperand16 = 0xffff;
	uint16_t mOperandAddress = 0xffff;	
	
	bool writeDevice(uint16_t adr, uint8_t data);

	bool readProgramMem(uint16_t adr, uint8_t& data);
	bool readProgramMem(uint16_t adr, uint8_t& data, bool adjustTiming);
	bool readZP(uint8_t adr, uint8_t& data);

	string getInterruptStack(uint16_t stackStart, uint16_t oStackPointer, uint16_t oProgramCounter, uint16_t oStatusRegister);
	string getCallStack();

	void push(uint8_t v);
	void pull(uint8_t& v);
	void pushWord(uint16_t word);
	void pullWord(uint16_t& word);
	string stack2Str();
	
	void  adjustForWaitStates(MemoryMappedDevice* dev);

	DeviceManager* mDeviceManager = NULL;

	int mExecutedCycles = 0;
	bool mExecSuccess = true;

public:

	bool readDevice(uint16_t adr, uint8_t& data);

	MemoryMappedDevice* mZPMemDev = NULL;
	MemoryMappedDevice* mStackMemDev = NULL;

	P6502(string name, double clockSpeed, DebugManager  *debugManager, ConnectionManager* connectionManager, DeviceManager *deviceManager);
	~P6502();

	// Reset device
	bool reset(); 

	// Device power on
	bool power() { return reset(); }

	bool serveNMI();
	bool serveIRQ();


	// Advance until clock cycle stopcycle has been reached
	bool advanceUntil(uint64_t stopCycle);

	// Advance one instruction if the stop cycle hasn't already been reached
	bool advanceInstr(uint64_t& endCycle);

	int getPC() { return (int)mProgramCounter; }

	int getOpcodePC() { return (int)mOpcodePC; }
	uint8_t getOpcode() { return mOpcode; }

	// Outputs the internal state of the device
	bool outputState(ostream& sout) override;

	int readAdr(uint8_t& data) { data = mReadVal8;  return (int)mRAccAdr; }
	int writtenAdr(uint8_t& data) { data = mWrittenVal;  return (int)mWAccAdr; }
	int operandAddress() { return (int)mOperandAddress; }

	Codec6502::InstructionInfo getInstrInfo() {
		return mInstructionInfo;
	}

	bool getInstrLogData(InstrLogData& instr_log_data);
	bool printInstrLogData(ostream& sout, InstrLogData& instr_log_data);

};

#endif