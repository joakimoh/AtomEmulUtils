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
	s << hex << setfill('0') << right << setw(4) << word;;
	return s.str();
}

string Codec6502::word2strB(uint16_t word)
{
	return byte2str(word % 256) + " " + byte2str(word / 256);
}

bool Codec6502::readZPAdr(ifstream& fin, uint16_t& adr)
{
	uint8_t zp_a;
	fin.read((char*)&zp_a, sizeof(zp_a));
	adr = (uint16_t) zp_a;
	mProgramCounter++;

	return true;
}

bool Codec6502::readRelAdr(ifstream& fin, int8_t &relAdr, uint16_t& adr)
{
	fin.read((char*)&relAdr, sizeof(relAdr));
	mProgramCounter++;
	adr = (mProgramCounter + relAdr + 1) & 0xffff;

	return true;
}

bool Codec6502::readAbsAdr(ifstream& fin, uint16_t& adr)
{
	uint8_t a_L, a_H;
	fin.read((char*)&a_L, sizeof(a_L));
	mProgramCounter++;
	fin.read((char*)&a_H, sizeof(a_H));
	mProgramCounter++;
	adr = a_H * 256 + a_L;

	return true;
}

bool Codec6502::decodeInstruction(uint8_t opcode, InstructionInfo &instr)
{
	instr = invalidInstr;
	if (mOpcodeDict.find(opcode) != mOpcodeDict.end()) {
		instr = mOpcodeDict[opcode];
		return true;
	}

	return false;
}

bool Codec6502::decode(int adr, string srcFileName, ostream& fout)
{

	ifstream fin(srcFileName, ios::in | ios::binary | ios::ate);

	if (!fin) {
		cout << "couldn't open file " << srcFileName << "\n";
		return (-1);
	}

	mProgramCounter = adr;

	fin.seekg(0);
	uint8_t opcode;
	while (!fin.fail()) {
		fin.read((char*)&opcode, sizeof(opcode));
		fout << word2str(mProgramCounter) << " ";
		fout << byte2str(opcode) << " ";
		if (mOpcodeDict.find(opcode) != mOpcodeDict.end()) {
			InstructionInfo instr = mOpcodeDict[opcode];
			uint16_t op_a = 0x0;
			switch (instr.mode) {
					case Accumulator:		// OPC mAccumulator
						fout << "      ";
						fout << instr2str[instr.instruction] << " mAccumulator";
						break;
					case Implied:		// 
						fout << "      ";
						fout << instr2str[instr.instruction] << "     ";
						break;
					case Relative:		// OPC <branch target>
					{
						int8_t r_a;
						if (!readRelAdr(fin, r_a, op_a))
							break;
						fout << byte2str(r_a + 0x100) << "    ";
						fout << instr2str[instr.instruction] << " &" << word2str(op_a);
						break;
					}
					case Immediate:		// OPC #&12
					{
						if (!readZPAdr(fin, op_a))
							break;
						fout << byte2str(op_a) << "    ";
						fout << instr2str[instr.instruction] << " #&" << byte2str(op_a);
						break;
					}
					case ZeroPage:		// OPC &12
					{
						if (!readZPAdr(fin, op_a))
							break;
						fout << byte2str(op_a) << "    ";
						fout << instr2str[instr.instruction] << " &" << byte2str(op_a);
						break;
					}
					case ZeroPage_X:		// OPC &12,X
					{
						if (!readZPAdr(fin, op_a))
							break;
						fout << byte2str(op_a) << "    ";
						fout << instr2str[instr.instruction] << " &" << byte2str(op_a) << ",X";
						break;
					}
					case ZeroPage_Y:		// OPC &12,Y
					{
						if (!readZPAdr(fin, op_a))
							break;
						fout << byte2str(op_a) << "    ";
						fout << instr2str[instr.instruction] << " &" << byte2str(op_a) << ",Y";
						break;
					}
					case Absolute:		// OPC &1234
					{
						if (!readAbsAdr(fin, op_a))
							break;
						fout << word2strB(op_a) << " ";
						fout << instr2str[instr.instruction] << " &" << word2str(op_a);
						break;
					}
					case Absolute_X:		// OPC &1234,X
					{
						if (!readAbsAdr(fin, op_a))
							break;
						fout << word2strB(op_a) << " ";
						fout << instr2str[instr.instruction] << " &" << word2str(op_a) << ",X";
						break;
					}
					case Absolute_Y:		// OPC &1234,Y
					{
						if (!readAbsAdr(fin, op_a))
							break;
						fout << word2strB(op_a) << " ";
						fout << instr2str[instr.instruction] << " &" << word2str(op_a) << ",Y";
						break;
					}
					case Indirect:		// OPC (&1234)
					{
						if (!readAbsAdr(fin, op_a))
							break;
						fout << word2strB(op_a) << " ";
						fout << instr2str[instr.instruction] << " (&" << word2str(op_a) << ")";
						break;
					}
					case Indirect_X:		// OPC (&12,X)
					{
						if (!readZPAdr(fin, op_a))
							break;
						fout << byte2str(op_a) << "    ";
						fout << instr2str[instr.instruction] << " (&" << byte2str(op_a) << ";X)";
						break;
					}
					case Indirect_Y:		// OPC (&12),Y
					{
						if (!readZPAdr(fin, op_a))
							break;
						fout << byte2str(op_a) << "    ";
						fout << instr2str[instr.instruction] << " (&" << byte2str(op_a) << "),Y";
						break;
					}
					default:
						fout << "      ";
						fout << "???";
						break;
			}
			fout << "\n";
		}
		else {
			fout << "      ???\n";
		}
		mProgramCounter++;
	}
	fin.close();


	return true;
}