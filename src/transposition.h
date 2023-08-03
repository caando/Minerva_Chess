//
// Created by Jikun on 26/6/23.
//

#ifndef MINERVA_CHESS_SRC_TRANSPOSITION_H_
#define MINERVA_CHESS_SRC_TRANSPOSITION_H_

#include "types.h"
#include "movegen.h"

#define INFINITY_CHESS 50000
#define MATE_VALUE 49000
#define MATE_SCORE 48000
// max ply that we can reach within a search
#define MAX_PLY 64

typedef struct {
  U64 hash_key;
  int depth;
  int flag;
  int score;
  int best_move;
} TranspositionTable;

// MVV LVA [attacker][victim]
constexpr int MvvLva[PieceCount][PieceCount] = {
{105, 205, 305, 405, 505, 605,  105, 205, 305, 405, 505, 605},
{104, 204, 304, 404, 504, 604,  104, 204, 304, 404, 504, 604},
{103, 203, 303, 403, 503, 603,  103, 203, 303, 403, 503, 603},
{102, 202, 302, 402, 502, 602,  102, 202, 302, 402, 502, 602},
{101, 201, 301, 401, 501, 601,  101, 201, 301, 401, 501, 601},
{100, 200, 300, 400, 500, 600,  100, 200, 300, 400, 500, 600},

{105, 205, 305, 405, 505, 605,  105, 205, 305, 405, 505, 605},
{104, 204, 304, 404, 504, 604,  104, 204, 304, 404, 504, 604},
{103, 203, 303, 403, 503, 603,  103, 203, 303, 403, 503, 603},
{102, 202, 302, 402, 502, 602,  102, 202, 302, 402, 502, 602},
{101, 201, 301, 401, 501, 601,  101, 201, 301, 401, 501, 601},
{100, 200, 300, 400, 500, 600,  100, 200, 300, 400, 500, 600}
};

extern int killerMoves[2][MAX_PLY];
extern int historyMoves[PieceCount][SquareCount];
extern int pvLength[MAX_PLY];
extern int pvTable[MAX_PLY][MAX_PLY];
extern int followPv, scorePv;
extern int hashEntries;
extern TranspositionTable *hashTable;

#define NO_HASH_ENTRY 100000

#define HASH_FLAG_EXACT 0
#define HASH_FLAG_ALPHA 1
#define HASH_FLAG_BETA 2


void clearHashTable();

void initHashTable(int mb);

int readHashEntry(int alpha, int beta, int* bestMove, int depth);

void writeHashEntry(int score, int best_move, int depth, int hashFlag);

void enable_pv_scoring(moves &move_list);

#endif //MINERVA_CHESS_SRC_TRANSPOSITION_H_
