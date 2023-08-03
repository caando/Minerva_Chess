//
// Created by Jikun on 26/6/23.
//

#include "variables.h"
#include "movegen.h"
#include "util.h"
#include "types.h"

void perftDriver(int depth) {
  if (depth == 0) {
    nodes++;
    return;
  }
  moves move_list;
  generateMoves(move_list);

  for (int move_count : move_list) {
    saveBoard();
    if (!makeMove(move_count, all_moves)) {
      continue;
    }
    perftDriver(depth - 1);
    takeBack();
  }
}

void perftTest(int depth) {
  nodes = 0ULL;
  printf("\n     Performance test\n\n");
  moves moveList;
  generateMoves(moveList);
  long start = getTimeMs();

  for (auto move : moveList) {
    saveBoard();
    if (!makeMove(move, all_moves)) {
      continue;
    }
    long cummulativeNodes = nodes;
    perftDriver(depth - 1);
    long oldNodes = nodes - cummulativeNodes;
    takeBack();
    printf("     move: %s%s%c  nodes: %ld\n",
           squareToCoordinates[getMoveSource(move)],
           squareToCoordinates[getMoveTarget(move)],
           getMovePromoted(move) ? promotedPieces[getMovePromoted(move)] : ' ',
           oldNodes);
  }

  printf("\n    Depth: %d\n", depth);
  printf("    Nodes: %lld\n", nodes);
  printf("     Time: %ld\n\n", getTimeMs() - start);
}