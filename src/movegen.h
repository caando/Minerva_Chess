//
// Created by Jikun on 26/6/23.
//

#ifndef MINERVA_CHESS_SRC_MOVEGEN_H_
#define MINERVA_CHESS_SRC_MOVEGEN_H_

#include "types.h"
#include <cstring>
#include <iostream>

// encode move
#define encodeMove(source, target, piece, promoted, capture, double, enpassant, castling) \
    (source) |          \
    (target << 6) |     \
    (piece << 12) |     \
    (promoted << 16) |  \
    (capture << 20) |   \
    (double << 21) |    \
    (enpassant << 22) | \
    (castling << 23)    \

// preserve board state
#define copyBoard()                                                      \
    Bitboard bitboards_copy[12], occupancies_copy[3];                     \
    Colour side_copy;                                                     \
    Square enpassant_copy;                                                \
    int castle_copy, fifty_copy;                                          \
    memcpy(bitboards_copy, bitboards, 96);                                \
    memcpy(occupancies_copy, occupancies, 24);                            \
    side_copy = side, enpassant_copy = enpassant, castle_copy = castle;   \
    fifty_copy = fifty;                                                   \
    U64 hashKey_copy = hashKey;                                         \

// restore board state
#define takeBack()                                                       \
    memcpy(bitboards, bitboards_copy, 96);                                \
    memcpy(occupancies, occupancies_copy, 24);                            \
    side = side_copy, enpassant = enpassant_copy, castle = castle_copy;   \
    fifty = fifty_copy;                                                   \
    hashKey = hashKey_copy;                                              \

// extract source square
#define getMoveSource(move) (move & 0x3f)

// extract target square
#define getMoveTarget(move) ((move & 0xfc0) >> 6)

// extract piece
#define getMovePiece(move) ((move & 0xf000) >> 12)

// extract promoted piece
#define getMovePromoted(move) ((move & 0xf0000) >> 16)

// extract capture flag
#define getMoveCapture(move) (move & 0x100000)

// extract double pawn push flag
#define getMoveDouble(move) (move & 0x200000)

// extract enpassant flag
#define getMoveEnpassant(move) (move & 0x400000)

// extract castling flag
#define getMoveCastling(move) (move & 0x800000)

// move types
enum { all_moves, only_captures };

bool isSquareAttacked(Square square, Colour side);

inline void addMove(moves *move_list, int move);

int makeMove(int move, int moveFlag);

void generateMoves(moves *moveList);

// print move (for UCI purposes)
void printMove(int move);

#endif //MINERVA_CHESS_SRC_MOVEGEN_H_
