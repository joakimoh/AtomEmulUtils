#ifndef CODEC_6502
#define CODEC_6502

#include <map>
#include <vector>
#include <string>

using namespace std;


class Codec6502 {

public:

	enum Instruction {

		// Documented instructions
		ADC = 0x00, AND = 0x01, ASL = 0x02, BCC = 0x03, BCS = 0x04, BEQ = 0x05, BIT = 0x06, BMI = 0x07,
		BNE = 0x08, BPL = 0x09, BRK = 0x0a, BVC = 0x0b, BVS = 0x0c, CLC = 0x0d, CLD = 0x0e, CLI = 0x0f,
		CLV = 0x10, CMP = 0x11, CPX = 0x12, CPY = 0x13, DEC = 0x14, DEX = 0x15, DEY = 0x16, EOR = 0x17,
		INC = 0x18, INX = 0x19, INY = 0x1a, JMP = 0x1b, JSR = 0x1c, LDA = 0x1d, LDX = 0x1e, LDY = 0x1f,
		LSR = 0x20, NOP = 0x21, ORA = 0x22, PHA = 0x23, PHP = 0x24, PLA = 0x25, PLP = 0x26, ROL = 0x27,
		ROR = 0x28, RTI = 0x29, RTS = 0x2a, SBC = 0x2b, SEC = 0x2c, SED = 0x2d, SEI = 0x2e, STA = 0x2f,
		STX = 0x30, STY = 0x31, TAX = 0x32, TAY = 0x33, TSX = 0x34, TXA = 0x35, TXS = 0x36, TYA = 0x37,

		// Undocumented instructions (not guaranteed to work)
		ALR = 0x38, ANC = 0x39, ANE = 0x3a, ARR = 0x3b, DCP = 0x3c, ISC = 0x3d, LAS = 0x3e, LAX = 0x3f,
		LXA = 0x40, RLA = 0x41, RRA = 0x42, SAX = 0x43, SBX = 0x44, SHA = 0x45, SHX = 0x46, SHY = 0x47,
		SLO = 0x48, SRE = 0x49, TAS = 0x4a, USBC= 0x4b,

		// Used when a non-existing instruction is encountered 
		NON = 0x4c
	};

	const vector<string> instr2str {

		// Documented instructions
		"ADC", "AND", "ASL", "BCC", "BCS", "BEQ", "BIT", "BMI", "BNE", "BPL", "BRK", "BVC", "BVS", "CLC",
		"CLD", "CLI", "CLV", "CMP", "CPX", "CPY", "DEC", "DEX", "DEY", "EOR", "INC", "INX", "INY", "JMP",
		"JSR", "LDA", "LDX", "LDY", "LSR", "NOP", "ORA", "PHA", "PHP", "PLA", "PLP", "ROL", "ROR", "RTI",
		"RTS", "SBC", "SEC", "SED", "SEI", "STA", "STX", "STY", "TAX", "TAY", "TSX", "TXA", "TXS", "TYA",

		// Undocumented instructions (not guaranteed to work)
		"ALR", "ANC", "ANE", "ARR", "DCP", "ISC", "LAS", "LAX", "LXA", "RLA", "RRA", "SAX", "SBX",
		"SHA", "SHX", "SHY", "SLO", "SRE", "TAS", "USBC",

		// Used when a non-existing instruction is encountered 
		"???"
	};

	enum Mode {
		ac,		// OPC A
		ip,		// OPC
		re,		// OPC <branch target>
		im,		// OPC #$12
		zp,		// OPC $12
		zX,		// OPC $12,X
		zY,		// OPC $12,Y
		ab,		// OPC $1234
		aX,		// OPC $1234,X
		aY,		// OPC $1234,Y
		id,		// OPC ($1234)
		iX,		// OPC ($12),X
		iY,		// OPC ($12),Y
		UD		// Undefined
	};

	typedef enum InstructionType { Doc, Ndc, Jam };

	typedef struct InstructionInfo_struct {
		Instruction			instruction;
		Mode				mode;
		InstructionType		type;
	} InstructionInfo;

