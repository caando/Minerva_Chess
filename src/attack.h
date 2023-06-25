//
// Created by Jikun on 25/6/23.
//

#ifndef MINERVA_CHESS_SRC_ATTACK_H_
#define MINERVA_CHESS_SRC_ATTACK_H_

#include "types.h"
#include "magic.h"

// pawn attacks table [side][square]
Bitboard PawnAttacks[2][SquareCount];

// knight attacks table [square]
Bitboard KnightAttacks[SquareCount];

// king attacks table [square]
Bitboard KingAttacks[SquareCount];

// bishop attack masks
Bitboard BishopMasks[SquareCount];

// rook attack masks
Bitboard RookMasks[SquareCount];

// bishop attacks table [square][occupancies]
Bitboard BishopAttacks[SquareCount][512];

// rook attacks table [square][occupancies]
Bitboard RookAttacks[SquareCount][4096];

// Attack mask of respective pieces, with bitboard manipulation and magics

Bitboard maskPawnAttacks(int side, int square);

Bitboard maskKnightAttacks(int square);

Bitboard maskKingAttacks(int square);

Bitboard maskBishopAttacks(int square);

Bitboard maskRookAttacks(int square);

// Implementation of piece attacks without bitboard manipulation and magics

Bitboard BishopAttacksNaive(int square, Bitboard block);

Bitboard RookAttacksNaive(int square, Bitboard block);

// Initialise attacks of pawns, knights and kings
void initLeapersAttacks();

// Change occupancy
Bitboard setOccupancy(int index, int bits_in_mask, U64 attack_mask);

#endif //MINERVA_CHESS_SRC_ATTACK_H_
