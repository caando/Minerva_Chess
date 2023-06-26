//
// Created by Jikun on 25/6/23.
//

#ifndef MINERVA_CHESS_SRC_VARIABLES_H_
#define MINERVA_CHESS_SRC_VARIABLES_H_

#include "types.h"
#include <cstdlib>

// piece bitboards
static Bitboard bitboards[PieceCount];

// occupancy bitboards (White, Black, Combined)
static Bitboard occupancies[ColourCount];

// side to move
static Colour side;

// enpassant square
static Square enpassant = no_sq;

// castling rights, bitwise or of all remaining castling rights
static int castle;

// hashed value of current position, with 1/2^64 chance of collision
static U64 hashKey;

static constexpr int MAX_MOVES = 500;

// positions repetition table
static  U64 repetitionTable[MAX_MOVES * 2];

// repetition index
static int repetitionIndex;

// half move counter
static int ply;

// fifty move rule counter
static int fifty;

// exit from engine flag
static int quit = 0;

// UCI "movestogo" command moves counter
static int movestogo = 30;

// UCI "movetime" command time counter
static int moveTime = -1;

// UCI "time" command holder (ms)
static int timer = -1;

// UCI "inc" command's time increment holder
static int inc = 0;

// UCI "starttime" command time holder
static int startTime = 0;

// UCI "stoptime" command time holder
static int stopTime = 0;

// variable to flag time control availability
static int timeset = 0;

// variable to flag when the time is up
static int stopped = 0;

// leaf nodes (number of positions reached during the test of the move generator at a given depth)
static U64 nodes = 0;

#endif //MINERVA_CHESS_SRC_VARIABLES_H_
