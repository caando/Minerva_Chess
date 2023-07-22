//
// Created by Jikun on 28/6/23.
//

#ifndef MINERVA_CHESS_SRC_DEBUG_H_
#define MINERVA_CHESS_SRC_DEBUG_H_

#include "debug.h"
#include "types.h"
#include "movegen.h"
#include "variables.h"

// print attacked squares
void print_attacked_squares(Colour side) {
  printf("\n");

// loop over board ranks
  for (int rank = 0; rank < 8; rank++) {
// loop over board files
    for (int file = 0; file < 8; file++) {
// init square
      int square = rank * 8 + file;

// print ranks
      if (!file)
        printf("  %d ", 8 - rank);

// check whether current square is attacked or not
      printf(" %d", isSquareAttacked(static_cast<Square>(square), side) ? Black : White);
    }

// print new line every rank
    printf("\n");
  }

// print files
  printf("\n     a b c d e f g h\n\n");
}

// print move list
void print_move_list(moves move_list) {
  // do nothing on empty move list
  if (!move_list.size()) {
    printf("\n     No move in the move list!\n");
    return;
  }

  printf("\n     move    piece     capture   double    enpass    castling\n\n");

  // loop over moves within a move list
  for (int move_count = 0; move_count < move_list.size(); move_count++) {
    // init move
    int move = move_list[move_count];

#ifdef WIN64
    // print move
            printf("      %s%s%c   %c         %d         %d         %d         %d\n", square_to_coordinates[get_move_source(move)],
                                                                                  square_to_coordinates[get_move_target(move)],
                                                                                  get_move_promoted(move) ? promoted_pieces[get_move_promoted(move)] : ' ',
                                                                                  ascii_pieces[get_move_piece(move)],
                                                                                  get_move_capture(move) ? 1 : 0,
                                                                                  get_move_double(move) ? 1 : 0,
                                                                                  get_move_enpassant(move) ? 1 : 0,
                                                                                  get_move_castling(move) ? 1 : 0);
#else
    // print move
    printf("     %s%s%c   %s         %d         %d         %d         %d\n", squareToCoordinates[getMoveSource(move)],
           squareToCoordinates[getMoveTarget(move)],
           getMovePromoted(move) ? promotedPieces[getMovePromoted(move)] : ' ',
           piecesToUnicode[getMovePiece(move)],
           getMoveCapture(move) ? 1 : 0,
           getMoveDouble(move) ? 1 : 0,
           getMoveEnpassant(move) ? 1 : 0,
           getMoveCastling(move) ? 1 : 0);
#endif

  }

  // print total number of moves
  printf("\n\n     Total number of moves: %d\n\n", move_list.size());

}

// print board
void print_board() {
  // print offset
  printf("\n");

  // loop over board ranks
  for (int rank = 0; rank < 8; rank++) {
    // loop ober board files
    for (int file = 0; file < 8; file++) {
      // init square
      int square = rank * 8 + file;

      // print ranks
      if (!file)
        printf("  %d ", 8 - rank);

      // define piece variable
      int piece = -1;

      // loop over all piece bitboards
      for (int bb_piece = WPawn; bb_piece <= BKing; bb_piece++) {
        // if there is a piece on current square
        if (getBit(bitboards[bb_piece], square))
          // get piece code
          piece = bb_piece;
      }

      // print different piece set depending on OS
#ifdef WIN64
      printf(" %c", (piece == -1) ? '.' : ascii_pieces[piece]);
#else
      printf(" %s", (piece == -1) ? "." : piecesToUnicode[piece]);
#endif
    }

    // print new line every rank
    printf("\n");
  }

  // print board files
  printf("\n     a b c d e f g h\n\n");

  // print side to move
  printf("     Side:     %s\n", !side ? "white" : "black");

  // print enpassant square
  printf("     Enpassant:   %s\n", (enpassant != no_sq) ? squareToCoordinates[enpassant] : "no");

  // print castling rights
  printf("     Castling:  %c%c%c%c\n\n", (castle & WhiteQueenSide) ? 'K' : '-',
         (castle & WhiteQueenSide) ? 'Q' : '-',
         (castle & BlackKingSide) ? 'k' : '-',
         (castle & BlackQueenSide) ? 'q' : '-');

  // print hash key
  printf("     Hash key:  %llx\n", hashKey);

  // fifty move rule counter
  printf("     Fifty move: %d\n\n", fifty);
}



#endif //MINERVA_CHESS_SRC_DEBUG_H_
