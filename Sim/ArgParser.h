#pragma once

#include <string>
#include <map>
#include <vector>
#include "../shared/DebugInfo.h"

using namespace std;

class ArgParser
{
public:

	double cMHz = 1.0;
	string mapFileName;

	DebugInfo debugInfo;


private:

	void printUsage(const char*);

	bool mParseSuccess = false;



public:

	ArgParser(int argc, const char* argv[]);

	bool failed();

};

