//
// Created by Jikun on 25/6/23.
//

#include "zobrist.h"
#include "variables.h"

// random piece keys [piece][square]
U64 pieceKey[PieceCount][SquareCount] = {0};

// random enpassant keys [square]
U64 enpassantKey[SquareCount] = {0};

// random castling keys [castling state]
U64 castlingKey[16] = {0};

U64 sideKey = 0;

void initialiseKeys() {

  // loop over piece codes
  iterPiece() {
    // loop over board squares
    iterSquare() {
      // init random piece keys
      pieceKey[piece][square] = rand64();
    }
  }

  // loop over board squares
  iterSquare() {
    // init random enpassant keys
    enpassantKey[square] = rand64();
  }

  // loop over castling keys
  iterCastle() {
    // init castling keys
    castlingKey[castle] = rand64();
  }

  // init random side key
  sideKey = rand64();
}

U64 generateHashkey() {
  // final hash key
  U64 final_key = 0ULL;

  // temp piece bitboard copy
  Bitboard bitboard;

  // loop over piece bitboards
  iterPiece() {
    // init piece bitboard copy
    bitboard = bitboards[piece];

    // loop over the pieces within a bitboard
    while (bitboard) {
      // init square occupied by the piece
      int square = LSOneIdx(bitboard);

      // hash piece
      final_key ^= pieceKey[piece][square];

      // pop LS1B
      remBit(bitboard, square);
    }
  }

  // if enpassant square is on board
  if (enpassant != no_sq)
    // hash enpassant
    final_key ^= enpassantKey[enpassant];

  // hash castling rights
  final_key ^= castlingKey[castle];

  // hash the side only if black is to move
  if (side == Black) final_key ^= sideKey;

  // return generated hash key
  return final_key;
}