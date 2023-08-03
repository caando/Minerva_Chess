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
  if ((side == White) && (pawnAttacks[Black][square] & bitboards[WPawn])) return true;
  if ((side == Black) && (pawnAttacks[White][square] & bitboards[BPawn])) return true;

  if (knightAttacks[square] & ((side == White) ? bitboards[WKnight] : bitboards[BKnight])) return true;

  if (getBishopAttacks(square, occupancies[Both]) & ((side == White) ? bitboards[WBishop] : bitboards[BBishop]))
    return true;

  if (getRookAttacks(square, occupancies[Both]) & ((side == White) ? bitboards[WRook] : bitboards[BRook])) return true;

  if (getQueenAttacks(square, occupancies[Both]) & ((side == White) ? bitboards[WQueen] : bitboards[BQueen]))
    return true;

  if (kingAttacks[square] & ((side == White) ? bitboards[WKing] : bitboards[BKing])) return true;

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

    remBit(bitboards[piece], sourceSquare);
    setBit(bitboards[piece], targetSquare);

    hashKey ^= pieceKey[piece][sourceSquare];
    hashKey ^= pieceKey[piece][targetSquare];

    fifty++;

    if (piece == WPawn || piece == BPawn) {
      fifty = 0;
    }

    if (getMoveCapture(move)) {
      fifty = 0;

      if (side == White) {
        for (int bbPiece = BPawn; bbPiece <= BKing; bbPiece++) {
          if (getBit(bitboards[bbPiece], targetSquare)) {
            remBit(bitboards[bbPiece], targetSquare);
            hashKey ^= pieceKey[bbPiece][targetSquare];
            break;
          }
        }
      } else {
        for (int bbPiece = WPawn; bbPiece <= WKing; bbPiece++) {
          if (getBit(bitboards[bbPiece], targetSquare)) {
            remBit(bitboards[bbPiece], targetSquare);
            hashKey ^= pieceKey[bbPiece][targetSquare];
            break;
          }
        }
      }
    }

    if (promotedPiece) {
      if (side == White) {
        remBit(bitboards[WPawn], targetSquare);
        hashKey ^= pieceKey[WPawn][targetSquare];
      } else {
        remBit(bitboards[BPawn], targetSquare);
        hashKey ^= pieceKey[BPawn][targetSquare];
      }
      setBit(bitboards[promotedPiece], targetSquare);
      hashKey ^= pieceKey[promotedPiece][targetSquare];
    }

    if (getMoveEnpassant(move)) {
      (side == White) ? remBit(bitboards[BPawn], targetSquare + 8) :
      remBit(bitboards[WPawn], targetSquare - 8);
      if (side == White) {
        remBit(bitboards[BPawn], targetSquare + 8);
        hashKey ^= pieceKey[BPawn][targetSquare + 8];
      } else {
        remBit(bitboards[WPawn], targetSquare - 8);
        hashKey ^= pieceKey[WPawn][targetSquare - 8];
      }
    }

    if (enpassant != no_sq) {
      hashKey ^= enpassantKey[enpassant];
    }
    enpassant = no_sq;

    if (getMoveDouble(move)) {
      if (side == White) {
        enpassant = static_cast<Square>(targetSquare + 8);
        hashKey ^= enpassantKey[targetSquare + 8];
      } else {
        enpassant = static_cast<Square>(targetSquare - 8);
        hashKey ^= enpassantKey[targetSquare - 8];
      }
    }

    if (getMoveCastling(move)) {
      switch (targetSquare) {
        case (g1):remBit(bitboards[WRook], h1);
          setBit(bitboards[WRook], f1);

          hashKey ^= pieceKey[WRook][h1];
          hashKey ^= pieceKey[WRook][f1];
          break;

        case (c1):remBit(bitboards[WRook], a1);
          setBit(bitboards[WRook], d1);

          hashKey ^= pieceKey[WRook][a1];
          hashKey ^= pieceKey[WRook][d1];
          break;

        case (g8):remBit(bitboards[BRook], h8);
          setBit(bitboards[BRook], f8);

          hashKey ^= pieceKey[BRook][h8];
          hashKey ^= pieceKey[BRook][f8];
          break;

        case (c8):remBit(bitboards[BRook], a8);
          setBit(bitboards[BRook], d8);

          hashKey ^= pieceKey[BRook][a8];
          hashKey ^= pieceKey[BRook][d8];
          break;
      }
    }

    hashKey ^= castlingKey[castle];
    castle &= castlingRights[sourceSquare];
    castle &= castlingRights[targetSquare];
    hashKey ^= castlingKey[castle];
    memset(occupancies, 0ULL, 24);

    for (int bbPiece = WPawn; bbPiece <= WKing; bbPiece++) {
      occupancies[White] |= bitboards[bbPiece];
    }

    for (int bbPiece = BPawn; bbPiece <= BKing; bbPiece++) {
      occupancies[Black] |= bitboards[bbPiece];
    }

    occupancies[Both] |= occupancies[White];
    occupancies[Both] |= occupancies[Black];

    if (side == White) {
      side = Black;
    } else {
      side = White;
    }

    hashKey ^= sideKey;

    if (isSquareAttacked(static_cast<Square>((side == White) ? LSOneIdx(bitboards[BKing]) : LSOneIdx(bitboards[WKing])),
                         side)) {
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
    bitboard = bitboards[piece];

    if (side == White) {
      if (piece == WPawn) {
        while (bitboard) {
          sourceSquare = LSOneIdx(bitboard);
          targetSquare = sourceSquare - 8;

          if (!(targetSquare < a8) && !getBit(occupancies[Both], targetSquare)) {
            if (sourceSquare >= a7 && sourceSquare <= h7) {
              addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, WQueen, 0, 0, 0, 0));
              addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, WRook, 0, 0, 0, 0));
              addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, WBishop, 0, 0, 0, 0));
              addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, WKnight, 0, 0, 0, 0));
            } else {
              addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, 0, 0, 0, 0, 0));
              if ((sourceSquare >= a2 && sourceSquare <= h2) && !getBit(occupancies[Both], targetSquare - 8))
                addMove(moveList, encodeMove(sourceSquare, (targetSquare - 8), piece, 0, 0, 1, 0, 0));
            }
          }

          attacks = pawnAttacks[side][sourceSquare] & occupancies[Black];

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

          if (enpassant != no_sq) {
            U64 enpassantAttacks = pawnAttacks[side][sourceSquare] & (1ULL << enpassant);
            if (enpassantAttacks) {
              int targetEnpassant = LSOneIdx(enpassantAttacks);
              addMove(moveList, encodeMove(sourceSquare, targetEnpassant, piece, 0, 1, 0, 1, 0));
            }
          }
          remBit(bitboard, sourceSquare);
        }
      }

      if (piece == WKing) {
        if (castle & WhiteKingSide) {
          if (!getBit(occupancies[Both], f1) && !getBit(occupancies[Both], g1)) {
            if (!isSquareAttacked(e1, Black) && !isSquareAttacked(f1, Black)) {
              addMove(moveList, encodeMove(e1, g1, piece, 0, 0, 0, 0, 1));
            }
          }
        }

        if (castle & WhiteQueenSide) {
          if (!getBit(occupancies[Both], d1) && !getBit(occupancies[Both], c1) && !getBit(occupancies[Both], b1)) {
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

          if (!(targetSquare > h1) && !getBit(occupancies[Both], targetSquare)) {
            if (sourceSquare >= a2 && sourceSquare <= h2) {
              addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, BQueen, 0, 0, 0, 0));
              addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, BRook, 0, 0, 0, 0));
              addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, BBishop, 0, 0, 0, 0));
              addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, BKnight, 0, 0, 0, 0));
            } else {
              addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, 0, 0, 0, 0, 0));
              if ((sourceSquare >= a7 && sourceSquare <= h7) && !getBit(occupancies[Both], targetSquare + 8)) {
                addMove(moveList, encodeMove(sourceSquare, (targetSquare + 8), piece, 0, 0, 1, 0, 0));
              }
            }
          }

          attacks = pawnAttacks[side][sourceSquare] & occupancies[White];

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

          if (enpassant != no_sq) {
            U64 enpassantAttacks = pawnAttacks[side][sourceSquare] & (1ULL << enpassant);
            if (enpassantAttacks) {
              int targetEnpassant = LSOneIdx(enpassantAttacks);
              addMove(moveList, encodeMove(sourceSquare, targetEnpassant, piece, 0, 1, 0, 1, 0));
            }
          }
          remBit(bitboard, sourceSquare);
        }
      }

      if (piece == BKing) {
        if (castle & BlackKingSide) {
          if (!getBit(occupancies[Both], f8) && !getBit(occupancies[Both], g8)) {
            if (!isSquareAttacked(e8, White) && !isSquareAttacked(f8, White))
              addMove(moveList, encodeMove(e8, g8, piece, 0, 0, 0, 0, 1));
          }
        }

        if (castle & BlackQueenSide) {
          if (!getBit(occupancies[Both], d8) && !getBit(occupancies[Both], c8) && !getBit(occupancies[Both], b8)) {
            if (!isSquareAttacked(e8, White) && !isSquareAttacked(d8, White))
              addMove(moveList, encodeMove(e8, c8, piece, 0, 0, 0, 0, 1));
          }
        }
      }
    }

    if ((side == White) ? piece == WKnight : piece == BKnight) {
      while (bitboard) {
        sourceSquare = LSOneIdx(bitboard);
        attacks = knightAttacks[sourceSquare] & ((side == White) ? ~occupancies[White] : ~occupancies[Black]);
        while (attacks) {
          targetSquare = LSOneIdx(attacks);
          if (!getBit(((side == White) ? occupancies[Black] : occupancies[White]), targetSquare)) {
            addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, 0, 0, 0, 0, 0));
          } else {
            addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, 0, 1, 0, 0, 0));
          }
          remBit(attacks, targetSquare);
        }
        remBit(bitboard, sourceSquare);
      }
    }

    if ((side == White) ? piece == WBishop : piece == BBishop) {
      while (bitboard) {
        sourceSquare = LSOneIdx(bitboard);
        attacks = getBishopAttacks(static_cast<Square>(sourceSquare), occupancies[Both])
            & ((side == White) ? ~occupancies[White] : ~occupancies[Black]);
        while (attacks) {
          targetSquare = LSOneIdx(attacks);
          if (!getBit(((side == White) ? occupancies[Black] : occupancies[White]), targetSquare)) {
            addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, 0, 0, 0, 0, 0));
          } else {
            addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, 0, 1, 0, 0, 0));
          }
          remBit(attacks, targetSquare);
        }
        remBit(bitboard, sourceSquare);
      }
    }

    if ((side == White) ? piece == WRook : piece == BRook) {
      while (bitboard) {
        sourceSquare = LSOneIdx(bitboard);
        attacks = getRookAttacks(static_cast<Square>(sourceSquare), occupancies[Both])
            & ((side == White) ? ~occupancies[White] : ~occupancies[Black]);
        while (attacks) {
          targetSquare = LSOneIdx(attacks);
          if (!getBit(((side == White) ? occupancies[Black] : occupancies[White]), targetSquare)) {
            addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, 0, 0, 0, 0, 0));
          } else {
            addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, 0, 1, 0, 0, 0));
          }
          remBit(attacks, targetSquare);
        }
        remBit(bitboard, sourceSquare);
      }
    }

    if ((side == White) ? piece == WQueen : piece == BQueen) {
      while (bitboard) {
        sourceSquare = LSOneIdx(bitboard);
        attacks = getQueenAttacks(static_cast<Square>(sourceSquare), occupancies[Both])
            & ((side == White) ? ~occupancies[White] : ~occupancies[Black]);
        while (attacks) {
          targetSquare = LSOneIdx(attacks);
          if (!getBit(((side == White) ? occupancies[Black] : occupancies[White]), targetSquare)) {
            addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, 0, 0, 0, 0, 0));
          } else {
            addMove(moveList, encodeMove(sourceSquare, targetSquare, piece, 0, 1, 0, 0, 0));
          }
          remBit(attacks, targetSquare);
        }
        remBit(bitboard, sourceSquare);
      }
    }

    if ((side == White) ? piece == WKing : piece == BKing) {
      while (bitboard) {
        sourceSquare = LSOneIdx(bitboard);
        attacks = kingAttacks[sourceSquare] & ((side == White) ? ~occupancies[White] : ~occupancies[Black]);
        while (attacks) {
          targetSquare = LSOneIdx(attacks);
          if (!getBit(((side == White) ? occupancies[Black] : occupancies[White]), targetSquare)) {
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