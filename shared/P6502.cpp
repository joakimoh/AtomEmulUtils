
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
#include "ClockedDevice.h"

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

P6502::P6502(string name, double deviceClockRate, double tickRate, DebugTracing  *debugTracing, ConnectionManager *connectionManager, DeviceManager* deviceManager):
	Device(name, P6502_DEV, MICROROCESSOR_DEVICE, debugTracing, connectionManager), mDeviceManager(deviceManager),
	ClockedDevice(tickRate, deviceClockRate)
{
	cPeriod = (int) round(1000 / tickRate);

	// Specify ports that can be connected to other devices
	registerPort("RESET", IN_PORT, 0x01, RESET, &mRESET);
	registerPort("IRQ", IN_PORT, 0x01, IRQ, &mIRQ);
	registerPort("NMI", IN_PORT, 0x01, NMI, &mNMI);
	registerPort("SO", IN_PORT, 0x01, SO, &mSO);
	registerPort("RDY", IN_PORT, 0x01, RDY, &mRDY);
	registerPort("SYNC", OUT_PORT, 0x01, SYNC, &mSYNC);
	registerPort("RW", OUT_PORT, 0x01, RW, &mRW);
	registerPort("DATA", OUT_PORT, 0x01, DATA, &mDATA);
	registerPort("ADDRESS", OUT_PORT, ADDRESS, &mADDRESS);

	initInstrTable();


}

P6502::~P6502()
{
	if (pInstrDataTbl != nullptr)
		delete pInstrDataTbl;
}

bool P6502::ADCCalc()
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

bool P6502::SBCCalc()
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
	mPageBoundaryCrossed = (((before >> 8) ^ (after >> 8)) != 0);
	return mPageBoundaryCrossed;
}

void P6502::tick(int cycles)
{
	deviceTick(cycles);
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


bool P6502::readMem(uint16_t adr, uint8_t& data)
{
	MemoryMappedDevice* dev;
	if ((dev = mDeviceManager->getSelectedMemoryMappedDevice(adr)) != NULL)
		return dev->read(adr, data);
	data = 0xff;
	return true;
}

bool P6502::writeMem(uint16_t adr, uint8_t data)
{
	MemoryMappedDevice* dev;
	if ((dev = mDeviceManager->getSelectedMemoryMappedDevice(adr)) != NULL)
		return dev->write(adr, data);
	return true;
}

string P6502::stack2Str()
{
	if (mStackPointer < 0xff) {
		stringstream sout;
		uint16_t a = (0x100 + mStackPointer + 1) & 0x1ff;
		uint8_t l, h;
		readMem(a, l);
		readMem(a + 1, h);
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
		readMem(a, d);
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
		readMem(a, d);
		s += "\tmem[0x" + Utility::int2HexStr(a, 4) + "] = 0x" + Utility::int2HexStr(d, 2) + "\n";
	}
	return s;
}

// Outputs the internal state of the device
bool P6502::outputState(ostream& sout)
{
	uint16_t pc = mProgramCounter;
	uint16_t a = mOpcodePC;
	string n_instr_s;

	// Read up to three bytes, if possible
	vector<uint8_t> bytes = {0, 0, 0};
	if (!readMem(a++, bytes[0]))
		return false;
	if (readMem(a, bytes[1]))
		a++;
	if (readMem(a, bytes[2]))
		a++;

	if (!mCodec.decodeInstrFromBytes(pc, bytes, n_instr_s, false)) {
		return false;
	}
	string x_instr_s = mCodec.decode(pOpcodePC, mOpcode, mOperand16);
	
	sout << "Executed instruction:    " << setfill(' ') << setw(30) << left << x_instr_s << "\n";
	sout << "Resulting State:         " << getState() << "\n";
	sout << "Next instruction:        " << setfill(' ') << setw(30) << left << n_instr_s << "\n";
	sout << "IRQ = " << (int)mIRQ << "\n";
	sout << "RESET = " << (int)mRESET << "\n";
	sout << "NMI = " << (int)mNMI << "\n";
	sout << "SO = " << (int)mSO << "\n";
	sout << "RDY = " << (int)mRDY << "\n";

	return true;
}

bool P6502::getInstrLogData(InstrLogData& instrLogData) {
	instrLogData = mInstrLogData;
	return true;
}

bool P6502::setInstrLogData() {
	mInstrLogData.logTime = getCycleCount() / (mTickRate * 1e6);
	mInstrLogData.instr = pInstructionInfo;
	mInstrLogData.A = mAcc;
	mInstrLogData.X = mRegisterX;
	mInstrLogData.Y = mRegisterY;
	mInstrLogData.SP = mStackPointer;
	mInstrLogData.SR = mStatusRegister;
	mInstrLogData.opcodePC = mOpcodePC;
	mInstrLogData.PC = mProgramCounter;
	mInstrLogData.opcode = mOpcode;
	mInstrLogData.operand = mOperand16;
	mInstrLogData.accessAdr = mOperandAddress;
	mInstrLogData.activeIRQ = (!mIRQ && mIrqTransition);
	mInstrLogData.activeNMI = (!mNMI && mNmiTransition);
	mInstrLogData.execFailure = !mExecSuccess;
	mInstrLogData.readVal = mReadVal;
	mInstrLogData.writtenVal = mWrittenVal;
	uint16_t a = (0x100 + mStackPointer + 1) & 0x1ff;
	uint8_t l, h;
	readMem(a, l);
	readMem(a + 1, h);
	uint16_t w = (h << 8) | l;
	mInstrLogData.stack = w;

	mInstrLogData.cycles = mExecutedCycles;

	if (mMemLogAdr > 0 && mDeviceManager != NULL) {
		uint8_t data;
		mDeviceManager->dumpDeviceMemory(mMemLogAdr, data);
		mInstrLogData.memContent = (int)data;
	}

	int expected_cycles = mInstrLogData.instr->cycles;
	Codec6502::InstructionInfo& instr_info = *mInstrLogData.instr;
	Codec6502::ModeInfo &mode_info = Codec6502::MODE_INFO[instr_info.mode];
	int extra_cycles = 0;
	if (mPageBoundaryCrossed)
		extra_cycles++;
	if (mBranchTaken)
		extra_cycles++;

	if (mInstrLogData.cycles != expected_cycles + extra_cycles) {
		cout << "*** E" << expected_cycles + extra_cycles << " X" << mInstrLogData.cycles << " " << (mPageBoundaryCrossed ? " P" : "") << (mBranchTaken ? " B" : "") << " ";
		printInstrLogData(cout, mInstrLogData);
		cout << "\n";
	}

	return true;
}

bool P6502::printInstrLogData(ostream& sout, InstrLogData& instrLogData)
{
	if (instrLogData.instr == nullptr) {
		sout << "???";
		return true;
	}

	string instr_s = mCodec.decode(instrLogData.opcodePC, instrLogData.opcode, instrLogData.operand);
	Codec6502::InstructionInfo instr = *instrLogData.instr;

	string t_s = Utility::encodeCPUTime(instrLogData.logTime);

	int expected_cycles = instrLogData.instr->cycles;
	bool possible_extra_cycles = Codec6502::MODE_INFO[instr.mode].addCycleAtPageBoundary && !instr.writesMem;
	string  expected_cycles_s = to_string(expected_cycles) + (possible_extra_cycles ? + "+": " ");
	sout << t_s << " [" << instrLogData.cycles << "(" << expected_cycles_s << ")] " << setfill(' ') << setw(30) << left << instr_s << right <<
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