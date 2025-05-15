#pragma once

#include <string>
#include <map>
#include <vector>
#include "../shared/DebugManager.h"
#include "../shared/Device.h"
#include "../shared/VideoSettings.h"

using namespace std;

class ArgParser
{
public:

	string mapFileName;
	Program program, data;
	
	DebugManager  debugManager;

	double emulationSpeed = 100; // %

	VideoFormat videoFormat = PAL_FMT;

	bool hwAcc = true;


private:

	void printUsage(const char*);
	void printInfo();

	bool mParseSuccess = false;



public:

	ArgParser(int argc, const char* argv[]);

	bool failed();

};

