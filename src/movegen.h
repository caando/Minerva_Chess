//
// Created by Jikun on 26/6/23.
//

#ifndef MINERVA_CHESS_SRC_MOVEGEN_H_
#define MINERVA_CHESS_SRC_MOVEGEN_H_

#include "types.h"
#include <cstring>
#include <iostream>
#include "attack.h"
#include "magic.h"
#include "variables.h"
#include "zobrist.h"
#include "debug.h"

#define encodeMove(source, target, piece, promoted, capture, _double, enpassant, castling) \
    ((source) |          \
    ((target) << 6) |     \
    ((piece) << 12) |     \
    ((promoted) << 16) |  \
    ((capture) << 20) |   \
    ((_double) << 21) |    \
    ((enpassant) << 22) | \
    ((castling) << 23))    \

#define getMoveSource(move) ((move) & 0b111111)

#define getMoveTarget(move) (((move) & 0b111111000000) >> 6)

#define getMovePiece(move) (((move) & 0b1111000000000000) >> 12)

#define getMovePromoted(move) (((move) & 0b11110000000000000000) >> 16)

#define getMoveCapture(move) ((move) & 0b100000000000000000000)

#define getMoveDouble(move) ((move) & 0b1000000000000000000000)

#define getMoveEnpassant(move) ((move) & 0b10000000000000000000000)

#define getMoveCastling(move) ((move) & 0b100000000000000000000000)

enum { ALL_MOVES, ONLY_CAPTURES };

void printMove(int move);

bool isSquareAttacked(Square square, Colour side);

void addMove(moves &moveList, int move);

int makeMove(int move, int moveFlag);

void generateMoves(moves &moveList);

#endif //MINERVA_CHESS_SRC_MOVEGEN_H_
