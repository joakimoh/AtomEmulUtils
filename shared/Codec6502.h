#ifndef CODEC_6502
#define CODEC_6502

#include <map>
#include <vector>
#include <string>

using namespace std;


class Codec6502 {

public:

	enum Instruction {

		// Valid instructions
		ADC = 0x00, AND = 0x01, ASL = 0x02, BCC = 0x03, BCS = 0x04, BEQ = 0x05, BIT = 0x06, BMI = 0x07,
		BNE = 0x08, BPL = 0x09, BRK = 0x0a, BVC = 0x0b, BVS = 0x0c, CLC = 0x0d, CLD = 0x0e, CLI = 0x0f,
		CLV = 0x10, CMP = 0x11, CPX = 0x12, CPY = 0x13, DEC = 0x14, DEX = 0x15, DEY = 0x16, EOR = 0x17,
		INC = 0x18, INX = 0x19, INY = 0x1a, JMP = 0x1b, JSR = 0x1c, LDA = 0x1d, LDX = 0x1e, LDY = 0x1f,
		LSR = 0x20, NOP = 0x21, ORA = 0x22, PHA = 0x23, PHP = 0x24, PLA = 0x25, PLP = 0x26, ROL = 0x27,
		ROR = 0x28, RTI = 0x29, RTS = 0x2a, SBC = 0x2b, SEC = 0x2c, SED = 0x2d, SEI = 0x2e, STA = 0x2f,
		STX = 0x30, STY = 0x31, TAX = 0x32, TAY = 0x33, TSX = 0x34, TXA = 0x35, TXS = 0x36, TYA = 0x37,

		// Undocumented NMOS instructions that are used by some programs although being illegal
		LAX = 0x38, SBX = 0x39, ISC = 0x3a, DCP = 0x3b,

		// Used when a non-existing instruction is encountered 
		UNDEFINED_INSTRUCTION = 0x3c
	};

	const vector<string> instr2str {

		// Valid instructions
		"ADC", "AND", "ASL", "BCC", "BCS", "BEQ", "BIT", "BMI", "BNE", "BPL", "BRK", "BVC", "BVS", "CLC",
		"CLD", "CLI", "CLV", "CMP", "CPX", "CPY", "DEC", "DEX", "DEY", "EOR", "INC", "INX", "INY", "JMP",
		"JSR", "LDA", "LDX", "LDY", "LSR", "NOP", "ORA", "PHA", "PHP", "PLA", "PLP", "ROL", "ROR", "RTI",
		"RTS", "SBC", "SEC", "SED", "SEI", "STA", "STX", "STY", "TAX", "TAY", "TSX", "TXA", "TXS", "TYA",

		// Undocumented NMOS instructions that are used by some programs although being illegal
		"LAX", "SBX", "ISC", "DCP",

		// Used when a non-existing instruction is encountered 
		"???"
	};

	enum Mode {
		Accumulator,	// OPC A
		Implied,		// OPC
		Relative,		// OPC <branch target>	-- add one cycle if page boundary crosseds
		Immediate,		// OPC #$12
		ZeroPage,		// OPC $12
		ZeroPage_X,		// OPC $12,X
		ZeroPage_Y,		// OPC $12,Y			-- add one cycle if page boundary crossed for some instructions
		Absolute,		// OPC $1234
		Absolute_X,		// OPC $1234,X			-- add one cycle if page boundary crossed for some instructions
		Absolute_Y,		// OPC $1234,Y			-- add one cycle if page boundary crossed for some instructions
		Indirect,		// OPC ($1234)
		PreInd_X,		// OPC ($12),X
		PostInd_Y,		// OPC ($12),Y			-- add one cycle if page boundary crossed for some instructions
		UndefinedMode	// Undefined
	};

	typedef struct InstructionInfo_struct {
		uint8_t				opcode;
		Instruction			instruction;
		Mode				mode;
		int					cycles;
		bool				addCycleAtPageBoundary;
		bool				readsMem;
		bool				writesMem;
		bool				undocNMOS = false;
	} InstructionInfo;

	const InstructionInfo invalidInstr { 0xff, UNDEFINED_INSTRUCTION, UndefinedMode, false, true };

