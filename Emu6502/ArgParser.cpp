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
	cout << "\t- NEC PD7002 ADC\n";
	cout << "\t- MC6850 Asynchronous Communications Interface Adapter (ACIA)\n";
	cout << "\t- SY6522 Versatile Interface Adapter (VIA)\n";
	cout << "\t- 8255 Programmable Peripheral Interface (PIA)\n";
	cout << "\t- HD6845 CRTC Controller\n";
	cout << "\t- MC6847 Video Display Generator\n";
	cout << "\t- SAA5050 Teletext Character Generator\n";
	cout << "\t- SD Card with SPI interface\n";
	cout << "\t- Standard Tape Recorder: Emulates the tape recorder you connect to the computer\n";
	cout << "\t- Custom Acorn Atom hardware: Cassette Interface, Keyboard amd Speaker\n";
	cout << "\t- Custom BBC Micro hardware: Serial ULA, Keyboard, ROM Selection and Video ULA\n";
	cout << "\n";
}

void ArgParser::printUsage(const char* name)
{
	cout << "Usage:\t" << name << " -map <memory map file> [-fmt <video format>] [-v] [-V] -nHA -halt\n\n";
	cout << "<video format>:\nEither 'PAL' or 'NTSC'. If not specified, PAL is assumed\n\n";
	cout << "<memory map file>:\n\tFile which defines devices and their memory mapping.\n\n";
	cout << "-v:\n\tDefault verbose output\n\n";
	cout << "-V:\n\tExtensive verbose output\n\n";
	cout << "-nHA:\n\tTurn off Graphics hardware acceleration.\n\n";
	cout << "-halt:\n\tHave the microprocessor halted initially.\n\n";

}

ArgParser::ArgParser(int argc, const char* argv[])
{

	if (argc <= 1) {
		printInfo();
		printUsage(argv[0]);
		return;
	}

	int a = 1;
	while (a < argc) {
		if (strcmp(argv[a], "-halt") == 0) {
			initialState = Engine::ENG_HALT;
		}
		else if (strcmp(argv[a], "-nHA") == 0) {
			hwAcc = false;
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
		else if (strcmp(argv[a], "-map") == 0) {
			mapFileName = argv[a + 1];
			a++;
		}
		else if (strcmp(argv[a], "-v") == 0) {
			debugTracing.setDebugLevel(DBG_VERBOSE);
		}
		else if (strcmp(argv[a], "-V") == 0) {
			debugTracing.setDebugLevel(DBG_VERBOSE | DBG_EXTENSIVE);
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
