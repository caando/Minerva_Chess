//
// Created by Jikun on 26/6/23.
//

#ifndef MINERVA_CHESS_SRC_SEARCH_H_
#define MINERVA_CHESS_SRC_SEARCH_H_

#include "types.h"

// full depth moves counter
constexpr int full_depth_moves = 4;

// depth limit to consider reduction
constexpr int reduction_limit = 3;

static inline int scoreMove(int move);

static inline int sortMoves(moves *move_list, int best_move);

static inline int isRepetition();

static inline int quiescence(int alpha, int beta);

static inline int negamax(int alpha, int beta, int depth);

void searchPosition(int depth);

#endif //MINERVA_CHESS_SRC_SEARCH_H_
