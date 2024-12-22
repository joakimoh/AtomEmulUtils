
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <filesystem>
#include <cstdint>
#include "ArgParser.h"
#include "../shared/Codec6502.h"

using namespace std;

int main(int argc, const char* argv[])
{


    ArgParser arg_parser = ArgParser(argc, argv);

    if (arg_parser.failed())
        return -1;

    ostream* fout_p = &cout;
    bool write_to_file = false;
    if (arg_parser.dstFileName != "") {
        write_to_file = true;
        cout << "Output file name = " << arg_parser.dstFileName << "\n";
        fout_p = new ofstream(arg_parser.dstFileName);
        if (!*fout_p) {
            cout << "can't write to file " << arg_parser.dstFileName << "\n";
            return (-1);
        }
    }

    Codec6502 codec;

    if (!codec.decode(arg_parser.startAdr, arg_parser.srcFileName, *fout_p)) {
        return -1;
    }

    if (write_to_file) {
        ((ofstream*)fout_p)->close();
        delete fout_p;
    }

    return 0;

}