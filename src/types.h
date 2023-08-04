//
// Created by Jikun on 21/6/23.
//

#ifndef MINERVA_CHESS_SRC_CONSTANTS_H_
#define MINERVA_CHESS_SRC_CONSTANTS_H_

#include <vector>

#define U64 unsigned long long

#define EMPTY_BOARD "8/8/8/8/8/8/8/8 b - - "
#define START_BOARD "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 "

enum Square {
  a8, b8, c8, d8, e8, f8, g8, h8,
  a7, b7, c7, d7, e7, f7, g7, h7,
  a6, b6, c6, d6, e6, f6, g6, h6,
  a5, b5, c5, d5, e5, f5, g5, h5,
  a4, b4, c4, d4, e4, f4, g4, h4,
  a3, b3, c3, d3, e3, f3, g3, h3,
  a2, b2, c2, d2, e2, f2, g2, h2,
  a1, b1, c1, d1, e1, f1, g1, h1,
  SquareCount, no_sq
};

#define iterSquare() for (int square = 0; square < SquareCount; square++)

enum Piece {
  WPawn,
  WKnight,
  WBishop,
  WRook,
  WQueen,
  WKing,
  BPawn,
  BKnight,
  BBishop,
  BRook,
  BQueen,
  BKing,
  PieceCount = 12
};

#define iterPiece() for (int piece = 0; piece < PieceCount; piece++)

enum Colour {
  White,
  Black,
  Both,
  ColourCount = 3
};

enum Castle {
  WhiteKingSide = 1,
  WhiteQueenSide = 2,
  BlackKingSide = 4,
  BlackQueenSide = 8,
};

#define iterCastle() for (int castle = 0; castle <= (WhiteKingSide | WhiteQueenSide | BlackKingSide | BlackQueenSide); castle++)

constexpr char *squareToCoordinates[] = {
"a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8",
"a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
"a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
"a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
"a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
"a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
"a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
"a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
};

constexpr char *piecesToAscii[PieceCount] = {"P", "N", "B", "R", "Q", "K", "p", "n", "b", "r", "q", "k"};

constexpr char *piecesToUnicode[PieceCount] = {"♟︎", "♞", "♝", "♜", "♛", "♚", "♙", "♘", "♗", "♖", "♕", "♔"};

constexpr int asciiToPieces[128] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, Piece::WBishop, 0, 0, 0, 0, 0, 0, 0, 0, Piece::WKing, 0, 0, Piece::WKnight, 0,
    Piece::WPawn, Piece::WQueen, Piece::WRook, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, Piece::BBishop, 0, 0, 0, 0, 0, 0, 0, 0, Piece::BKing, 0, 0, Piece::BKnight, 0,
    Piece::BPawn, Piece::BQueen, Piece::BRook, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

// promoted pieces
constexpr char promotedPieces[PieceCount] = {
    'p',
    'n', // WKnight
    'b', // WBishop
    'r', // WRook
    'q', // WQueen
    'p',
    'k',
    'n', // BKnight
    'b', // BBishop
    'r', // BRook
    'q', // BQueen
    'k'
};

typedef U64 Bitboard;

#define setBit(bitboard, square) ((bitboard) |= (1ULL << (square)))
#define getBit(bitboard, square) ((bitboard) & (1ULL << (square)))
#define remBit(bitboard, square) ((bitboard) &= ~(1ULL << (square)))

typedef std::vector<int> moves;

#pragma pack(push, 1)
struct BoardState {
  Bitboard bitboards[PieceCount];
  Bitboard occupancies[ColourCount];
  Colour side;
  Square enpassant;
  int castle;
  int fifty;
  U64 hashKey;
};
#pragma pack(pop)

#endif //MINERVA_CHESS_SRC_CONSTANTS_H_
