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

// score moves
inline int scoreMove(int move) {
  // if PV move scoring is allowed
  if (scorePv) {
    // make sure we are dealing with PV move
    if (pvTable[0][ply] == move) {
      // disable score PV flag
      scorePv = 0;

      // give PV move the highest score to search it first
      return 20000;
    }
  }

  // score capture move
  if (getMoveCapture(move)) {
    // init source piece
    int piece = getMovePiece(move);

    // init target piece
    int target_piece = WPawn;

    // pick up bitboard piece index ranges depending on side
    int start_piece, end_piece;

    // pick up side to move
    if (side == White) {
      start_piece = BPawn;
      end_piece = BKing;
    } else {
      start_piece = WPawn;
      end_piece = WKing;
    }

    // loop over bitboards opposite to the current side to move
    for (int bb_piece = start_piece; bb_piece <= end_piece; bb_piece++) {
      // if there's a piece on the target square
      if (getBit(bitboards[bb_piece], getMoveTarget(move))) {
        // remove it from corresponding bitboard
        target_piece = bb_piece;
        break;
      }
    }

    // Todo: optimise
    /* extract move features
    int source_square = get_move_source(move);
    int target_square = getMoveTarget(move);

    // make the first capture, so that X-ray defender show up
    pop_bit(bitboards[piece], source_square);

    // captures of undefended pieces are good by definition
    if (!is_square_attacked(target_square, side ^ 1)) {
        // restore captured piece
        set_bit(bitboards[piece], source_square);

        // score undefended captures greater than other captures
        return 15000;
    }

    // restore captured piece
    set_bit(bitboards[piece], source_square);*/

    // score move by MVV LVA lookup [source piece][target piece]
    return MvvLva[piece][target_piece] + 10000;
  }

    // score quiet move
  else {
    // score 1st killer move
    if (killer_moves[0][ply] == move)
      return 9000;

      // score 2nd killer move
    else if (killer_moves[1][ply] == move)
      return 8000;

      // score history move
    else
      return history_moves[getMovePiece(move)][getMoveTarget(move)];
  }
}

// sort moves in descending order
inline int sortMoves(moves &moveList, int best_move) {
  // move scores
  std::vector<int> moveScores(moveList.size(), 0);

  // score all the moves within a move list
  for (int count = 0; count < moveList.size(); count++) {
    // if hash move available
    if (best_move == moveList[count])
      // score move
      moveScores[count] = 30000;

    else
      // score move
      moveScores[count] = scoreMove(moveList[count]);
  }

  // loop over current move within a move list
  for (int curMove = 0; curMove < moveList.size(); curMove++) {
    // loop over next move within a move list
    for (int next_move = curMove + 1; next_move < moveList.size(); next_move++) {
      // compare current and next move scores
      if (moveScores[curMove] < moveScores[next_move]) {
        // swap scores
        int temp_score = moveScores[curMove];
        moveScores[curMove] = moveScores[next_move];
        moveScores[next_move] = temp_score;

        // swap moves
        int temp_move = moveList[curMove];
        moveList[curMove] = moveList[next_move];
        moveList[next_move] = temp_move;
      }
    }
  }
}

// position repetition detection
inline int isRepetition() {
  // loop over repetition indices range
  for (int index = 0; index < repetitionIndex; index++)
    // if we found the hash key same with a current
    if (repetitionTable[index] == hashKey)
      // we found a repetition
      return 1;

  // if no repetition found
  return 0;
}

