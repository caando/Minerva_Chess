//
// Created by Jikun on 25/6/23.
//

#include "util.h"

// Pseudorandom number generator
U64 rand64() {
  static U64 next = 123456789;

  next = next * 1000000009 + 1234511;
  return next;
}