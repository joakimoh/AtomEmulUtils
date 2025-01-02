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
	cout << "Emulates an Acorn\n";
	cout << "Usage:\t" << name << " -map <memory map file> [-clk <clock speed>] [-v] <advanced options>\n\n";
	cout << "<clock speed>:\nClock speed in MHz. If not specified, 1 Mhz is assumed\n\n";
	cout << "<memory map file>:\n\tFile which defines devices and their memory mapping.\n\n";
	cout << "-v:\n\tVerbose output\n\n";
	cout << "\nADVANCED OPTIONS:\n";
	cout << "-stop <hex address>: stop execution at this address\n\n";
	cout << "-dump <hex address> <hex size>: dump memory content address to address+size-1 after stopping execution\n\n";
}

ArgParser::ArgParser(int argc, const char* argv[])
{

	if (argc <= 1) {
		printUsage(argv[0]);
		return;
	}

	int a = 1;
	bool genFiles = false;
	while (a < argc) {
		if(strcmp(argv[a], "-clk") == 0) {
			cMHz = stod(argv[a + 1]);
			a++;
		}
		else if (strcmp(argv[a], "-stop") == 0) {
			debugInfo.stopAdr = stoi(argv[a + 1], 0, 16);
			a++;
		}
	else if(strcmp(argv[a], "-dump") == 0) {
		debugInfo.dumpAdr = stoi(argv[a + 1], 0, 16);
		a++;
		if (a >= argc) {
			printUsage(argv[0]);
			return;
		}
		debugInfo.dumpSz = stoi(argv[a + 1], 0, 16);
		a++;
	}
		else if (strcmp(argv[a], "-map") == 0) {
			mapFileName = argv[a + 1];
			a++;
		}
		else if (strcmp(argv[a], "-v") == 0) {
			debugInfo.verbose = true;
		}
		else {
			cout << "Unknown option " << argv[a] << "\n";
			printUsage(argv[0]);
			return;
		}
		a++;
	}

	if (argc < 2) {
		printUsage(argv[0]);
		return;
	}


	mParseSuccess = true;
}
