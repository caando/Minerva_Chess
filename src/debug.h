//
// Created by Jikun on 28/6/23.
//

#ifndef MINERVA_CHESS_SRC_DEBUG_H_
#define MINERVA_CHESS_SRC_DEBUG_H_

#include "types.h"

// print attacked squares
void print_attacked_squares(Colour side);

// print move list
void print_move_list(moves &move_list);

// print board
void print_board();

#endif //MINERVA_CHESS_SRC_DEBUG_H_
