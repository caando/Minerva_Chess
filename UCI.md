# UCI

Following Stockfish UCI commands found [here](https://github.com/official-stockfish/Stockfish/wiki/Commands)

## Supported commands

- `quit` > quit the program
- `uci` > toggle UCI mode
- `position [fen <fenstring> | startpos] moves <move1> ... <movei>` > setup board state with moves played
- `go movetime <x>` > search for `x` ms
- `go mate <x>` > search for mate in `x` moves
- `stop` > prematurely stop calculations
- `d` > display current position
- `eval` > evaluate the board position