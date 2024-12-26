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
	cout << "Usage:\t" << name << " -rom <ROM dir> [-clk <clock speed>] [-v]\n\n";
	cout << "<clock speed>:\n\Clock speed in MHz. If not specified, 1 Mhz is assumed\n\n";
	cout << "<ROM dir>:\n\tDirectory in which ROM files are located\n\n";
	cout << "-v:\n\tVerbose output\n\n";
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
		else if (strcmp(argv[a], "-rom") == 0) {
			romPath = argv[a + 1];
			a++;
		}
		else if (strcmp(argv[a], "-v") == 0) {
			verbose = true;
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
