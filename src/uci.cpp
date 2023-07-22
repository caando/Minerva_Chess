//
// Created by Jikun on 26/6/23.
//

#include <cstdio>
#include <cstring>
#include "uci.h"
#include "util.h"
#include "movegen.h"
#include "variables.h"
#include "zobrist.h"
#include "search.h"
#include "transposition.h"
#include "perft.h"
#include "debug.h"

/**********************************\
 ==================================

       Miscellaneous functions
          forked from VICE
         by Richard Allbert

 ==================================
\**********************************/

// parse user/GUI move string input (e.g. "e7e8q")
int parseMove(char *move_string) {
  // create move list instance
  moves moveList;

  // generate moves
  generateMoves(moveList);

  // parse source square
  int source_square = (move_string[0] - 'a') + (8 - (move_string[1] - '0')) * 8;

  // parse target square
  int target_square = (move_string[2] - 'a') + (8 - (move_string[3] - '0')) * 8;

  // loop over the moves within a move list
  for (int move_count = 0; move_count < moveList.size(); move_count++) {
    // init move
    int move = moveList[move_count];

    // make sure source & target squares are available within the generated move
    if (source_square == getMoveSource(move) && target_square == getMoveTarget(move)) {
      // init promoted piece
      int promoted_piece = getMovePromoted(move);

      // promoted piece is available
      if (promoted_piece) {
        // promoted to queen
        if ((promoted_piece == WQueen || promoted_piece == BQueen) && move_string[4] == 'q')
          // return legal move
          return move;

          // promoted to rook
        else if ((promoted_piece == WRook || promoted_piece == BRook) && move_string[4] == 'r')
          // return legal move
          return move;

          // promoted to bishop
        else if ((promoted_piece == WBishop || promoted_piece == BBishop) && move_string[4] == 'b')
          // return legal move
          return move;

          // promoted to knight
        else if ((promoted_piece == WKnight || promoted_piece == BKnight) && move_string[4] == 'n')
          // return legal move
          return move;

        // continue the loop on possible wrong promotions (e.g. "e7e8f")
        continue;
      }

      // return legal move
      return move;
    }
  }

  // return illegal move
  return 0;
}

// reset board variables
void resetBoard() {
  // reset board position (bitboards)
  memset(bitboards, 0ULL, sizeof(bitboards));

  // reset occupancies (bitboards)
  memset(occupancies, 0ULL, sizeof(occupancies));

  // reset game state variables
  side = White;
  enpassant = no_sq;
  castle = 0;

  // reset repetition index
  repetitionIndex = 0;

  // reset fifty move rule counter
  fifty = 0;

  // reset repetition table
  memset(repetitionTable, 0ULL, sizeof(repetitionTable));
}

