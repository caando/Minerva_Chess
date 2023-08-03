//
// Created by Jikun on 24/7/23.
//

#include "movegen.h"

void printMove(int move) {
  if (getMovePromoted(move)) {
    printf("%s%s%c", squareToCoordinates[getMoveSource(move)],
           squareToCoordinates[getMoveTarget(move)],
           promotedPieces[getMovePromoted(move)]);
  } else {
    printf("%s%s", squareToCoordinates[getMoveSource(move)],
           squareToCoordinates[getMoveTarget(move)]);
  }
}

bool isSquareAttacked(Square square, Colour side) {
  if ((side == White) && (pawnAttacks[Black][square] & board.bitboards[WPawn])) return true;
  if ((side == Black) && (pawnAttacks[White][square] & board.bitboards[BPawn])) return true;

  if (knightAttacks[square] & ((side == White) ? board.bitboards[WKnight] : board.bitboards[BKnight])) return true;

  if (getBishopAttacks(square, board.occupancies[Both])
      & ((side == White) ? board.bitboards[WBishop] : board.bitboards[BBishop]))
    return true;

  if (getRookAttacks(square, board.occupancies[Both])
      & ((side == White) ? board.bitboards[WRook] : board.bitboards[BRook]))
    return true;

  if (getQueenAttacks(square, board.occupancies[Both])
      & ((side == White) ? board.bitboards[WQueen] : board.bitboards[BQueen]))
    return true;

  if (kingAttacks[square] & ((side == White) ? board.bitboards[WKing] : board.bitboards[BKing])) return true;

  return false;
}

void addMove(moves &moveList, int move) {
  moveList.emplace_back(move);
}

