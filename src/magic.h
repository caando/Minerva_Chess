//
// Created by Jikun on 25/6/23.
//

#ifndef MINERVA_CHESS_SRC_MAGIC_H_
#define MINERVA_CHESS_SRC_MAGIC_H_

#include "types.h"

// bishop and rook
enum BishopRook { Rook, Bishop };

U64 findMagicNumber(Square square, int relevantBits, BishopRook isBishop);

void initMagicNumbers();

void initSlidersAttacks(BishopRook isBishop);

static inline Bitboard getBishopAttacks(Square square, Bitboard occupancy);

static inline Bitboard getRookAttacks(Square square, Bitboard occupancy);

static inline Bitboard getQueenAttacks(Square square, Bitboard occupancy);

#endif //MINERVA_CHESS_SRC_MAGIC_H_
