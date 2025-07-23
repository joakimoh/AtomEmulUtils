#ifndef UTILITY_H
#define UTILITY_H

#include <string>

using namespace std;


class Utility {

public:
	static string int2HexStr(int v, int n);
	static string int2BinStr(int v, int n);
	static string encodeCPUTime(double t_s);
	static string mask2Str(uint8_t mask);
};


#endif