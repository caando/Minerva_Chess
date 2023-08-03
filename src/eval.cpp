//
// Created by Jikun on 26/6/23.
//

#include "eval.h"

int evaluate() {
  Bitboard bitboard;
  Piece piece;
  Square square;
  int pieces[33];
  int squares[33];
  int index = 2;

  for (int bb_piece = WPawn; bb_piece <= BKing; bb_piece++) {
    bitboard = board.bitboards[bb_piece];

    while (bitboard) {
      piece = static_cast<Piece>(bb_piece);
      square = static_cast<Square>(LSOneIdx(bitboard));

      if (piece == WKing) {
        pieces[0] = nnuePieces[piece];
        squares[0] = nnueSquares[square];
      } else if (piece == BKing) {
        pieces[1] = nnuePieces[piece];
        squares[1] = nnueSquares[square];
      } else {
        pieces[index] = nnuePieces[piece];
        squares[index] = nnueSquares[square];
        index++;
      }

      remBit(bitboard, square);
    }
  }

  pieces[index] = 0;
  squares[index] = 0;
  return (nnue_evaluate(board.side, pieces, squares) * (100 - board.fifty) / 100);
}
