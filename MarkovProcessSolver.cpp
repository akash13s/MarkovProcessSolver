//
// Created by Akash Shrivastva on 11/9/23.
//

#include "MarkovProcessSolver.h"

using namespace std;

void readCommandLineArguments(int argc, char *argv[], ProgramArguments *arguments) {
    string inputFileExtension = ".txt";
    for (int i = 0; i < argc; i++) {
        string arg = argv[i];

        if (arg == "-min") {
            arguments->maximise = false;
        } else if (arg == "-df") {
            if (i+1<argc) {
                arguments->discountFactor = stod(argv[i+1]);
            }
        } else if (arg == "-tol") {
            if (i+1<argc) {
                arguments->tolerance = stod(argv[i+1]);
            }
        } else if (arg.length() >= inputFileExtension.length() &&
                   arg.substr(arg.length() - inputFileExtension.length()) == inputFileExtension) {
            arguments->inputFile = arg;
        }
    }
}

int main(int argc, char *argv[]) {
    ProgramArguments *arguments = new ProgramArguments();
    readCommandLineArguments(argc, argv, arguments);

    MarkovProcessSolver *solver = new MarkovProcessSolver(arguments);
    solver->solve();
}