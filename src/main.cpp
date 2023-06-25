//
// Created by Jikun on 26/6/23.
//

#include "attack.h"
#include "magic.h"
#include "zobrist.h"
#include "nnue/nnue.h"

// init all variables
void initAll() {
  // init leaper pieces attacks
  initLeapersAttacks();

  // init slider pieces attacks
  initSlidersAttacks(Bishop);
  initSlidersAttacks(Rook);

  // init random keys for hashing purposes
  initialiseKeys();

  // init NNUE weights
  nnue_init("nnue/nn-46832cfbead3.nnue");
}

int main()
{
  // init all
  initAll();

  return 0;
}