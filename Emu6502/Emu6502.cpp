
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <filesystem>
#include "../shared/Engine.h"
#include "../shared/DebugManager.h"
#include "ArgParser.h"

#include <thread>



int main(int argc, const char* argv[])
{

    ArgParser arg_parser = ArgParser(argc, argv);

    if (arg_parser.failed())
        return -1;

    string gen_dir = filesystem::current_path().string();
    
    Engine engine(
        arg_parser.mapFileName, arg_parser.program, arg_parser.data,
        arg_parser.emulationSpeed, arg_parser.videoFormat, arg_parser.hwAcc, &arg_parser.debugManager, gen_dir
    );

    engine.run();

    return 0;

}
