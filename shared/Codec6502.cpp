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
	for (int opcode = 0; opcode < instructions.size(); opcode++) {
		//cout << "OPCODE 0x" << hex << (int) opcode << "\n";
		InstructionInfo instr = instructions[(uint8_t) opcode];
		mOpcodeDict[(uint8_t) opcode] = instr;
	}
}

string Codec6502::byte2str(uint8_t byte)
{
	stringstream s;
	s << hex << setfill('0') << right << setw(2) << (int) byte;
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
	mPC++;

	return true;
}

bool Codec6502::readRelAdr(ifstream& fin, int8_t &relAdr, uint16_t& adr)
{
	fin.read((char*)&relAdr, sizeof(relAdr));
	mPC++;
	adr = (mPC + relAdr + 1) & 0xffff;

	return true;
}

bool Codec6502::readAbsAdr(ifstream& fin, uint16_t& adr)
{
	uint8_t a_L, a_H;
	fin.read((char*)&a_L, sizeof(a_L));
	mPC++;
	fin.read((char*)&a_H, sizeof(a_H));
	mPC++;
	adr = a_H * 256 + a_L;

	return true;
}

bool Codec6502::decode(int adr, string srcFileName, ostream& fout)
{

	ifstream fin(srcFileName, ios::in | ios::binary | ios::ate);

	if (!fin) {
		cout << "couldn't open file " << srcFileName << "\n";
		return (-1);
	}

	mPC = adr;

	fin.seekg(0);
	uint8_t opcode;
	while (!fin.fail()) {
		fin.read((char*)&opcode, sizeof(opcode));
		fout << word2str(mPC) << " ";
		fout << byte2str(opcode) << " ";
		if (mOpcodeDict.find(opcode) != mOpcodeDict.end()) {
			InstructionInfo instr = mOpcodeDict[opcode];
			uint16_t op_a = 0x0;
			if (instr.type != InstructionType::Doc)
				instr.mode = Mode::UD;
			switch (instr.mode) {
					case ac:		// OPC A
						fout << "      ";
						fout << instr2str[instr.instruction] << " A";
						break;
					case ip:		// 
						fout << "      ";
						fout << instr2str[instr.instruction] << "     ";
						break;
					case re:		// OPC <branch target>
					{
						int8_t r_a;
						if (!readRelAdr(fin, r_a, op_a))
							break;
						fout << byte2str(r_a + 0x100) << "    ";
						fout << instr2str[instr.instruction] << " $" << word2str(op_a);
						break;
					}
					case im:		// OPC #$12
					{
						if (!readZPAdr(fin, op_a))
							break;
						fout << byte2str(op_a) << "    ";
						fout << instr2str[instr.instruction] << " #$" << byte2str(op_a);
						break;
					}
					case zp:		// OPC $12
					{
						if (!readZPAdr(fin, op_a))
							break;
						fout << byte2str(op_a) << "    ";
						fout << instr2str[instr.instruction] << " $" << byte2str(op_a);
						break;
					}
					case zX:		// OPC $12,X
					{
						if (!readZPAdr(fin, op_a))
							break;
						fout << byte2str(op_a) << "    ";
						fout << instr2str[instr.instruction] << " $" << byte2str(op_a) << ",X";
						break;
					}
					case zY:		// OPC $12,Y
					{
						if (!readZPAdr(fin, op_a))
							break;
						fout << byte2str(op_a) << "    ";
						fout << instr2str[instr.instruction] << " $" << byte2str(op_a) << ",Y";
						break;
					}
					case ab:		// OPC $1234
					{
						if (!readAbsAdr(fin, op_a))
							break;
						fout << word2strB(op_a) << " ";
						fout << instr2str[instr.instruction] << " $" << word2str(op_a);
						break;
					}
					case aX:		// OPC $1234,X
					{
						if (!readAbsAdr(fin, op_a))
							break;
						fout << word2strB(op_a) << " ";
						fout << instr2str[instr.instruction] << " $" << word2str(op_a) << ",X";
						break;
					}
					case aY:		// OPC $1234,Y
					{
						if (!readAbsAdr(fin, op_a))
							break;
						fout << word2strB(op_a) << " ";
						fout << instr2str[instr.instruction] << " $" << word2str(op_a) << ",Y";
						break;
					}
					case id:		// OPC ($1234)
					{
						if (!readAbsAdr(fin, op_a))
							break;
						fout << word2strB(op_a) << " ";
						fout << instr2str[instr.instruction] << " ($" << word2str(op_a) << ")";
						break;
					}
					case iX:		// OPC ($12),X
					{
						if (!readZPAdr(fin, op_a))
							break;
						fout << byte2str(op_a) << "    ";
						fout << instr2str[instr.instruction] << " ($" << byte2str(op_a) << "),X";
						break;
					}
					case iY:		// OPC ($12),Y
					{
						if (!readZPAdr(fin, op_a))
							break;
						fout << byte2str(op_a) << "    ";
						fout << instr2str[instr.instruction] << " ($" << byte2str(op_a) << "),Y";
						break;
					}
					default:
						fout << "      ";
						fout << "???";
						break;
			}
			fout << "\n";
			mPC++;
		}
		else {
			fout << "Undefined opcode 0x" << hex << opcode << "\n";
		}
	}
	fin.close();


	return true;
}