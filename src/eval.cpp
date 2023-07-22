//
// Created by Jikun on 26/6/23.
//

#include "eval.h"
#include "variables.h"
#include "util.h"
#include "./nnue/nnue.h"

// position evaluation
int evaluate() {
  // current pieces bitboard copy
  Bitboard bitboard;

  // init piece & square
  Piece piece;
  Square square;

  // array of piece codes converted to Stockfish piece codes
  int pieces[33];

  // array of square indices converted to Stockfish square indices
  int squares[33];

  // pieces and squares current index to write next piece square pair at
  int index = 2;

  // loop over piece bitboards
  for (int bb_piece = WPawn; bb_piece <= BKing; bb_piece++)
  {
    // init piece bitboard copy
    bitboard = bitboards[bb_piece];

    // loop over pieces within a bitboard
    while (bitboard)
    {
      // init piece
      piece = static_cast<Piece>(bb_piece);

      // init square
      square = static_cast<Square>(LSOneIdx(bitboard));

      /*
          Code to initialize pieces and squares arrays
          to serve the purpose of direct probing of NNUE
      */

      // case white king
      if (piece == WKing)
      {
        /* convert white king piece code to stockfish piece code and
           store it at the first index of pieces array
        */
        pieces[0] = nnuePieces[piece];

        /* convert white king square index to stockfish square index and
           store it at the first index of pieces array
        */
        squares[0] = nnueSquares[square];
      }

        // case black king
      else if (piece == BKing)
      {
        /* convert black king piece code to stockfish piece code and
           store it at the second index of pieces array
        */
        pieces[1] = nnuePieces[piece];

        /* convert black king square index to stockfish square index and
           store it at the second index of pieces array
        */
        squares[1] = nnueSquares[square];
      }

        // all the rest pieces
      else
      {
        /*  convert all the rest of piece code with corresponding square codes
            to stockfish piece codes and square indices respectively
        */
        pieces[index] = nnuePieces[piece];
        squares[index] = nnueSquares[square];
        index++;
      }

      // pop ls1b
      remBit(bitboard, square);
    }
  }

  // set zero terminating characters at the end of pieces & squares arrays
  pieces[index] = 0;
  squares[index] = 0;

  /*
      We need to make sure that fifty rule move counter gives a penalty
      to the evaluation, otherwise it won't be capable of mating in
      simple endgames like KQK or KRK! This expression is used:
                      nnue_score * (100 - fifty) / 100
  */

  // get NNUE score (final score! No need to adjust by the side!)
  return (nnue_evaluate(side, pieces, squares) * (100 - fifty) / 100);
}