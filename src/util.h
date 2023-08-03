//
// Created by Jikun on 25/6/23.
//

#ifndef MINERVA_CHESS_SRC_UTIL_H_
#define MINERVA_CHESS_SRC_UTIL_H_

#include "types.h"
#include <bit>
#include <random>
#include <iostream>
#ifdef WIN64
#include <windows.h>
#else
# include <sys/time.h>
#endif

// Pseudorandom number generator
inline U64 rand64() {
  static std::mt19937 rng(0);
  return 1LL * rng() * rng();
}

inline int LSOneIdx(U64 bitboard) {
  if (bitboard) {
    return std::popcount((bitboard & -bitboard) - 1);
  } else {
    return -1;
  }
}

inline int getTimeMs() {
#ifdef WIN64
  return GetTickCount();
#else
  struct timeval time_value{};
  gettimeofday(&time_value, nullptr);
  return time_value.tv_sec * 1000 + time_value.tv_usec / 1000;
#endif
}

#endif //MINERVA_CHESS_SRC_UTIL_H_
