//
// Created by Jikun on 25/6/23.
//

#include "zobrist.h"
#include "variables.h"

U64 pieceKey[PieceCount][SquareCount];
U64 enpassantKey[SquareCount];
U64 castlingKey[16];
U64 sideKey;

void initialiseKeys() {
  iterPiece() {
    iterSquare() {
      pieceKey[piece][square] = rand64();
    }
  }

  iterSquare() {
    enpassantKey[square] = rand64();
  }

  iterCastle() {
    castlingKey[castle] = rand64();
  }

  sideKey = rand64();
}

U64 generateHashkey() {
  U64 finalKey = 0ULL;
  Bitboard bitboard;

  iterPiece() {
    bitboard = board.bitboards[piece];
    while (bitboard) {
      int square = LSOneIdx(bitboard);
      finalKey ^= pieceKey[piece][square];
      remBit(bitboard, square);
    }
  }

  if (board.enpassant != no_sq) {
    finalKey ^= enpassantKey[board.enpassant];
  }
  finalKey ^= castlingKey[board.castle];
  if (board.side == Black) {
    finalKey ^= sideKey;
  }

  return finalKey;
}