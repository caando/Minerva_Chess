//
// Created by Jikun on 26/6/23.
//

#include "attack.h"
#include "magic.h"
#include "zobrist.h"
#include "transposition.h"
#include "nnue/nnue.h"
#include "uci.h"

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

int main()
{
  // init all
  initAll();



  // connect to GUI
//  uciLoop();
  parsePosition("position startpos");
  parseGo("movetime 1000");


  // free hash table memory on exit
  free(hash_table);

  return 0;
}