#ifndef P6502IC_H
#define P6502IC_H

#include "Codec6502.h"
#include "Device.h"
#include <vector>
#include <mutex>
#include "DebugTracing.h"
#include <cstdint>
#include <cmath>
#include <fstream>
#include "ClockedDevice.h"
#include "P6502.h"

using namespace std;

class P6502IC : public P6502 {

private:

	int cPeriod = 1000; // clock period in ns

	uint64_t cycleCount = 0; // #clock cycles passed since RESET

	bool mStop = false;

	bool mResetTransition = false;
	bool mIrqTransition = false;
	bool mNmiTransition = false;
	bool mSOTransition = false;

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

	//
	// Handling of variable cycles for branch instructions.
	// 
	// Variable cycles for other instructions are handled by the AdrHdlr above.
	// 
	// For the branch instruction the extra cycles shall only be added if
	// the branch is taken which is not known before the instruction is executed.
	//
	bool addBranchTakenCycles();



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

	// Execute an instruction which opcode has been fetched already 
	bool executeInstr();



	bool readDevice(uint16_t adr, uint8_t& data);
	bool writeDevice(uint16_t adr, uint8_t data);

	bool readProgramMem(uint16_t adr, uint8_t& data);
	bool readProgramMem(uint16_t adr, uint8_t& data, bool adjustTiming);
	bool readZP(uint8_t adr, uint8_t& data);

	void push(uint8_t v);
	void pull(uint8_t& v);
	void pushWord(uint16_t word);
	void pullWord(uint16_t& word);
	

	void  adjustForWaitStates(MemoryMappedDevice* dev);


	int mExecutedCycles = 0;
	bool mExecSuccess = true;

	int mMemLogAdr = -1;

	bool serveNMI() override;
	bool serveIRQ() override;

public:

	P6502IC(string name, double deviceClockRate, double tickRate, DebugTracing* debugTracing, ConnectionManager* connectionManager, DeviceManager* deviceManager);
	~P6502IC();

	// Reset device
	bool reset() override;

	// Advance until time tickTime
	bool advanceUntil(uint64_t tickTime) override;

	// Advance one instruction if the stop cycle hasn't already been reached
	bool advanceInstr(uint64_t& endTick) override;

};

#endif