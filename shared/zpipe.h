#ifndef PIPE_H
#define PIPE_H

#include <fstream>
#include <cstdint>
#include <vector>

using namespace std;

// Modified version of inf() that uncompresses bytes
// from an already open file and writes them
// into a byte vector.
bool decodeBytes(ifstream& source, vector<uint8_t> &bytes);

// Modified version of def() that compresses a byte vector
// and writes it to an already open file.
bool encodeBytes(vector<uint8_t> &bytes, ofstream& source);

#endif