//
// Created by Jikun on 26/6/23.
//

#include "variables.h"
#include "movegen.h"
#include "util.h"
#include "types.h"

// perft driver
inline void perftDriver(int depth) {
  // reccursion escape condition
  if (depth == 0) {
    // increment nodes count (count reached positions)
    nodes++;
    return;
  }

  // create move list instance
  moves move_list;

  // generate moves
  generateMoves(move_list);

  // loop over generated moves
  for (int move_count = 0; move_count < move_list.size(); move_count++) {
    // preserve board state
    copyBoard();

    // make move
    if (!makeMove(move_list[move_count], all_moves))
      // skip to the next move
      continue;

    // call perft driver recursively
    perftDriver(depth - 1);

    // take back
    takeBack();
  }
}

// perft test
void perftTest(int depth) {
  // reset nodes count
  nodes = 0ULL;

  printf("\n     Performance test\n\n");

  // create move list instance
  moves move_list;

  // generate moves
  generateMoves(move_list);

  // init start time
  long start = getTimeMs();

  // loop over generated moves
  for (int move_count = 0; move_count < move_list.size(); move_count++) {
    // preserve board state
    copyBoard();

    // make move
    if (!makeMove(move_list[move_count], all_moves))
      // skip to the next move
      continue;

    // cummulative nodes
    long cummulative_nodes = nodes;

    // call perft driver recursively
    perftDriver(depth - 1);

    // old nodes
    long old_nodes = nodes - cummulative_nodes;

    // take back
    takeBack();

    // print move
    printf("     move: %s%s%c  nodes: %ld\n", squareToCoordinates[getMoveSource(move_list[move_count])],
           squareToCoordinates[getMoveTarget(move_list[move_count])],
           getMovePromoted(move_list[move_count]) ? promotedPieces[getMovePromoted(move_list[move_count])]
                                                         : ' ',
           old_nodes);
  }

  // print results
  printf("\n    Depth: %d\n", depth);
  printf("    Nodes: %lld\n", nodes);
  printf("     Time: %ld\n\n", getTimeMs() - start);
}