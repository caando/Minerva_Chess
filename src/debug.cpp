//
// Created by Jikun on 28/6/23.
//

#ifndef MINERVA_CHESS_SRC_DEBUG_H_
#define MINERVA_CHESS_SRC_DEBUG_H_

#include "debug.h"
#include "types.h"
#include "movegen.h"
#include "variables.h"

void printAttackedSquares(Colour side) {
  printf("\n");
  for (int rank = 0; rank < 8; rank++) {
    for (int file = 0; file < 8; file++) {
      int square = rank * 8 + file;
      if (!file) {
        printf("  %d ", 8 - rank);
      }
      printf(" %d", isSquareAttacked(static_cast<Square>(square), side) ? Black : White);
    }
    printf("\n");
  }
  printf("\n     a b c d e f g h\n\n");
}

void printMoveList(moves &move_list) {
  if (move_list.empty()) {
    printf("\n     No move in the move list!\n");
    return;
  }
  printf("\n     move    piece     capture   double    enpass    castling\n\n");
  for (int move : move_list) {

#ifdef WIN64
    printf("      %s%s%c   %c         %d         %d         %d         %d\n", square_to_coordinates[get_move_source(move)],
                                                                          square_to_coordinates[get_move_target(move)],
                                                                          get_move_promoted(move) ? promoted_pieces[get_move_promoted(move)] : ' ',
                                                                          ascii_pieces[get_move_piece(move)],
                                                                          get_move_capture(move) ? 1 : 0,
                                                                          get_move_double(move) ? 1 : 0,
                                                                          get_move_enpassant(move) ? 1 : 0,
                                                                          get_move_castling(move) ? 1 : 0);
#else
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
  printf("\n\n     Total number of moves: %zu\n\n", move_list.size());

}

void printBoard() {
  printf("\n");
  for (int rank = 0; rank < 8; rank++) {
    for (int file = 0; file < 8; file++) {
      int square = rank * 8 + file;
      if (!file) {
        printf("  %d ", 8 - rank);
      }
      int piece = -1;

      for (int bbPiece = WPawn; bbPiece <= BKing; bbPiece++) {
        if (getBit(board.bitboards[bbPiece], square)) {
          piece = bbPiece;
        }
      }
#ifdef WIN64
      printf(" %c", (piece == -1) ? '.' : ascii_pieces[piece]);
#else
      printf(" %s", (piece == -1) ? "." : piecesToUnicode[piece]);
#endif
    }
    printf("\n");
  }
  printf("\n     a b c d e f g h\n\n");
  printf("     Side:     %s\n", !board.side ? "white" : "black");
  printf("     Enpassant:   %s\n", (board.enpassant != no_sq) ? squareToCoordinates[board.enpassant] : "no");
  printf("     Castling:  %c%c%c%c\n\n", (board.castle & WhiteQueenSide) ? 'K' : '-',
         (board.castle & WhiteQueenSide) ? 'Q' : '-',
         (board.castle & BlackKingSide) ? 'k' : '-',
         (board.castle & BlackQueenSide) ? 'q' : '-');
  printf("     Hash key:  %llx\n", board.hashKey);
  printf("     Fifty move: %d\n\n", board.fifty);
}

#endif //MINERVA_CHESS_SRC_DEBUG_H_
