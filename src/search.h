//
// Created by Jikun on 26/6/23.
//

#ifndef MINERVA_CHESS_SRC_SEARCH_H_
#define MINERVA_CHESS_SRC_SEARCH_H_

#include "types.h"

// full depth moves counter
static constexpr int full_depth_moves = 4;

// depth limit to consider reduction
static constexpr int reduction_limit = 3;

inline int scoreMove(int move);

inline int sortMoves(moves *move_list, int best_move);

inline int isRepetition();

inline int quiescence(int alpha, int beta);

inline int negamax(int alpha, int beta, int depth);

void searchPosition(int depth);

#endif //MINERVA_CHESS_SRC_SEARCH_H_
