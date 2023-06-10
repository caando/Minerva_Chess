//
// Created by Woo Jia Hao on 10/6/23.
//

#include "uci.h"
#include <iostream>
#include <set>

namespace uci {
    const std::string quit_command{"quit"};
    const std::string position_command{"position"};
    const std::string go_command{"go"};
    const std::string stop_command{"stop"};
    const std::string display_command{"d"};
    const std::string eval_command{"eval"};
    const std::string help_command{"help"};
    const std::set<std::string> commands{
            quit_command,
            position_command,
            go_command,
            stop_command,
            display_command,
            eval_command,
            help_command
    };

    void start() {
        std::cout << "MINERVA CHESS ENGINE v0.1.0\n";
        std::cout << "=== Launched in UCI mode ===\n";
        std::string input;
        do {
            std::cout << "> ";
            std::cin >> input;
            if (!commands.contains(input)) {
                std::cout << "Unknown command: '" << input << "'. Type help for more information";
            }

            if (input == help_command) {
                UCIHandler::handleHelp();
            } else if (input == position_command) {
                UCIHandler::handlePosition(input);
            }
        } while (input != "quit");
    }

    void UCIHandler::handleHelp() {
        std::cout << "Supported commands:\n";
        std::cout << "quit > quit the program\n";
        std::cout << "position [fen <fenstring> | startpos] moves <move1> ... <movei> > setup board state\n";
        std::cout << "go movetime <x> > search for x ms\n";
        std::cout << "go mate <x> > search for mate in x moves\n";
        std::cout << "stop > prematurely stops calculations\n";
        std::cout << "d > display current position\n";
        std::cout << "eval > evaluate current position\n";
    }

    void UCIHandler::handlePosition(std::string &fen, const std::vector<std::string> &moves) {
        std::cout << fen << '\n';
    }
}