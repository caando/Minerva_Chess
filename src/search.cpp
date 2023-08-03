//
// Created by Jikun on 26/6/23.
//

#include <string>
#include "search.h"
#include "transposition.h"
#include "variables.h"
#include "eval.h"
#include "zobrist.h"
#include "util.h"
#include "communication.h"
#include "debug.h"
#include <array>

inline int scoreMove(int move) {
  if (scorePv) {
    if (pvTable[0][ply] == move) {
      scorePv = 0;
      return 20000;
    }
  }

  if (getMoveCapture(move)) {
    int piece = getMovePiece(move);
    int targetPiece = WPawn;
    int startPiece, endPiece;

    if (side == White) {
      startPiece = BPawn;
      endPiece = BKing;
    } else {
      startPiece = WPawn;
      endPiece = WKing;
    }

    for (int bbPiece = startPiece; bbPiece <= endPiece; bbPiece++) {
      if (getBit(bitboards[bbPiece], getMoveTarget(move))) {
        targetPiece = bbPiece;
        break;
      }
    }

    /*
    int source_square = get_move_source(move);
    int target_square = getMoveTarget(move);
    pop_bit(bitboards[piece], source_square);
    if (!is_square_attacked(target_square, side ^ 1)) {
        // restore captured piece
        set_bit(bitboards[piece], source_square);

        return 15000;
    }
    set_bit(bitboards[piece], source_square);
     */

    return MvvLva[piece][targetPiece] + 10000;
  } else {
    if (killerMoves[0][ply] == move) {
      return 9000;
    } else if (killerMoves[1][ply] == move) {
      return 8000;
    } else {
      return historyMoves[getMovePiece(move)][getMoveTarget(move)];
    }
  }
}

inline int sortMoves(moves &moveList, int bestMove) {
  std::array<int, 256> moveScores{};

  for (int count = 0; count < moveList.size(); count++) {
    if (bestMove == moveList[count]) {
      moveScores[count] = 30000;
    } else {
      moveScores[count] = scoreMove(moveList[count]);
    }
  }

  for (int curMove = 0; curMove < moveList.size(); curMove++) {
    for (int next_move = curMove + 1; next_move < moveList.size(); next_move++) {
      if (moveScores[curMove] < moveScores[next_move]) {
        std::swap(moveScores[curMove], moveScores[next_move]);
        std::swap(moveList[curMove], moveList[next_move]);
      }
    }
  }
  return 0;
}

inline int isRepetition() {
  for (int index = 0; index < repetitionIndex; index++) {
    if (repetitionTable[index] == hashKey) {
      return 1;
    }
  }
  return 0;
}

inline int quiescence(int alpha, int beta) {
  if ((nodes & 0b11111111111) == 0) {
    communicate();
  }
  nodes++;
  if (ply > MAX_PLY - 1) {
    return evaluate();
  }

  int evaluation = evaluate();

  if (evaluation >= beta) {
    return beta;
  }
  if (evaluation > alpha) {
    alpha = evaluation;
  }

  moves moveList;
  generateMoves(moveList);
  sortMoves(moveList, 0);

  for (int move : moveList) {
    saveBoard();
    ply++;
    repetitionIndex++;
    repetitionTable[repetitionIndex] = hashKey;

    if (makeMove(move, ONLY_CAPTURES) == 0) {
      ply--;
      repetitionIndex--;
      continue;
    }

    int score = -quiescence(-beta, -alpha);
    ply--;
    repetitionIndex--;
    takeBack();
    if (stopped == 1) return 0;
    if (score > alpha) {
      alpha = score;
      if (score >= beta) {
        return beta;
      }
    }
  }
  return alpha;
}

