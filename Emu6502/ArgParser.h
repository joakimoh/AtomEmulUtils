#pragma once

#include <string>
#include <map>
#include <vector>
#include "../shared/DebugTracing.h"
#include "../shared/Device.h"
#include "../shared/VideoSettings.h"
#include "../shared/Engine.h"

using namespace std;

class ArgParser
{
public:

	VideoFormat videoFormat = PAL_FMT;

	bool hwAcc = true;
	string mapFileName;
	DebugTracing debugTracing;

	Engine::RunState initialState = Engine::ENG_TBD;


private:

	void printUsage(const char*);
	void printInfo();

	bool mParseSuccess = false;



public:

	ArgParser(int argc, const char* argv[]);

	bool failed();

};

