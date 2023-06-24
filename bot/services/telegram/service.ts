// Service layer for Telegram <> Chess Engine <> Database interactions

import { Chess } from "chess.js";
import {
  addGame,
  getUser,
  getUserOngoingGame,
  updateGameFen
} from "../database";
import { engine } from "../engine";
import { UserSide } from "../models/game";
import {
  chessEngineError,
  createGameError,
  invalidMoveError,
  missingGameError,
  ongoingGameError
} from "./errors";

export async function startGame(username: string) {
  const user = await getUser(username);
  const prevGame = await getUserOngoingGame(user);
  if (prevGame !== null) return ongoingGameError;

  const game = await addGame(user.id);
  if (game instanceof Error) return createGameError;

  if (game.userSide === UserSide.BLACK) {
    const chess = new Chess(game.fen);
    const move: string | null = await engine.getMove(chess.fen());
    if (!move) return chessEngineError;
    chess.move(move);
    await updateGameFen(game, chess.fen());
  }

  return game;
}

export async function makeMove(username: string, move: string) {
  // TODO: Handle cases where chess engine/user has checkmate or is in check
  const game = await getUserCurrentGame(username);

  if (!game) return missingGameError;

  const chess = new Chess(game.fen);
  try {
    chess.move(move);
    const engineMove: string | null = await engine.getMove(chess.fen());
    if (!engineMove) return chessEngineError;
    chess.move(engineMove);
    await updateGameFen(game, chess.fen());
    return {
      game: game,
      engineMove: engineMove
    };
  } catch (e) {
    return invalidMoveError;
  }
}

export async function getUserCurrentGame(username: string) {
  const user = await getUser(username);
  const game = await getUserOngoingGame(user);
  return game;
}

export function getHelpTextConfiguration(): Omit<
  object,
  "text" | "chat_id" | "message_id" | "inline_message_id"
> {
  return {
    parse_mode: "Markdown",
    reply_markup: {
      inline_keyboard: [
        [{ text: "Start a game! 🟢", callback_data: "start-game" }],
        [{ text: "View current game 👀", callback_data: "view-current" }],
        [{ text: "More help ❓", callback_data: "tutor" }],
        [{ text: "Past games 👓", callback_data: "view-games" }]
      ]
    }
  };
}
