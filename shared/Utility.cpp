#include "Utility.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cmath>

using namespace std;

string Utility::mask2DirStr(int v, int n)
{
	stringstream sout;
	for (int i = 0; i < n; i++) {
		if ((v >> (n-i-1)) & 0x1)
			sout << "O";
		else
			sout << "I";
	}
	return sout.str();
}
string Utility::mask2Str(uint8_t mask)
{
	int msb = 7;
	int lsb = 0;
	for (; msb > 0 && (mask & (1 << msb)) == 0; msb--);
	for (; lsb < 8 && (mask & (1 << lsb)) == 0; lsb++);
	if (lsb == msb)
		return "b" + to_string(lsb);
	else
		return "b" + to_string(msb) + ":b" + to_string(lsb);
}

string Utility::int2HexStr(int v, int n)
{
	stringstream sout;
	sout << setfill('0') << setw(n) << hex << v;
	return sout.str();
}

string Utility::int2BinStr(int v, int n)
{
	stringstream sout;
	for (int i = 0; i < n; i++) {
		if ((v & (1 << (n - i - 1))) != 0)
			sout << "1";
		else
			sout << "0";
	}
	return sout.str();
}

string Utility::encodeCPUTime(double t_s) {
    int t_s_i = (int) trunc(t_s);
    double t_ms = (t_s - t_s_i) * 1000;
	int t_ms_i = (int) trunc(t_ms);
	double t_us = (t_ms - t_ms_i) * 1000;
	int t_us_i = (int)round(t_us);
    stringstream s;
	s <<
		right << setw(4) << setfill('0') << t_s_i << " " <<
		right << setw(3) << setfill('0') << t_ms_i << " " <<
		right << setw(3) << setfill('0') << t_us_i;
    return s.str();
}