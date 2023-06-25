import { Chess } from "chess.js";
import { Context, NarrowedContext, Telegraf } from "telegraf";
import { message } from "telegraf/filters";
import { CallbackQuery, Message } from "telegraf/typings/core/types/typegram";
import { Update } from "typegram";
import { whiteStartPos } from "../database";
import { UserSide } from "../models/game";
import {
  chessEngineError,
  createGameError,
  invalidMoveError,
  missingGameError,
  ongoingGameError
} from "./errors";
import IgnoreOldMiddleWare from "./middlewares/ignoreOld";
import {
  getMenuTextConfiguration,
  getTutorTextConfiguration,
  getUserCurrentGame,
  makeMove,
  renderBoard,
  startGame
} from "./service";
import {
  editMessage,
  getBoardImage,
  menuText,
  sendChessboard,
  sendEditableMessage,
  tutorText
} from "./utility";

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
  { command: "ping", description: "Check if the bot is online" },
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
    ctx.editMessageText(menuText, getMenuTextConfiguration());
  }
});

// TODO: Abstract the behavior to work for both scenarios
bot.action("start-game", async (ctx) => {
  const message = ctx.update.callback_query.message;
  if (!message) return;
  editMessage(message, "Got it! We will try to start a game for you!");
  const username = ctx.from?.username;
  if (!username) {
    editMessage(
      message,
      "Something went wrong internally, contact @woojiahao for assistance"
    );
    return;
  }

  const game = await startGame(username);

  if (game instanceof Error) {
    if (game === createGameError) {
      editMessage(
        message,
        "Failed to create a new challenge, contact @woojiahao to receive support"
      );
    } else if (game === chessEngineError) {
      editMessage(message, "Something went wrong internally");
    } else if (game === ongoingGameError) {
      await ctx.deleteMessage(message.message_id);
      const ongoingGame = await getUserCurrentGame(username);
      if (!ongoingGame) return;
      sendChessboard(
        ctx,
        ongoingGame,
        "Existing ongoing game!",
        `You are ${ongoingGame.userSide}`
      );
    }
  } else {
    ctx.deleteMessage(message.message_id);
    await sendChessboard(
      ctx,
      game,
      "Game started!",
      `You are ${game.userSide}`
    );
  }
});

bot.command("start", async (ctx) => {
  const username = ctx.from.username;
  const [createMessage, editCreateMessage] = await sendEditableMessage(
    ctx,
    "Processing your challenge..."
  );
  if (!username) {
    editCreateMessage(
      "Something went wrong, contact @woojiahao to receive support"
    );
    return;
  }

  const game = await startGame(username);

  if (game instanceof Error) {
    if (game === createGameError) {
      editCreateMessage(
        "Failed to create a new challenge, contact @woojiahao to receive support"
      );
    } else if (game === chessEngineError) {
      editCreateMessage("Something went wrong internally");
    } else if (game === ongoingGameError) {
      await ctx.deleteMessage(createMessage.message_id);
      const ongoingGame = await getUserCurrentGame(username);
      if (!ongoingGame) return;
      sendChessboard(
        ctx,
        ongoingGame,
        "Existing ongoing game!",
        `You are ${ongoingGame.userSide}`
      );
    }
  } else {
    ctx.deleteMessage(createMessage.message_id);
    await sendChessboard(
      ctx,
      game,
      "Game started!",
      `You are ${game.userSide}`
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
      "You do not have any ongoing games, use /start to create one"
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
    ctx.reply("You do not have any ongoing games, use /start to create one");
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
  } catch (e) {
    return;
  }
});

bot.action("forfeit-game", (ctx) => {
  console.log("Forfeiting game");
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
