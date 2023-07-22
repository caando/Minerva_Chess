//
// Created by Jikun on 25/6/23.
//

#include "magic.h"
#include "magic_constants.h"
#include "attack.h"
#include "util.h"
#include <bit>
#include <cstring>
#include <iostream>

U64 generateMagicNumber() {
  return rand64() & rand64() & rand64();
}

U64 findMagicNumber(Square square, int relevantBits, BishopRook isBishop) {
  // init occupancies
  Bitboard occupancies[4096];

  // init attack tables
  Bitboard attacks[4096];

  // init used attacks
  Bitboard usedAttacks[4096];

  // init attack mask for a current piece
  Bitboard attackMask = isBishop == Bishop ? maskBishopAttacks(square) : maskRookAttacks(square);

  // init occupancy indices
  int occupancyIndices = 1 << relevantBits;

  // loop over occupancy indices
  for (int index = 0; index < occupancyIndices; index++) {
    // init occupancies
    occupancies[index] = setOccupancy(index, relevantBits, attackMask);

    // init attacks
    attacks[index] = isBishop == Bishop ? bishopAttacksNaive(square, occupancies[index]) :
                     rookAttacksNaive(square, occupancies[index]);
  }

  // test magic numbers loop
  for (int random_count = 0; random_count < 100000000; random_count++) {
    // generate magic number candidate
    U64 magic_number = generateMagicNumber();

    // skip inappropriate magic numbers
    if (std::popcount((attackMask * magic_number) & 0xFF00000000000000) < 6) continue;

    // init used attacks
    memset(usedAttacks, 0ULL, sizeof(usedAttacks));

    // init index & fail flag
    int index, fail;

    // test magic index loop
    for (index = 0, fail = 0; !fail && index < occupancyIndices; index++) {
      // init magic index
      int magicIndex = (int) ((occupancies[index] * magic_number) >> (64 - relevantBits));

      // if magic index works
      if (usedAttacks[magicIndex] == 0ULL)
        // init used attacks
        usedAttacks[magicIndex] = attacks[index];

        // otherwise
      else if (usedAttacks[magicIndex] != attacks[index])
        // magic index doesn't work
        fail = 1;
    }

    // if magic number works
    if (!fail)
      // return it
      return magic_number;
  }

  // if magic number doesn't work
  printf("  Magic number fails!\n");
  return 0ULL;
}

// init magic numbers
//void initMagicNumbers() {
//  // loop over 64 board squares
//  iterSquare()
//    // init rook magic numbers
//    rookMagicNumbers[square] = findMagicNumber(static_cast<Square>(square), rookRelevantBits[square], Rook);
//
//  // loop over 64 board squares
//  iterSquare()
//    // init bishop magic numbers
//    bishopMagicNumbers[square] = findMagicNumber(static_cast<Square>(square), bishopRelevantBits[square], Bishop);
//}
// Deprecated in order to constexpr magic numbers

// init slider piece's attack tables
void initSlidersAttacks(BishopRook isBishop) {
  // loop over 64 board squares
  for (int square = 0; square < 64; square++) {
    // init bishop & rook masks
    bishopMasks[square] = maskBishopAttacks(static_cast<Square>(square));
    rookMasks[square] = maskRookAttacks(static_cast<Square>(square));

    // init current mask
    Bitboard attack_mask = isBishop == Bishop ? bishopMasks[square] : rookMasks[square];

    // init relevant occupancy bit count
    int relevantBitsCount = std::popcount(attack_mask);

    // init occupancy indices
    int occupancyIndices = (1 << relevantBitsCount);

    // loop over occupancy indices
    for (int index = 0; index < occupancyIndices; index++) {
      // bishop
      if (isBishop) {
        // init current occupancy variation
        U64 occupancy = setOccupancy(index, relevantBitsCount, attack_mask);

        // init magic index
        int magic_index = (occupancy * bishopMagicNumbers[square]) >> (64 - bishopRelevantBits[square]);

        // init bishop attacks
        bishopAttacks[square][magic_index] = bishopAttacksNaive(static_cast<Square>(square), occupancy);
      }

        // rook
      else {
        // init current occupancy variation
        U64 occupancy = setOccupancy(index, relevantBitsCount, attack_mask);

        // init magic index
        int magic_index = (occupancy * rookMagicNumbers[square]) >> (64 - rookRelevantBits[square]);

        // init rook attacks
        rookAttacks[square][magic_index] = rookAttacksNaive(static_cast<Square>(square), occupancy);

      }
    }
  }
}

// get bishop attacks
Bitboard getBishopAttacks(Square square, Bitboard occupancy) {
  // get bishop attacks assuming current board occupancy
  occupancy &= bishopMasks[square];
  occupancy *= bishopMagicNumbers[square];
  occupancy >>= 64 - bishopRelevantBits[square];

  // return bishop attacks
  return bishopAttacks[square][occupancy];
}

// get rook attacks
Bitboard getRookAttacks(Square square, Bitboard occupancy) {
  // get rook attacks assuming current board occupancy
  occupancy &= rookMasks[square];
  occupancy *= rookMagicNumbers[square];
  occupancy >>= 64 - rookRelevantBits[square];

  // return rook attacks
  return rookAttacks[square][occupancy];
}

// get queen attacks
Bitboard getQueenAttacks(Square square, Bitboard occupancy) {
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