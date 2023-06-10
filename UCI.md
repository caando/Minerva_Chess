# UCI

Following Stockfish UCI commands found [here](https://github.com/official-stockfish/Stockfish/wiki/Commands)

The CLI automatically runs in UCI mode so `uci` is not necessary.

## Supported commands

- `quit` > quit the program
- `position [fen <fenstring> | startpos] moves <move1> ... <movei>` > setup board state with moves played
- `go movetime <x>` > search for `x` ms
- `go mate <x>` > search for mate in `x` moves
- `stop` > prematurely stop calculations
- `d` > display current position
- `eval` > evaluate the board position