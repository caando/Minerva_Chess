//
// Created by Jikun on 26/6/23.
//

#include "transposition.h"
#include "variables.h"
#include <cstdio>

// killer moves [id][ply]
int killer_moves[2][MAX_PLY] = {0};

// history moves [piece][square]
int history_moves[PieceCount][SquareCount] = {0};

// PV length [ply]
int pvLength[MAX_PLY] = {0};

// PV table [ply][ply]
int pvTable[MAX_PLY][MAX_PLY] = {0};

// follow PV & score PV move
int followPv = 0, scorePv = 0;

// number hash table entries
int hashEntries = 0;

// define TT instance
tt *hash_table = nullptr;

// clear TT (hash table)
void clearHashTable()
{
  // init hash table entry pointer
  tt *hash_entry;

  // loop over TT elements
  for (hash_entry = hash_table; hash_entry < hash_table + hashEntries; hash_entry++)
  {
    // reset TT inner fields
    hash_entry->hash_key = 0;
    hash_entry->depth = 0;
    hash_entry->flag = 0;
    hash_entry->score = 0;
  }
}

// dynamically allocate memory for hash table
void initHashTable(int mb)
{
  // init hash size
  int hashSize = 0x100000 * mb;

  // init number of hash entries
  hashEntries =  hashSize / sizeof(tt);

  // free hash table if not empty
  if (hash_table != nullptr)
  {
    printf("    Clearing hash memory...\n");

    // free hash table dynamic memory
    free(hash_table);
  }

  // allocate memory
  hash_table = (tt *) malloc(hashEntries * sizeof(tt));

  // if allocation has failed
  if (hash_table == nullptr)
  {
    printf("    Couldn't allocate memory for hash table, tryinr %dMB...", mb / 2);

    // try to allocate with half size
    initHashTable(mb / 2);
  }

    // if allocation succeeded
  else
  {
    // clear hash table
    clearHashTable();

    printf("Hash table is initialied with %d entries\n", hashEntries);
  }


}

// read hash entry data
int readHashEntry(int alpha, int beta, int* bestMove, int depth)
{
  // create a TT instance pointer to particular hash entry storing
  // the scoring data for the current board position if available
  tt *hash_entry = &hash_table[hashKey % hashEntries];

  // make sure we're dealing with the exact position we need
  if (hash_entry->hash_key == hashKey)
  {
    // make sure that we match the exact depth our search is now at
    if (hash_entry->depth >= depth)
    {
      // extract stored score from TT entry
      int score = hash_entry->score;

      // retrieve score independent from the actual path
      // from root node (position) to current node (position)
      if (score < -MATE_SCORE) score += ply;
      if (score > MATE_SCORE) score -= ply;

      // match the exact (PV node) score
      if (hash_entry->flag == HASH_FLAG_EXACT)
        // return exact (PV node) score
        return score;

      // match alpha (fail-low node) score
      if ((hash_entry->flag == HASH_FLAG_ALPHA) &&
          (score <= alpha))
        // return alpha (fail-low node) score
        return alpha;

      // match beta (fail-high node) score
      if ((hash_entry->flag == HASH_FLAG_BETA) &&
          (score >= beta))
        // return beta (fail-high node) score
        return beta;
    }

    // store best move
    *bestMove = hash_entry->best_move;
  }

  // if hash entry doesn't exist
  return NO_HASH_ENTRY;
}

// write hash entry data
void writeHashEntry(int score, int best_move, int depth, int hashFlag)
{
  // create a TT instance pointer to particular hash entry storing
  // the scoring data for the current board position if available
  tt *hash_entry = &hash_table[hashKey % hashEntries];

  // store score independent from the actual path
  // from root node (position) to current node (position)
  if (score < -MATE_SCORE) score -= ply;
  if (score > MATE_SCORE) score += ply;

  // write hash entry data
  hash_entry->hash_key = hashKey;
  hash_entry->score = score;
  hash_entry->flag = hashFlag;
  hash_entry->depth = depth;
  hash_entry->best_move = best_move;
}

// enable PV move scoring
void enable_pv_scoring(moves &move_list)
{
  // disable following PV
  followPv = 0;

  // loop over the moves within a move list
  for (int count = 0; count < move_list.size(); count++)
  {
    // make sure we hit PV move
    if (pvTable[0][ply] == move_list[count])
    {
      // enable move scoring
      scorePv = 1;

      // enable following PV
      followPv = 1;
    }
  }
}