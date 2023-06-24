//
// Created by Jikun on 25/6/23.
//

#include "zobrist.h"
#include "variables.h"

void InitialiseKeys() {

  // loop over piece codes
  iterPiece() {
    // loop over board squares
    iterSquare() {
      // init random piece keys
      PieceKey[piece][square] = rand64();
    }
  }

  // loop over board squares
  iterSquare() {
    // init random enpassant keys
    EnpassantKey[square] = rand64();
  }

  // loop over castling keys
  iterCastle() {
    // init castling keys
    CastlingKey[castle] = rand64();
  }

  // init random side key
  SideKey = rand64();
}

U64 GenerateHashkey() {
  // final hash key
  U64 final_key = 0ULL;

  // temp piece bitboard copy
  U64 bitboard;

  // loop over piece bitboards
  iterPiece() {
    // init piece bitboard copy
    bitboard = bitboards[piece];

    // loop over the pieces within a bitboard
    while (bitboard) {
      // init square occupied by the piece
      int square = LSOneIdx(bitboard);

      // hash piece
      final_key ^= PieceKey[piece][square];

      // pop LS1B
      remBit(bitboard, square);
    }
  }

  // if enpassant square is on board
  if (enpassant != no_sq)
    // hash enpassant
    final_key ^= EnpassantKey[enpassant];

  // hash castling rights
  final_key ^= CastlingKey[castle];

  // hash the side only if black is to move
  if (side == Black) final_key ^= SideKey;

  // return generated hash key
  return final_key;
}