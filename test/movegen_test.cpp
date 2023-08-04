//
// Created by Jikun on 23/7/23.
//

#include <gtest/gtest.h>
#include "../src/types.h"
#include "../src/uci.h"
#include "../src/movegen.h"
#include "../src/transposition.h"
#include "../src/nnue/nnue.h"

// init all variables
void initAll() {
  // init leaper pieces attacks
  initLeapersAttacks();

  // init slider pieces attacks
  initSlidersAttacks(Bishop);
  initSlidersAttacks(Rook);

  // init random keys for hashing purposes
  initialiseKeys();

  // init hash table with default 256 MB
  initHashTable(128);

  // init NNUE weights
  nnue_init("nn-eba324f53044.nnue");
}

TEST(GenerateMoves, startpos_test) {
  initAll();
  parsePosition("position startpos");
  moves moveList;
  generateMoves(moveList);
  EXPECT_EQ(moveList.size(), 20);
  std::vector<std::pair<int, int>> generatedMoves;
  for (auto move : moveList)
    generatedMoves.emplace_back(getMoveSource(move), getMoveTarget(move));
  std::vector<std::pair<int, int>> expectedMoves = {
      {a2, a3},
      {a2, a4},
      {b2, b3},
      {b2, b4},
      {c2, c3},
      {c2, c4},
      {d2, d3},
      {d2, d4},
      {e2, e3},
      {e2, e4},
      {f2, f3},
      {f2, f4},
      {g2, g3},
      {g2, g4},
      {h2, h3},
      {h2, h4},
      {b1, a3},
      {b1, c3},
      {g1, f3},
      {g1, h3},
  };
  EXPECT_EQ(generatedMoves, expectedMoves);
}

TEST(MakeMove, validMove) {
  initAll();
  parsePosition("position startpos");
  int move = encodeMove(e2, e4, WPawn, 0, 0, 1, 0, 0);
  EXPECT_EQ(makeMove(move, ALL_MOVES), 1);
}

TEST(MakeMove, invalidMove) {
  initAll();
  parsePosition("position fen 8/8/3k4/8/3K4/8/8/8 w - - 0 1");
  int move = encodeMove(d4, d5, WKing, 0, 0, 0, 0, 0);
  EXPECT_EQ(makeMove(move, ALL_MOVES), 0);
}
