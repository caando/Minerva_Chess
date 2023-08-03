//
// Created by Jikun on 25/6/23.
//

#ifndef MINERVA_CHESS_SRC_ZOBRIST_H_
#define MINERVA_CHESS_SRC_ZOBRIST_H_

#include "types.h"
#include "util.h"

extern U64 pieceKey[PieceCount][SquareCount];
extern U64 enpassantKey[SquareCount];
extern U64 castlingKey[16];
extern U64 sideKey;

void initialiseKeys();

U64 generateHashkey();

#endif //MINERVA_CHESS_SRC_ZOBRIST_H_
