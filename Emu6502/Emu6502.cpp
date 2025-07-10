
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include "../shared/Engine.h"
#include "../shared/DebugManager.h"
#include "ArgParser.h"

#include <thread>



int main(int argc, const char* argv[])
{

    ArgParser arg_parser = ArgParser(argc, argv);

    if (arg_parser.failed())
        return -1;

    Engine engine(
        arg_parser.mapFileName, arg_parser.program, arg_parser.data,
        arg_parser.emulationSpeed, arg_parser.videoFormat, arg_parser.hwAcc, &arg_parser.debugManager
    );

    engine.run();

    return 0;

}
