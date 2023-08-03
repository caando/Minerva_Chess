//
// Created by Jikun on 26/6/23.
//

#include "variables.h"

BoardState board{
    .bitboards = {0},
    .occupancies = {0},
    .side = White,
    .enpassant = no_sq,
    .castle = 0,
    .fifty = 0,
    .hashKey = 0
};

U64 repetitionTable[1000] = {0};
int repetitionIndex = 0;
int ply = 0;

// UCI stuff
int quit = 0;
int movestogo = 30;
int moveTime = -1;
int timer = -1;
int inc = 0;
int startTime = 0;
int stopTime = 0;
int timeset = 0;
int stopped = 0;
U64 nodes = 0;

