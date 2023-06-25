import { Chess } from "chess.js";
import { Context, NarrowedContext, Telegraf } from "telegraf";
import { message } from "telegraf/filters";
import {
  CallbackQuery,
  InlineKeyboardButton,
  Message
} from "telegraf/typings/core/types/typegram";
import { Update } from "typegram";
import {
  chessEngineError,
  createGameError,
  invalidMoveError,
  missingGameError,
  ongoingGameError
} from "./errors";
import IgnoreOldMiddleWare from "./middlewares/ignoreOld";
import {
  chunkSelectGamesAction,
  getMenuTextConfiguration,
  getTutorTextConfiguration,
  getUserCurrentGame,
  makeMove,
  renderBoard,
  startGame
} from "./service";
import {
  editMessage,
  gameStatusToText,
  menuText,
  sendChessboard,
  sendEditableMessage,
  tutorText
} from "./utility";
import { forfeitGame, getGames, getUserGames } from "../database";
import { GameStatus } from "../models/game";

// Explicitly type alias the two commonly used contexts for ease of use later on
/**
 * Context type provided in callbacks. Includes the message that had the inline keyboard.
 */
export type CallbackContext = NarrowedContext<
  Context<Update> & {
    match: RegExpExecArray;
  },
  Update.CallbackQueryUpdate<CallbackQuery>
>;

/**
 * Context type provided in commands. Includes the message that triggered the command.
 */
export type CommandContext = NarrowedContext<
  Context<Update>,
  {
    message: Update.New & Update.NonChannel & Message.TextMessage;
    update_id: number;
  }
>;

// TODO: Setup logger
const bot: Telegraf<Context<Update>> = new Telegraf(
  process.env.TELEGRAM_BOT_TOKEN as string
);

bot.use(IgnoreOldMiddleWare);

bot.telegram.setMyCommands([
  {
    command: "menu",
    description: "Display a menu to navigate using Minerva Chess bot"
  },
  { command: "help", description: "Show a help text to use this bot" },
  {
    command: "start",
    description:
      "Starts a game against the bot if no ongoing games are happening"
  },
  { command: "board", description: "View your ongoing game if any" }
]);

bot.on(message("new_chat_members"), (ctx) => {
  if (!ctx.botInfo || ctx.botInfo.username !== bot.botInfo?.username) return;
  ctx.reply(menuText, getMenuTextConfiguration());
});

bot.command("ping", (ctx) => {
  ctx.reply(`Hello ${ctx.from.first_name}!`);
});

bot.action("tutor", (ctx) => {
  const message = ctx.update.callback_query.message;
  if (!message) {
    ctx.reply(tutorText, getTutorTextConfiguration());
  } else {
    ctx.editMessageText(tutorText, getTutorTextConfiguration());
  }
});

bot.help((ctx) => {
  ctx.reply(tutorText, getTutorTextConfiguration());
});

bot.command("menu", (ctx) => {
  ctx.reply(menuText, getMenuTextConfiguration());
});

bot.action("show-menu", (ctx) => {
  const message = ctx.update.callback_query.message;
  if (!message) {
    ctx.reply(menuText, getMenuTextConfiguration());
  } else {
    if ("caption" in message) {
      bot.telegram.deleteMessage(message.chat.id, message.message_id);
      ctx.reply(menuText, getMenuTextConfiguration());
    } else {
      ctx.editMessageText(menuText, getMenuTextConfiguration());
    }
  }
});

// TODO: Abstract the behavior to work for both scenarios
bot.action("start-game", async (ctx) => {
  const message = ctx.update.callback_query.message;
  const username = ctx.from?.username;
  if (!username) return;

  const game = await startGame(username);

  if (game instanceof Error) {
    if (game === createGameError) {
      if (!message || !("photo" in message)) {
        ctx.reply(
          "Failed to create a new challenge, contact @woojiahao to receive support"
        );
      } else {
        ctx.editMessageText(
          "Failed to create a new challenge, contact @woojiahao to receive support"
        );
      }
    } else if (game === chessEngineError) {
      if (!message || !("photo" in message)) {
        ctx.reply("Something went wrong internally");
      } else {
        ctx.editMessageText("Something went wrong internally");
      }
    } else if (game === ongoingGameError) {
      const ongoingGame = await getUserCurrentGame(username);
      if (!ongoingGame) return;
      renderBoard(
        ctx,
        ongoingGame.id,
        -1,
        `
        *Existing ongoing game!*

        You are ${ongoingGame.userSide}
        `.replace(/  +/g, "")
      );
      return;
    }
  } else {
    renderBoard(
      ctx,
      game.id,
      -1,
      `
        *Game started!*

        You are ${game.userSide}
        `.replace(/  +/g, "")
    );
  }
});

bot.command("start", async (ctx) => {
  const username = ctx.from.username;
  if (!username) return;

  const game = await startGame(username);

  if (game instanceof Error) {
    if (game === createGameError) {
      ctx.reply(
        "Failed to create a new challenge, contact @woojiahao to receive support"
      );
    } else if (game === chessEngineError) {
      ctx.reply("Something went wrong internally");
    } else if (game === ongoingGameError) {
      const ongoingGame = await getUserCurrentGame(username);
      if (!ongoingGame) return;
      renderBoard(
        ctx,
        ongoingGame.id,
        -1,
        `
        *Existing ongoing game!*

        You are ${ongoingGame.userSide}
        `.replace(/  +/g, "")
      );
    }
  } else {
    renderBoard(
      ctx,
      game.id,
      -1,
      `
        *Game started!*

        You are ${game.userSide}
        `.replace(/  +/g, "")
    );
  }
});

