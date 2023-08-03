//
// Created by Jikun on 26/6/23.
//

#include "transposition.h"
#include "variables.h"
#include <cstdio>

int killerMoves[2][MAX_PLY] = {{0}};
int historyMoves[PieceCount][SquareCount] = {{0}};
int pvLength[MAX_PLY] = {0};
int pvTable[MAX_PLY][MAX_PLY] = {{0}};
int followPv = 0, scorePv = 0;
int hashEntries = 0;
TranspositionTable *hashTable = nullptr;

void clearHashTable() {
  TranspositionTable *hash_entry;
  for (hash_entry = hashTable; hash_entry < hashTable + hashEntries; hash_entry++) {
    hash_entry->hash_key = 0;
    hash_entry->depth = 0;
    hash_entry->flag = 0;
    hash_entry->score = 0;
  }
}

void initHashTable(int mb) {
  int hashSize = 0x100000 * mb;
  hashEntries = hashSize / sizeof(TranspositionTable);
  if (hashTable != nullptr) {
    printf("    Clearing hash memory...\n");
    free(hashTable);
  }
  hashTable = (TranspositionTable *) malloc(hashEntries * sizeof(TranspositionTable));
  if (hashTable == nullptr) {
    printf("    Couldn't allocate memory for hash table, tryinr %dMB...", mb / 2);
    initHashTable(mb / 2);
  } else {
    clearHashTable();
    printf("Hash table is initialised with %d entries\n", hashEntries);
  }
}

int readHashEntry(int alpha, int beta, int *bestMove, int depth) {
  TranspositionTable *hashEntry = &hashTable[hashKey % hashEntries];
  if (hashEntry->hash_key == hashKey) {
    if (hashEntry->depth >= depth) {
      int score = hashEntry->score;
      if (score < -MATE_SCORE) {
        score += ply;
      }
      if (score > MATE_SCORE) {
        score -= ply;
      }
      if (hashEntry->flag == HASH_FLAG_EXACT) {
        return score;
      }
      if ((hashEntry->flag == HASH_FLAG_ALPHA) && (score <= alpha)) {
        return alpha;
      }
      if ((hashEntry->flag == HASH_FLAG_BETA) && (score >= beta)) {
        return beta;
      }
    }
    *bestMove = hashEntry->best_move;
  }
  return NO_HASH_ENTRY;
}

void writeHashEntry(int score, int best_move, int depth, int hashFlag) {
  TranspositionTable *hashEntry = &hashTable[hashKey % hashEntries];
  if (score < -MATE_SCORE) score -= ply;
  if (score > MATE_SCORE) score += ply;
  hashEntry->hash_key = hashKey;
  hashEntry->score = score;
  hashEntry->flag = hashFlag;
  hashEntry->depth = depth;
  hashEntry->best_move = best_move;
}

void enable_pv_scoring(moves &move_list) {
  followPv = 0;
  for (int count : move_list) {
    if (pvTable[0][ply] == count) {
      scorePv = 1;
      followPv = 1;
    }
  }
}