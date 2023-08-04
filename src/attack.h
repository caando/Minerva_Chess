//
// Created by Jikun on 25/6/23.
//

#ifndef MINERVA_CHESS_SRC_ATTACK_H_
#define MINERVA_CHESS_SRC_ATTACK_H_

#include "types.h"
#include "magic_constants.h"

extern Bitboard pawnAttacks[2][SquareCount];

extern Bitboard knightAttacks[SquareCount];

extern Bitboard kingAttacks[SquareCount];

extern Bitboard bishopMasks[SquareCount];

extern Bitboard rookMasks[SquareCount];

extern Bitboard bishopAttacks[SquareCount][512];

extern Bitboard rookAttacks[SquareCount][4096];

Bitboard maskPawnAttacks(Colour side, Square square);

Bitboard maskKnightAttacks(Square square);

Bitboard maskKingAttacks(Square square);

Bitboard maskBishopAttacks(Square square);

Bitboard maskRookAttacks(Square square);

Bitboard bishopAttacksNaive(Square square, Bitboard block);

Bitboard rookAttacksNaive(Square square, Bitboard block);

Bitboard setOccupancy(int index, int bitsInMask, Bitboard attackMask);

#endif //MINERVA_CHESS_SRC_ATTACK_H_
