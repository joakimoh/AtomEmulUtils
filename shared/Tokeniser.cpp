#include "Tokeniser.h"

Tokeniser::Tokeniser(string s, char del) : mSS(s), mDel(del)
{
}

bool Tokeniser::nextToken(string& token)
{
	if (!mSS.eof())
		return false;

	getline(mSS, token, mDel);
	return true;
}