inline int negamax(int alpha, int beta, int depth) {
  pvLength[ply] = ply;
  int score;
  int bestMove = 0;
  int hashFlag = HASH_FLAG_ALPHA;

  if ((ply && isRepetition()) || fifty >= 100) {
    return 0;
  }

  int pvNode = beta - alpha > 1;

  if (ply && (score = readHashEntry(alpha, beta, &bestMove, depth)) != NO_HASH_ENTRY && pvNode == 0) {
    return score;
  }

  if ((nodes & 0b11111111111) == 0) {
    communicate();
  }
  if (depth == 0) {
    return quiescence(alpha, beta);
  }
  if (ply > MAX_PLY - 1) {
    return evaluate();
  }

  nodes++;

  int inCheck = isSquareAttacked(static_cast<Square>((side == White) ? LSOneIdx(bitboards[WKing]) :
                                                     LSOneIdx(bitboards[BKing])),
                                 side == White ? Black : White);

  if (inCheck) depth++;
  int legalMoves = 0;
  int staticEval = evaluate();
  if (depth < 3 && !pvNode && !inCheck && abs(beta - 1) > -INFINITY_CHESS + 100) {
    int evalMargin = 120 * depth;
    if (staticEval - evalMargin >= beta) {
      return staticEval - evalMargin;
    }
  }

  if (depth >= 3 && inCheck == 0 && ply) {
    saveBoard();
    ply++;
    repetitionIndex++;
    repetitionTable[repetitionIndex] = hashKey;
    if (enpassant != no_sq) hashKey ^= enpassantKey[enpassant];
    enpassant = no_sq;
    if (side == White) {
      side = Black;
    } else {
      side = White;
    }
    hashKey ^= sideKey;
    score = -negamax(-beta, -beta + 1, depth - 1 - 2);

    ply--;

    repetitionIndex--;

    takeBack();

    if (stopped == 1) {
      return 0;
    }
    if (score >= beta) {
      return beta;
    }
  }

  if (!pvNode && !inCheck && depth <= 3) {
    score = staticEval + 125;

    int newScore;

    if (score < beta) {
      if (depth == 1) {
        newScore = quiescence(alpha, beta);

        return (newScore > score) ? newScore : score;
      }

      score += 175;

      if (score < beta && depth <= 2) {
        newScore = quiescence(alpha, beta);

        if (newScore < beta)
          return (newScore > score) ? newScore : score;
      }
    }
  }

  moves moveList;
  moveList.reserve(256);

  generateMoves(moveList);

  if (followPv) {
    enable_pv_scoring(moveList);
  }

  sortMoves(moveList, bestMove);

  int movesSearched = 0;

  for (int count : moveList) {
    saveBoard();

    ply++;

    repetitionIndex++;
    repetitionTable[repetitionIndex] = hashKey;

    if (makeMove(count, ALL_MOVES) == 0) {
      ply--;

      repetitionIndex--;

      continue;
    }

    legalMoves++;

    if (movesSearched == 0) {
      score = -negamax(-beta, -alpha, depth - 1);
    } else {
      if (
          movesSearched >= fullDepthMoves &&
              depth >= reductionLimit &&
              inCheck == 0 &&
              getMoveCapture(count) == 0 &&
              getMovePromoted(count) == 0
          ) {
        score = -negamax(-alpha - 1, -alpha, depth - 2);
      } else {
        score = alpha + 1;
      }

      if (score > alpha) {
        score = -negamax(-alpha - 1, -alpha, depth - 1);
        if ((score > alpha) && (score < beta)) {
          score = -negamax(-beta, -alpha, depth - 1);
        }
      }
    }

    ply--;

    repetitionIndex--;

    takeBack();

    if (stopped == 1) {
      return 0;
    }

    movesSearched++;

    if (score > alpha) {
      hashFlag = HASH_FLAG_EXACT;

      bestMove = count;

      if (getMoveCapture(count) == 0)
        historyMoves[getMovePiece(count)][getMoveTarget(count)] += depth;

      alpha = score;

      pvTable[ply][ply] = count;

      for (int nextPly = ply + 1; nextPly < pvLength[ply + 1]; nextPly++) {
        pvTable[ply][nextPly] = pvTable[ply + 1][nextPly];
      }

      pvLength[ply] = pvLength[ply + 1];

      if (score >= beta) {
        writeHashEntry(beta, bestMove, depth, HASH_FLAG_BETA);
        if (getMoveCapture(count) == 0) {
          killerMoves[1][ply] = killerMoves[0][ply];
          killerMoves[0][ply] = count;
        }
        return beta;
      }
    }
  }

  if (legalMoves == 0) {
    if (inCheck) {
      return -MATE_VALUE + ply;
    } else {
      return 0;
    }
  }

  writeHashEntry(alpha, bestMove, depth, hashFlag);

  return alpha;
}

void searchPosition(int depth) {
  int start = getTimeMs();
  int score;
  nodes = 0;
  stopped = 0;
  followPv = 0;
  scorePv = 0;

  memset(killerMoves, 0, sizeof(killerMoves));
  memset(historyMoves, 0, sizeof(historyMoves));
  memset(pvTable, 0, sizeof(pvTable));
  memset(pvLength, 0, sizeof(pvLength));

  int alpha = -INFINITY_CHESS;
  int beta = INFINITY_CHESS;

  for (int currentDepth = 1; currentDepth <= depth; currentDepth++) {
    if (stopped == 1) {
      break;
    }
    followPv = 1;
    score = negamax(alpha, beta, currentDepth);

    if ((score <= alpha) || (score >= beta)) {
      alpha = -INFINITY_CHESS;
      beta = INFINITY_CHESS;
      continue;
    }

    alpha = score - 50;
    beta = score + 50;

    if (pvLength[0]) {
      if (score > -MATE_VALUE && score < -MATE_SCORE) {
        printf("info score mate %d depth %d nodes %lld time %d pv ",
               -(score + MATE_VALUE) / 2 - 1,
               currentDepth,
               nodes,
               getTimeMs() - start);
      } else if (score > MATE_SCORE && score < MATE_VALUE) {
        printf("info score mate %d depth %d nodes %lld time %d pv ",
               (MATE_VALUE - score) / 2 + 1,
               currentDepth,
               nodes,
               getTimeMs() - start);
      } else {
        printf("info score cp %d depth %d nodes %lld time %d pv ", score, currentDepth, nodes, getTimeMs() - start);
      }

      for (int count = 0; count < pvLength[0]; count++) {
        printMove(pvTable[0][count]);
        printf(" ");
      }
      printf("\n");
    }
  }

  printf("bestmove ");

  if (pvTable[0][0]) {
    printMove(pvTable[0][0]);
  }
  printf("\n");
}