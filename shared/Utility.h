#ifndef UTILITY_H
#define UTILITY_H

#include <string>

using namespace std;


class Utility {

public:
	static string int2hexStr(int v, int n);
	static string int2binStr(int v, int n);
	static string encodeCPUTime(double t_s);
};


#endif