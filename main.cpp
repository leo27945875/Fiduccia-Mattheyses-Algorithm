#include <iostream>
#include <stdexcept>
#include "include/structure.hpp"
#include "include/partition.hpp"
#include "include/utils.hpp"


int main(int argc, char *argv[]){
    if (argc != 3)
        throw std::runtime_error("[main] There must be 2 arguments (input & output filenames).");

    std::string inputFile  = argv[1];
    std::string outputFile = argv[2];

    END_LINE;
    LOGKV("Input File", inputFile);
    LOGKV("Output File", outputFile);
    END_LINE;

    FMAlgo algo(inputFile);
    algo.run(outputFile);
    return 0;
}