	vector<InstructionInfo> instructions
	{
		//		Instructions sorted by opcode
		//		0				1				2				3				4				5				6				7				8				9				a				b				c				d				e				f
		/* 0 */ {BRK, ip, Doc}, {ORA, iX, Doc}, {NON, UD, Jam}, {SLO, iX, Ndc}, {NOP, zp, Ndc}, {ORA, zp, Doc}, {ASL, zp, Doc}, {SLO, zp, Ndc}, {PHP, ip, Doc}, {ORA, im, Doc}, {ASL, ac, Doc}, {ANC, im, Ndc}, {NOP, ab, Ndc}, {ORA, ab, Doc}, {ASL, ab, Doc}, {SLO, ab, Ndc},
		/* 1 */ {BPL, re, Doc}, {ORA, iY, Doc}, {NON, UD, Jam}, {SLO, iY, Ndc}, {NOP, zX, Ndc}, {ORA, zX, Doc}, {ASL, zX, Doc}, {SLO, zX, Ndc}, {CLC, ip, Doc}, {ORA, aY, Doc}, {NOP, ip, Ndc}, {SLO, aY, Ndc}, {NOP, aX, Ndc}, {ORA, aX, Doc}, {ASL, aX, Doc}, {SLO, aX, Ndc},
		/* 2 */ {JSR, ab, Doc}, {AND, iX, Doc}, {NON, UD, Jam}, {RLA, iX, Ndc}, {BIT, zp, Doc}, {AND, zp, Doc}, {ROL, zp, Doc}, {RLA, zp, Ndc}, {PLP, ip, Doc}, {AND, im, Doc}, {ROL, ac, Doc}, {ANC, im, Ndc}, {BIT, ab, Doc}, {AND, ab, Doc}, {ROL, ab, Doc}, {RLA, ab, Ndc },
		/* 3 */ {BMI, re, Doc}, {AND, iY, Doc}, {NON, UD, Jam}, {RLA, iY, Ndc}, {NOP, zX, Ndc}, {AND, zX, Doc}, {ROL, zX, Doc}, {RLA, zX, Ndc}, {SEC, ip, Doc}, {AND, aY, Doc}, {NOP, ip, Ndc}, {RLA, aY, Ndc}, {NOP, aX, Ndc}, {AND, aX, Doc}, {ROL, aX, Doc}, {RLA, aX, Ndc },
		/* 4 */ {RTI, ip, Doc}, {EOR, iX, Doc}, {NON, UD, Jam}, {SRE, iX, Ndc}, {NOP, zp, Ndc}, {EOR, zp, Doc}, {LSR, zp, Doc}, {SRE, zp, Ndc}, {PHA, ip, Doc}, {EOR, im, Doc}, {LSR, ac, Doc}, {ALR, im, Ndc}, {JMP, ab, Doc}, {EOR, ab, Doc}, {LSR, ab, Doc}, {SRE, ab, Ndc },
		/* 5 */ {BVC, re, Doc}, {EOR, iY, Doc}, {NON, UD, Jam}, {SRE, iY, Ndc}, {NOP, zX, Ndc}, {EOR, zX, Doc}, {LSR, zX, Doc}, {SRE, zX, Ndc}, {CLI, ip, Doc}, {EOR, aY, Doc}, {NOP, ip, Ndc}, {SRE, aY, Ndc}, {NOP, aX, Ndc}, {EOR, aX, Doc}, {LSR, aX, Doc}, {SRE, aX, Ndc },
		/* 6 */ {RTS, ip, Doc}, {ADC, iX, Doc}, {NON, UD, Jam}, {RRA, iX, Ndc}, {NOP, zp, Ndc}, {ADC, zp, Doc}, {ROR, zp, Doc}, {RRA, zp, Ndc}, {PLA, ip, Doc}, {ADC, im, Doc}, {ROR, ac, Doc}, {ARR, im, Ndc}, {JMP, id, Doc}, {ADC, ab, Doc}, {ROR, ab, Doc}, {RRA, ab, Ndc },
		/* 7 */ {BVS, re, Doc}, {ADC, iY, Doc}, {NON, UD, Jam}, {RRA, iY, Ndc}, {NOP, zX, Ndc}, {ADC, zX, Doc}, {ROR, zX, Doc}, {RRA, zX, Ndc}, {SEI, ip, Doc}, {ADC, aY, Doc}, {NOP, ip, Ndc}, {RRA, aY, Ndc}, {NOP, aX, Doc}, {ADC, aX, Doc}, {ROR, aX, Doc}, {RRA, aX, Ndc },
		/* 8 */ {NOP, im, Ndc}, {STA, iX, Doc}, {NOP, im, Doc}, {SAX, iX, Ndc}, {STY, zp, Doc}, {STA, zp, Doc}, {STX, zp, Doc}, {SAX, zp, Ndc}, {DEY, ip, Doc}, {NOP, im, Ndc}, {TXA, ip, Doc}, {ANE, im, Ndc}, {STY, ab, Doc}, {STA, ab, Doc}, {STX, ab, Doc}, {SAX, ab, Ndc },
		/* 9 */ {BCC, re, Doc}, {STA, iY, Doc}, {NON, UD, Jam}, {SHA, iY, Ndc}, {STY, zX, Doc}, {STA, zX, Doc}, {STX, zY, Doc}, {SAX, zY, Ndc}, {TYA, ip, Doc}, {STA, aY, Doc}, {TXS, ip, Doc}, {TAS, aY, Ndc}, {SHY, aX, Ndc}, {STA, aX, Doc}, {SHX, aY, Ndc}, {SHA, aY, Ndc },
		/* a */ {LDY, im, Doc}, {LDA, iX, Doc}, {LDX, im, Doc}, {LAX, iX, Ndc}, {LDY, zp, Doc}, {LDA, zp, Doc}, {LDX, zp, Doc}, {LAX, zp, Ndc}, {TAY, ip, Doc}, {LDA, im, Doc}, {TAX, ip, Doc}, {LXA, im, Ndc}, {LDY, ab, Doc}, {LDA, ab, Doc}, {LDX, ab, Doc}, {LAX, ab, Ndc },
		/* b */ {BCS, re, Doc}, {LDA, iY, Doc}, {NON, UD, Jam}, {LAX, iY, Ndc}, {LDY, zX, Doc}, {LDA, zX, Doc}, {LDX, zY, Doc}, {LAX, zY, Ndc}, {CLV, ip, Doc}, {LDA, aY, Doc}, {TSX, ip, Doc}, {LAS, aY, Ndc}, {LDY, aX, Doc}, {LDA, aX, Doc}, {LDX, aY, Doc}, {LAX, aY, Ndc },
		/* c */ {CPY, im, Doc}, {CMP, iX, Doc}, {NOP, im, Ndc}, {DCP, iX, Ndc}, {CPY, zp, Doc}, {CMP, zp, Doc}, {DEC, zp, Doc}, {DCP, zp, Ndc}, {INY, ip, Doc}, {CMP, im, Doc}, {DEX, ip, Doc}, {SBX, im, Ndc}, {CPY, ab, Doc}, {CMP, ab, Doc}, {DEC, ab, Doc}, {DCP, ab, Ndc },
		/* d */ {BNE, re, Doc}, {CMP, iY, Doc}, {NON, UD, Jam}, {DCP, iY, Ndc}, {NOP, zX, Ndc}, {CMP, zX, Doc}, {DEC, zX, Doc}, {DCP, zX, Ndc}, {CLD, ip, Doc}, {CMP, aY, Doc}, {NOP, ip, Ndc}, {DCP, aY, Ndc}, {NOP, aX, Ndc}, {CMP, aX, Doc}, {DEC, aX, Doc}, {DCP, aX, Ndc },
		/* e */ {CPX, im, Doc}, {SBC, iX, Doc}, {NOP, im, Ndc}, {ISC, iX, Ndc}, {CPX, zp, Doc}, {SBC, zp, Doc}, {INC, zp, Doc}, {ISC, zp, Ndc}, {INX, ip, Doc}, {SBC, im, Doc}, {NOP, ip, Doc}, {USBC,im, Ndc}, {CPX, ab, Doc}, {SBC, ab, Doc}, {INC, ab, Doc}, {ISC, ab, Ndc },
		/* f */ {BEQ, re, Doc}, {SBC, iY, Doc}, {NON, UD, Doc}, {ISC, iY, Doc}, {NOP, zX, Ndc}, {SBC, zX, Doc}, {INC, zX, Doc}, {ISC, zX, Doc}, {SED, ip, Doc}, {SBC, aY, Doc}, {NOP, ip, Ndc}, {ISC, aY, Ndc}, {NOP, aX, Doc}, {SBC, aX, Doc}, {INC, aX, Doc}, {ISC, aX, Ndc }
	};

private:

	map<uint8_t, InstructionInfo> mOpcodeDict;

	string byte2str(uint8_t byte);
	string word2strB(uint16_t word);
	string word2str(uint16_t word);
	bool readZPAdr(ifstream& fin, uint16_t &adr);
	bool readRelAdr(ifstream& fin, int8_t& relAdr, uint16_t &adr);
	bool readAbsAdr(ifstream& fin, uint16_t &adr);

	uint16_t mPC = 0x0;

public:

	Codec6502();

	bool decode(int adr, string srcFileName, ostream &fout);

};

#endif