#pragma once

#include <string>
#include <map>
#include <vector>

using namespace std;

class ArgParser
{
public:

	double cMHz = 1.0;
	bool verbose = false;
	string mapFileName;

private:

	void printUsage(const char*);

	bool mParseSuccess = false;



public:

	ArgParser(int argc, const char* argv[]);

	bool failed();

};

