/*

  Function to "listen" to GUI's input during search.
  It's waiting for the user input from STDIN.
  OS dependent.

  First Richard Allbert aka BluefeverSoftware grabbed it from somewhere...
  And then Code Monkey King has grabbed it from VICE)...
  And then caan_do grabbed it from Code Monkey King...

*/

#ifndef MINERVA_CHESS_SRC_COMMUNICATION_H_
#define MINERVA_CHESS_SRC_COMMUNICATION_H_

int inputWaiting();

// read GUI/user input
void readInput();

// a bridge function to interact between search and GUI input
void communicate();

#endif //MINERVA_CHESS_SRC_COMMUNICATION_H_