// parse FEN string
void parseFen(char *fen) {
  // prepare for new game
  resetBoard();

  // loop over board ranks
  for (int rank = 0; rank < 8; rank++) {
    // loop over board files
    for (int file = 0; file < 8; file++) {
      // init current square
      int square = rank * 8 + file;

      // match ascii pieces within FEN string
      if ((*fen >= 'a' && *fen <= 'z') || (*fen >= 'A' && *fen <= 'Z')) {
        // init piece type
        int piece = asciiToPieces[*fen];

        // set piece on corresponding bitboard
        setBit(bitboards[piece], square);

        // increment pointer to FEN string
        fen++;
      }

      // match empty square numbers within FEN string
      if (*fen >= '0' && *fen <= '9') {
        // init offset (convert char 0 to int 0)
        int offset = *fen - '0';

        // define piece variable
        int piece = -1;

        // loop over all piece bitboards
        for (int bb_piece = WPawn; bb_piece <= BKing; bb_piece++) {
          // if there is a piece on current square
          if (getBit(bitboards[bb_piece], square))
            // get piece code
            piece = bb_piece;
        }

        // on empty current square
        if (piece == -1)
          // decrement file
          file--;

        // adjust file counter
        file += offset;

        // increment pointer to FEN string
        fen++;
      }

      // match rank separator
      if (*fen == '/')
        // increment pointer to FEN string
        fen++;
    }
  }

  // got to parsing side to move (increment pointer to FEN string)
  fen++;

  // parse side to move
  (*fen == 'w') ? (side = White) : (side = Black);

  // go to parsing castling rights
  fen += 2;

  // parse castling rights
  while (*fen != ' ') {
    switch (*fen) {
      case 'K': castle |= WhiteKingSide;
        break;
      case 'Q': castle |= WhiteQueenSide;
        break;
      case 'k': castle |= BlackKingSide;
        break;
      case 'q': castle |= BlackQueenSide;
        break;
      case '-': break;
    }

    // increment pointer to FEN string
    fen++;
  }

  // go to parsing enpassant square (increment pointer to FEN string)
  fen++;

  // parse enpassant square
  if (*fen != '-') {
    // parse enpassant file & rank
    int file = fen[0] - 'a';
    int rank = 8 - (fen[1] - '0');

    // init enpassant square
    enpassant = static_cast<Square>(rank * 8 + file);
  }

    // no enpassant square
  else
    enpassant = no_sq;

  // go to parsing half move counter (increment pointer to FEN string)
  fen++;

  // parse half move counter to init fifty move counter
  fifty = atoi(fen);

  // loop over white pieces bitboards
  for (int piece = WPawn; piece <= WKing; piece++)
    // populate white occupancy bitboard
    occupancies[White] |= bitboards[piece];

  // loop over black pieces bitboards
  for (int piece = BPawn; piece <= BKing; piece++)
    // populate white occupancy bitboard
    occupancies[Black] |= bitboards[piece];

  // init all occupancies
  occupancies[Both] |= occupancies[White];
  occupancies[Both] |= occupancies[Black];

  // init hash key
  hashKey = generateHashkey();
}

// parse UCI "position" command
void parsePosition(char *command) {
  // shift pointer to the right where next token begins
  command += 9;

  // init pointer to the current character in the command string
  char *currentChar = command;

  // parse UCI "startpos" command
  if (strncmp(command, "startpos", 8) == 0)
    // init chess board with start position
    parseFen(START_BOARD);

    // parse UCI "fen" command
  else {
    // make sure "fen" command is available within command string
    currentChar = strstr(command, "fen");

    // if no "fen" command is available within command string
    if (currentChar == nullptr)
      // init chess board with start position
      parseFen(START_BOARD);

      // found "fen" substring
    else {
      // shift pointer to the right where next token begins
      currentChar += 4;

      // init chess board with position from FEN string
      parseFen(currentChar);
    }
  }

  // parse moves after position
  currentChar = strstr(command, "moves");

  // moves available
  if (currentChar != nullptr) {
    // shift pointer to the right where next token begins
    currentChar += 6;

    // loop over moves within a move string
    while (*currentChar) {
      // parse next move
      int move = parseMove(currentChar);

      // if no more moves
      if (move == 0)
        // break out of the loop
        break;

      // increment repetition index
      repetitionIndex++;

      // wtire hash key into a repetition table
      repetitionTable[repetitionIndex] = hashKey;

      // make move on the chess board
      makeMove(move, all_moves);

      // move current character mointer to the end of current move
      while (*currentChar && *currentChar != ' ') currentChar++;

      // go to the next move
      currentChar++;
    }
  }
}

// reset time control variables
void resetTimeControl() {
  // reset timing
  quit = 0;
  movestogo = 30;
  moveTime = -1;
  timer = -1;
  inc = 0;
  startTime = 0;
  stopTime = 0;
  timeset = 0;
  stopped = 0;
}

