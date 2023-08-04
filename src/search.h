//
// Created by Jikun on 26/6/23.
//

#ifndef MINERVA_CHESS_SRC_SEARCH_H_
#define MINERVA_CHESS_SRC_SEARCH_H_

#include "types.h"

constexpr int fullDepthMoves = 4;
constexpr int reductionLimit = 3;

inline int scoreMove(int move);

inline int sortMoves(moves &moveList, int bestMove);

inline int isRepetition();

inline int quiescence(int alpha, int beta);

inline int negamax(int alpha, int beta, int depth);

void searchPosition(int depth);

#endif //MINERVA_CHESS_SRC_SEARCH_H_
