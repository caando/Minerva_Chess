//
// Created by Jikun on 25/6/23.
//

#ifndef MINERVA_CHESS_SRC_VARIABLES_H_
#define MINERVA_CHESS_SRC_VARIABLES_H_

#include "types.h"

// Board
extern BoardState board;

// Repetition
extern U64 repetitionTable[1000];
extern int repetitionIndex;
extern int ply;

// UCI stuff
extern int quit;
extern int movestogo;
extern int moveTime;
extern int timer;
extern int inc;
extern int startTime;
extern int stopTime;
extern int timeset;
extern int stopped;
extern U64 nodes;

#endif //MINERVA_CHESS_SRC_VARIABLES_H_
