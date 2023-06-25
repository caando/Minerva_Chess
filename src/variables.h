//
// Created by Jikun on 25/6/23.
//

#ifndef MINERVA_CHESS_SRC_VARIABLES_H_
#define MINERVA_CHESS_SRC_VARIABLES_H_

#include "types.h"
#include "stdlib.h"

// piece bitboards
Bitboard bitboards[PieceCount];

// occupancy bitboards (White, Black, Combined)
Bitboard occupancies[ColourCount];

// side to move
Colour side;

// enpassant square
Square enpassant = no_sq;

// castling rights, bitwise or of all remaining castling rights
int castle;

// hashed value of current position, with 1/2^64 chance of collision
U64 hashKey;

constexpr int MAX_MOVES = 500;

// positions repetition table
U64 repetitionTable[MAX_MOVES * 2];

// repetition index
int repetitionIndex;

// half move counter
int ply;

// fifty move rule counter
int fifty;

// exit from engine flag
int quit = 0;

// UCI "movestogo" command moves counter
int movestogo = 30;

// UCI "movetime" command time counter
int moveTime = -1;

// UCI "time" command holder (ms)
int time = -1;

// UCI "inc" command's time increment holder
int inc = 0;

// UCI "starttime" command time holder
int startTime = 0;

// UCI "stoptime" command time holder
int stopTime = 0;

// variable to flag time control availability
int timeset = 0;

// variable to flag when the time is up
int stopped = 0;

// leaf nodes (number of positions reached during the test of the move generator at a given depth)
U64 nodes = 0;

#endif //MINERVA_CHESS_SRC_VARIABLES_H_
