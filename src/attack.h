//
// Created by Jikun on 25/6/23.
//

#ifndef MINERVA_CHESS_SRC_ATTACK_H_
#define MINERVA_CHESS_SRC_ATTACK_H_

#include "types.h"
#include "magic_constants.h"

// pawn attacks table [side][square]
extern Bitboard pawnAttacks[2][SquareCount];

// knight attacks table [square]
extern Bitboard knightAttacks[SquareCount];

// king attacks table [square]
extern Bitboard kingAttacks[SquareCount];

// bishop attack masks
extern Bitboard bishopMasks[SquareCount];

// rook attack masks
extern Bitboard rookMasks[SquareCount];

// bishop attacks table [square][occupancies]
extern Bitboard bishopAttacks[SquareCount][512];

// rook attacks table [square][occupancies]
extern Bitboard rookAttacks[SquareCount][4096];

// Attack mask of respective pieces, with bitboard manipulation and magics

Bitboard maskPawnAttacks(Colour side, Square square);

Bitboard maskKnightAttacks(Square square);

Bitboard maskKingAttacks(Square square);

Bitboard maskBishopAttacks(Square square);

Bitboard maskRookAttacks(Square square);

// Implementation of piece attacks without bitboard manipulation and magics

Bitboard bishopAttacksNaive(Square square, Bitboard block);

Bitboard rookAttacksNaive(Square square, Bitboard block);

// Initialise attacks of pawns, knights and kings
void initLeapersAttacks();

// Change occupancy
Bitboard setOccupancy(int index, int bits_in_mask, Bitboard attack_mask);

#endif //MINERVA_CHESS_SRC_ATTACK_H_
