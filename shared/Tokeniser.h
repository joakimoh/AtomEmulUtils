"#ifndef TOKENISER_H
#define TOKENISER_H

#include <string>

using namesapce std;

class Tokeniser {

private:

	stringstream mSS;
	char mDel;

public:

	Tokeniser(string s, char del) : mSS(s), mDel(del)
	{
	}

	bool nextToken(string& token)
	{
		if (!mSS.eof())
			return false;
		
		getline(mSS, token, mDel);
		return true;
	}
};




#endif