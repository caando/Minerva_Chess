//
// Created by Jikun on 25/6/23.
//

#include "util.h"
#include <bit>
#ifdef WIN64
#include <windows.h>
#else
# include <sys/time.h>
#endif

// Pseudorandom number generator
U64 rand64() {
  static U64 next = 1;

  next = next * 1103515245 + 12345;
  return next;
}

int LSOneIdx(U64 bitboard) {
  // make sure bitboard is not 0
  if (bitboard)
  {
    // count trailing bits before LS1B
    return std::popcount((bitboard & -bitboard) - 1);
  }

    //otherwise
  else
    // return illegal index
    return -1;
}

// get time in milliseconds
int getTimeMs() {
#ifdef WIN64
  return GetTickCount();
#else
  struct timeval time_value;
  gettimeofday(&time_value, NULL);
  return time_value.tv_sec * 1000 + time_value.tv_usec / 1000;
#endif
}
