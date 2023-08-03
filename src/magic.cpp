//
// Created by Jikun on 25/6/23.
//

#include "magic.h"
#include "magic_constants.h"
#include "attack.h"
#include "util.h"
#include <bit>

void initLeapersAttacks() {
  iterSquare() {
    pawnAttacks[White][square] = maskPawnAttacks(White, static_cast<Square>(square));
    pawnAttacks[Black][square] = maskPawnAttacks(Black, static_cast<Square>(square));

    knightAttacks[square] = maskKnightAttacks(static_cast<Square>(square));

    kingAttacks[square] = maskKingAttacks(static_cast<Square>(square));
  }
}

void initSlidersAttacks(BishopRook isBishop) {
  for (int square = 0; square < 64; square++) {
    bishopMasks[square] = maskBishopAttacks(static_cast<Square>(square));
    rookMasks[square] = maskRookAttacks(static_cast<Square>(square));

    Bitboard attack_mask = isBishop == Bishop ? bishopMasks[square] : rookMasks[square];

    int relevantBitsCount = std::popcount(attack_mask);

    int occupancyIndices = (1 << relevantBitsCount);

    for (int index = 0; index < occupancyIndices; index++) {
      if (isBishop) {
        U64 occupancy = setOccupancy(index, relevantBitsCount, attack_mask);
        int magic_index = (occupancy * bishopMagicNumbers[square]) >> (64 - bishopRelevantBits[square]);
        bishopAttacks[square][magic_index] = bishopAttacksNaive(static_cast<Square>(square), occupancy);
      } else {
        U64 occupancy = setOccupancy(index, relevantBitsCount, attack_mask);
        int magic_index = (occupancy * rookMagicNumbers[square]) >> (64 - rookRelevantBits[square]);
        rookAttacks[square][magic_index] = rookAttacksNaive(static_cast<Square>(square), occupancy);
      }
    }
  }
}

Bitboard getBishopAttacks(Square square, Bitboard occupancy) {
  occupancy &= bishopMasks[square];
  occupancy *= bishopMagicNumbers[square];
  occupancy >>= 64 - bishopRelevantBits[square];

  return bishopAttacks[square][occupancy];
}

Bitboard getRookAttacks(Square square, Bitboard occupancy) {
  occupancy &= rookMasks[square];
  occupancy *= rookMagicNumbers[square];
  occupancy >>= 64 - rookRelevantBits[square];

  return rookAttacks[square][occupancy];
}

Bitboard getQueenAttacks(Square square, Bitboard occupancy) {
  U64 queen_attacks = 0ULL;
  U64 bishop_occupancy = occupancy;
  U64 rook_occupancy = occupancy;

  bishop_occupancy &= bishopMasks[square];
  bishop_occupancy *= bishopMagicNumbers[square];
  bishop_occupancy >>= 64 - bishopRelevantBits[square];

  queen_attacks = bishopAttacks[square][bishop_occupancy];

  rook_occupancy &= rookMasks[square];
  rook_occupancy *= rookMagicNumbers[square];
  rook_occupancy >>= 64 - rookRelevantBits[square];

  queen_attacks |= rookAttacks[square][rook_occupancy];
  return queen_attacks;
}