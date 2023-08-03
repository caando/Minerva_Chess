//
// Created by Jikun on 26/6/23.
//

#ifndef MINERVA_CHESS_SRC_UNI_H_
#define MINERVA_CHESS_SRC_UNI_H_

#include "variables.h"

#define version " - 1.0.0"
#define INPUT_BUFFER 10000

int parseMove(const char *moveString);

void resetBoard();

void parseFen(char *fen);

void parsePosition(char *command);

void resetTimeControl();

void parseGo(char *command);

void uciLoop();

#endif //MINERVA_CHESS_SRC_UNI_H_
