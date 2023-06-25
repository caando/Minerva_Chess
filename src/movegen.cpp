//
// Created by Jikun on 26/6/23.
//

#include "movegen.h"
#include "attack.h"
#include "magic.h"
#include "variables.h"
#include "zobrist.h"
#include <string>

// is square current given attacked by the current given side
static inline bool isSquareAttacked(Square square, Colour side) {
  // attacked by White pawns
  if ((side == White) && (pawnAttacks[Black][square] & bitboards[WPawn])) return true;

  // attacked by Black pawns
  if ((side == Black) && (pawnAttacks[White][square] & bitboards[BPawn])) return true;

  // attacked by knights
  if (knightAttacks[square] & ((side == White) ? bitboards[WKnight] : bitboards[BKnight])) return true;

  // attacked by bishops
  if (getBishopAttacks(square, occupancies[Both]) & ((side == White) ? bitboards[WBishop] : bitboards[BBishop]))
    return true;

  // attacked by rooks
  if (getRookAttacks(square, occupancies[Both]) & ((side == White) ? bitboards[WRook] : bitboards[BRook])) return true;

  // attacked by bishops
  if (getQueenAttacks(square, occupancies[Both]) & ((side == White) ? bitboards[WQueen] : bitboards[BQueen]))
    return true;

  // attacked by kings
  if (kingAttacks[square] & ((side == White) ? bitboards[WKing] : bitboards[BKing])) return true;

  // by default return false
  return false;
}

// move list structure
typedef struct {
  // moves
  int moves[256];

  // move count
  int count;
} moves;

// add move to the move list
static inline void add_move(moves * move_list, int
move) {
// store move
move_list->moves[move_list->count] =
move;

// increment move count
move_list->count++;
}

// preserve board state
#define copy_board()                                                      \
    Bitboard bitboards_copy[12], occupancies_copy[3];                     \
    Colour side_copy;                                                     \
    Square enpassant_copy;                                                \
    int castle_copy, fifty_copy;                                          \
    memcpy(bitboards_copy, bitboards, 96);                                \
    memcpy(occupancies_copy, occupancies, 24);                            \
    side_copy = side, enpassant_copy = enpassant, castle_copy = castle;   \
    fifty_copy = fifty;                                                   \
    U64 hash_key_copy = hash_key;                                         \

// restore board state
#define take_back()                                                       \
    memcpy(bitboards, bitboards_copy, 96);                                \
    memcpy(occupancies, occupancies_copy, 24);                            \
    side = side_copy, enpassant = enpassant_copy, castle = castle_copy;   \
    fifty = fifty_copy;                                                   \
    hash_key = hash_key_copy;                                             \

