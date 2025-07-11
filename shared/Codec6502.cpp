#include "Codec6502.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <filesystem>
#include <cstdint>
#include "Codec6502.h"

using namespace std;

Codec6502::Codec6502() {

	mOpcodeDict.resize(256);

	for (int i = 0; i < 256; mOpcodeDict[i++] = invalidInstr);

	// Initialise opcode dictionary
	for (int i = 0; i < instructions.size(); i++) {
		InstructionInfo instr = instructions[(uint8_t) i];
		mOpcodeDict[(uint8_t)instr.opcode] = instr;
	}
}

string Codec6502::byte2str(uint16_t byte)
{
	stringstream s;
	s << hex << setfill('0') << right << setw(2) << (int) (byte & 0xff);
	return s.str();
}

string Codec6502::word2str(uint16_t word)
{
	stringstream s;
	s << hex << setfill('0') << right << setw(4) << word;
	return s.str();
}

string Codec6502::word2strB(uint16_t word)
{
	return byte2str(word % 256) + " " + byte2str(word / 256);
}

bool Codec6502::decodeInstruction(uint8_t opcode, InstructionInfo &instr)
{

	instr = mOpcodeDict[opcode];

	return true;
}

bool Codec6502::decode(int adr, string srcFileName, ostream& fout)
{

	ifstream fin(srcFileName, ios::in | ios::binary | ios::ate);

	if (!fin) {
		cout << "couldn't open file " << srcFileName << "\n";
		return false;
	}

	mProgramCounter = adr;

	fin.seekg(0);
	uint8_t opcode = 0x0;
	while (!fin.fail()) {

		uint16_t operand = 0x0;

		vector<char> instr_bytes;

		fin.read((char*)&opcode, sizeof(opcode));
		instr_bytes.push_back(opcode);

		uint16_t PC_for_opcode = mProgramCounter;
		InstructionInfo instr = mOpcodeDict[opcode];

		switch (instr.mode) {
					case Accumulator:	// OPC A
					case Implied:		// 
						break;
					case Relative:		// OPC <branch target>
					case Immediate:		// OPC #&12
					case ZeroPage:		// OPC &12
					case ZeroPage_X:	// OPC &12,X
					case ZeroPage_Y:	// OPC &12,Y
					case PreInd_X:	// OPC (&12,X)
					case PostInd_Y:	// OPC (&12),Y
					{
						uint8_t op8 = 0x0;

						fin.read((char*)&op8, sizeof(op8));
						instr_bytes.push_back(op8);

						operand = op8;
						mProgramCounter++;
						break;
					}
					case Absolute:		// OPC &1234
					case Absolute_X:	// OPC &1234,X
					case Absolute_Y:	// OPC &1234,Y
					case Indirect:		// OPC (&1234)			
					{
						uint8_t op16_L = 0x0, op16_H = 0x0;

						fin.read((char*)&op16_L, sizeof(op16_L));
						instr_bytes.push_back(op16_L);

						fin.read((char*)&op16_H, sizeof(op16_H));
						instr_bytes.push_back(op16_H);

						operand = op16_H * 256 + op16_L;
						mProgramCounter += 2;
						break;
					}
					default:
						break;
			}

		stringstream ss;
		fout << left << setw(30) << decode(PC_for_opcode, opcode, operand);
		for (int i = 0; i < instr_bytes.size(); i++) {
			char c = (char)instr_bytes[i];
			if (c >= 0x20 && c < 0x7f)
				ss << c;
			else
				ss << '.';
		}

		fout << ss.str() << "\n";

		mProgramCounter++;
	}
	fin.close();


	return true;
}

