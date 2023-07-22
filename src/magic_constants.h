//
// Created by Jikun on 25/6/23.
//

#ifndef MINERVA_CHESS_SRC_MAGIC_CONSTANTS_H_
#define MINERVA_CHESS_SRC_MAGIC_CONSTANTS_H_

#include "types.h"

// credits to BITBOARD CHESS ENGINE

// A and H files affect the attack pattern of pawns and knights since it cannot attack out of board
constexpr U64 notAFile = 18374403900871474942ULL;
constexpr U64 notHFile = 9187201950435737471ULL;

// AB and HG files affect the attack pattern of knights since it cannot attack out of board
constexpr U64 notHGFile = 4557430888798830399ULL;
constexpr U64 notABFile = 18229723555195321596ULL;

// rook magic numbers
extern U64 rookMagicNumbers[SquareCount];

// bishop magic numbers
extern U64 bishopMagicNumbers[SquareCount];

// bishop relevant occupancy bit count for every square on board
constexpr int bishopRelevantBits[SquareCount] = {
6, 5, 5, 5, 5, 5, 5, 6,
5, 5, 5, 5, 5, 5, 5, 5,
5, 5, 7, 7, 7, 7, 5, 5,
5, 5, 7, 9, 9, 7, 5, 5,
5, 5, 7, 9, 9, 7, 5, 5,
5, 5, 7, 7, 7, 7, 5, 5,
5, 5, 5, 5, 5, 5, 5, 5,
6, 5, 5, 5, 5, 5, 5, 6
};

// rook relevant occupancy bit count for every square on board
constexpr int rookRelevantBits[SquareCount] = {
    12, 11, 11, 11, 11, 11, 11, 12,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    12, 11, 11, 11, 11, 11, 11, 12
};

// castling rights update constants
constexpr int castlingRights[SquareCount] = {
    7, 15, 15, 15, 3, 15, 15, 11,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    13, 15, 15, 15, 12, 15, 15, 14
};

#endif //MINERVA_CHESS_SRC_MAGIC_CONSTANTS_H_
