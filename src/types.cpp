//
// Created by Jikun on 26/6/23.
//

#include "types.h"

const char *squareToCoordinates[] = {
    "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8",
    "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
    "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
    "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
    "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
    "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
    "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
    "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
};

const char *piecesToAscii[PieceCount] = {"P", "N", "B", "R", "Q", "K", "p", "n", "b", "r", "q", "k"};

const char *piecesToUnicode[PieceCount] = {"♟︎", "♞", "♝", "♜", "♛", "♚", "♙", "♘", "♗", "♖", "♕", "♔"};

const Piece asciiToPieces[128] = {
    ['P'] = Piece::WPawn,
    ['N'] = Piece::WKnight,
    ['B'] = Piece::WBishop,
    ['R'] = Piece::WRook,
    ['Q'] = Piece::WQueen,
    ['K'] = Piece::WKing,
    ['p'] = Piece::BPawn,
    ['n'] = Piece::BKnight,
    ['b'] = Piece::BBishop,
    ['r'] = Piece::BRook,
    ['q'] = Piece::BQueen,
    ['k'] = Piece::BKing
};

// promoted pieces
const char promotedPieces[PieceCount] = {
    [WQueen] = 'q',
    [WRook] = 'r',
    [WBishop] = 'b',
    [WKnight] = 'n',
    [BQueen] = 'q',
    [BRook] = 'r',
    [BBishop] = 'b',
    [BKnight] = 'n'
};