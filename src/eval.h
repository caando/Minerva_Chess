//
// Created by Jikun on 26/6/23.
//

#ifndef MINERVA_CHESS_SRC_EVAL_H_
#define MINERVA_CHESS_SRC_EVAL_H_

#include "types.h"

// convert BBC piece code to Stockfish piece codes
extern const int nnuePieces[12];

// convert BBC square indices to Stockfish indices
extern const int nnueSquares[64];

int evaluate();

#endif //MINERVA_CHESS_SRC_EVAL_H_
