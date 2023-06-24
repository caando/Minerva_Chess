// Service layer for Telegram <> Chess Engine <> Database interactions

import { Chess } from "chess.js";
import { addGame, getUser, getUserOngoingGame } from "../database";
import { engine } from "../engine";
import { UserSide } from "../models/game";
import { chessEngineError, createGameError } from "./errors";

export async function startGame(username: string) {
  const user = await getUser(username);
  const prevGame = await getUserOngoingGame(user);
  // TODO: Only allow single game
  // TODO: Move behavior to database layer
  if (prevGame !== null) {
    prevGame.set("status", "ENDED");
    prevGame.save();
  }

  const game = await addGame(user.id);
  if (game instanceof Error) return createGameError;

  if (game.userSide === UserSide.BLACK) {
    const chess = new Chess(game.fen);
    const move: string | null = await engine.getMove(chess.fen());
    if (!move) return chessEngineError;
    chess.move(move);
    // TODO: Move this to database layer
    game.set("fen", chess.fen());
    game.save();
  }

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
        [{ text: "View current game 👀", callback_data: "view-game" }],
        [{ text: "More help ❓", callback_data: "tutor" }],
        [{ text: "Past games 👓", callback_data: "view-games" }]
      ]
    }
  };
}
