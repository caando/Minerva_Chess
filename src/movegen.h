//
// Created by Jikun on 26/6/23.
//

#ifndef MINERVA_CHESS_SRC_MOVEGEN_H_
#define MINERVA_CHESS_SRC_MOVEGEN_H_

#include "types.h"

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

static inline bool isSquareAttacked(Square square, Colour side);

// move list structure
typedef struct {
  // moves
  int moves[256];

  // move count
  int count;
} moves;

static inline void addMove(moves * move_list, int move);

static inline int makeMove(int move, int move_flag);

static inline void generateMoves(moves *moveList);

#endif //MINERVA_CHESS_SRC_MOVEGEN_H_
