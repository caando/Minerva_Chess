//
// Created by Jikun on 25/6/23.
//

#include "attack.h"
#include "util.h"

Bitboard pawnAttacks[2][SquareCount] = {{0}};
Bitboard knightAttacks[SquareCount] = {0};
Bitboard kingAttacks[SquareCount] = {0};
Bitboard bishopMasks[SquareCount] = {0};
Bitboard rookMasks[SquareCount] = {0};
Bitboard bishopAttacks[SquareCount][512] = {{0}};
Bitboard rookAttacks[SquareCount][4096];

Bitboard maskPawnAttacks(Colour side, Square square) {
  Bitboard attacks = 0ULL;
  Bitboard bitboard = 0ULL;
  setBit(bitboard, square);
  if (!side) {
    if ((bitboard >> 7) & notAFile) attacks |= (bitboard >> 7);
    if ((bitboard >> 9) & notHFile) attacks |= (bitboard >> 9);
  } else {
    if ((bitboard << 7) & notHFile) attacks |= (bitboard << 7);
    if ((bitboard << 9) & notAFile) attacks |= (bitboard << 9);
  }
  return attacks;
}

Bitboard maskKnightAttacks(Square square) {
  Bitboard attacks = 0ULL;
  Bitboard bitboard = 0ULL;
  setBit(bitboard, square);

  if ((bitboard >> 17) & notHFile) {
    attacks |= (bitboard >> 17);
  }
  if ((bitboard >> 15) & notAFile) {
    attacks |= (bitboard >> 15);
  }
  if ((bitboard >> 10) & notHGFile) {
    attacks |= (bitboard >> 10);
  }
  if ((bitboard >> 6) & notABFile) {
    attacks |= (bitboard >> 6);
  }
  if ((bitboard << 17) & notAFile) {
    attacks |= (bitboard << 17);
  }
  if ((bitboard << 15) & notHFile) {
    attacks |= (bitboard << 15);
  }
  if ((bitboard << 10) & notABFile) {
    attacks |= (bitboard << 10);
  }
  if ((bitboard << 6) & notHGFile) {
    attacks |= (bitboard << 6);
  }

  return attacks;
}

Bitboard maskKingAttacks(Square square) {
  Bitboard attacks = 0ULL;
  Bitboard bitboard = 0ULL;
  setBit(bitboard, square);

  if (bitboard >> 8) {
    attacks |= (bitboard >> 8);
  }
  if ((bitboard >> 9) & notHFile) {
    attacks |= (bitboard >> 9);
  }
  if ((bitboard >> 7) & notAFile) {
    attacks |= (bitboard >> 7);
  }
  if ((bitboard >> 1) & notHFile) {
    attacks |= (bitboard >> 1);
  }
  if (bitboard << 8) {
    attacks |= (bitboard << 8);
  }
  if ((bitboard << 9) & notAFile) {
    attacks |= (bitboard << 9);
  }
  if ((bitboard << 7) & notHFile) {
    attacks |= (bitboard << 7);
  }
  if ((bitboard << 1) & notAFile) {
    attacks |= (bitboard << 1);
  }
  return attacks;
}

Bitboard maskBishopAttacks(Square square) {
  Bitboard attacks = 0ULL;
  int r, f;
  int tr = square / 8;
  int tf = square % 8;

  for (r = tr + 1, f = tf + 1; r <= 6 && f <= 6; r++, f++) {
    attacks |= (1ULL << (r * 8 + f));
  }
  for (r = tr - 1, f = tf + 1; r >= 1 && f <= 6; r--, f++) {
    attacks |= (1ULL << (r * 8 + f));
  }
  for (r = tr + 1, f = tf - 1; r <= 6 && f >= 1; r++, f--) {
    attacks |= (1ULL << (r * 8 + f));
  }
  for (r = tr - 1, f = tf - 1; r >= 1 && f >= 1; r--, f--) {
    attacks |= (1ULL << (r * 8 + f));
  }
  return attacks;
}

Bitboard maskRookAttacks(Square square) {
  Bitboard attacks = 0ULL;
  int r, f;
  int tr = square / 8;
  int tf = square % 8;

  for (r = tr + 1; r <= 6; r++) attacks |= (1ULL << (r * 8 + tf));
  for (r = tr - 1; r >= 1; r--) attacks |= (1ULL << (r * 8 + tf));
  for (f = tf + 1; f <= 6; f++) attacks |= (1ULL << (tr * 8 + f));
  for (f = tf - 1; f >= 1; f--) attacks |= (1ULL << (tr * 8 + f));
  return attacks;
}

Bitboard bishopAttacksNaive(Square square, Bitboard block) {
  Bitboard attacks = 0ULL;
  int r, f;
  int tr = square / 8;
  int tf = square % 8;

  for (r = tr + 1, f = tf + 1; r <= 7 && f <= 7; r++, f++) {
    attacks |= (1ULL << (r * 8 + f));
    if ((1ULL << (r * 8 + f)) & block) break;
  }

  for (r = tr - 1, f = tf + 1; r >= 0 && f <= 7; r--, f++) {
    attacks |= (1ULL << (r * 8 + f));
    if ((1ULL << (r * 8 + f)) & block) break;
  }

  for (r = tr + 1, f = tf - 1; r <= 7 && f >= 0; r++, f--) {
    attacks |= (1ULL << (r * 8 + f));
    if ((1ULL << (r * 8 + f)) & block) break;
  }

  for (r = tr - 1, f = tf - 1; r >= 0 && f >= 0; r--, f--) {
    attacks |= (1ULL << (r * 8 + f));
    if ((1ULL << (r * 8 + f)) & block) break;
  }

  return attacks;
}

Bitboard rookAttacksNaive(Square square, Bitboard block) {
  Bitboard attacks = 0ULL;
  int r, f;
  int tr = square / 8;
  int tf = square % 8;

  for (r = tr + 1; r <= 7; r++) {
    attacks |= (1ULL << (r * 8 + tf));
    if ((1ULL << (r * 8 + tf)) & block) break;
  }

  for (r = tr - 1; r >= 0; r--) {
    attacks |= (1ULL << (r * 8 + tf));
    if ((1ULL << (r * 8 + tf)) & block) break;
  }

  for (f = tf + 1; f <= 7; f++) {
    attacks |= (1ULL << (tr * 8 + f));
    if ((1ULL << (tr * 8 + f)) & block) break;
  }

  for (f = tf - 1; f >= 0; f--) {
    attacks |= (1ULL << (tr * 8 + f));
    if ((1ULL << (tr * 8 + f)) & block) break;
  }

  return attacks;
}

Bitboard setOccupancy(int index, int bitsInMask, Bitboard attackMask) {
  Bitboard occupancy = 0ULL;

  for (int count = 0; count < bitsInMask; count++) {
    int square = LSOneIdx(attackMask);
    remBit(attackMask, square);
    if (index & (1 << count)) {
      occupancy |= (1ULL << square);
    }
  }

  return occupancy;
}