//
// Created by Woo Jia Hao on 10/6/23.
//

#ifndef MINERVA_CHESS_UCI_H
#define MINERVA_CHESS_UCI_H

#include <string>
#include <vector>

namespace uci {
    void start();

    struct Command {
        std::string command;
        std::vector<std::string> args;
    };

    class Handler {
    public:
        static void handleHelp();

        static void handlePosition(std::string &fen, const std::vector<std::string> &moves = {});
    };

    class CommandParser {
    public:
        static Command parseLine(std::string &line);
    };
}

#endif //MINERVA_CHESS_UCI_H
