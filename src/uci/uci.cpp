#include "uci.h"

void uci_start() {
    std::string line;
    std::string currentFEN = "startpos";  // default

    while (std::getline(std::cin, line)) {
        if (line == "uci") {
            std::cout << "id name chess-engine" << std::endl;
            std::cout << "id author jdf18;mickyj0101" << std::endl;
            std::cout << "uciok" << std::endl;
        } else if (line == "isready") {
            std::cout << "readyok" << std::endl;
        } else if (line == "quit") {
            break;
        }
    }
}

int main() {
    uci_start();
    return 0;
}