//
// Created by Jikun on 25/6/23.
//

#ifndef MINERVA_CHESS_SRC_ZOBRIST_H_
#define MINERVA_CHESS_SRC_ZOBRIST_H_

#include "types.h"
#include "util.h"

// random piece keys [piece][square]
U64 PieceKey[PieceCount][SquareCount];

// random enpassant keys [square]
U64 EnpassantKey[SquareCount];

// random castling keys [castling state]
U64 CastlingKey[16];

U64 SideKey;

void InitialiseKeys();

U64 GenerateHashkey();

#endif //MINERVA_CHESS_SRC_ZOBRIST_H_
