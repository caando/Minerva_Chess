//
// Created by Jikun on 25/6/23.
//

#ifndef MINERVA_CHESS_SRC_MAGIC_CONSTANTS_H_
#define MINERVA_CHESS_SRC_MAGIC_CONSTANTS_H_

#include "types.h"

// credits to BITBOARD CHESS ENGINE

// A and H files affect the attack pattern of pawns and knights since it cannot attack out of board
extern const U64 notAFile;
extern const U64 notHFile;

// AB and HG files affect the attack pattern of knights since it cannot attack out of board
extern const U64 notHGFile;
extern const U64 notABFile;

// rook magic numbers
extern U64 rookMagicNumbers[SquareCount];

// bishop magic numbers
extern U64 bishopMagicNumbers[SquareCount];

// bishop relevant occupancy bit count for every square on board
extern const int bishopRelevantBits[SquareCount];

// rook relevant occupancy bit count for every square on board
extern const int rookRelevantBits[SquareCount];

// castling rights update constants
extern const int castlingRights[SquareCount];

#endif //MINERVA_CHESS_SRC_MAGIC_CONSTANTS_H_