// quiescence search
inline int quiescence(int alpha, int beta) {
  // every 2047 nodes
  if ((nodes & 2047) == 0)
    // "listen" to the GUI/user input
    communicate();

  // increment nodes count
  nodes++;

  // we are too deep, hence there's an overflow of arrays relying on max ply constant
  if (ply > MAX_PLY - 1)
    // evaluate position
    return evaluate();

  // evaluate position
  int evaluation = evaluate();

  // fail-hard beta cutoff
  if (evaluation >= beta) {
    // node (position) fails high
    return beta;
  }

  // found a better move
  if (evaluation > alpha) {
    // PV node (position)
    alpha = evaluation;
  }

  // create move list instance
  moves moveList;

  // generate moves
  generateMoves(moveList);

  // sort moves
  sortMoves(moveList, 0);

  // loop over moves within a movelist
  for (int count = 0; count < moveList[count]; count++) {
    // preserve board state
    copyBoard();

    // increment ply
    ply++;

    // increment repetition index & store hash key
    repetitionIndex++;
    repetitionTable[repetitionIndex] = hashKey;


    // make sure to make only legal moves
    if (makeMove(moveList[count], only_captures) == 0) {
      // decrement ply
      ply--;

      // decrement repetition index
      repetitionIndex--;

      // skip to next move
      continue;
    }

    // score current move
    int score = -quiescence(-beta, -alpha);

    // decrement ply
    ply--;

    // decrement repetition index
    repetitionIndex--;

    // take move back
    takeBack();

    // return 0 if time is up
    if (stopped == 1) return 0;

    // found a better move
    if (score > alpha) {
      // PV node (position)
      alpha = score;

      // fail-hard beta cutoff
      if (score >= beta) {
        // node (position) fails high
        return beta;
      }
    }
  }

  // node (position) fails low
  return alpha;
}

