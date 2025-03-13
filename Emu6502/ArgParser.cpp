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
	cout << "-trace <hex address> <pre trace len> <post trace len>: debug around a certain fetch address\n";
	cout << "-ctrace <hex address> <pre trace len> <post trace len>: as trace but the debugging will be repeated every time the fetch address is encountered\n";
	cout << "\tor written to. The tracing starts <pre trace len> instructions prior to the trigger and lasts <post trace len>\n";
	cout << "\tinstructions after the trigger.\n\n";
	cout << "-clog <hex adr>:\n\tCyclicallly logs the result of the execution of an instruction at the specified address\n";
	cout << "\t(as -ctrace <hex adr> 0 0 but faster)\n\n";
	cout << "-ilog <hex adr>:\n\tStart logging instruction execution after an interrupt and when execution reaches the specified address\n\n";
	cout << "-dbg <string with one or more of the letters below>: Debugging of different detail.\n";
	cout << "\t'e' errors\n";
	cout << "\t'w' warnings\n";
	cout << "\t'u' microprocessor execution\n";
	cout << "\t'p' device port updates\n";
	cout << "\t'i' interrupts\n";
	cout << "\t'k' keyboard\n";
	cout << "\t'v' video display units\n";
	cout << "\t's' serial/parallel I/O peripherals\n";
	cout << "\t'd' device execution in general\n";
	cout << "\t't' triggering on R/W accesses\n";
	cout << "\t'x' measuring execution time of the different components\n";
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
			debugManager.enableCyclicLogging(stoi(argv[a + 1], 0, 16));
			a++;
		}
		else if (strcmp(argv[a], "-ilog") == 0) {
			debugManager.enableInterruptLogging(stoi(argv[a + 1], 0, 16));
			a++;
		}
		else if (strcmp(argv[a], "-stop") == 0) {
			debugManager.enableExecStop(stoi(argv[a + 1], 0, 16));
			a++;
		}
		else if (strcmp(argv[a], "-trace") == 0 || strcmp(argv[a], "-ctrace") == 0) {
			uint16_t adr = stoi(argv[a + 1], 0, 16);
			bool recurring = strcmp(argv[a], "-ctrace") == 0;
			a++;
			if (a >= argc) {
				printUsage(argv[0]);
				return;
			}
			int pre_trace_len = stoi(argv[a + 1]);
			a++;
			if (a >= argc) {
				printUsage(argv[0]);
				return;
			}
			int post_trace_len = stoi(argv[a + 1]); 
			debugManager.enableTracing(adr, pre_trace_len, post_trace_len, recurring);
			a++;
		}
		else if(strcmp(argv[a], "-dump") == 0) {
			uint16_t adr = stoi(argv[a + 1], 0, 16);
			a++;
			if (a >= argc) {
				printUsage(argv[0]);
				return;
			}
			int sz = stoi(argv[a + 1], 0, 16);
			debugManager.enableMemDump(adr, sz);
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
			debugManager.setDebugLevel(DBG_VERBOSE);
		}
		else if (strcmp(argv[a], "-dbg") == 0) {
			a++;
			if (a >= argc) {
				printUsage(argv[0]);
				return;
			}
			if (strstr(argv[a ], "e") != NULL)
				debugManager.setDebugLevel(DBG_ERROR);
			if (strstr(argv[a], "w") != NULL)
				debugManager.setDebugLevel(DBG_WARNING);
			if (strstr(argv[a], "d") != NULL)
				debugManager.setDebugLevel(DBG_DEVICE);
			if (strstr(argv[a], "u") != NULL)
				debugManager.setDebugLevel(DBG_6502);
			if (strstr(argv[a], "p") != NULL)
				debugManager.setDebugLevel(DBG_PORT);
			if (strstr(argv[a], "i") != NULL)
				debugManager.setDebugLevel(DBG_INTERRUPTS);
			if (strstr(argv[a], "k") != NULL)
				debugManager.setDebugLevel(DBG_KEYBOARD);
			if (strstr(argv[a], "v") != NULL)
				debugManager.setDebugLevel(DBG_VDU);
			if (strstr(argv[a], "s") != NULL)
				debugManager.setDebugLevel(DBG_IO_PERIPHERAL);
			if (strstr(argv[a], "t") != NULL)
				debugManager.setDebugLevel(DBG_TRGGERING); 
			if (strstr(argv[a], "x") != NULL)
				debugManager.setDebugLevel(DBG_TIME);
			if (strstr(argv[a], "a") != NULL)
				debugManager.setDebugLevel(DBG_ALL);
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
