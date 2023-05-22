-- TODO: Store board states when moving to Minerva Chess Engine UCI
-- TODO: Cache last seen board state
-- TODO: Store player side
CREATE TABLE IF NOT EXISTS games (
  -- TODO: Figure out cleaner way to handle the
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  gameId TEXT NOT NULL,
  status TEXT NOT NULL,
  username TEXT NOT NULL
);