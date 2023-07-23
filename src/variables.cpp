//
// Created by Jikun on 26/6/23.
//

#include "variables.h"

// piece bitboards
Bitboard bitboards[PieceCount] = {0};

// occupancy bitboards (White, Black, Combined)
Bitboard occupancies[ColourCount] = {0};

// side to move
Colour side = White;

// enpassant square
Square enpassant = no_sq;

// castling rights, bitwise or of all remaining castling rights
int castle = 0;

// hashed value of current position, with 1/2^64 chance of collision
U64 hashKey = 0;

// positions repetition table
U64 repetitionTable[1000] = {0};

// repetition index
int repetitionIndex = 0;

// half move counter
int ply = 0;

// fifty move rule counter
int fifty = 0;

// exit from engine flag
int quit = 0;

// UCI "movestogo" command moves counter
int movestogo = 30;

// UCI "movetime" command time counter
int moveTime = -1;

// UCI "time" command holder (ms)
int timer = -1;

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

