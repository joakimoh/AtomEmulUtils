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
	cout << "Usage:\t" << name << " -map <memory map file> [-pgm <program> <hex adr>] [-speed <emulation speed>] [-v] <advanced options>\n\n";
	cout << "<emulation speed>:\nEmulation speed in %. If not specified, 100% (real time) is assumed\n\n";
	cout << "<memory map file>:\n\tFile which defines devices and their memory mapping.\n\n";
	cout << "<program> <hex adr>:\n\tBinary file with (program) data to be loaded into RAM at address <hex adr>.\n\n";
	cout << "-v:\n\tVerbose output\n\n";
	cout << "\nADVANCED OPTIONS:\n";
	cout << "-stop <hex address>: stop execution at this address\n\n";
	cout << "-dump <hex address> <hex size>: dump memory content address to address+size-1 after stopping execution\n\n";
	cout << "-trace <hex address> <pre trace len> <post trace len>: turn on 'all debug mode when an certain address is read\n";
	cout << "\tor written to. The tracing starts <pre trace len> instructions prior to the trigger and lasts <post trace len>\n";
	cout << "\tinstructions after the trigger.\n\n";
	cout << "-clog <hex adr>:\n\tCyclicallly logs the result of the execution of an instruction at the specified address\n\n";
	cout << "-ilog <hex adr>:\n\tStart logging instruction execution after an interrupt and when execution reaches the specified address\n\n";
	cout << "-dbg <string with one or more of the letters below>: Debugging of different detail.\n";
	cout << "\t'e' errors\n";
	cout << "\t'w' warnings\n";
	cout << "\t'u' microprocessor execution\n";
	cout << "\t'p' device port updates\n";
	cout << "\t'i' interrupts\n";
	cout << "\t'd' device execution\n";
	cout << "\t'a' all the above\n\n";
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
		if(strcmp(argv[a], "-speed") == 0) {
			emulationSpeed = stod(argv[a + 1]);
			a++;
		}
		else if (strcmp(argv[a], "-clog") == 0) {
			debugInfo.cyclicLogAdr = stoi(argv[a + 1], 0, 16);
			a++;
		}
		else if (strcmp(argv[a], "-ilog") == 0) {
			debugInfo.interruptLogAdr = stoi(argv[a + 1], 0, 16);
			a++;
		}
		else if (strcmp(argv[a], "-stop") == 0) {
			debugInfo.stopAdr = stoi(argv[a + 1], 0, 16);
			a++;
		}
		else if (strcmp(argv[a], "-trace") == 0) {
			debugInfo.traceAdr = stoi(argv[a + 1], 0, 16);
			a++;
			if (a >= argc) {
				printUsage(argv[0]);
				return;
			}
			debugInfo.preTraceLen = stoi(argv[a + 1]);
			a++;
			if (a >= argc) {
				printUsage(argv[0]);
				return;
			}
			debugInfo.postTraceLen = stoi(argv[a + 1]);
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
		else if (strcmp(argv[a], "-pgm") == 0) {
			program.fileName = argv[a + 1];
			a++;
			if (a >= argc) {
				printUsage(argv[0]);
				return;
			}
			program.loadAdr = stoi(argv[a + 1], 0, 16);
			a++;
		}
		else if (strcmp(argv[a], "-data") == 0) {
			data.fileName = argv[a + 1];
			a++;
			if (a >= argc) {
				printUsage(argv[0]);
				return;
			}
			data.loadAdr = stoi(argv[a + 1], 0, 16);
			a++;
		}
		else if (strcmp(argv[a], "-map") == 0) {
			mapFileName = argv[a + 1];
			a++;
		}
		else if (strcmp(argv[a], "-v") == 0) {
			debugInfo.dbgLevel |= DBG_VERBOSE;
		}
		else if (strcmp(argv[a], "-dbg") == 0) {
			a++;
			if (a >= argc) {
				printUsage(argv[0]);
				return;
			}
			if (strstr(argv[a ], "e") != NULL)
				debugInfo.dbgLevel |= DBG_ERROR;
			else if (strstr(argv[a], "w") != NULL)
				debugInfo.dbgLevel |= DBG_WARNING;
			else if (strstr(argv[a], "d") != NULL)
				debugInfo.dbgLevel |= DBG_DEVICE;
			else if (strstr(argv[a], "u") != NULL)
				debugInfo.dbgLevel |= DBG_6502;
			else if (strstr(argv[a], "p") != NULL)
				debugInfo.dbgLevel |= DBG_PORT;
			else if (strstr(argv[a], "i") != NULL)
				debugInfo.dbgLevel |= DBG_INTERRUPTS;
			else if (strstr(argv[a], "a") != NULL)
				debugInfo.dbgLevel |= DBG_ALL;
			else {
				printUsage(argv[0]);
				return;
			}
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
