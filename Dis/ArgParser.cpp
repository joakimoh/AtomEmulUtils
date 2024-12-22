#include "ArgParser.h"
#include <sys/stat.h>
#include <filesystem>
#include <iostream>
#include <string.h>

using namespace std;

bool ArgParser::failed()
{
	return !mParseSuccess;
}

void ArgParser::printUsage(const char* name)
{
	cout << "Disassembles 6502 machine code\n";
	cout << "Usage:\t" << name << " <src file> -a <adr> -o <dst file> [-v]\n\n";
	cout << "<src  file>:\n\tFile with 6502 machine code.\n\n";
	cout << "<adr>:\n\tStart address. If not specified, zero is assumed\n\n";
	cout << "-v:\n\tVerbose output\n\n";
	cout << "-o <dst file>:\n\tFile to put disassembled code in. If not specified output goes to screen.\n\n";
}

ArgParser::ArgParser(int argc, const char* argv[])
{



	if (argc <= 1) {
		printUsage(argv[0]);
		return;
	}

	filesystem::path fin_path = argv[1];
	if (!filesystem::exists(fin_path)) {
		cout << "file '" << argv[1] << "' cannot be opened!\n";
		return;
	}
	srcFileName = argv[1];

	int ac = 2;
	bool genFiles = false;
	while (ac < argc) {
		if (strcmp(argv[ac], "-o") == 0) {
			dstFileName = argv[ac + 1];
			ac++;
		}
		else if (strcmp(argv[ac], "-a") == 0) {
			startAdr = stoi(argv[ac + 1]);
			ac++;
		}
		else if (strcmp(argv[ac], "-v") == 0) {
			verbose = true;
		}
		else {
			cout << "Unknown option " << argv[ac] << "\n";
			printUsage(argv[0]);
			return;
		}
		ac++;
	}

	if (argc < 2) {
		printUsage(argv[0]);
		return;
	}


	mParseSuccess = true;
}
