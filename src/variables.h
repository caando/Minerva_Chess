//
// Created by Jikun on 25/6/23.
//

#ifndef MINERVA_CHESS_SRC_VARIABLES_H_
#define MINERVA_CHESS_SRC_VARIABLES_H_

#include "types.h"
#include <cstdlib>

// piece bitboards
extern Bitboard bitboards[PieceCount];

// occupancy bitboards (White, Black, Combined)
extern Bitboard occupancies[ColourCount];

// side to move
extern Colour side;

// enpassant square
extern Square enpassant;

// castling rights, bitwise or of all remaining castling rights
extern int castle;

// hashed value of current position, with 1/2^64 chance of collision
extern U64 hashKey;

extern const int MAX_MOVES;

// positions repetition table
extern U64 repetitionTable[1000];

// repetition index
extern int repetitionIndex;

// half move counter
extern int ply;

// fifty move rule counter
extern int fifty;

// exit from engine flag
extern int quit;

// UCI "movestogo" command moves counter
extern int movestogo;

// UCI "movetime" command time counter
extern int moveTime;

// UCI "time" command holder (ms)
extern int timer;

// UCI "inc" command's time increment holder
extern int inc;

// UCI "starttime" command time holder
extern int startTime;

// UCI "stoptime" command time holder
extern int stopTime;

// variable to flag time control availability
extern int timeset;

// variable to flag when the time is up
extern int stopped;

// leaf nodes (number of positions reached during the test of the move generator at a given depth)
extern U64 nodes;

#endif //MINERVA_CHESS_SRC_VARIABLES_H_
