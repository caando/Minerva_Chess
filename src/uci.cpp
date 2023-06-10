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
            std::getline(std::cin, input);
            Command command = CommandParser::parseLine(input);
            if (!commands.contains(command.command)) {
                std::cout << "Unknown command: '" << input << "'. Type help for more information";
            }

            if (command.command == help_command) {
                Handler::handleHelp();
            } else if (command.command == position_command) {
                Handler::handlePosition(command.args[0], command.args);
            }
        } while (input != "quit");
    }

    void Handler::handleHelp() {
        std::cout << "Supported commands:\n";
        std::cout << "quit > quit the program\n";
        std::cout << "position [fen <fenstring> | startpos] moves <move1> ... <movei> > setup board state\n";
        std::cout << "go movetime <x> > search for x ms\n";
        std::cout << "go mate <x> > search for mate in x moves\n";
        std::cout << "stop > prematurely stops calculations\n";
        std::cout << "d > display current position\n";
        std::cout << "eval > evaluate current position\n";
    }

    void Handler::handlePosition(std::string &fen, const std::vector<std::string> &moves) {
        std::cout << fen << '\n';

        for (auto &arg: moves) {
            std::cout << arg << std::endl;
        }
    }

    Command CommandParser::parseLine(std::string &line) {
        char delimiter{' '};
        int start{0};
        std::vector<std::string> parts{};

        for (int i{0}; i < line.size(); i++) {
            if (line[i] == delimiter) {
                parts.push_back(line.substr(start, i - start));
                start = i + 1;
            }
        }

        if (start != line.size()) parts.push_back(line.substr(start, line.size()));

        std::string command{parts[0]};
        std::vector<std::string> args{};

        for (int i{1}; i < parts.size(); i++) {
            args.push_back(parts[i]);
        }

        return {command, args};
    }
}