bot.action("view-current", async (ctx) => {
  // Username is only ever undefined if message is sent from the channel
  // Can safely ignore the cases where username is undefined
  // See: https://core.telegram.org/bots/api#message
  const username = ctx.from?.username;
  if (!username) return;
  const game = await getUserCurrentGame(username);

  if (!game) {
    ctx.editMessageText(
      "You do not have any ongoing games, use /start to create one",
      {
        reply_markup: {
          inline_keyboard: [
            [{ text: "Start game 🟢", callback_data: "start-game" }]
          ]
        }
      }
    );
    return;
  }

  await renderBoard(ctx, game.id, -1);
});

bot.command("board", async (ctx) => {
  // Username is only ever undefined if message is sent from the channel
  // Can safely ignore the cases where username is undefined
  // See: https://core.telegram.org/bots/api#message
  const username = ctx.from?.username;
  if (!username) return;
  const game = await getUserCurrentGame(username);

  if (!game) {
    ctx.reply(
      "You do not have any ongoing games, use the button below or send /start",
      {
        reply_markup: {
          inline_keyboard: [
            [{ text: "Start game 🟢", callback_data: "start-game" }]
          ]
        }
      }
    );
    return;
  }

  renderBoard(ctx, game.id, -1);
});

bot.action(/^render-board:(\d+):([-\d]+)$/, async (ctx) => {
  const gameId = parseInt(ctx.match[1]);
  const step = parseInt(ctx.match[2]);

  renderBoard(ctx, gameId, step);
});

bot.hears(/^[^/]([\w\d ]+)$/, async (ctx) => {
  // If username somehow invalid, we skip
  const username = ctx.from.username;
  if (!username) return;

  // If no ongoing games, we skip
  const userCurrentGame = await getUserCurrentGame(username);
  if (!userCurrentGame) return;

  try {
    // Attempt to make the move on the current board state
    // If invalid move, immediately ignore
    const chess = new Chess(userCurrentGame.fen);
    const move = ctx.message.text;
    chess.move(move);

    // If move is valid SAN, permanently make the move in the DB
    const [message, editMessage] = await sendEditableMessage(
      ctx,
      `Retrieving your current game...`
    );
    const game = await makeMove(username, move);

    if (game instanceof Error) {
      if (game === missingGameError) {
        editMessage(
          "You do not have any ongoing games, use /start to create one"
        );
      } else if (game === chessEngineError) {
        editMessage("Something wrong happened internally");
      } else if (game === invalidMoveError) {
        editMessage("Invalid move");
      }
    } else {
      ctx.deleteMessage(message.message_id);
      if (game.status === GameStatus.STARTED) {
        // Game still progressing as per usual, check if in check
        chess.clear();
        chess.load(game.game.fen);
        if (chess.inCheck()) {
          await renderBoard(
            ctx,
            game.game.id,
            -1,
            `
            *Your move!*

            Minerva Chess played *${game.engineMove}* and you are in check
            `.replace(/  +/g, "")
          );
        } else {
          await renderBoard(
            ctx,
            game.game.id,
            -1,
            `
            *Your move!*

            Minerva Chess played *${game.engineMove}*
            `.replace(/  +/g, "")
          );
        }
      } else {
        // Game had some decided outcome already so just display the fail outcome
        await renderBoard(
          ctx,
          game.game.id,
          -1,
          `
            *${gameStatusToText(game.status)}*
            `.replace(/  +/g, "")
        );
      }
    }
  } catch (e) {
    return;
  }
});

bot.action("forfeit-game-confirmation", async (ctx) => {
  const username = ctx.from?.username;
  if (!username) return;

  const ongoingGame = await getUserCurrentGame(username);
  // If no ongoing game, ignore the forfeit
  if (!ongoingGame) return;

  ctx.editMessageCaption("Are you sure you want to forfeit this match?", {
    parse_mode: "Markdown",
    reply_markup: {
      inline_keyboard: [
        [
          { text: "Yes 🏳", callback_data: "forfeit-game" },
          { text: "No! 💪🏻", callback_data: `render-board:${ongoingGame.id}:-1` }
        ]
      ]
    }
  });
});

bot.action("forfeit-game", async (ctx) => {
  const username = ctx.from?.username;
  if (!username) return;

  const ongoingGame = await getUserCurrentGame(username);
  // If no ongoing game, ignore the forfeit
  if (!ongoingGame) return;

  forfeitGame(ongoingGame);
  ctx.editMessageCaption("You have forfeited the match, Minerva Chess wins", {
    reply_markup: {
      inline_keyboard: [
        [{ text: "Start another 🔁", callback_data: "start-game" }],
        [{ text: "View menu 🔎", callback_data: "show-menu" }]
      ]
    }
  });
});

bot.action("view-games", async (ctx) => {
  const username = ctx.from?.username;
  if (!username) return;
  const toDisplay = await chunkSelectGamesAction(username, 8, 50);
  ctx.editMessageText(
    `Review your play history of your most recent ${50} games`,
    {
      reply_markup: {
        inline_keyboard: toDisplay
      }
    }
  );
});

bot.command("games", async (ctx) => {
  const username = ctx.from?.username;
  if (!username) return;
  const toDisplay = await chunkSelectGamesAction(username, 8, 50);
  ctx.reply(`Review your play history of your most recent ${50} games`, {
    reply_markup: {
      inline_keyboard: toDisplay
    }
  });
});

bot.command("teabag", async (ctx) => {
  await ctx.sendAnimation(`https://tenor.com/view/halo-teabag-gif-12948320`);
});

bot.command("google", async (ctx) => {
  if (ctx.update.message.text === "/google en passant") {
    await sendEditableMessage(ctx, `Holy hell`);
  }
});

export default bot;
