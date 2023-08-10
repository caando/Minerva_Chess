//
// Created by Jikun on 25/6/23.
//

#ifndef MINERVA_CHESS_SRC_ATTACK_H_
#define MINERVA_CHESS_SRC_ATTACK_H_

#include "types.h"
#include "magic_constants.h"
#include <array>

extern const std::array<std::array<Bitboard, SquareCount>, 2> pawnAttacks;
extern std::array<Bitboard, 64> knightAttacks;
extern std::array<std::array<Bitboard, 512>, SquareCount> bishopAttacks;
extern std::array<std::array<Bitboard, 4096>, SquareCount> rookAttacks;
extern std::array<Bitboard, 64> kingAttacks;

enum BishopRook { Rook, Bishop };

void initLeapersAttacks();

void initBishopAttacks();
void initRookAttacks();

Bitboard getBishopAttacks(Square square, Bitboard occupancy);

Bitboard getRookAttacks(Square square, Bitboard occupancy);

Bitboard getQueenAttacks(Square square, Bitboard occupancy);

#endif //MINERVA_CHESS_SRC_ATTACK_H_
