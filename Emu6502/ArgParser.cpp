#include "ArgParser.h"
#include <sys/stat.h>
#include <filesystem>
#include <iostream>
#include <string.h>
#include "../shared/Tokeniser.h"

using namespace std;

bool ArgParser::failed()
{
	return !mParseSuccess;
}

void  ArgParser::printInfo()
{
	cout << "Emulates 6502-based microcontroller systems of the 1980's.\n";
	cout << "\nAny microcontroller system that uses the emulated hardware devices should in theory be possible\n";
	cout << "to support. The system of concern is specified by a configuration file that specifies the\n";
	cout << "used devices, their memory mapping, and how they are connected.\n\n";
	cout << "Currently the following hardware devices are supported:\n";
	cout << "\t- R6502 NMOS Microcontroller (used in the BBC Micro e.g.)\n";
	cout << "\t- Static RAM\n";
	cout << "\t- Dynamic RAM\n";
	cout << "\t- ROM\n";
	cout << "\t- MC6850 Asynchronous Communications Interface Adapter (ACIA)\n";
	cout << "\t- SY6522 Versatile Interface Adapter (VIA)\n";
	cout << "\t- 8255 Programmable Peripheral Interface (PIA)\n";
	cout << "\t- HD6845 CRTC Controller\n";
	cout << "\t- MC6847 Video Display Generator\n";
	cout << "\t- SAA5050 Teletext Character Generator\n";
	cout << "\t- Standard Tape Recorder: Emulates the tape recorder you connect to the computer\n";
	cout << "\t- Custom Acorn Atom hardware: Cassette Interface, Keyboard amd Speaker\n";
	cout << "\t- Custom BBC Micro hardware: Serial ULA, Keyboard, ROM Selection and Video ULA\n";
	cout << "\n";
}

void ArgParser::printUsage(const char* name)
{
	cout << "Usage:\t" << name << " -map <memory map file> [-fmt <video format>] [-pgm <program> <hex adr>] [-speed <emulation speed>] [-v] <advanced options>\n\n";
	cout << "<video format>:\nEither 'PAL' or 'NTSC'. If not specified, PAL is assumed\n\n";
	cout << "<emulation speed>:\nEmulation speed in %. If not specified, 100% (real time) is assumed\n\n";
	cout << "<memory map file>:\n\tFile which defines devices and their memory mapping.\n\n";
	cout << "<program> <hex adr>:\n\tBinary file with (program) data to be loaded into RAM at address <hex adr>.\n\n";
	cout << "-v:\n\tVerbose output\n\n";
	cout << "\nADVANCED OPTIONS:\n";
	cout << "-nHA: Turn off Graphics hardware acceleration.\n\n";
	cout << "-stop <hex address>: stop execution at this address\n\n";
	cout << "-dump <hex address> <hex size>: dump memory content address to address+size-1 after stopping execution\n\n";
	cout << "-trace <hex address> <pre trace len> <post trace len>: debug around a certain fetch address\n";
	cout << "-xtrace <hex address> <pre trace len> <post trace len>: same as -trace but with more information (and therefoe slower)\n";
	cout << "-ctrace <hex address> <pre trace len> <post trace len>: as trace but the debugging will be repeated every time the fetch address is encountered\n";
	cout << "-ktrace <hex address> <pre trace len> <post trace len>: as trace but the debugging condition only checked for after user presses <CTRL-T>\n";
	cout << "\tor written to. The tracing starts <pre trace len> instructions prior to the trigger and lasts <post trace len>\n";
	cout << "\tinstructions after the trigger.\n\n";
	cout << "-log <hex adr>:\n\tStart logging instruction execution after execution reaches the specified address\n";
	cout << "-clog <hex adr>:\n\tCyclicallly logs the result of the execution of an instruction at the specified address\n";
	cout << "\t(as -ctrace <hex adr> 0 0 but faster)\n\n";
	cout << "-ilog <hex adr>:\n\tStart logging instruction execution after an interrupt and when execution reaches the specified address\n";
	cout << "-mlog <adr>:\n\tmemory concent to add along with the log\n\n";
	cout << "-port <device name>:<port name> {, <device_name:<port name>\n\tlog updates of specific ports\n";
	cout << "-dbg <string with one or more of the letters below>: Debugging of different detail.\n";
	cout << "-dev <dev name>: specify that debugging shall be limited to a certain device instance <dev name>.\n";
	cout << "\t'e' errors\n";
	cout << "\t'w' warnings\n";
	cout << "\t'u' microprocessor execution (can also be enabled at run-time with <CRTL-D>)\n";
	cout << "\t'p' device port updates\n";
	cout << "\t'i' interrupts & reset\n";
	cout << "\t'r' only reset\n";
	cout << "\t'k' keyboard\n";
	cout << "\t'v' video display units (can also be enabled at run time with <CTRL-V>)\n";
	cout << "\t's' serial/parallel I/O peripherals\n";
	cout << "\t'a' audio\n";
	cout << "\t'd' device execution in general\n";
	cout << "\t't' triggering on R/W accesses\n";
	cout << "\t'c' cassette tape I/O\n";
	cout << "\t'x' measuring execution time of the different components\n";
	cout << "\t'S' SPI devices\n";
	cout << "\t'A' all the above\n\n";
}