	vector<InstructionInfo> instructions {


		{0x69, ADC, 					Immediate,		2, false,	false,	false, false},
		{0x65, ADC, 					ZeroPage,		3, false,	true,	false, false},
		{0x75, ADC, 					ZeroPage_X,		4, false,	true,	false, false},
		{0x6d, ADC, 					Absolute,		4, false,	true,	false, false},
		{0x7d, ADC, 					Absolute_X,		4, true,	true,	false, false},
		{0x79, ADC, 					Absolute_Y,		4, true,	true,	false, false},
		{0x61, ADC, 					PreInd_X,		6, false,	true,	false, false},
		{0x71, ADC, 					PostInd_Y,		5, true,	true,	false, false},

		{0x29, AND, 					Immediate,		2, false,	false,	false, false},
		{0x25, AND, 					ZeroPage,		3, false,	true,	false, false},
		{0x35, AND, 					ZeroPage_X,		4, false,	true,	false, false},
		{0x2d, AND, 					Absolute,		4, false,	true,	false, false},
		{0x3d, AND, 					Absolute_X,		4, true,	true,	false, false},
		{0x39, AND, 					Absolute_Y,		4, true,	true,	false, false},
		{0x21, AND, 					PreInd_X,		6, false,	true,	false, false},
		{0x31, AND, 					PostInd_Y,		5, true,	true,	false, false},

		{0x0a, ASL, 					Accumulator,	2, false,	false,	false, false},
		{0x06, ASL, 					ZeroPage,		5, false,	true,	true, false},
		{0x16, ASL, 					ZeroPage_X,		6, false,	true,	true, false},
		{0x0e, ASL, 					Absolute,		6, false,	true,	true, false},
		{0x1e, ASL, 					Absolute_X,		7, false,	true,	true, false},

		{0x90, BCC, 					Relative,		2, true,	false,	false, false},

		{0xb0, BCS, 					Relative,		2, true,	false,	false, false},

		{0xf0, BEQ, 					Relative,		2, true,	false,	false, false},

		{0x24, BIT, 					ZeroPage,		3, false,	true,	false, false},
		{0x2c, BIT, 					Absolute,		4, false,	true,	false, false},

		{0x30, BMI, 					Relative,		2, true,	false,	false, false},

		{0xd0, BNE, 					Relative,		2, true,	false,	false, false},

		{0x10, BPL, 					Relative,		2, true,	false,	false, false},

		{0x00, BRK,						Implied,		7, false,	false,	false, false},

		{0x50, BVC, 					Relative,		2, true,	false,	false, false},
		{0x70, BVS, 					Relative,		2, true,	false,	false, false},

		{0x18, CLC, 					Implied,		2, false,	false,	false, false},

		{0xd8, CLD, 					Implied,		2, false,	false,	false, false},

		{0x58, CLI, 					Implied,		2, false,	false,	false, false},

		{0xb8, CLV, 					Implied,		2, false,	false,	false, false},

		{0xc9, CMP, 					Immediate,		2, false,	false,	false, false},
		{0xc5, CMP, 					ZeroPage,		3, false,	true,	false, false},
		{0xd5, CMP, 					ZeroPage_X,		4, false,	true,	false, false},
		{0xcd, CMP, 					Absolute,		4, false,	true,	false, false},
		{0xdd, CMP, 					Absolute_X,		4, true,	true,	false, false},
		{0xd9, CMP, 					Absolute_Y,		4, true,	true,	false, false},
		{0xc1, CMP, 					PreInd_X,		6, false,	true,	false, false},
		{0xd1, CMP, 					PostInd_Y,		5, true,	true,	false, false},

		{0xe0, CPX, 					Immediate,		2, false,	false,	false, false},
		{0xe4, CPX, 					ZeroPage,		3, false,	true,	false, false},
		{0xec, CPX, 					Absolute,		4, false,	true,	false, false},

		{0xc0, CPY, 					Immediate,		2, false,	false,	false, false},
		{0xc4, CPY, 					ZeroPage,		3, false,	true,	false, false},
		{0xcc, CPY, 					Absolute,		4, false,	true,	false, false},

		{0xc6, DEC, 					ZeroPage,		5, false,	true,	true, false},
		{0xd6, DEC, 					ZeroPage_X,		6, false,	true,	true, false},
		{0xce, DEC, 					Absolute,		6, false,	true,	true, false},
		{0xde, DEC, 					Absolute_X,		7, false,	true,	true, false},

		{0xca, DEX, 					Implied,		2, false,	false,	false, false},

		{0x88, DEY, 					Implied,		2, false,	false,	false, false},

		{0x49, EOR, 					Immediate,		2, false,	false,	false, false},
		{0x45, EOR, 					ZeroPage,		3, false,	true,	false, false},
		{0x55, EOR, 					ZeroPage_X,		4, false,	true,	false, false},
		{0x4d, EOR, 					Absolute,		4, false,	true,	false, false},
		{0x5d, EOR, 					Absolute_X,		4, true ,	true,	false, false},
		{0x59, EOR, 					Absolute_Y,		4, true ,	true,	false, false},
		{0x41, EOR, 					PreInd_X,		6, false,	true,	false, false},
		{0x51, EOR, 					PostInd_Y,		5, true ,	true,	false, false},

		{0xe6, INC, 					ZeroPage,		5, false,	true,	true, false},
		{0xf6, INC, 					ZeroPage_X,		6, false,	true,	true, false},
		{0xee, INC, 					Absolute,		6, false,	true,	true, false},
		{0xfe, INC, 					Absolute_X,		7, false,	true,	true, false},

		{0xe8, INX, 					Implied,		2, false,	false,	false, false},

		{0xc8, INY, 					Implied,		2, false,	false,	false, false},

		{0x4c, JMP, 					Absolute,		3, false,	false,	false, false},
		{0x6c, JMP,						Indirect,		5, false,	false,	false, false},

		{0x20, JSR, 					Absolute,		6, false,	false,	false, false},

		{0xa9, LDA, 					Immediate,		2, false,	false,	false, false},
		{0xa5, LDA, 					ZeroPage,		3, false,	true,	false, false},
		{0xb5, LDA, 					ZeroPage_X,		4, false,	true,	false, false},
		{0xad, LDA, 					Absolute,		4, false,	true,	false, false},
		{0xbd, LDA, 					Absolute_X,		4, true ,	true,	false, false},
		{0xb9, LDA, 					Absolute_Y,		4, true ,	true,	false, false},
		{0xa1, LDA, 					PreInd_X,		6, false,	true,	false, false},
		{0xb1, LDA, 					PostInd_Y,		5, true ,	true,	false, false},

		{0xa2, LDX, 					Immediate,		2, false,	false,	false, false},
		{0xa6, LDX, 					ZeroPage,		3, false,	true,	false, false},
		{0xb6, LDX, 					ZeroPage_Y,		4, false,	true ,	false, false},
		{0xae, LDX, 					Absolute,		4, false,	true,	false, false},
		{0xbe, LDX, 					Absolute_Y,		4, true,	true,	false, false},
		

		{0xa0, LDY, 					Immediate,		2, false,	false,	false, false},
		{0xa4, LDY, 					ZeroPage,		3, false,	true,	false, false},
		{0xb4, LDY, 					ZeroPage_X,		4, false,	true,	false, false},
		{0xac, LDY, 					Absolute,		4, false,	true,	false, false},
		{0xbc, LDY, 					Absolute_X,		4, true ,	true,	false, false},

		{0x4a, LSR, 					Accumulator,	2, false,	false,	false, false},
		{0x46, LSR, 					ZeroPage,		5, false,	true,	true , false},
		{0x56, LSR, 					ZeroPage_X,		6, false,	true,	true , false},
		{0x4e, LSR, 					Absolute,		6, false,	true,	true , false},
		{0x5e, LSR, 					Absolute_X,		7, false,	true,	true , false},

		{0xea, NOP, 					Implied,		2, false,	false,	false, false},

		{0x09, ORA, 					Immediate,		2, false,	false,	false, false},
		{0x05, ORA, 					ZeroPage,		3, false,	true,	false, false},
		{0x15, ORA, 					ZeroPage_X,		4, false,	true,	false, false},
		{0x0d, ORA, 					Absolute,		4, false,	true ,	false, false},
		{0x1d, ORA, 					Absolute_X,		4, true,	true ,	false, false},
		{0x19, ORA, 					Absolute_Y,		4, true,	true ,	false, false},
		{0x01, ORA, 					PreInd_X,		6, false,	true ,	false, false},
		{0x11, ORA, 					PostInd_Y,		5, true,	true ,	false, false},

		{0x48, PHA, 					Implied,		3, false,	false,	false, false},

		{0x08, PHP, 					Implied,		3, false,	false,	false, false},

		{0x68, PLA, 					Implied,		4, false,	false,	false, false},

		{0x28, PLP, 					Implied,		4, false,	false,	false, false},

		{0x2a, ROL, 					Accumulator,	2, false,	false,	false, false},
		{0x26, ROL, 					ZeroPage,		5, false,	true,	true , false},
		{0x36, ROL, 					ZeroPage_X,		6, false,	true,	true , false},
		{0x2e, ROL, 					Absolute,		6, false,	true,	true , false},
		{0x3e, ROL, 					Absolute_X,		7, false,	true,	true , false},

		{0x6a, ROR, 					Accumulator,	2, false,	false,	false, false},
		{0x66, ROR, 					ZeroPage,		5, false,	true,	true , false},
		{0x76, ROR, 					ZeroPage_X,		6, false,	true,	true , false},
		{0x6e, ROR, 					Absolute,		6, false,	true,	true , false},
		{0x7e, ROR, 					Absolute_X,		7, false,	true,	true , false},

		{0x40, RTI, 					Implied,		6, false,	false,	false, false},

		{0x60, RTS, 					Implied,		6, false,	false,	false, false},

		{0xe9, SBC, 					Immediate,		2, false,	false,	false, false},
		{0xe5, SBC, 					ZeroPage,		3, false,	true,	false, false},
		{0xf5, SBC, 					ZeroPage_X,		4, false,	true,	false, false},
		{0xed, SBC, 					Absolute,		4, false,	true,	false, false},
		{0xfd, SBC, 					Absolute_X,		4, true,	true,	false, false},
		{0xf9, SBC, 					Absolute_Y,		4, true,	true,	false, false},
		{0xe1, SBC, 					PreInd_X,		6, false,	true,	false, false},
		{0xf1, SBC, 					PostInd_Y,		5, true,	true,	false, false},

		{0x38, SEC, 					Implied,		2, false,	false,	false, false},

		{0xf8, SED, 					Implied,		2, false,	false,	false, false},

		{0x78, SEI, 					Implied,		2, false,	false,	false, false},

		{0x85, STA, 					ZeroPage,		3, false,	false,	true , false},
		{0x95, STA, 					ZeroPage_X,		4, false,	false,	true , false},
		{0x8d, STA, 					Absolute,		4, false,	false,	true , false},
		{0x9d, STA, 					Absolute_X,		5, false,	false,	true , false},
		{0x99, STA, 					Absolute_Y,		5, false,	false,	true , false},
		{0x81, STA, 					PreInd_X,		6, false,	false,	true , false},
		{0x91, STA, 					PostInd_Y,		6, false,	false,	true , false},

		{0x86, STX, 					ZeroPage,		3, false,	false,	true , false},
		{0x96, STX, 					ZeroPage_Y,		4, false,	false,	true , false},
		{0x8e, STX, 					Absolute,		4, false,	false,	true , false},

		{0x84, STY, 					ZeroPage,		3, false,	false,	true , false},
		{0x94, STY, 					ZeroPage_X,		4, false,	false,	true , false},
		{0x8c, STY, 					Absolute,		4, false,	false,	true , false},

		{0xaa, TAX, 					Implied,		2, false,	false,	false, false},

		{0xa8, TAY, 					Implied,		2, false,	false,	false, false},

		{0xba, TSX, 					Implied,		2, false,	false,	false, false},

		{0x8a, TXA, 					Implied,		2, false,	false,	false, false},

		{0x9a, TXS, 					Implied,		2, false,	false,	false, false},

		{0x98, TYA, 					Implied,		2, false,	false,	false, false},

		//
		// Undocumented 6502 NMOS instructions that are illegal but still seem to be used by some programs
		//

		{ 0xaf, LAX,					Absolute,		4, false,	true,	false,	true },
		{ 0xb3, LAX,					PostInd_Y,		5, true,	true,	false,	true },
		{ 0xcb, SBX, 					Immediate,		2, false,	false,	false,  true },
		{ 0xf3, ISC, 					PostInd_Y,		8, false,	true,	true,	true },
		{ 0xdb, DCP, 					Absolute_Y,		7, false,	true,	true,	true }

	
};

#define P6502_JSR_OPCODE 0x20
#define P6502_RTS_OPCODE 0x60
	

private:

	vector<InstructionInfo> mOpcodeDict;

	string byte2str(uint16_t byte);
	string word2strB(uint16_t word);
	string word2str(uint16_t word);


	uint16_t mProgramCounter = 0x0;

	

public:

	Codec6502();

	bool decodeInstruction(uint8_t opcode, InstructionInfo& instr);

	bool decode(int adr, string srcFileName, ostream &fout);

	string decode(uint16_t PC, uint8_t opcode, uint16_t operand);

	bool decodeInstrFromBytes(uint16_t& pc, vector<uint8_t> bytes, string& decodedInstr);
	bool decodeInstrFromBytes(uint16_t& pc, vector<uint8_t> bytes, string& decodedInstr, bool outputASCII);

};

#endif