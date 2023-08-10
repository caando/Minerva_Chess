//
// Created by Jikun on 25/6/23.
//

#ifndef MINERVA_CHESS_SRC_ATTACK_H_
#define MINERVA_CHESS_SRC_ATTACK_H_

#include "types.h"
#include "magic_constants.h"

extern Bitboard pawnAttacks[2][SquareCount];

extern Bitboard knightAttacks[SquareCount];

extern Bitboard bishopAttacks[SquareCount][512];

extern Bitboard rookAttacks[SquareCount][4096];

extern Bitboard kingAttacks[SquareCount];

enum BishopRook { Rook, Bishop };

void initLeapersAttacks();

void initSlidersAttacks(BishopRook isBishop);

Bitboard getBishopAttacks(Square square, Bitboard occupancy);

Bitboard getRookAttacks(Square square, Bitboard occupancy);

Bitboard getQueenAttacks(Square square, Bitboard occupancy);

#endif //MINERVA_CHESS_SRC_ATTACK_H_
