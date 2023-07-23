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

// transposition table data structure
typedef struct {
  U64 hash_key;   // "almost" unique chess position identifier
  int depth;      // current search depth
  int flag;       // flag the type of node (fail-low/fail-high/PV)
  int score;      // score (alpha/beta/PV)
  int best_move;
} tt;               // transposition table (TT aka hash table)

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

// killer moves [id][ply]
extern int killer_moves[2][MAX_PLY];

// history moves [piece][square]
extern int history_moves[PieceCount][SquareCount];

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
extern int pvLength[MAX_PLY];

// PV table [ply][ply]
extern int pvTable[MAX_PLY][MAX_PLY];

// follow PV & score PV move
extern int followPv, scorePv;

// number hash table entries
extern int hashEntries;

// define TT instance
extern tt *hash_table;

// no hash entry found constant
#define NO_HASH_ENTRY 100000

// transposition table hash flags
#define HASH_FLAG_EXACT 0
#define HASH_FLAG_ALPHA 1
#define HASH_FLAG_BETA 2


void clearHashTable();

void initHashTable(int mb);

int readHashEntry(int alpha, int beta, int* bestMove, int depth);

void writeHashEntry(int score, int best_move, int depth, int hashFlag);

void enable_pv_scoring(moves &move_list);

#endif //MINERVA_CHESS_SRC_TRANSPOSITION_H_