// negamax alpha beta search
inline int negamax(int alpha, int beta, int depth) {
  // init PV length
  pvLength[ply] = ply;

  // variable to store current move's score (from the static evaluation perspective)
  int score;

  // best move (to store in TT)
  int best_move = 0;

  // define hash flag
  int hash_flag = HASH_FLAG_ALPHA;

  // if position repetition occurs
  if (ply && isRepetition() || fifty >= 100)
    // return draw score
    return 0;

  // a hack by Pedro Castro to figure out whether the current node is PV node or not
  int pv_node = beta - alpha > 1;

  // read hash entry if we're not in a root ply and hash entry is available
  // and current node is not a PV node
  if (ply && (score = readHashEntry(alpha, beta, &best_move, depth)) != NO_HASH_ENTRY && pv_node == 0)
    // if the move has already been searched (hence has a value)
    // we just return the score for this move without searching it
    return score;

  // every 2047 nodes
  if ((nodes & 2047) == 0)
    // "listen" to the GUI/user input
    communicate();

  // recursion escape condition
  if (depth == 0)
    // run quiescence search
    return quiescence(alpha, beta);

  // we are too deep, hence there's an overflow of arrays relying on max ply constant
  if (ply > MAX_PLY - 1)
    // evaluate position
    return evaluate();

  // increment nodes count
  nodes++;

  // is king in check
  int in_check = isSquareAttacked(static_cast<Square>((side == White) ? LSOneIdx(bitboards[WKing]) :
                                                      LSOneIdx(bitboards[BKing])),
                                  side == White ? Black : White);

  // increase search depth if the king has been exposed into a check
  if (in_check) depth++;

  // legal moves counter
  int legal_moves = 0;

  // get static evaluation score
  int staticEval = evaluate();

  // evaluation pruning / static null move pruning
  if (depth < 3 && !pv_node && !in_check && abs(beta - 1) > -INFINITY + 100) {
    // define evaluation margin
    int eval_margin = 120 * depth;

    // evaluation margin substracted from static evaluation score fails high
    if (staticEval - eval_margin >= beta)
      // evaluation margin substracted from static evaluation score
      return staticEval - eval_margin;
  }

  // null move pruning
  if (depth >= 3 && in_check == 0 && ply) {
    // preserve board state
    copyBoard();

    // increment ply
    ply++;

    // increment repetition index & store hash key
    repetitionIndex++;
    repetitionTable[repetitionIndex] = hashKey;

    // hash enpassant if available
    if (enpassant != no_sq) hashKey ^= enpassantKey[enpassant];

    // reset enpassant capture square
    enpassant = no_sq;

    // switch the side, literally giving opponent an extra move to make
    if (side == White) {
      side = Black;
    } else {
      side = White;
    }

    // hash the side
    hashKey ^= sideKey;

    /* search moves with reduced depth to find beta cutoffs
       depth - 1 - R where R is a reduction limit */
    score = -negamax(-beta, -beta + 1, depth - 1 - 2);

    // decrement ply
    ply--;

    // decrement repetition index
    repetitionIndex--;

    // restore board state
    takeBack();

    // return 0 if time is up
    if (stopped == 1) return 0;

    // fail-hard beta cutoff
    if (score >= beta)
      // node (position) fails high
      return beta;
  }

  // razoring
  if (!pv_node && !in_check && depth <= 3) {
    // get static eval and add first bonus
    score = staticEval + 125;

    // define new score
    int new_score;

    // static evaluation indicates a fail-low node
    if (score < beta) {
      // on depth 1
      if (depth == 1) {
        // get quiscence score
        new_score = quiescence(alpha, beta);

        // return quiescence score if it's greater then static evaluation score
        return (new_score > score) ? new_score : score;
      }

      // add second bonus to static evaluation
      score += 175;

      // static evaluation indicates a fail-low node
      if (score < beta && depth <= 2) {
        // get quiscence score
        new_score = quiescence(alpha, beta);

        // quiescence score indicates fail-low node
        if (new_score < beta)
          // return quiescence score if it's greater then static evaluation score
          return (new_score > score) ? new_score : score;
      }
    }
  }

  // create move list instance
  moves moveList;
  moveList.reserve(20);

  // generate moves
  generateMoves(moveList);

  // if we are now following PV line
  if (followPv)
    // enable PV move scoring
    enable_pv_scoring(moveList);

  // sort moves
  sortMoves(moveList, best_move);

  // number of moves searched in a move list
  int moves_searched = 0;

  // loop over moves within a movelist
  for (int count = 0; count < moveList.size(); count++) {
    // preserve board state
    copyBoard();

    // increment ply
    ply++;

    // increment repetition index & store hash key
    repetitionIndex++;
    repetitionTable[repetitionIndex] = hashKey;

    // make sure to make only legal moves
    if (makeMove(moveList[count], all_moves) == 0) {
      // decrement ply
      ply--;

      // decrement repetition index
      repetitionIndex--;

      // skip to next move
      continue;
    }

    // increment legal moves
    legal_moves++;

    // full depth search
    if (moves_searched == 0)
      // do normal alpha beta search
      score = -negamax(-beta, -alpha, depth - 1);

      // late move reduction (LMR)
    else {
      // condition to consider LMR
      if (
          moves_searched >= full_depth_moves &&
              depth >= reduction_limit &&
              in_check == 0 &&
              getMoveCapture(moveList[count]) == 0 &&
              getMovePromoted(moveList[count]) == 0
          )
        // search current move with reduced depth:
        score = -negamax(-alpha - 1, -alpha, depth - 2);

        // hack to ensure that full-depth search is done
      else score = alpha + 1;

      // principle variation search PVS
      if (score > alpha) {
        /* Once you've found a move with a score that is between alpha and beta,
           the rest of the moves are searched with the goal of proving that they are all bad.
           It's possible to do this a bit faster than a search that worries that one
           of the remaining moves might be good. */
        score = -negamax(-alpha - 1, -alpha, depth - 1);

        /* If the algorithm finds out that it was wrong, and that one of the
           subsequent moves was better than the first PV move, it has to search again,
           in the normal alpha-beta manner.  This happens sometimes, and it's a waste of time,
           but generally not often enough to counteract the savings gained from doing the
           "bad move proof" search referred to earlier. */
        if ((score > alpha) && (score < beta))
          /* re-search the move that has failed to be proved to be bad
             with normal alpha beta score bounds*/
          score = -negamax(-beta, -alpha, depth - 1);
      }
    }

    // decrement ply
    ply--;

    // decrement repetition index
    repetitionIndex--;

    // take move back
    takeBack();

    // return 0 if time is up
    if (stopped == 1)
      return 0;

    // increment the counter of moves searched so far
    moves_searched++;

    // found a better move
    if (score > alpha) {
      // switch hash flag from storing score for fail-low node
      // to the one storing score for PV node
      hash_flag = HASH_FLAG_EXACT;

      // store best move (for TT)
      best_move = moveList[count];

      // on quiet moves
      if (getMoveCapture(moveList[count]) == 0)
        // store history moves
        history_moves[getMovePiece(moveList[count])][getMoveTarget(moveList[count])] += depth;

      // PV node (position)
      alpha = score;

      // write PV move
      pvTable[ply][ply] = moveList[count];

      // loop over the next ply
      for (int next_ply = ply + 1; next_ply < pvLength[ply + 1]; next_ply++)
        // copy move from deeper ply into a current ply's line
        pvTable[ply][next_ply] = pvTable[ply + 1][next_ply];

      // adjust PV length
      pvLength[ply] = pvLength[ply + 1];

      // fail-hard beta cutoff
      if (score >= beta) {
        // store hash entry with the score equal to beta
        writeHashEntry(beta, best_move, depth, HASH_FLAG_BETA);

        // on quiet moves
        if (getMoveCapture(moveList[count]) == 0) {
          // store killer moves
          killer_moves[1][ply] = killer_moves[0][ply];
          killer_moves[0][ply] = moveList[count];
        }

        // node (position) fails high
        return beta;
      }
    }
  }

  // we don't have any legal moves to make in the current postion
  if (legal_moves == 0) {
    // king is in check
    if (in_check)
      // return mating score (assuming closest distance to mating position)
      return -MATE_VALUE + ply;

      // king is not in check
    else
      // return stalemate score
      return 0;
  }

  // store hash entry with the score equal to alpha
  writeHashEntry(alpha, best_move, depth, hash_flag);

  // node (position) fails low
  return alpha;
}

