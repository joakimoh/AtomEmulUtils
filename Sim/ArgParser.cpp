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
	cout << "Usage:\t" << name << " -clk <clock speed> [-v]\n\n";
	cout << "<clock speed>:\n\Clock speed in MHz. If not specified, 1 Mhz is assumed\n\n";
	cout << "-v:\n\tVerbose output\n\n";
}

ArgParser::ArgParser(int argc, const char* argv[])
{

	if (argc <= 1) {
		printUsage(argv[0]);
		return;
	}

	int ac = 2;
	bool genFiles = false;
	while (ac < argc) {
		if(strcmp(argv[ac], "-clk") == 0) {
			cMHz = stod(argv[ac + 1]);
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
