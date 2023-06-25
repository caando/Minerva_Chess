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
  static U64 next = 123456789;

  next = next * 1000000009 + 1234511;
  return next;
}

static inline int LSOneIdx(Bitboard bitboard) {
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
