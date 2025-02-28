#pragma once

#include <string>
#include <map>
#include <vector>
#include "../shared/DebugInfo.h"
#include "../shared/Device.h"

using namespace std;

class ArgParser
{
public:

	string mapFileName;
	Program program, data;
	
	DebugInfo  debugInfo;

	double emulationSpeed = 100; // %


private:

	void printUsage(const char*);

	bool mParseSuccess = false;



public:

	ArgParser(int argc, const char* argv[]);

	bool failed();

};

