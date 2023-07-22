// Service layer for Telegram <> Chess Engine <> Database interactions

import { Chess } from "chess.js";
import { Context, Telegraf } from "telegraf";
import {
  InlineKeyboardButton,
  Update
} from "telegraf/typings/core/types/typegram";
import {
  addGame,
  addGameHistory,
  getGame,
  getGameHistory,
  getUser,
  getUserGames,
  getUserOngoingGame,
  setGameStatus,
  updateGameFen
} from "../database";
import { engine } from "../engine";
import { GameStatus, UserSide } from "../models/game";
import { HistoryPlayer } from "../models/history";
import {
  chessEngineError,
  createGameError,
  invalidMoveError,
  missingGameError,
  ongoingGameError
} from "./errors";
import { gameStatusToText, getBoardImage } from "./utility";

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
    await addGameHistory(game, move, chess.fen(), HistoryPlayer.BOT);
  }

  return game;
}

export async function makeMove(username: string, move: string) {
  const game = await getUserCurrentGame(username);

  if (!game) return missingGameError;

  const chess = new Chess(game.fen);
  try {
    chess.move(move);
    await updateGameFen(game, chess.fen());
    await addGameHistory(game, move, chess.fen(), HistoryPlayer.USER);

    if (chess.isCheckmate()) {
      // User has checkmated the engine, end the game
      await setGameStatus(game, GameStatus.USER_WIN);
      return {
        game: game,
        engineMove: null,
        status: GameStatus.USER_WIN
      };
    } else if (chess.isStalemate()) {
      // User's move stalemated the game
      await setGameStatus(game, GameStatus.STALEMATE);
      return {
        game: game,
        engineMove: null,
        status: GameStatus.STALEMATE
      };
    } else if (chess.isDraw()) {
      // User's move drew the game
      await setGameStatus(game, GameStatus.DRAW);
      return {
        game: game,
        engineMove: null,
        status: GameStatus.DRAW
      };
    }

    const engineMove: string | null = await engine.getMove(chess.fen());
    if (!engineMove) return chessEngineError;
    chess.move(engineMove);
    await updateGameFen(game, chess.fen());
    await addGameHistory(game, engineMove, chess.fen(), HistoryPlayer.BOT);
    if (chess.isCheckmate()) {
      // Bot checkmate
      await setGameStatus(game, GameStatus.BOT_WIN);
      return {
        game: game,
        engineMove: engineMove,
        status: GameStatus.BOT_WIN
      };
    } else if (chess.isStalemate()) {
      // Bot's move stalemates
      await setGameStatus(game, GameStatus.STALEMATE);
      return {
        game: game,
        engineMove: engineMove,
        status: GameStatus.STALEMATE
      };
    } else if (chess.isDraw()) {
      // Bot's move draws
      await setGameStatus(game, GameStatus.DRAW);
      return {
        game: game,
        engineMove: engineMove,
        status: GameStatus.DRAW
      };
    }

    // Regular move so do nothing
    return {
      game: game,
      engineMove: engineMove,
      status: GameStatus.STARTED
    };
  } catch (e) {
    return invalidMoveError;
  }
}

export async function getUserCurrentGame(username: string) {
  // TODO: replace this with join select
  const user = await getUser(username);
  const game = await getUserOngoingGame(user);
  return game;
}

type replyConfiguration = Omit<
  object,
  "text" | "chat_id" | "message_id" | "inline_message_id"
>;
export function getMenuTextConfiguration(): replyConfiguration {
  return {
    parse_mode: "Markdown",
    reply_markup: {
      inline_keyboard: [
        [{ text: "Start a game! 🟢", callback_data: "start-game" }],
        [{ text: "View current game 👀", callback_data: "view-current" }],
        [{ text: "More help ❓", callback_data: "tutor" }],
        [{ text: "Past games 👓", callback_data: "view-games" }],
        [{ text: "Play statistics 📊", callback_data: "view-stats" }]
      ]
    }
  };
}

export function getTutorTextConfiguration(): replyConfiguration {
  return {
    parse_mode: "Markdown",
    reply_markup: {
      inline_keyboard: [
        [{ text: "Back to main menu ⏪", callback_data: "show-menu" }]
      ]
    }
  };
}

