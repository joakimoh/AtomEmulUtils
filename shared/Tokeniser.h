#ifndef TOKENISER_H
#define TOKENISER_H

#include <string>
#include <sstream>

using namespace std;

class Tokeniser {

private:

	stringstream mSS;
	char mDel;

public:

	Tokeniser(string s, char del);

	bool nextToken(string& token);
};




#endif