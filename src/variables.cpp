//
// Created by Jikun on 26/6/23.
//

#include "variables.h"

// Board
Bitboard bitboards[PieceCount] = {0};
Bitboard occupancies[ColourCount] = {0};
Colour side = White;
Square enpassant = no_sq;
int castle = 0;
int fifty = 0;
U64 hashKey = 0;

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

