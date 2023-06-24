export const createGameError = new Error("Failed to create game");
export const chessEngineError = new Error(
  "Internal error occurred with Minerva Chess engine"
);
export const missingGameError = new Error("No game found");
export const invalidMoveError = new Error("Invalid move made");
export const ongoingGameError = new Error("Another game is ongoing for a user");
