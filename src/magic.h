//
// Created by Jikun on 25/6/23.
//

#ifndef MINERVA_CHESS_SRC_MAGIC_H_
#define MINERVA_CHESS_SRC_MAGIC_H_

#include "types.h"
#include "magic_constants.h"
#include "attack.h"
#include "util.h"
#include <bit>
#include <cstring>
#include <iostream>

enum BishopRook { Rook, Bishop };

void initLeapersAttacks();

void initSlidersAttacks(BishopRook isBishop);

Bitboard getBishopAttacks(Square square, Bitboard occupancy);

Bitboard getRookAttacks(Square square, Bitboard occupancy);

Bitboard getQueenAttacks(Square square, Bitboard occupancy);

#endif //MINERVA_CHESS_SRC_MAGIC_H_