int makeMove(int move, int moveFlag) {
  if (moveFlag == ALL_MOVES) {
    saveBoard();

    int sourceSquare = getMoveSource(move);
    int targetSquare = getMoveTarget(move);
    int piece = getMovePiece(move);
    int promotedPiece = getMovePromoted(move);

    remBit(board.bitboards[piece], sourceSquare);
    setBit(board.bitboards[piece], targetSquare);

    board.hashKey ^= pieceKey[piece][sourceSquare];
    board.hashKey ^= pieceKey[piece][targetSquare];

    board.fifty++;

    if (piece == WPawn || piece == BPawn) {
      board.fifty = 0;
    }

    if (getMoveCapture(move)) {
      board.fifty = 0;

      if (board.side == White) {
        for (int bbPiece = BPawn; bbPiece <= BKing; bbPiece++) {
          if (getBit(board.bitboards[bbPiece], targetSquare)) {
            remBit(board.bitboards[bbPiece], targetSquare);
            board.hashKey ^= pieceKey[bbPiece][targetSquare];
            break;
          }
        }
      } else {
        for (int bbPiece = WPawn; bbPiece <= WKing; bbPiece++) {
          if (getBit(board.bitboards[bbPiece], targetSquare)) {
            remBit(board.bitboards[bbPiece], targetSquare);
            board.hashKey ^= pieceKey[bbPiece][targetSquare];
            break;
          }
        }
      }
    }

    if (promotedPiece) {
      if (board.side == White) {
        remBit(board.bitboards[WPawn], targetSquare);
        board.hashKey ^= pieceKey[WPawn][targetSquare];
      } else {
        remBit(board.bitboards[BPawn], targetSquare);
        board.hashKey ^= pieceKey[BPawn][targetSquare];
      }
      setBit(board.bitboards[promotedPiece], targetSquare);
      board.hashKey ^= pieceKey[promotedPiece][targetSquare];
    }

    if (getMoveEnpassant(move)) {
      (board.side == White) ? remBit(board.bitboards[BPawn], targetSquare + 8) :
      remBit(board.bitboards[WPawn], targetSquare - 8);
      if (board.side == White) {
        remBit(board.bitboards[BPawn], targetSquare + 8);
        board.hashKey ^= pieceKey[BPawn][targetSquare + 8];
      } else {
        remBit(board.bitboards[WPawn], targetSquare - 8);
        board.hashKey ^= pieceKey[WPawn][targetSquare - 8];
      }
    }

    if (board.enpassant != no_sq) {
      board.hashKey ^= enpassantKey[board.enpassant];
    }
    board.enpassant = no_sq;

    if (getMoveDouble(move)) {
      if (board.side == White) {
        board.enpassant = static_cast<Square>(targetSquare + 8);
        board.hashKey ^= enpassantKey[targetSquare + 8];
      } else {
        board.enpassant = static_cast<Square>(targetSquare - 8);
        board.hashKey ^= enpassantKey[targetSquare - 8];
      }
    }

    if (getMoveCastling(move)) {
      switch (targetSquare) {
        case (g1):remBit(board.bitboards[WRook], h1);
          setBit(board.bitboards[WRook], f1);

          board.hashKey ^= pieceKey[WRook][h1];
          board.hashKey ^= pieceKey[WRook][f1];
          break;

        case (c1):remBit(board.bitboards[WRook], a1);
          setBit(board.bitboards[WRook], d1);

          board.hashKey ^= pieceKey[WRook][a1];
          board.hashKey ^= pieceKey[WRook][d1];
          break;

        case (g8):remBit(board.bitboards[BRook], h8);
          setBit(board.bitboards[BRook], f8);

          board.hashKey ^= pieceKey[BRook][h8];
          board.hashKey ^= pieceKey[BRook][f8];
          break;

        case (c8):remBit(board.bitboards[BRook], a8);
          setBit(board.bitboards[BRook], d8);

          board.hashKey ^= pieceKey[BRook][a8];
          board.hashKey ^= pieceKey[BRook][d8];
          break;
      }
    }

    board.hashKey ^= castlingKey[board.castle];
    board.castle &= castlingRights[sourceSquare];
    board.castle &= castlingRights[targetSquare];
    board.hashKey ^= castlingKey[board.castle];
    memset(board.occupancies, 0ULL, 24);

    for (int bbPiece = WPawn; bbPiece <= WKing; bbPiece++) {
      board.occupancies[White] |= board.bitboards[bbPiece];
    }

    for (int bbPiece = BPawn; bbPiece <= BKing; bbPiece++) {
      board.occupancies[Black] |= board.bitboards[bbPiece];
    }

    board.occupancies[Both] |= board.occupancies[White];
    board.occupancies[Both] |= board.occupancies[Black];

    if (board.side == White) {
      board.side = Black;
    } else {
      board.side = White;
    }

    board.hashKey ^= sideKey;

    if (isSquareAttacked(static_cast<Square>((board.side == White) ? LSOneIdx(board.bitboards[BKing])
                                                                   : LSOneIdx(board.bitboards[WKing])), board.side)) {
      takeBack();
      return 0;
    } else {
      return 1;
    }

  } else {
    if (getMoveCapture(move)) {
      return makeMove(move, ALL_MOVES);
    } else
      return 0;
  }
}