string Codec6502::decode(uint16_t PC, uint8_t opcode, uint16_t operand)
{
	stringstream sout;
	
	sout << word2str(PC) << " ";
	sout << byte2str(opcode) << " ";


	InstructionInfo instr = mOpcodeDict[opcode];
	switch (instr.mode) {
		case Accumulator:		// OPC A
			sout << "      ";
			sout << instr2str[instr.instruction] << " A";
			break;
		case Implied:		// 
			sout << "      ";
			sout << instr2str[instr.instruction] << "     ";
			break;
		case Relative:		// OPC <branch target>
		{
			uint16_t r_a = (PC + 2 + (int8_t) operand) & 0xffff;
			sout << byte2str(operand + 0x100) << "    ";
			sout << instr2str[instr.instruction] << " &" << word2str(r_a);
			break;
		}
		case Immediate:		// OPC #&12
		{
			sout << byte2str(operand) << "    ";
			sout << instr2str[instr.instruction] << " #&" << byte2str(operand);
			break;
		}
		case ZeroPage:		// OPC &12
		{
			sout << byte2str(operand) << "    ";
			sout << instr2str[instr.instruction] << " &" << byte2str(operand);
			break;
		}
		case ZeroPage_X:		// OPC &12,X
		{;
			sout << byte2str(operand) << "    ";
			sout << instr2str[instr.instruction] << " &" << byte2str(operand) << ",X";
			break;
		}
		case ZeroPage_Y:		// OPC &12,Y
		{
			sout << byte2str(operand) << "    ";
			sout << instr2str[instr.instruction] << " &" << byte2str(operand) << ",Y";
			break;
		}
		case Absolute:		// OPC &1234
		{
			sout << word2strB(operand) << " ";
			sout << instr2str[instr.instruction] << " &" << word2str(operand);
			break;
		}
		case Absolute_X:		// OPC &1234,X
		{
			sout << word2strB(operand) << " ";
			sout << instr2str[instr.instruction] << " &" << word2str(operand) << ",X";
			break;
		}
		case Absolute_Y:		// OPC &1234,Y
		{
			sout << word2strB(operand) << " ";
			sout << instr2str[instr.instruction] << " &" << word2str(operand) << ",Y";
			break;
		}
		case Indirect:		// OPC (&1234)
		{
			sout << word2strB(operand) << " ";
			sout << instr2str[instr.instruction] << " (&" << word2str(operand) << ")";
			break;
		}
		case PreInd_X:		// OPC (&12,X)
		{
			sout << byte2str(operand) << "    ";
			sout << instr2str[instr.instruction] << " (&" << byte2str(operand) << ";X)";
			break;
		}
		case PostInd_Y:		// OPC (&12),Y
		{
			sout << byte2str(operand) << "    ";
			sout << instr2str[instr.instruction] << " (&" << byte2str(operand) << "),Y";
			break;
		}
		default:
			sout << "      ";
			sout << "???";
			break;
		}

	return sout.str();
}

bool Codec6502::decodeInstrFromBytes(uint16_t &pc, vector<uint8_t> bytes, string &decodedInstr)
{
	int pos = 0;
	uint16_t operand = 0x0;

	if (bytes.size() < 2)
		return false;

	uint8_t opcode = bytes[pos++];
	if (pos >= bytes.size())
		return false;

	uint16_t PC_for_opcode = pc;
	InstructionInfo instr = mOpcodeDict[opcode];
	switch (instr.mode) {
	case Accumulator:	// OPC A
	case Implied:		// 
		break;
	case Relative:		// OPC <branch target>
	case Immediate:		// OPC #&12
	case ZeroPage:		// OPC &12
	case ZeroPage_X:	// OPC &12,X
	case ZeroPage_Y:	// OPC &12,Y
	case PreInd_X:	// OPC (&12,X)
	case PostInd_Y:	// OPC (&12),Y
	{
		uint8_t op8 = 0x0;
		op8 = bytes[pos++];
		operand = op8;
		pc++;
		break;
	}
	case Absolute:		// OPC &1234
	case Absolute_X:	// OPC &1234,X
	case Absolute_Y:	// OPC &1234,Y
	case Indirect:		// OPC (&1234)			
	{
		uint8_t op16_L = 0x0, op16_H = 0x0;
		if (pos + 2 >= bytes.size())
			return false;
		op16_L = bytes[pos++];
		op16_H = bytes[pos++];

		operand = op16_H * 256 + op16_L;
		pc += 2;
		break;
	}
	default:
		break;
	}

	stringstream ss;
	ss << left << setw(30) << decode(PC_for_opcode, opcode, operand);
	for (int i = 0; i < pos; i++) {
		char c = (char) bytes[i];
		if (c >= 0x20 && c < 0x7f)
			ss << c;
		else
			ss << '.';
	}

	decodedInstr = ss.str();

	pc++;

	return true;
}