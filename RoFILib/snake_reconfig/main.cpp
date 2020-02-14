#include <exception>
#include <fstream>
#include <tuple>
#include <stdlib.h>
#include "../configuration/Configuration.h"
#include "../configuration/IO.h"
#include "Snake_algorithms.h"

int main(int argc, char* argv[])
{
    if (argc < 2) {
        throw std::invalid_argument("No filepath given");
    }

    double path_pref = 1;
    double free_pref = 1;

    if (argc > 3) {
        path_pref = std::atof(argv[2]);
        free_pref = std::atof(argv[3]);
    }

    std::ifstream initInput;
    initInput.open(argv[1]);
    Configuration init;
    IO::readConfiguration(initInput, init);
    init.computeMatrices();
    /*
    {
        std::cout << "Loaded: \n" << IO::toString(init);
        auto sg = SpaceGrid(init);
        std::cout << "Freeness: " << std::endl << sg.getFreeness() << std::endl;
        sg.printGrid();
    }
    */
    /* */
    std::vector<Configuration> path;
    AlgorithmStat stat;
    int moduleCount = init.getIDs().size();
    int limit = 10000/moduleCount;

    path = SnakeStar(init, &stat, limit, path_pref, free_pref);

    std::cout << IO::toString(path[path.size()-1]);
    SpaceGrid debug(path[path.size()-1]);
    std::cout << debug.getFreeness() << std::endl;
    std::cout << stat.toString();
    if (path.empty()) {
        std::cout << "Could not find a path with given parameters\n";
        return 0;
    }
    /* */
}