// Renders the board state for historical look
export async function renderBoard(
  bot: Telegraf<Context<Update>>,
  ctx: Context,
  gameId: number,
  step: number,
  caption = ""
) {
  const game = await getGame(gameId);
  if (!game) {
    if ("callback_query" in ctx.update && ctx.update.callback_query.message) {
      ctx.editMessageText("Invalid game");
    } else {
      ctx.reply("Invalid game");
    }
    return;
  }

  const history = await getGameHistory(game);
  if (step !== -1 && (step < 0 || step >= history.length)) {
    if ("callback_query" in ctx.update && ctx.update.callback_query.message) {
      ctx.editMessageText("Invalid step");
    } else {
      ctx.reply("Invalid step");
    }
    return;
  }

  const actualStep = step === -1 ? history.length - 1 : step;
  const actions: InlineKeyboardButton[][] = [];
  const prevStep = actualStep - 1;
  const nextStep = actualStep + 1;

  actions.push([
    prevStep >= 0
      ? {
          text: "Previous move ⏪",
          callback_data: `render-board:${gameId}:${prevStep}`
        }
      : { text: "Invalid 🚫", callback_data: "invalid" },
    nextStep < history.length
      ? {
          text: "Next move ⏩",
          callback_data: `render-board:${gameId}:${nextStep}`
        }
      : { text: "Invalid 🚫", callback_data: "invalid" }
  ]);

  actions.push([
    { text: "Very beginning 🦕", callback_data: `render-board:${game.id}:0` },
    {
      text: "Present board 🦉",
      callback_data: `render-board:${game.id}:${history.length - 1}`
    }
  ]);

  if (game.status === "STARTED") {
    // Only allow forfeits on current matches
    actions.push([
      { text: "Forfeit game 🏳", callback_data: "forfeit-game-confirmation" }
    ]);
  } else {
    // For other status games, we allow them to start a new game if they don't have any other
    // ongoing games
    if (!(await getUserCurrentGame(game.player.username))) {
      actions.push([{ text: "Start game 🟢", callback_data: "start-game" }]);
    }
  }

  const boardUrl = getBoardImage(history[actualStep].fen, game.userSide);
  const boardCaption =
    caption === ""
      ? `
  *Game*

  *Move ${actualStep + 1}* ${history[actualStep].move}
  *Turn* ${actualStep === 0 ? "Starting" : history[actualStep].player}
  *Outcome* ${gameStatusToText(game.status)}
  `.replace(/  +/g, "")
      : caption;

  if ("callback_query" in ctx.update && ctx.update.callback_query.message) {
    // Render triggered from callback query
    // Replace the current callback query message with the board
    const message = ctx.update.callback_query.message;

    // Existing message has a media so include one
    if ("photo" in message) {
      // If current message is already a message with some media attached
      await ctx.editMessageMedia({
        type: "photo",
        media: boardUrl
      });
      await ctx.editMessageCaption(boardCaption, {
        parse_mode: "Markdown",
        reply_markup: {
          inline_keyboard: actions
        }
      });
      return;
    } else {
      // Current message does not contain any media so we need to create a new message
      await bot.telegram.deleteMessage(message.chat.id, message.message_id);
    }
  }
  await ctx.sendPhoto(boardUrl, {
    caption: boardCaption,
    parse_mode: "Markdown",
    reply_markup: {
      inline_keyboard: actions
    }
  });
}

// TODO: Support history of more than 50 games
export async function chunkSelectGamesAction(
  username: string,
  chunkSize: number,
  limit: number
) {
  const games = await getUserGames(username);
  const toDisplay: InlineKeyboardButton[][] = [];
  for (let i = 0; i < limit; i += chunkSize) {
    let j = i + 1;
    toDisplay.push(
      games.slice(i, Math.min(i + chunkSize, limit)).map((game) => ({
        text: `${j++}`,
        callback_data: `render-board:${game.id}:${-1}`
      }))
    );
  }
  return toDisplay;
}
