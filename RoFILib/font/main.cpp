#include <algorithm>
#include <iostream>
#include <string>
#include "../configuration/IO.h"

bool checkLetter(char letter) {
    return letter == 'a' || letter == 'i';
}

void printCam(const Configuration &cfg) {
    long count = cfg.getModules().size();
    long rows = count / 7;
    double middle = static_cast<double>(rows) / 2 - 0.5;
    std::cout << "CP " << middle << " 0 " << std::min(-4 * rows, static_cast<long>(-32)) << std::endl;
    std::cout << "CV 0 -1 0" << std::endl;
    std::cout << "CF " << middle << " -6 0" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Missing arguments" << std::endl;
        return 1;
    }

    std::string dir = argv[1];
    char letter = argv[2][0];

    if (!checkLetter(letter)) {
        std::cerr << "Wrong input" << std::endl;
        return 2;
    }

    std::string fileName = dir;
    fileName.append(1, letter);
    fileName.append(".in");
    std::ifstream file;
    file.open(fileName);

    if (!file.is_open()) {
        std::cerr << "Configuration file for given letter does not exist" << std::endl;
        return 3;
    }

    Configuration cfg;
    bool rv = IO::readConfiguration(file, cfg);
    if (!rv) {
        std::cerr << "Configuration file "  << fileName << " is invalid" << std::endl;
        return 4;
    }

    file.close();

    printCam(cfg);
    std::cout << IO::toString(cfg);

    return 0;
}