// search position for the best move
void searchPosition(int depth) {
  // search start time
  int start = getTimeMs();

  // define best score variable
  int score = 0;

  // reset nodes counter
  nodes = 0;

  // reset "time is up" flag
  stopped = 0;

  // reset follow PV flags
  followPv = 0;
  scorePv = 0;

  // clear helper data structures for search
  memset(killer_moves, 0, sizeof(killer_moves));
  memset(history_moves, 0, sizeof(history_moves));
  memset(pvTable, 0, sizeof(pvTable));
  memset(pvLength, 0, sizeof(pvLength));

  // define initial alpha beta bounds
  int alpha = -INFINITY;
  int beta = INFINITY;

  // iterative deepening
  for (int current_depth = 1; current_depth <= depth; current_depth++) {
    // if time is up
    if (stopped == 1)
      // stop calculating and return best move so far
      break;

    // enable follow PV flag
    followPv = 1;

    // find best move within a given position
    score = negamax(alpha, beta, current_depth);

    // we fell outside the window, so try again with a full-width window (and the same depth)
    if ((score <= alpha) || (score >= beta)) {
      alpha = -INFINITY;
      beta = INFINITY;
      continue;
    }

    // set up the window for the next iteration
    alpha = score - 50;
    beta = score + 50;

    // if PV is available
    if (pvLength[0]) {
      // print search info
      if (score > -MATE_VALUE && score < -MATE_SCORE)
        printf("info score mate %d depth %d nodes %lld time %d pv ",
               -(score + MATE_VALUE) / 2 - 1,
               current_depth,
               nodes,
               getTimeMs() - start);

      else if (score > MATE_SCORE && score < MATE_VALUE)
        printf("info score mate %d depth %d nodes %lld time %d pv ",
               (MATE_VALUE - score) / 2 + 1,
               current_depth,
               nodes,
               getTimeMs() - start);

      else
        printf("info score cp %d depth %d nodes %lld time %d pv ", score, current_depth, nodes, getTimeMs() - start);

      // loop over the moves within a PV line
      for (int count = 0; count < pvLength[0]; count++) {
        // print PV move
        printMove(pvTable[0][count]);
        printf(" ");
      }

      // print new line
      printf("\n");
    }
  }

  // print best move
  printf("bestmove ");

  if (pvTable[0][0])
    printMove(pvTable[0][0]);

  else
    // shouldn't get here
    printf("(none)");

  printf("\n");
}