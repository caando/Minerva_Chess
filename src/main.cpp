//
// Created by Jikun on 26/6/23.
//

#include "attack.h"
#include "zobrist.h"
#include "transposition.h"
#include "nnue/nnue.h"
#include "uci.h"

void initAll() {
  // TODO: move to compile time
  initBishopAttacks();
  initRookAttacks();

  initialiseKeys();

  initHashTable(128);

  nnue_init("nn-eba324f53044.nnue");
}

int main() {
  initAll();

  uciLoop();

  free(hashTable);

  return 0;
}