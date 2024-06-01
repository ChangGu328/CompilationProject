#include <iostream>
#include <string>
#include "mpg_analysis.hpp"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
        return 1;
    } else {
        std::string filename = argv[1];
        MpgAnalysis mp(filename);
        if (!mp.mgpAnalysis()) {
            std::cout << ">compile succeed!" << "\n";
        }
        mp.showPcodeInStack();
    }
}
