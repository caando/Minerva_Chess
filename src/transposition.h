//
// Created by Jikun on 26/6/23.
//

#ifndef MINERVA_CHESS_SRC_TRANSPOSITION_H_
#define MINERVA_CHESS_SRC_TRANSPOSITION_H_

#include "types.h"
#include "movegen.h"

#define INFINITY 50000
#define MATE_VALUE 49000
#define MATE_SCORE 48000

/*
    most valuable victim & less valuable attacker

    (Victims) Pawn Knight Bishop   Rook  Queen   King
  (Attackers)
        Pawn   105    205    305    405    505    605
      Knight   104    204    304    404    504    604
      Bishop   103    203    303    403    503    603
        Rook   102    202    302    402    502    602
       Queen   101    201    301    401    501    601
        King   100    200    300    400    500    600
*/

// MVV LVA [attacker][victim]
static int MvvLva[12][12] = {
    105, 205, 305, 405, 505, 605,  105, 205, 305, 405, 505, 605,
    104, 204, 304, 404, 504, 604,  104, 204, 304, 404, 504, 604,
    103, 203, 303, 403, 503, 603,  103, 203, 303, 403, 503, 603,
    102, 202, 302, 402, 502, 602,  102, 202, 302, 402, 502, 602,
    101, 201, 301, 401, 501, 601,  101, 201, 301, 401, 501, 601,
    100, 200, 300, 400, 500, 600,  100, 200, 300, 400, 500, 600,

    105, 205, 305, 405, 505, 605,  105, 205, 305, 405, 505, 605,
    104, 204, 304, 404, 504, 604,  104, 204, 304, 404, 504, 604,
    103, 203, 303, 403, 503, 603,  103, 203, 303, 403, 503, 603,
    102, 202, 302, 402, 502, 602,  102, 202, 302, 402, 502, 602,
    101, 201, 301, 401, 501, 601,  101, 201, 301, 401, 501, 601,
    100, 200, 300, 400, 500, 600,  100, 200, 300, 400, 500, 600
};

// max ply that we can reach within a search
#define MAX_PLY 64

// killer moves [id][ply]
int killer_moves[2][MAX_PLY];

// history moves [piece][square]
int history_moves[PieceCount][MAX_PLY];

/*
      ================================
            Triangular PV table
      --------------------------------
        PV line: e2e4 e7e5 g1f3 b8c6
      ================================

           0    1    2    3    4    5

      0    m1   m2   m3   m4   m5   m6

      1    0    m2   m3   m4   m5   m6

      2    0    0    m3   m4   m5   m6

      3    0    0    0    m4   m5   m6

      4    0    0    0    0    m5   m6

      5    0    0    0    0    0    m6
*/

// PV length [ply]
int pvLength[MAX_PLY];

// PV table [ply][ply]
int pvTable[MAX_PLY][MAX_PLY];

// follow PV & score PV move
int followPv, scorePv;

// number hash table entries
int hashEntries = 0;

// no hash entry found constant
#define NO_HASH_ENTRY 100000

// transposition table hash flags
#define HASH_FLAG_EXACT 0
#define HASH_FLAG_ALPHA 1
#define HASH_FLAG_BETA 2

// transposition table data structure
typedef struct {
  U64 hash_key;   // "almost" unique chess position identifier
  int depth;      // current search depth
  int flag;       // flag the type of node (fail-low/fail-high/PV)
  int score;      // score (alpha/beta/PV)
  int best_move;
} tt;               // transposition table (TT aka hash table)

// define TT instance
tt *hash_table = nullptr;

void clearHashTable();

void initHashTable(int mb);

static inline int readHashEntry(int alpha, int beta, int* bestMove, int depth);

static inline void writeHashEntry(int score, int best_move, int depth, int hashFlag);

static inline void enable_pv_scoring(moves *move_list);

#endif //MINERVA_CHESS_SRC_TRANSPOSITION_H_