// make move on chess board
static inline int make_move(int move, int move_flag) {
  // quiet moves
  if (move_flag == all_moves) {
    // preserve board state
    copy_board();

    // parse move
    int source_square = get_move_source(move);
    int target_square = get_move_target(move);
    int piece = get_move_piece(move);
    int promoted_piece = get_move_promoted(move);
    int capture = get_move_capture(move);
    int double_push = get_move_double(move);
    int enpass = get_move_enpassant(move);
    int castling = get_move_castling(move);

    // move piece
    remBit(bitboards[piece], source_square);
    setBit(bitboards[piece], target_square);

    // hash piece
    hash_key ^= pieceKey[piece][source_square]; // remove piece from source square in hash key
    hash_key ^= pieceKey[piece][target_square]; // set piece to the target square in hash key

    // increment fifty move rule counter
    fifty++;

    // if pawn moved
    if (piece == WPawn || piece == BPawn)
      // reset fifty move rule counter
      fifty = 0;

    // handling capture moves
    if (capture) {
      // reset fifty move rule counter
      fifty = 0;

      // pick up bitboard piece index ranges depending on side
      int start_piece, end_piece;

      // White to move
      if (side == White) {
        start_piece = BPawn;
        end_piece = BKing;
      }

        // Black to move
      else {
        start_piece = WPawn;
        end_piece = WKing;
      }

      // loop over bitboards opposite to the current side to move
      for (int bb_piece = start_piece; bb_piece <= end_piece; bb_piece++) {
        // if there's a piece on the target square
        if (getBit(bitboards[bb_piece], target_square)) {
          // remove it from corresponding bitboard
          remBit(bitboards[bb_piece], target_square);

          // remove the piece from hash key
          hash_key ^= pieceKey[bb_piece][target_square];
          break;
        }
      }
    }

    // handle pawn promotions
    if (promoted_piece) {
      // White to move
      if (side == White) {
        // erase the pawn from the target square
        remBit(bitboards[WPawn], target_square);

        // remove pawn from hash key
        hash_key ^= pieceKey[WPawn][target_square];
      }

        // Black to move
      else {
        // erase the pawn from the target square
        remBit(bitboards[BPawn], target_square);

        // remove pawn from hash key
        hash_key ^= pieceKey[BPawn][target_square];
      }

      // set up promoted piece on chess board
      setBit(bitboards[promoted_piece], target_square);

      // add promoted piece into the hash key
      hash_key ^= pieceKey[promoted_piece][target_square];
    }

    // handle enpassant captures
    if (enpass) {
      // erase the pawn depending on side to move
      (side == White) ? remBit(bitboards[BPawn], target_square + 8) :
      remBit(bitboards[WPawn], target_square - 8);

      // White to move
      if (side == White) {
        // remove captured pawn
        remBit(bitboards[BPawn], target_square + 8);

        // remove pawn from hash key
        hash_key ^= pieceKey[BPawn][target_square + 8];
      }

        // Black to move
      else {
        // remove captured pawn
        remBit(bitboards[WPawn], target_square - 8);

        // remove pawn from hash key
        hash_key ^= pieceKey[WPawn][target_square - 8];
      }
    }

    // hash enpassant if available (remove enpassant square from hash key )
    if (enpassant != no_sq) hash_key ^= enpassantKey[enpassant];

    // reset enpassant square
    enpassant = no_sq;

    // handle double pawn push
    if (double_push) {
      // White to move
      if (side == White) {
        // set enpassant square
        enpassant = static_cast<Square>(target_square + 8);

        // hash enpassant
        hash_key ^= enpassantKey[target_square + 8];
      }

        // Black to move
      else {
        // set enpassant square
        enpassant = static_cast<Square>(target_square - 8);

        // hash enpassant
        hash_key ^= enpassantKey[target_square - 8];
      }
    }

    // handle castling moves
    if (castling) {
      // switch target square
      switch (target_square) {
        // White castles king side
        case (g1):
          // move H rook
          remBit(bitboards[WRook], h1);
          setBit(bitboards[WRook], f1);

          // hash rook
          hash_key ^= pieceKey[WRook][h1];  // remove rook from h1 from hash key
          hash_key ^= pieceKey[WRook][f1];  // put rook on f1 into a hash key
          break;

          // White castles queen side
        case (c1):
          // move A rook
          remBit(bitboards[WRook], a1);
          setBit(bitboards[WRook], d1);

          // hash rook
          hash_key ^= pieceKey[WRook][a1];  // remove rook from a1 from hash key
          hash_key ^= pieceKey[WRook][d1];  // put rook on d1 into a hash key
          break;

          // Black castles king side
        case (g8):
          // move H rook
          remBit(bitboards[BRook], h8);
          setBit(bitboards[BRook], f8);

          // hash rook
          hash_key ^= pieceKey[BRook][h8];  // remove rook from h8 from hash key
          hash_key ^= pieceKey[BRook][f8];  // put rook on f8 into a hash key
          break;

          // Black castles queen side
        case (c8):
          // move A rook
          remBit(bitboards[BRook], a8);
          setBit(bitboards[BRook], d8);

          // hash rook
          hash_key ^= pieceKey[BRook][a8];  // remove rook from a8 from hash key
          hash_key ^= pieceKey[BRook][d8];  // put rook on d8 into a hash key
          break;
      }
    }

    // hash castling
    hash_key ^= castlingKey[castle];

    // update castling rights
    castle &= castlingRights[source_square];
    castle &= castlingRights[target_square];

    // hash castling
    hash_key ^= castlingKey[castle];

    // reset occupancies
    memset(occupancies, 0ULL, 24);

    // loop over White pieces bitboards
    for (int bb_piece = WPawn; bb_piece <= WKing; bb_piece++)
      // update White occupancies
      occupancies[White] |= bitboards[piece];

    // loop over Black pieces bitboards
    for (int bb_piece = BPawn; bb_piece <= BKing; bb_piece++)
      // update Black occupancies
      occupancies[Black] |= bitboards[bb_piece];

    // update Both sides occupancies
    occupancies[Both] |= occupancies[White];
    occupancies[Both] |= occupancies[Black];

    // change side
    if (side == White) {
      side = Black;
    } else {
      side = White;
    }

    // hash side
    hash_key ^= sideKey;

    // make sure that king has not been exposed into a check
    if (isSquareAttacked(static_cast<Square>((side == White) ? LSOneIdx(bitboards[BKing]) : LSOneIdx(bitboards[WKing])),
                         side)) {
      // take move back
      take_back();

      // return illegal move
      return 0;
    }

      // otherwise
    else
      // return legal move
      return 1;

  }

    // capture moves
  else {
    // make sure move is the capture
    if (get_move_capture(move))
      make_move(move, all_moves);

      // otherwise the move is not a capture
    else
      // don't make it
      return 0;
  }
}

