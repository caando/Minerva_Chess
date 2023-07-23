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

// bishop and rook
enum BishopRook { Rook, Bishop };

U64 findMagicNumber(Square square, int relevantBits, BishopRook isBishop);

void initMagicNumbers();

void initSlidersAttacks(BishopRook isBishop);

// get bishop attacks
inline Bitboard getBishopAttacks(Square square, Bitboard occupancy) {
  // get bishop attacks assuming current board occupancy
  occupancy &= bishopMasks[square];
  occupancy *= bishopMagicNumbers[square];
  occupancy >>= 64 - bishopRelevantBits[square];

  // return bishop attacks
  return bishopAttacks[square][occupancy];
}

// get rook attacks
inline Bitboard getRookAttacks(Square square, Bitboard occupancy) {
  // get rook attacks assuming current board occupancy
  occupancy &= rookMasks[square];
  occupancy *= rookMagicNumbers[square];
  occupancy >>= 64 - rookRelevantBits[square];

  // return rook attacks
  return rookAttacks[square][occupancy];
}

// get queen attacks
inline Bitboard getQueenAttacks(Square square, Bitboard occupancy) {
  // init result attacks bitboard
  U64 queen_attacks = 0ULL;

  // init bishop occupancies
  U64 bishop_occupancy = occupancy;

  // init rook occupancies
  U64 rook_occupancy = occupancy;

  // get bishop attacks assuming current board occupancy
  bishop_occupancy &= bishopMasks[square];
  bishop_occupancy *= bishopMagicNumbers[square];
  bishop_occupancy >>= 64 - bishopRelevantBits[square];

  // get bishop attacks
  queen_attacks = bishopAttacks[square][bishop_occupancy];

  // get rook attacks assuming current board occupancy
  rook_occupancy &= rookMasks[square];
  rook_occupancy *= rookMagicNumbers[square];
  rook_occupancy >>= 64 - rookRelevantBits[square];

  // get rook attacks
  queen_attacks |= rookAttacks[square][rook_occupancy];

  // return queen attacks
  return queen_attacks;
}

#endif //MINERVA_CHESS_SRC_MAGIC_H_
