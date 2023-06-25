//
// Created by Jikun on 25/6/23.
//

#include "attack.h"
#include "util.h"

// generate pawn attacks
Bitboard maskPawnAttacks(Colour side, Square square) {
  // result attacks bitboard
  Bitboard attacks = 0ULL;

  // piece bitboard
  Bitboard bitboard = 0ULL;

  // set piece on board
  setBit(bitboard, square);

  // white pawns
  if (!side) {
    // generate pawn attacks
    if ((bitboard >> 7) & notAFile) attacks |= (bitboard >> 7);
    if ((bitboard >> 9) & notHFile) attacks |= (bitboard >> 9);
  }

    // black pawns
  else {
    // generate pawn attacks
    if ((bitboard << 7) & notHFile) attacks |= (bitboard << 7);
    if ((bitboard << 9) & notAFile) attacks |= (bitboard << 9);
  }

  // return attack map
  return attacks;
}

// generate knight attacks
Bitboard maskKnightAttacks(Square square) {
  // result attacks bitboard
  Bitboard attacks = 0ULL;

  // piece bitboard
  Bitboard bitboard = 0ULL;

  // set piece on board
  setBit(bitboard, square);

  // generate knight attacks
  if ((bitboard >> 17) & notHFile) attacks |= (bitboard >> 17);
  if ((bitboard >> 15) & notAFile) attacks |= (bitboard >> 15);
  if ((bitboard >> 10) & notHGFile) attacks |= (bitboard >> 10);
  if ((bitboard >> 6) & notABFile) attacks |= (bitboard >> 6);
  if ((bitboard << 17) & notAFile) attacks |= (bitboard << 17);
  if ((bitboard << 15) & notHFile) attacks |= (bitboard << 15);
  if ((bitboard << 10) & notABFile) attacks |= (bitboard << 10);
  if ((bitboard << 6) & notHGFile) attacks |= (bitboard << 6);

  // return attack map
  return attacks;
}

// generate king attacks
Bitboard maskKingAttacks(Square square) {
  // result attacks bitboard
  Bitboard attacks = 0ULL;

  // piece bitboard
  Bitboard bitboard = 0ULL;

  // set piece on board
  setBit(bitboard, square);

  // generate king attacks
  if (bitboard >> 8) attacks |= (bitboard >> 8);
  if ((bitboard >> 9) & notHFile) attacks |= (bitboard >> 9);
  if ((bitboard >> 7) & notAFile) attacks |= (bitboard >> 7);
  if ((bitboard >> 1) & notHFile) attacks |= (bitboard >> 1);
  if (bitboard << 8) attacks |= (bitboard << 8);
  if ((bitboard << 9) & notAFile) attacks |= (bitboard << 9);
  if ((bitboard << 7) & notHFile) attacks |= (bitboard << 7);
  if ((bitboard << 1) & notAFile) attacks |= (bitboard << 1);

  // return attack map
  return attacks;
}

// mask bishop attacks
Bitboard maskBishopAttacks(Square square) {
  // result attacks bitboard
  Bitboard attacks = 0ULL;

  // init ranks & files
  int r, f;

  // init target rank & files
  int tr = square / 8;
  int tf = square % 8;

  // mask relevant bishop occupancy bits
  for (r = tr + 1, f = tf + 1; r <= 6 && f <= 6; r++, f++) attacks |= (1ULL << (r * 8 + f));
  for (r = tr - 1, f = tf + 1; r >= 1 && f <= 6; r--, f++) attacks |= (1ULL << (r * 8 + f));
  for (r = tr + 1, f = tf - 1; r <= 6 && f >= 1; r++, f--) attacks |= (1ULL << (r * 8 + f));
  for (r = tr - 1, f = tf - 1; r >= 1 && f >= 1; r--, f--) attacks |= (1ULL << (r * 8 + f));

  // return attack map
  return attacks;
}

// mask rook attacks
Bitboard maskRookAttacks(Square square) {
  // result attacks bitboard
  Bitboard attacks = 0ULL;

  // init ranks & files
  int r, f;

  // init target rank & files
  int tr = square / 8;
  int tf = square % 8;

  // mask relevant rook occupancy bits
  for (r = tr + 1; r <= 6; r++) attacks |= (1ULL << (r * 8 + tf));
  for (r = tr - 1; r >= 1; r--) attacks |= (1ULL << (r * 8 + tf));
  for (f = tf + 1; f <= 6; f++) attacks |= (1ULL << (tr * 8 + f));
  for (f = tf - 1; f >= 1; f--) attacks |= (1ULL << (tr * 8 + f));

  // return attack map
  return attacks;
}

// generate bishop attacks on the fly
Bitboard bishopAttacksNaive(Square square, Bitboard block) {
  // result attacks bitboard
  Bitboard attacks = 0ULL;

  // init ranks & files
  int r, f;

  // init target rank & files
  int tr = square / 8;
  int tf = square % 8;

  // generate bishop atacks
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

  // return attack map
  return attacks;
}

// generate rook attacks on the fly
Bitboard rookAttacksNaive(Square square, Bitboard block) {
  // result attacks bitboard
  Bitboard attacks = 0ULL;

  // init ranks & files
  int r, f;

  // init target rank & files
  int tr = square / 8;
  int tf = square % 8;

  // generate rook attacks
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

  // return attack map
  return attacks;
}

// init leaper pieces attacks
void initLeapersAttacks() {
  // loop over 64 board squares
  iterSquare() {
    // init pawn attacks
    pawnAttacks[White][square] = maskPawnAttacks(White, square);
    pawnAttacks[Black][square] = maskPawnAttacks(Black, square);

    // init knight attacks
    knightAttacks[square] = maskKnightAttacks(square);

    // init king attacks
    kingAttacks[square] = maskKingAttacks(square);
  }
}

// set occupancies
Bitboard setOccupancy(int index, int bitsInMask, Bitboard attackMask) {
  // occupancy map
  Bitboard occupancy = 0ULL;

  // loop over the range of bits within attack mask
  for (int count = 0; count < bitsInMask; count++) {
    // get LS1B index of attacks mask
    int square = LSOneIdx(attackMask);

    // pop LS1B in attack map
    remBit(attackMask, square);

    // make sure occupancy is on board
    if (index & (1 << count))
      // populate occupancy map
      occupancy |= (1ULL << square);
  }

  // return occupancy map
  return occupancy;
}