// generate all moves
static inline void generate_moves(moves * move_list) {
  // init move count
  move_list->count = 0;

  // define source & target squares
  int source_square, target_square;

  // define current piece's bitboard copy & it's attacks
  U64 bitboard, attacks;

  // loop over all the bitboards
  iterPiece() {
    // init piece bitboard copy
    bitboard = bitboards[piece];

    // generate White pawns & White king castling moves
    if (side == White) {
      // pick up White pawn bitboards index
      if (piece == WPawn) {
        // loop over White pawns within White pawn bitboard
        while (bitboard) {
          // init source square
          source_square = LSOneIdx(bitboard);

          // init target square
          target_square = source_square - 8;

          // generate quiet pawn moves
          if (!(target_square < a8) && !getBit(occupancies[Both], target_square)) {
            // pawn promotion
            if (source_square >= a7 && source_square <= h7) {
              add_move(move_list, encodeMove(source_square, target_square, piece, WQueen, 0, 0, 0, 0));
              add_move(move_list, encodeMove(source_square, target_square, piece, WRook, 0, 0, 0, 0));
              add_move(move_list, encodeMove(source_square, target_square, piece, WBishop, 0, 0, 0, 0));
              add_move(move_list, encodeMove(source_square, target_square, piece, WKnight, 0, 0, 0, 0));
            } else {
              // one square ahead pawn move
              add_move(move_list, encodeMove(source_square, target_square, piece, 0, 0, 0, 0, 0));

              // two squares ahead pawn move
              if ((source_square >= a2 && source_square <= h2) && !getBit(occupancies[Both], target_square - 8))
                add_move(move_list, encodeMove(source_square, target_square - 8, piece, 0, 0, 1, 0, 0));
            }
          }

          // init pawn attacks bitboard
          attacks = pawnAttacks[side][source_square] & occupancies[Black];

          // generate pawn captures
          while (attacks) {
            // init target square
            target_square = LSOneIdx(attacks);

            // pawn promotion
            if (source_square >= a7 && source_square <= h7) {
              add_move(move_list, encodeMove(source_square, target_square, piece, WQueen, 1, 0, 0, 0));
              add_move(move_list, encodeMove(source_square, target_square, piece, WRook, 1, 0, 0, 0));
              add_move(move_list, encodeMove(source_square, target_square, piece, WBishop, 1, 0, 0, 0));
              add_move(move_list, encodeMove(source_square, target_square, piece, WKnight, 1, 0, 0, 0));
            } else
              // one square ahead pawn move
              add_move(move_list, encodeMove(source_square, target_square, piece, 0, 1, 0, 0, 0));

            // pop ls1b of the pawn attacks
            remBit(attacks, target_square);
          }

          // generate enpassant captures
          if (enpassant != no_sq) {
            // lookup pawn attacks and bitwise AND with enpassant square (bit)
            U64 enpassant_attacks = pawnAttacks[side][source_square] & (1ULL << enpassant);

            // make sure enpassant capture available
            if (enpassant_attacks) {
              // init enpassant capture target square
              int target_enpassant = LSOneIdx(enpassant_attacks);
              add_move(move_list, encodeMove(source_square, target_enpassant, piece, 0, 1, 0, 1, 0));
            }
          }

          // pop ls1b from piece bitboard copy
          remBit(bitboard, source_square);
        }
      }

      // castling moves
      if (piece == WKing) {
        // king side castling is available
        if (castle & WhiteKingSide) {
          // make sure square between king and king's rook are empty
          if (!getBit(occupancies[Both], f1) && !getBit(occupancies[Both], g1)) {
            // make sure king and the f1 squares are not under attacks
            if (!isSquareAttacked(e1, Black) && !isSquareAttacked(f1, Black))
              add_move(move_list, encodeMove(e1, g1, piece, 0, 0, 0, 0, 1));
          }
        }

        // queen side castling is available
        if (castle & WhiteQueenSide) {
          // make sure square between king and queen's rook are empty
          if (!getBit(occupancies[Both], d1) && !getBit(occupancies[Both], c1) && !getBit(occupancies[Both], b1)) {
            // make sure king and the d1 squares are not under attacks
            if (!isSquareAttacked(e1, Black) && !isSquareAttacked(d1, Black))
              add_move(move_list, encodeMove(e1, c1, piece, 0, 0, 0, 0, 1));
          }
        }
      }
    }

      // generate Black pawns & Black king castling moves
    else {
      // pick up Black pawn bitboards index
      if (piece == BPawn) {
        // loop over White pawns within White pawn bitboard
        while (bitboard) {
          // init source square
          source_square = LSOneIdx(bitboard);

          // init target square
          target_square = source_square + 8;

          // generate quiet pawn moves
          if (!(target_square > h1) && !getBit(occupancies[Both], target_square)) {
            // pawn promotion
            if (source_square >= a2 && source_square <= h2) {
              add_move(move_list, encodeMove(source_square, target_square, piece, BQueen, 0, 0, 0, 0));
              add_move(move_list, encodeMove(source_square, target_square, piece, BRook, 0, 0, 0, 0));
              add_move(move_list, encodeMove(source_square, target_square, piece, BBishop, 0, 0, 0, 0));
              add_move(move_list, encodeMove(source_square, target_square, piece, BKnight, 0, 0, 0, 0));
            } else {
              // one square ahead pawn move
              add_move(move_list, encodeMove(source_square, target_square, piece, 0, 0, 0, 0, 0));

              // two squares ahead pawn move
              if ((source_square >= a7 && source_square <= h7) && !getBit(occupancies[Both], target_square + 8))
                add_move(move_list, encodeMove(source_square, target_square + 8, piece, 0, 0, 1, 0, 0));
            }
          }

          // init pawn attacks bitboard
          attacks = pawnAttacks[side][source_square] & occupancies[White];

          // generate pawn captures
          while (attacks) {
            // init target square
            target_square = LSOneIdx(attacks);

            // pawn promotion
            if (source_square >= a2 && source_square <= h2) {
              add_move(move_list, encodeMove(source_square, target_square, piece, BQueen, 1, 0, 0, 0));
              add_move(move_list, encodeMove(source_square, target_square, piece, BRook, 1, 0, 0, 0));
              add_move(move_list, encodeMove(source_square, target_square, piece, BBishop, 1, 0, 0, 0));
              add_move(move_list, encodeMove(source_square, target_square, piece, BKnight, 1, 0, 0, 0));
            } else
              // one square ahead pawn move
              add_move(move_list, encodeMove(source_square, target_square, piece, 0, 1, 0, 0, 0));

            // pop ls1b of the pawn attacks
            remBit(attacks, target_square);
          }

          // generate enpassant captures
          if (enpassant != no_sq) {
            // lookup pawn attacks and bitwise AND with enpassant square (bit)
            U64 enpassant_attacks = pawnAttacks[side][source_square] & (1ULL << enpassant);

            // make sure enpassant capture available
            if (enpassant_attacks) {
              // init enpassant capture target square
              int target_enpassant = LSOneIdx(enpassant_attacks);
              add_move(move_list, encodeMove(source_square, target_enpassant, piece, 0, 1, 0, 1, 0));
            }
          }

          // pop ls1b from piece bitboard copy
          remBit(bitboard, source_square);
        }
      }

      // castling moves
      if (piece == BKing) {
        // king side castling is available
        if (castle & BlackKingSide) {
          // make sure square between king and king's rook are empty
          if (!getBit(occupancies[Both], f8) && !getBit(occupancies[Both], g8)) {
            // make sure king and the f8 squares are not under attacks
            if (!isSquareAttacked(e8, White) && !isSquareAttacked(f8, White))
              add_move(move_list, encodeMove(e8, g8, piece, 0, 0, 0, 0, 1));
          }
        }

        // queen side castling is available
        if (castle & BlackQueenSide) {
          // make sure square between king and queen's rook are empty
          if (!getBit(occupancies[Both], d8) && !getBit(occupancies[Both], c8) && !getBit(occupancies[Both], b8)) {
            // make sure king and the d8 squares are not under attacks
            if (!isSquareAttacked(e8, White) && !isSquareAttacked(d8, White))
              add_move(move_list, encodeMove(e8, c8, piece, 0, 0, 0, 0, 1));
          }
        }
      }
    }

    // genarate knight moves
    if ((side == White) ? piece == WKnight : piece == BKnight) {
      // loop over source squares of piece bitboard copy
      while (bitboard) {
        // init source square
        source_square = LSOneIdx(bitboard);

        // init piece attacks in order to get set of target squares
        attacks = knightAttacks[source_square] & ((side == White) ? ~occupancies[White] : ~occupancies[Black]);

        // loop over target squares available from generated attacks
        while (attacks) {
          // init target square
          target_square = LSOneIdx(attacks);

          // quiet move
          if (!getBit(((side == White) ? occupancies[Black] : occupancies[White]), target_square))
            add_move(move_list, encodeMove(source_square, target_square, piece, 0, 0, 0, 0, 0));

          else
            // capture move
            add_move(move_list, encodeMove(source_square, target_square, piece, 0, 1, 0, 0, 0));

          // pop ls1b in current attacks set
          remBit(attacks, target_square);
        }


        // pop ls1b of the current piece bitboard copy
        remBit(bitboard, source_square);
      }
    }

    // generate bishop moves
    if ((side == White) ? piece == WKnight : piece == BKnight) {
      // loop over source squares of piece bitboard copy
      while (bitboard) {
        // init source square
        source_square = LSOneIdx(bitboard);

        // init piece attacks in order to get set of target squares
        attacks = getBishopAttacks(static_cast<Square>(source_square), static_cast<Square>(occupancies[Both]))
            & ((side == White) ? ~occupancies[White] : ~occupancies[Black]);

        // loop over target squares available from generated attacks
        while (attacks) {
          // init target square
          target_square = LSOneIdx(attacks);

          // quiet move
          if (!getBit(((side == White) ? occupancies[Black] : occupancies[White]), target_square))
            add_move(move_list, encodeMove(source_square, target_square, piece, 0, 0, 0, 0, 0));

          else
            // capture move
            add_move(move_list, encodeMove(source_square, target_square, piece, 0, 1, 0, 0, 0));

          // pop ls1b in current attacks set
          remBit(attacks, target_square);
        }


        // pop ls1b of the current piece bitboard copy
        remBit(bitboard, source_square);
      }
    }

    // generate rook moves
    if ((side == White) ? piece == WRook : piece == BRook) {
      // loop over source squares of piece bitboard copy
      while (bitboard) {
        // init source square
        source_square = LSOneIdx(bitboard);

        // init piece attacks in order to get set of target squares
        attacks = getRookAttacks(static_cast<Square>(source_square), static_cast<Square>(occupancies[Both]))
            & ((side == White) ? ~occupancies[White] : ~occupancies[Black]);

        // loop over target squares available from generated attacks
        while (attacks) {
          // init target square
          target_square = LSOneIdx(attacks);

          // quiet move
          if (!getBit(((side == White) ? occupancies[Black] : occupancies[White]), target_square))
            add_move(move_list, encodeMove(source_square, target_square, piece, 0, 0, 0, 0, 0));

          else
            // capture move
            add_move(move_list, encodeMove(source_square, target_square, piece, 0, 1, 0, 0, 0));

          // pop ls1b in current attacks set
          remBit(attacks, target_square);
        }


        // pop ls1b of the current piece bitboard copy
        remBit(bitboard, source_square);
      }
    }

    // generate queen moves
    if ((side == White) ? piece == WQueen : piece == BQueen) {
      // loop over source squares of piece bitboard copy
      while (bitboard) {
        // init source square
        source_square = LSOneIdx(bitboard);

        // init piece attacks in order to get set of target squares
        attacks = getQueenAttacks(static_cast<Square>(source_square), static_cast<Square>(occupancies[Both]))
            & ((side == White) ? ~occupancies[White] : ~occupancies[Black]);

        // loop over target squares available from generated attacks
        while (attacks) {
          // init target square
          target_square = LSOneIdx(attacks);

          // quiet move
          if (!getBit(((side == White) ? occupancies[Black] : occupancies[White]), target_square))
            add_move(move_list, encodeMove(source_square, target_square, piece, 0, 0, 0, 0, 0));

          else
            // capture move
            add_move(move_list, encodeMove(source_square, target_square, piece, 0, 1, 0, 0, 0));

          // pop ls1b in current attacks set
          remBit(attacks, target_square);
        }


        // pop ls1b of the current piece bitboard copy
        remBit(bitboard, source_square);
      }
    }

    // generate king moves
    if ((side == White) ? piece == WKing : piece == BKing) {
      // loop over source squares of piece bitboard copy
      while (bitboard) {
        // init source square
        source_square = LSOneIdx(bitboard);

        // init piece attacks in order to get set of target squares
        attacks = kingAttacks[source_square] & ((side == White) ? ~occupancies[White] : ~occupancies[Black]);

        // loop over target squares available from generated attacks
        while (attacks) {
          // init target square
          target_square = LSOneIdx(attacks);

          // quiet move
          if (!getBit(((side == White) ? occupancies[Black] : occupancies[White]), target_square))
            add_move(move_list, encodeMove(source_square, target_square, piece, 0, 0, 0, 0, 0));

          else
            // capture move
            add_move(move_list, encodeMove(source_square, target_square, piece, 0, 1, 0, 0, 0));

          // pop ls1b in current attacks set
          remBit(attacks, target_square);
        }

        // pop ls1b of the current piece bitboard copy
        remBit(bitboard, source_square);
      }
    }
  }
}
