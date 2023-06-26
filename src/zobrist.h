//
// Created by Jikun on 25/6/23.
//

#ifndef MINERVA_CHESS_SRC_ZOBRIST_H_
#define MINERVA_CHESS_SRC_ZOBRIST_H_

#include "types.h"
#include "util.h"

// random piece keys [piece][square]
static U64 pieceKey[PieceCount][SquareCount];

// random enpassant keys [square]
static U64 enpassantKey[SquareCount];

// random castling keys [castling state]
static U64 castlingKey[16];

static U64 sideKey;

void initialiseKeys();

U64 generateHashkey();

#endif //MINERVA_CHESS_SRC_ZOBRIST_H_