ArgParser::ArgParser(int argc, const char* argv[])
{

	if (argc <= 1) {
		printInfo();
		printUsage(argv[0]);
		return;
	}

	int a = 1;
	bool genFiles = false;
	while (a < argc) {
		if (strcmp(argv[a], "-speed") == 0) {
			emulationSpeed = stod(argv[a + 1]);
			a++;
		}
		else if (strcmp(argv[a], "-dev") == 0) {
			debugManager.setLogDevice(argv[a + 1]);
			a++;
		}
		else if (strcmp(argv[a], "-nHA") == 0) {
			hwAcc = false;
		}
		else if (strcmp(argv[a], "-log") == 0) {
			debugManager.enableLogging(stoi(argv[a + 1], 0, 16));
			a++;
		}
		else if (strcmp(argv[a], "-fmt") == 0) {
			a++;
			if (a >= argc) {
				printUsage(argv[0]);
				return;
			}
			if (strcmp(argv[a], "NTSC") == 0)
			videoFormat = NTSC_FMT;
			else if (strcmp(argv[a], "PAL") != 0) {
				cout << "Illegal video format '" << argv[a] << "'\n";
				printUsage(argv[0]);
				return;
			}
		}
		else if (strcmp(argv[a], "-mlog") == 0) {
			debugManager.setMemLogAdr(stoi(argv[a + 1], 0, 16));
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
		else if (strcmp(argv[a], "-trace") == 0 || strcmp(argv[a], "-ctrace") == 0 || strcmp(argv[a], "-xtrace") == 0 || strcmp(argv[a], "-ktrace") == 0) {
			uint16_t adr = stoi(argv[a + 1], 0, 16);
			bool recurring = strcmp(argv[a], "-ctrace") == 0;
			bool quick_trace = strcmp(argv[a], "-xqtrace") != 0 || recurring;
			bool kb_triggered_start = strcmp(argv[a], "-ktrace") == 0;
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
			debugManager.enableTracing(adr, pre_trace_len, post_trace_len, recurring, !quick_trace, kb_triggered_start);
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
				debugManager.setDebugLevel(DBG_INTERRUPTS | DBG_RESET);
			if (strstr(argv[a], "r") != NULL)
				debugManager.setDebugLevel(DBG_RESET);
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
				debugManager.setDebugLevel(DBG_AUDIO);
			if (strstr(argv[a], "c") != NULL)
				debugManager.setDebugLevel(DBG_TAPE);
			if (strstr(argv[a], "A") != NULL)
				debugManager.setDebugLevel(DBG_ALL);
			if (strstr(argv[a], "S") != NULL)
				debugManager.setDebugLevel(DBG_SPI);
		}
		else if (strcmp(argv[a], "-port") == 0) {
			a++;
			if (a >= argc) {
				printUsage(argv[0]);
				return;
			}
			Tokeniser dev_tok(argv[a], ',');
			string device_port_s;
			while (dev_tok.nextToken(device_port_s)) {
				Tokeniser port_tok(device_port_s, ':');
				string device, port;
				if (!port_tok.nextToken(device) || !port_tok.nextToken(port)) {
					printUsage(argv[0]);
					return;
				}
				debugManager.setDebugPort(device, port);
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
