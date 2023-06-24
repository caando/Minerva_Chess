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
uint64_t hash_key;

const int MAX_MOVES = 500;

// positions repetition table
uint64_t repetition_table[MAX_MOVES * 2];

// repetition index
int repetition_index;

// half move counter
int ply;

// fifty move rule counter
int fifty;

#endif //MINERVA_CHESS_SRC_VARIABLES_H_
