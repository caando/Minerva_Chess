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

int parseMove(char *moveString) {
  moves moveList;
  generateMoves(moveList);
  int source_square = (moveString[0] - 'a') + (8 - (moveString[1] - '0')) * 8;
  int target_square = (moveString[2] - 'a') + (8 - (moveString[3] - '0')) * 8;
  for (int move : moveList) {
    if (source_square == getMoveSource(move) && target_square == getMoveTarget(move)) {
      int promotedPiece = getMovePromoted(move);
      if (promotedPiece) {
        if ((promotedPiece == WQueen || promotedPiece == BQueen) && moveString[4] == 'q') {
          return move;
        } else if ((promotedPiece == WRook || promotedPiece == BRook) && moveString[4] == 'r') {
          return move;
        } else if ((promotedPiece == WBishop || promotedPiece == BBishop) && moveString[4] == 'b') {
          return move;
        } else if ((promotedPiece == WKnight || promotedPiece == BKnight) && moveString[4] == 'n') {
          return move;
        }
        continue;
      }
      return move;
    }
  }
  return 0;
}

void resetBoard() {
  memset(bitboards, 0ULL, sizeof(bitboards));
  memset(occupancies, 0ULL, sizeof(occupancies));
  side = White;
  enpassant = no_sq;
  castle = 0;
  repetitionIndex = 0;
  fifty = 0;
  memset(repetitionTable, 0ULL, sizeof(repetitionTable));
}

void parseFen(char *fen) {
  resetBoard();
  for (int rank = 0; rank < 8; rank++) {
    for (int file = 0; file < 8; file++) {
      int square = rank * 8 + file;
      if ((*fen >= 'a' && *fen <= 'z') || (*fen >= 'A' && *fen <= 'Z')) {
        int piece = asciiToPieces[*fen];
        setBit(bitboards[piece], square);
        fen++;
      }
      if (*fen >= '0' && *fen <= '9') {
        int offset = *fen - '0';
        int piece = -1;
        for (int bb_piece = WPawn; bb_piece <= BKing; bb_piece++) {
          if (getBit(bitboards[bb_piece], square)) {
            piece = bb_piece;
          }
        }
        if (piece == -1) {
          file--;
        }
        file += offset;
        fen++;
      }
      if (*fen == '/') {
        fen++;
      }
    }
  }
  fen++;
  (*fen == 'w') ? (side = White) : (side = Black);
  fen += 2;
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
    fen++;
  }
  fen++;
  if (*fen != '-') {
    int file = fen[0] - 'a';
    int rank = 8 - (fen[1] - '0');
    enpassant = static_cast<Square>(rank * 8 + file);
  } else {
    enpassant = no_sq;
  }
  fen++;
  fifty = atoi(fen);
  for (int piece = WPawn; piece <= WKing; piece++) {
    occupancies[White] |= bitboards[piece];
  }
  for (int piece = BPawn; piece <= BKing; piece++) {
    occupancies[Black] |= bitboards[piece];
  }
  occupancies[Both] |= occupancies[White];
  occupancies[Both] |= occupancies[Black];
  hashKey = generateHashkey();
}

void parsePosition(char *command) {
  command += 9;
  char *currentChar = command;
  if (strncmp(command, "startpos", 8) == 0) {
    parseFen(START_BOARD);
  } else {
    currentChar = strstr(command, "fen");
    if (currentChar == nullptr) {
      parseFen(START_BOARD);
    } else {
      currentChar += 4;
      parseFen(currentChar);
    }
  }
  currentChar = strstr(command, "moves");
  if (currentChar != nullptr) {
    currentChar += 6;
    while (*currentChar) {
      int move = parseMove(currentChar);
      if (move == 0) {
        break;
      }
      repetitionIndex++;
      repetitionTable[repetitionIndex] = hashKey;
      makeMove(move, ALL_MOVES);
      while (*currentChar && *currentChar != ' ') {
        currentChar++;
      }
      currentChar++;
    }
  }
}

void resetTimeControl() {
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

void parseGo(char *command) {
  resetTimeControl();
  int depth = -1;
  char *argument = nullptr;

  if ((argument = strstr(command, "infinite"))) {}
  if ((argument = strstr(command, "binc")) && side == Black) {
    inc = atoi(argument + 5);
  }
  if ((argument = strstr(command, "winc")) && side == White) {
    inc = atoi(argument + 5);
  }
  if ((argument = strstr(command, "wtime")) && side == White) {
    timer = atoi(argument + 6);
  }
  if ((argument = strstr(command, "btime")) && side == Black) {
    timer = atoi(argument + 6);
  }
  if ((argument = strstr(command, "movestogo"))) {
    movestogo = atoi(argument + 10);
  }
  if ((argument = strstr(command, "movetime"))) {
    moveTime = atoi(argument + 9);
  }
  if ((argument = strstr(command, "depth"))) {
    depth = atoi(argument + 6);
  }
  if ((argument = strstr(command, "perft"))) {
    depth = atoi(argument + 6);
    perftTest(depth);
    return;
  }
  if (moveTime != -1) {
    timer = moveTime;
    movestogo = 1;
  }
  startTime = getTimeMs();
  if (timer != -1) {
    timeset = 1;
    timer /= movestogo;
    timer -= 50;
    if (timer < 0) {
      timer = 0;
      inc -= 50;
      if (inc < 0) {
        inc = 1;
      }
    }
    stopTime = startTime + timer + inc;
  }
  if (depth == -1) {
    depth = 64;
  }
  printf("time: %d  inc: %d  start: %u  stop: %u  depth: %d  timeset:%d\n",
         timer, inc, startTime, stopTime, depth, timeset);
  searchPosition(depth);
}

void uciLoop() {
  printf("\nExtended commands for debugging:\n");
  printf("'d' - print current board position\n");
  printf("'go perft 5' will run perft test for current position at the depth of 5\n\n");
  int maxHash = 256;
  int mb = 128;
  setbuf(stdin, nullptr);
  setbuf(stdout, nullptr);
  char input[INPUT_BUFFER];
  parseFen(START_BOARD);

  while (true) {
    memset(input, 0, sizeof(input));
    fflush(stdout);

    if (!fgets(input, INPUT_BUFFER, stdin)) {
      continue;
    }
    if (input[0] == '\n') {
      continue;
    }

    if (strncmp(input, "isready", 7) == 0) {
      printf("readyok\n");
      continue;
    } else if (strncmp(input, "position", 8) == 0) {
      parsePosition(input);
      clearHashTable();
    } else if (strncmp(input, "ucinewgame", 10) == 0) {
      parsePosition("position startpos");
      clearHashTable();
    } else if (strncmp(input, "go", 2) == 0) {
      parseGo(input);
    } else if (strncmp(input, "quit", 4) == 0) {
      break;
    } else if (strncmp(input, "uci", 3) == 0) {
      printf("id name MinervaChess %s\n", version);
      printf("id author caan_do\n");
      printf("option name Hash type spin default 64 min 4 max %d\n", maxHash);
      printf("uciok\n");
    } else if (!strncmp(input, "setoption name Hash value ", 26)) {
      sscanf(input, "%*s %*s %*s %*s %d", &mb);
      if (mb < 4) {
        mb = 4;
      }
      if (mb > maxHash) {
        mb = maxHash;
      }
      printf("    Set hash table size to %dMB\n", mb);
      initHashTable(mb);
    }
  }
}