#include "uci.h"

#include "iostream"
#include "fstream"

void uci_start() {
    std::string line;
    std::string currentFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

    std::ofstream log("uci_log.txt", std::ios::out | std::ios::app);
    if (!log.is_open()) {
        std::cerr << "Failed to open log file\n";
        return;
    }
    log << "ENGINE RESTART" << std::endl;


    while (std::getline(std::cin, line)) {
        log << "UI → Engine: " << line << std::endl;
        if (line == "uci") {
            std::cout << "id name chess-engine" << std::endl;
            std::cout << "id author jdf18;mickyj0101" << std::endl;
            std::cout << "uciok" << std::endl;
        } else if (line == "isready") {
            std::cout << "readyok" << std::endl;
        } else if (line.rfind("position", 0) == 0) {
            std::istringstream stream(line);
            std::string token;
            stream >> token; // 'position'
            stream >> token;
            if (token == "startpos") {
                currentFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
            } else if (token == "fen") {
                std::string fen;
                std::getline(stream, fen);
                currentFEN = fen.substr(1);  // remove leading space
            }
        } else if (line.rfind("go", 0) == 0) {
            std::string bestMove = get_best_move(currentFEN);
            std::cout << "bestmove " << bestMove << std::endl;
            log << "Engine → UI: " << bestMove << std::endl;
        } else if (line == "quit") {
            break;
        }
    }
    log.close();
}

int main() {
    uci_start();
    return 0;
}