// parse UCI command "go"
void parseGo(char *command) {
  // reset time control
  resetTimeControl();

  // init parameters
  int depth = -1;

  // init argument
  char *argument = nullptr;

  // infinite search
  if ((argument = strstr(command, "infinite"))) {}

  // match UCI "binc" command
  if ((argument = strstr(command, "binc")) && side == Black)
    // parse black time increment
    inc = atoi(argument + 5);

  // match UCI "winc" command
  if ((argument = strstr(command, "winc")) && side == White)
    // parse white time increment
    inc = atoi(argument + 5);

  // match UCI "wtime" command
  if ((argument = strstr(command, "wtime")) && side == White)
    // parse white time limit
    timer = atoi(argument + 6);

  // match UCI "btime" command
  if ((argument = strstr(command, "btime")) && side == Black)
    // parse black time limit
    timer = atoi(argument + 6);

  // match UCI "movestogo" command
  if ((argument = strstr(command, "movestogo")))
    // parse number of moves to go
    movestogo = atoi(argument + 10);

  // match UCI "movetime" command
  if ((argument = strstr(command, "movetime")))
    // parse amount of time allowed to spend to make a move
    moveTime = atoi(argument + 9);

  // match UCI "depth" command
  if ((argument = strstr(command, "depth")))
    // parse search depth
    depth = atoi(argument + 6);

  // run perft at given depth
  if ((argument = strstr(command, "perft"))) {
    // parse search depth
    depth = atoi(argument + 6);

    // run perft
    perftTest(depth);

    return;
  }

  // if move time is not available
  if (moveTime != -1) {
    // set time equal to move time
    timer = moveTime;

    // set moves to go to 1
    movestogo = 1;
  }

  // init start time
  startTime = getTimeMs();

  // init search depth
  depth = depth;

  // if time control is available
  if (timer != -1) {
    // flag we're playing with time control
    timeset = 1;

    // set up timing
    timer /= movestogo;

    // lag compensation
    timer -= 50;

    // if time is up
    if (timer < 0) {
      // restore negative time to 0
      timer = 0;

      // inc lag compensation on 0+inc time controls
      inc -= 50;

      // timing for 0 seconds left and no inc
      if (inc < 0) inc = 1;
    }

    // init stoptime
    stopTime = startTime + timer + inc;
  }

  // if depth is not available
  if (depth == -1)
    // set depth to 64 plies (takes ages to complete...)
    depth = 64;

  // print debug info
  printf("time: %d  inc: %d  start: %u  stop: %u  depth: %d  timeset:%d\n",
         timer, inc, startTime, stopTime, depth, timeset);

  // search position
  searchPosition(depth);
}

// main UCI loop
void uciLoop() {
  // print user defined options
  printf("\nExtended commands for debugging:\n");
  printf("'d' - print current board position\n");
  printf("'go perft 5' will run perft test for current position at the depth of 5\n\n");

  // just make it big enough
#define INPUT_BUFFER 10000

  // max hash MB
  int maxHash = 256;

  // default MB value
  int mb = 256;

  // reset STDIN & STDOUT buffers
  setbuf(stdin, nullptr);
  setbuf(stdout, nullptr);

  // define user / GUI input buffer
  char input[INPUT_BUFFER];

  // main loop
  while (1) {
    // reset user /GUI input
    memset(input, 0, sizeof(input));

    // make sure output reaches the GUI
    fflush(stdout);

    // get user / GUI input
    if (!fgets(input, INPUT_BUFFER, stdin))
      // continue the loop
      continue;

    // make sure input is available
    if (input[0] == '\n')
      // continue the loop
      continue;

    // parse UCI "isready" command
    if (strncmp(input, "isready", 7) == 0) {
      printf("readyok\n");
      continue;
    }

      // parse UCI "position" command
    else if (strncmp(input, "position", 8) == 0) {
      // call parse position function
      parsePosition(input);

      // clear hash table
      clearHashTable();
    }
      // parse UCI "ucinewgame" command
    else if (strncmp(input, "ucinewgame", 10) == 0) {
      // call parse position function
      parsePosition("position startpos");

      // clear hash table
      clearHashTable();
    }
      // parse UCI "go" command
    else if (strncmp(input, "go", 2) == 0)
      // call parse go function
      parseGo(input);

      // parse UCI "quit" command
    else if (strncmp(input, "quit", 4) == 0)
      // quit from the UCI loop (terminate program)
      break;

      // parse UCI "uci" command
    else if (strncmp(input, "uci", 3) == 0) {
      // print engine info
      printf("id name MinervaChess %s\n");
      printf("id author caan_do\n");
      printf("option name Hash type spin default 64 min 4 max %d\n", maxHash);
      printf("uciok\n");
    } else if (!strncmp(input, "setoption name Hash value ", 26)) {
      // init MB
      sscanf(input, "%*s %*s %*s %*s %d", &mb);

      // adjust MB if going beyond the aloowed bounds
      if (mb < 4) mb = 4;
      if (mb > maxHash) mb = maxHash;

      // set hash table size in MB
      printf("    Set hash table size to %dMB\n", mb);
      initHashTable(mb);
    }
  }
}