void generateMoves(moves &moveList) {
  moveList.clear();
  int sourceSquare, targetSquare;
  Bitboard bitboard, attacks;

  iterPiece() {
    bitboard = board.bitboards[piece];

    if (board.side == White) {
      if (piece == WPawn) {
        while (bitboard) {
          sourceSquare = LSOneIdx(bitboard);
          targetSquare = sourceSquare - 8;

          if (!(targetSquare < a8) && !getBit(board.occupancies[Both], targetSquare)) {
            if (sourceSquare >= a7 && sourceSquare <= h7) {
              addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, WQueen, 0, 0, 0, 0));
              addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, WRook, 0, 0, 0, 0));
              addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, WBishop, 0, 0, 0, 0));
              addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, WKnight, 0, 0, 0, 0));
            } else {
              addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, 0, 0, 0, 0, 0));
              if ((sourceSquare >= a2 && sourceSquare <= h2) && !getBit(board.occupancies[Both], targetSquare - 8))
                addMove(moveList, encodeMove(sourceSquare, (targetSquare - 8), piece, 0, 0, 1, 0, 0));
            }
          }

          attacks = pawnAttacks[board.side][sourceSquare] & board.occupancies[Black];

          while (attacks) {
            targetSquare = LSOneIdx(attacks);

            if (sourceSquare >= a7 && sourceSquare <= h7) {
              addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, WQueen, 1, 0, 0, 0));
              addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, WRook, 1, 0, 0, 0));
              addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, WBishop, 1, 0, 0, 0));
              addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, WKnight, 1, 0, 0, 0));
            } else {
              addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, 0, 1, 0, 0, 0));
            }
            remBit(attacks, targetSquare);
          }

          if (board.enpassant != no_sq) {
            U64 enpassantAttacks = pawnAttacks[board.side][sourceSquare] & (1ULL << board.enpassant);
            if (enpassantAttacks) {
              int targetEnpassant = LSOneIdx(enpassantAttacks);
              addMove(moveList, encodeMove(sourceSquare, targetEnpassant, piece, 0, 1, 0, 1, 0));
            }
          }
          remBit(bitboard, sourceSquare);
        }
      }

      if (piece == WKing) {
        if (board.castle & WhiteKingSide) {
          if (!getBit(board.occupancies[Both], f1) && !getBit(board.occupancies[Both], g1)) {
            if (!isSquareAttacked(e1, Black) && !isSquareAttacked(f1, Black)) {
              addMove(moveList, encodeMove(e1, g1, piece, 0, 0, 0, 0, 1));
            }
          }
        }

        if (board.castle & WhiteQueenSide) {
          if (!getBit(board.occupancies[Both], d1) && !getBit(board.occupancies[Both], c1)
              && !getBit(board.occupancies[Both], b1)) {
            if (!isSquareAttacked(e1, Black) && !isSquareAttacked(d1, Black)) {
              addMove(moveList, encodeMove(e1, c1, piece, 0, 0, 0, 0, 1));
            }
          }
        }
      }
    } else {
      if (piece == BPawn) {
        while (bitboard) {
          sourceSquare = LSOneIdx(bitboard);
          targetSquare = sourceSquare + 8;

          if (!(targetSquare > h1) && !getBit(board.occupancies[Both], targetSquare)) {
            if (sourceSquare >= a2 && sourceSquare <= h2) {
              addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, BQueen, 0, 0, 0, 0));
              addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, BRook, 0, 0, 0, 0));
              addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, BBishop, 0, 0, 0, 0));
              addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, BKnight, 0, 0, 0, 0));
            } else {
              addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, 0, 0, 0, 0, 0));
              if ((sourceSquare >= a7 && sourceSquare <= h7) && !getBit(board.occupancies[Both], targetSquare + 8)) {
                addMove(moveList, encodeMove(sourceSquare, (targetSquare + 8), piece, 0, 0, 1, 0, 0));
              }
            }
          }

          attacks = pawnAttacks[board.side][sourceSquare] & board.occupancies[White];

          while (attacks) {
            targetSquare = LSOneIdx(attacks);
            if (sourceSquare >= a2 && sourceSquare <= h2) {
              addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, BQueen, 1, 0, 0, 0));
              addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, BRook, 1, 0, 0, 0));
              addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, BBishop, 1, 0, 0, 0));
              addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, BKnight, 1, 0, 0, 0));
            } else {
              addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, 0, 1, 0, 0, 0));
            }
            remBit(attacks, targetSquare);
          }

          if (board.enpassant != no_sq) {
            U64 enpassantAttacks = pawnAttacks[board.side][sourceSquare] & (1ULL << board.enpassant);
            if (enpassantAttacks) {
              int targetEnpassant = LSOneIdx(enpassantAttacks);
              addMove(moveList, encodeMove(sourceSquare, targetEnpassant, piece, 0, 1, 0, 1, 0));
            }
          }
          remBit(bitboard, sourceSquare);
        }
      }

      if (piece == BKing) {
        if (board.castle & BlackKingSide) {
          if (!getBit(board.occupancies[Both], f8) && !getBit(board.occupancies[Both], g8)) {
            if (!isSquareAttacked(e8, White) && !isSquareAttacked(f8, White))
              addMove(moveList, encodeMove(e8, g8, piece, 0, 0, 0, 0, 1));
          }
        }

        if (board.castle & BlackQueenSide) {
          if (!getBit(board.occupancies[Both], d8) && !getBit(board.occupancies[Both], c8)
              && !getBit(board.occupancies[Both], b8)) {
            if (!isSquareAttacked(e8, White) && !isSquareAttacked(d8, White))
              addMove(moveList, encodeMove(e8, c8, piece, 0, 0, 0, 0, 1));
          }
        }
      }
    }

    if ((board.side == White) ? piece == WKnight : piece == BKnight) {
      while (bitboard) {
        sourceSquare = LSOneIdx(bitboard);
        attacks = knightAttacks[sourceSquare]
            & ((board.side == White) ? ~board.occupancies[White] : ~board.occupancies[Black]);
        while (attacks) {
          targetSquare = LSOneIdx(attacks);
          if (!getBit(((board.side == White) ? board.occupancies[Black] : board.occupancies[White]), targetSquare)) {
            addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, 0, 0, 0, 0, 0));
          } else {
            addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, 0, 1, 0, 0, 0));
          }
          remBit(attacks, targetSquare);
        }
        remBit(bitboard, sourceSquare);
      }
    }

    if ((board.side == White) ? piece == WBishop : piece == BBishop) {
      while (bitboard) {
        sourceSquare = LSOneIdx(bitboard);
        attacks = getBishopAttacks(static_cast<Square>(sourceSquare), board.occupancies[Both])
            & ((board.side == White) ? ~board.occupancies[White] : ~board.occupancies[Black]);
        while (attacks) {
          targetSquare = LSOneIdx(attacks);
          if (!getBit(((board.side == White) ? board.occupancies[Black] : board.occupancies[White]), targetSquare)) {
            addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, 0, 0, 0, 0, 0));
          } else {
            addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, 0, 1, 0, 0, 0));
          }
          remBit(attacks, targetSquare);
        }
        remBit(bitboard, sourceSquare);
      }
    }

    if ((board.side == White) ? piece == WRook : piece == BRook) {
      while (bitboard) {
        sourceSquare = LSOneIdx(bitboard);
        attacks = getRookAttacks(static_cast<Square>(sourceSquare), board.occupancies[Both])
            & ((board.side == White) ? ~board.occupancies[White] : ~board.occupancies[Black]);
        while (attacks) {
          targetSquare = LSOneIdx(attacks);
          if (!getBit(((board.side == White) ? board.occupancies[Black] : board.occupancies[White]), targetSquare)) {
            addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, 0, 0, 0, 0, 0));
          } else {
            addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, 0, 1, 0, 0, 0));
          }
          remBit(attacks, targetSquare);
        }
        remBit(bitboard, sourceSquare);
      }
    }

    if ((board.side == White) ? piece == WQueen : piece == BQueen) {
      while (bitboard) {
        sourceSquare = LSOneIdx(bitboard);
        attacks = getQueenAttacks(static_cast<Square>(sourceSquare), board.occupancies[Both])
            & ((board.side == White) ? ~board.occupancies[White] : ~board.occupancies[Black]);
        while (attacks) {
          targetSquare = LSOneIdx(attacks);
          if (!getBit(((board.side == White) ? board.occupancies[Black] : board.occupancies[White]), targetSquare)) {
            addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, 0, 0, 0, 0, 0));
          } else {
            addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, 0, 1, 0, 0, 0));
          }
          remBit(attacks, targetSquare);
        }
        remBit(bitboard, sourceSquare);
      }
    }

    if ((board.side == White) ? piece == WKing : piece == BKing) {
      while (bitboard) {
        sourceSquare = LSOneIdx(bitboard);
        attacks =
            kingAttacks[sourceSquare] & ((board.side == White) ? ~board.occupancies[White] : ~board.occupancies[Black]);
        while (attacks) {
          targetSquare = LSOneIdx(attacks);
          if (!getBit(((board.side == White) ? board.occupancies[Black] : board.occupancies[White]), targetSquare)) {
            addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, 0, 0, 0, 0, 0));
          } else {
            addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, 0, 1, 0, 0, 0));
          }
          remBit(attacks, targetSquare);
        }
        remBit(bitboard, sourceSquare);
      }
    }
  }
}