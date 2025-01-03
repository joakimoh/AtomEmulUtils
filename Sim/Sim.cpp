
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <filesystem>
#include <cstdint>
#include "ArgParser.h"
#include "../shared/P6502.h"
#include <thread>
#include "../shared/Codec6502.h"

using namespace std;

int main(int argc, const char* argv[])
{

    ArgParser arg_parser = ArgParser(argc, argv);

    if (arg_parser.failed())
        return -1;

    Devices devices(arg_parser.mapFileName, arg_parser.debugInfo, arg_parser.program);

    // Create processor object
    P6502 processor(arg_parser.cMHz, devices, arg_parser.debugInfo);

    // Start processor thread (run method of processor)
    std::thread processor_t(&P6502::run, &processor);

    // Wait for processor thread to complete
    processor_t.join();

    return 0;

}