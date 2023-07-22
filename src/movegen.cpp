//
// Created by Jikun on 26/6/23.
//

#include "movegen.h"
#include "attack.h"
#include "magic.h"
#include "variables.h"
#include "zobrist.h"
#include "debug.h"

// is square current given attacked by the current given side
bool isSquareAttacked(Square square, Colour side) {
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

// add move to the move list
inline void addMove(moves &moveList, int move) {
// store move
  moveList.emplace_back(move);
}

// make move on chess board
int makeMove(int move, int moveFlag) {
  // quiet moves
  if (moveFlag == all_moves) {
    // preserve board state
    copyBoard();

    // parse move
    int sourceSquare = getMoveSource(move);
    int targetSquare = getMoveTarget(move);
    int piece = getMovePiece(move);
    int promotedPiece = getMovePromoted(move);
    int capture = getMoveCapture(move);
    int double_push = getMoveDouble(move);
    int enpass = getMoveEnpassant(move);
    int castling = getMoveCastling(move);

    // move piece
    remBit(bitboards[piece], sourceSquare);
    setBit(bitboards[piece], targetSquare);

    // hash piece
    hashKey ^= pieceKey[piece][sourceSquare]; // remove piece from source square in hash key
    hashKey ^= pieceKey[piece][targetSquare]; // set piece to the target square in hash key

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
      int startPiece, endPiece;

      // White to move
      if (side == White) {
        startPiece = BPawn;
        endPiece = BKing;
      }

        // Black to move
      else {
        startPiece = WPawn;
        endPiece = WKing;
      }

      // loop over bitboards opposite to the current side to move
      for (int bb_piece = startPiece; bb_piece <= endPiece; bb_piece++) {
        // if there's a piece on the target square
        if (getBit(bitboards[bb_piece], targetSquare)) {
          // remove it from corresponding bitboard
          remBit(bitboards[bb_piece], targetSquare);

          // remove the piece from hash key
          hashKey ^= pieceKey[bb_piece][targetSquare];
          break;
        }
      }
    }

    // handle pawn promotions
    if (promotedPiece) {
      // White to move
      if (side == White) {
        // erase the pawn from the target square
        remBit(bitboards[WPawn], targetSquare);

        // remove pawn from hash key
        hashKey ^= pieceKey[WPawn][targetSquare];
      }

        // Black to move
      else {
        // erase the pawn from the target square
        remBit(bitboards[BPawn], targetSquare);

        // remove pawn from hash key
        hashKey ^= pieceKey[BPawn][targetSquare];
      }

      // set up promoted piece on chess board
      setBit(bitboards[promotedPiece], targetSquare);

      // add promoted piece into the hash key
      hashKey ^= pieceKey[promotedPiece][targetSquare];
    }

    // handle enpassant captures
    if (enpass) {
      // erase the pawn depending on side to move
      (side == White) ? remBit(bitboards[BPawn], targetSquare + 8) :
      remBit(bitboards[WPawn], targetSquare - 8);

      // White to move
      if (side == White) {
        // remove captured pawn
        remBit(bitboards[BPawn], targetSquare + 8);

        // remove pawn from hash key
        hashKey ^= pieceKey[BPawn][targetSquare + 8];
      }

        // Black to move
      else {
        // remove captured pawn
        remBit(bitboards[WPawn], targetSquare - 8);

        // remove pawn from hash key
        hashKey ^= pieceKey[WPawn][targetSquare - 8];
      }
    }

    // hash enpassant if available (remove enpassant square from hash key )
    if (enpassant != no_sq) hashKey ^= enpassantKey[enpassant];

    // reset enpassant square
    enpassant = no_sq;

    // handle double pawn push
    if (double_push) {
      // White to move
      if (side == White) {
        // set enpassant square
        enpassant = static_cast<Square>(targetSquare + 8);

        // hash enpassant
        hashKey ^= enpassantKey[targetSquare + 8];
      }

        // Black to move
      else {
        // set enpassant square
        enpassant = static_cast<Square>(targetSquare - 8);

        // hash enpassant
        hashKey ^= enpassantKey[targetSquare - 8];
      }
    }

    // handle castling moves
    if (castling) {
      // switch target square
      switch (targetSquare) {
        // White castles king side
        case (g1):
          // move H rook
          remBit(bitboards[WRook], h1);
          setBit(bitboards[WRook], f1);

          // hash rook
          hashKey ^= pieceKey[WRook][h1];  // remove rook from h1 from hash key
          hashKey ^= pieceKey[WRook][f1];  // put rook on f1 into a hash key
          break;

          // White castles queen side
        case (c1):
          // move A rook
          remBit(bitboards[WRook], a1);
          setBit(bitboards[WRook], d1);

          // hash rook
          hashKey ^= pieceKey[WRook][a1];  // remove rook from a1 from hash key
          hashKey ^= pieceKey[WRook][d1];  // put rook on d1 into a hash key
          break;

          // Black castles king side
        case (g8):
          // move H rook
          remBit(bitboards[BRook], h8);
          setBit(bitboards[BRook], f8);

          // hash rook
          hashKey ^= pieceKey[BRook][h8];  // remove rook from h8 from hash key
          hashKey ^= pieceKey[BRook][f8];  // put rook on f8 into a hash key
          break;

          // Black castles queen side
        case (c8):
          // move A rook
          remBit(bitboards[BRook], a8);
          setBit(bitboards[BRook], d8);

          // hash rook
          hashKey ^= pieceKey[BRook][a8];  // remove rook from a8 from hash key
          hashKey ^= pieceKey[BRook][d8];  // put rook on d8 into a hash key
          break;
      }
    }

    // hash castling
    hashKey ^= castlingKey[castle];

    // update castling rights
    castle &= castlingRights[sourceSquare];
    castle &= castlingRights[targetSquare];

    // hash castling
    hashKey ^= castlingKey[castle];

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
    hashKey ^= sideKey;

    // make sure that king has not been exposed into a check
    if (isSquareAttacked(static_cast<Square>((side == White) ? LSOneIdx(bitboards[BKing]) : LSOneIdx(bitboards[WKing])),
                         side)) {
      // take move back
      takeBack();

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
    if (getMoveCapture(move))
      makeMove(move, all_moves);

      // otherwise the move is not a capture
    else
      // don't make it
      return 0;
  }
}

// generate all moves
void generateMoves(moves &moveList) {
  // define source & target squares
  int sourceSquare, targetSquare;

  // define current piece's bitboard copy & it's attacks
  Bitboard bitboard, attacks;

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
          sourceSquare = LSOneIdx(bitboard);

          // init target square
          targetSquare = sourceSquare - 8;

          // generate quiet pawn moves
          if (!(targetSquare < a8) && !getBit(occupancies[Both], targetSquare)) {
            // pawn promotion
            if (sourceSquare >= a7 && sourceSquare <= h7) {
              addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, WQueen, 0, 0, 0, 0));
              addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, WRook, 0, 0, 0, 0));
              addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, WBishop, 0, 0, 0, 0));
              addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, WKnight, 0, 0, 0, 0));
            } else {
              // one square ahead pawn move
              addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, 0, 0, 0, 0, 0));

              // two squares ahead pawn move
              if ((sourceSquare >= a2 && sourceSquare <= h2) && !getBit(occupancies[Both], targetSquare - 8))
                addMove(moveList, encodeMove(sourceSquare, targetSquare - 8, piece, 0, 0, 1, 0, 0));
            }
          }

          // init pawn attacks bitboard
          attacks = pawnAttacks[side][sourceSquare] & occupancies[Black];

          // generate pawn captures
          while (attacks) {
            // init target square
            targetSquare = LSOneIdx(attacks);

            // pawn promotion
            if (sourceSquare >= a7 && sourceSquare <= h7) {
              addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, WQueen, 1, 0, 0, 0));
              addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, WRook, 1, 0, 0, 0));
              addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, WBishop, 1, 0, 0, 0));
              addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, WKnight, 1, 0, 0, 0));
            } else
              // one square ahead pawn move
              addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, 0, 1, 0, 0, 0));

            // pop ls1b of the pawn attacks
            remBit(attacks, targetSquare);
          }

          // generate enpassant captures
          if (enpassant != no_sq) {
            // lookup pawn attacks and bitwise AND with enpassant square (bit)
            U64 enpassantAttacks = pawnAttacks[side][sourceSquare] & (1ULL << enpassant);

            // make sure enpassant capture available
            if (enpassantAttacks) {
              // init enpassant capture target square
              int targetEnpassant = LSOneIdx(enpassantAttacks);
              addMove(moveList, encodeMove(sourceSquare, targetEnpassant, piece, 0, 1, 0, 1, 0));
            }
          }

          // pop ls1b from piece bitboard copy
          remBit(bitboard, sourceSquare);
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
              addMove(moveList, encodeMove(e1, g1, piece, 0, 0, 0, 0, 1));
          }
        }

        // queen side castling is available
        if (castle & WhiteQueenSide) {
          // make sure square between king and queen's rook are empty
          if (!getBit(occupancies[Both], d1) && !getBit(occupancies[Both], c1) && !getBit(occupancies[Both], b1)) {
            // make sure king and the d1 squares are not under attacks
            if (!isSquareAttacked(e1, Black) && !isSquareAttacked(d1, Black))
              addMove(moveList, encodeMove(e1, c1, piece, 0, 0, 0, 0, 1));
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
          sourceSquare = LSOneIdx(bitboard);

          // init target square
          targetSquare = sourceSquare + 8;

          // generate quiet pawn moves
          if (!(targetSquare > h1) && !getBit(occupancies[Both], targetSquare)) {
            // pawn promotion
            if (sourceSquare >= a2 && sourceSquare <= h2) {
              addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, BQueen, 0, 0, 0, 0));
              addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, BRook, 0, 0, 0, 0));
              addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, BBishop, 0, 0, 0, 0));
              addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, BKnight, 0, 0, 0, 0));
            } else {
              // one square ahead pawn move
              addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, 0, 0, 0, 0, 0));

              // two squares ahead pawn move
              if ((sourceSquare >= a7 && sourceSquare <= h7) && !getBit(occupancies[Both], targetSquare + 8))
                addMove(moveList, encodeMove(sourceSquare, targetSquare + 8, piece, 0, 0, 1, 0, 0));
            }
          }

          // init pawn attacks bitboard
          attacks = pawnAttacks[side][sourceSquare] & occupancies[White];

          // generate pawn captures
          while (attacks) {
            // init target square
            targetSquare = LSOneIdx(attacks);

            // pawn promotion
            if (sourceSquare >= a2 && sourceSquare <= h2) {
              addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, BQueen, 1, 0, 0, 0));
              addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, BRook, 1, 0, 0, 0));
              addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, BBishop, 1, 0, 0, 0));
              addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, BKnight, 1, 0, 0, 0));
            } else
              // one square ahead pawn move
              addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, 0, 1, 0, 0, 0));

            // pop ls1b of the pawn attacks
            remBit(attacks, targetSquare);
          }

          // generate enpassant captures
          if (enpassant != no_sq) {
            // lookup pawn attacks and bitwise AND with enpassant square (bit)
            U64 enpassantAttacks = pawnAttacks[side][sourceSquare] & (1ULL << enpassant);

            // make sure enpassant capture available
            if (enpassantAttacks) {
              // init enpassant capture target square
              int targetEnpassant = LSOneIdx(enpassantAttacks);
              addMove(moveList, encodeMove(sourceSquare, targetEnpassant, piece, 0, 1, 0, 1, 0));
            }
          }

          // pop ls1b from piece bitboard copy
          remBit(bitboard, sourceSquare);
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
              addMove(moveList, encodeMove(e8, g8, piece, 0, 0, 0, 0, 1));
          }
        }

        // queen side castling is available
        if (castle & BlackQueenSide) {
          // make sure square between king and queen's rook are empty
          if (!getBit(occupancies[Both], d8) && !getBit(occupancies[Both], c8) && !getBit(occupancies[Both], b8)) {
            // make sure king and the d8 squares are not under attacks
            if (!isSquareAttacked(e8, White) && !isSquareAttacked(d8, White))
              addMove(moveList, encodeMove(e8, c8, piece, 0, 0, 0, 0, 1));
          }
        }
      }
    }

    // genarate knight moves
    if ((side == White) ? piece == WKnight : piece == BKnight) {
      // loop over source squares of piece bitboard copy
      while (bitboard) {
        // init source square
        sourceSquare = LSOneIdx(bitboard);

        // init piece attacks in order to get set of target squares
        attacks = knightAttacks[sourceSquare] & ((side == White) ? ~occupancies[White] : ~occupancies[Black]);

        // loop over target squares available from generated attacks
        while (attacks) {
          // init target square
          targetSquare = LSOneIdx(attacks);

          // quiet move
          if (!getBit(((side == White) ? occupancies[Black] : occupancies[White]), targetSquare))
            addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, 0, 0, 0, 0, 0));

          else
            // capture move
            addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, 0, 1, 0, 0, 0));

          // pop ls1b in current attacks set
          remBit(attacks, targetSquare);
        }


        // pop ls1b of the current piece bitboard copy
        remBit(bitboard, sourceSquare);
      }
    }

    // generate bishop moves
    if ((side == White) ? piece == WBishop : piece == BBishop) {
      // loop over source squares of piece bitboard copy
      while (bitboard) {
        // init source square
        sourceSquare = LSOneIdx(bitboard);

        // init piece attacks in order to get set of target squares
        attacks = getBishopAttacks(static_cast<Square>(sourceSquare), occupancies[Both])
            & ((side == White) ? ~occupancies[White] : ~occupancies[Black]);

        // loop over target squares available from generated attacks
        while (attacks) {
          // init target square
          targetSquare = LSOneIdx(attacks);

          // quiet move
          if (!getBit(((side == White) ? occupancies[Black] : occupancies[White]), targetSquare))
            addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, 0, 0, 0, 0, 0));

          else
            // capture move
            addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, 0, 1, 0, 0, 0));

          // pop ls1b in current attacks set
          remBit(attacks, targetSquare);
        }


        // pop ls1b of the current piece bitboard copy
        remBit(bitboard, sourceSquare);
      }
    }

    // generate rook moves
    if ((side == White) ? piece == WRook : piece == BRook) {
      // loop over source squares of piece bitboard copy
      while (bitboard) {
        // init source square
        sourceSquare = LSOneIdx(bitboard);

        // init piece attacks in order to get set of target squares
        attacks = getRookAttacks(static_cast<Square>(sourceSquare), occupancies[Both])
            & ((side == White) ? ~occupancies[White] : ~occupancies[Black]);

        // loop over target squares available from generated attacks
        while (attacks) {
          // init target square
          targetSquare = LSOneIdx(attacks);

          // quiet move
          if (!getBit(((side == White) ? occupancies[Black] : occupancies[White]), targetSquare))
            addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, 0, 0, 0, 0, 0));

          else
            // capture move
            addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, 0, 1, 0, 0, 0));

          // pop ls1b in current attacks set
          remBit(attacks, targetSquare);
        }


        // pop ls1b of the current piece bitboard copy
        remBit(bitboard, sourceSquare);
      }
    }

    // generate queen moves
    if ((side == White) ? piece == WQueen : piece == BQueen) {
      // loop over source squares of piece bitboard copy
      while (bitboard) {
        // init source square
        sourceSquare = LSOneIdx(bitboard);

        // init piece attacks in order to get set of target squares
        attacks = getQueenAttacks(static_cast<Square>(sourceSquare), occupancies[Both])
            & ((side == White) ? ~occupancies[White] : ~occupancies[Black]);

        // loop over target squares available from generated attacks
        while (attacks) {
          // init target square
          targetSquare = LSOneIdx(attacks);

          // quiet move
          if (!getBit(((side == White) ? occupancies[Black] : occupancies[White]), targetSquare))
            addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, 0, 0, 0, 0, 0));

          else
            // capture move
            addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, 0, 1, 0, 0, 0));

          // pop ls1b in current attacks set
          remBit(attacks, targetSquare);
        }


        // pop ls1b of the current piece bitboard copy
        remBit(bitboard, sourceSquare);
      }
    }

    // generate king moves
    if ((side == White) ? piece == WKing : piece == BKing) {
      // loop over source squares of piece bitboard copy
      while (bitboard) {
        // init source square
        sourceSquare = LSOneIdx(bitboard);

        // init piece attacks in order to get set of target squares
        attacks = kingAttacks[sourceSquare] & ((side == White) ? ~occupancies[White] : ~occupancies[Black]);

        // loop over target squares available from generated attacks
        while (attacks) {
          // init target square
          targetSquare = LSOneIdx(attacks);

          // quiet move
          if (!getBit(((side == White) ? occupancies[Black] : occupancies[White]), targetSquare))
            addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, 0, 0, 0, 0, 0));

          else
            // capture move
            addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, 0, 1, 0, 0, 0));

          // pop ls1b in current attacks set
          remBit(attacks, targetSquare);
        }

        // pop ls1b of the current piece bitboard copy
        remBit(bitboard, sourceSquare);
      }
    }
  }
}

void printMove(int move) {
  if (getMovePromoted(move))
    std::cout << squareToCoordinates[getMoveSource(move)] << squareToCoordinates[getMoveTarget(move)]
              << promotedPieces[getMovePromoted(move)];
  else
    std::cout << squareToCoordinates[getMoveSource(move)] << squareToCoordinates[getMoveTarget(move)];
}
