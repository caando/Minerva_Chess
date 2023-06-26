//
// Created by Jikun on 25/6/23.
//

#include "util.h"
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
  signed long long signedBitboard = bitboard;
  return __builtin_ctz(signedBitboard & -(signedBitboard));
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
