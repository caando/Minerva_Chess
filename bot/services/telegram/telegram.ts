import { Context, Telegraf } from "telegraf";
import { message } from "telegraf/filters";
import { Update } from "typegram";
import {
  chessEngineError,
  createGameError,
  invalidMoveError,
  missingGameError
} from "./errors";
import IgnoreOldMiddleWare from "./middlewares/ignoreOld";
import {
  getHelpTextConfiguration,
  getUserCurrentGame,
  makeMove,
  startGame
} from "./service";
import {
  editMessage,
  getBoardImage,
  helpText,
  sendEditableMessage,
  sendPhotoWithCaption,
  tutorText
} from "./utility";
import { Chess } from "chess.js";

// TODO: Setup logger
const bot: Telegraf<Context<Update>> = new Telegraf(
  process.env.TELEGRAM_BOT_TOKEN as string
);

bot.use(IgnoreOldMiddleWare);

bot.on(message("new_chat_members"), (ctx) => {
  if (!ctx.botInfo || ctx.botInfo.username !== bot.botInfo?.username) return;
  ctx.reply(helpText, getHelpTextConfiguration());
});

bot.command("ping", (ctx) => {
  ctx.reply(`Hello ${ctx.from.first_name}!`);
});

bot.action("tutor", (ctx) => {
  const message = ctx.update.callback_query.message;
  if (!message) return;
  bot.telegram.editMessageText(
    message.chat.id,
    message.message_id,
    undefined,
    tutorText,
    {
      parse_mode: "Markdown",
      reply_markup: {
        inline_keyboard: [
          [{ text: "Back to main guide ⏪", callback_data: "show-help" }]
        ]
      }
    }
  );
});

// bot.action("view-games", (ctx) => {
// })

bot.help((ctx) => {
  ctx.reply(helpText, getHelpTextConfiguration());
});

bot.action("show-help", (ctx) => {
  const message = ctx.update.callback_query.message;
  if (!message) return;
  bot.telegram.editMessageText(
    message.chat.id,
    message.message_id,
    undefined,
    helpText,
    getHelpTextConfiguration()
  );
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
    }
  } else {
    ctx.deleteMessage(message.message_id);
    await ctx.sendPhoto(getBoardImage(game.fen, game.userSide), {
      caption: `Your challenge against the engine has started. You are ${game.userSide}. Make moves using messages like \`e2e4\`. Receive more help using /help`,
      parse_mode: "Markdown"
    });
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
    }
  } else {
    ctx.deleteMessage(createMessage.message_id);
    await sendPhotoWithCaption(
      ctx,
      getBoardImage(game.fen, game.userSide),
      `Your challenge against the engine has started. You are ${game.userSide}. Make moves using messages like \`e2e4\`. Receive more help using /help`
    );
  }
});

bot.action("view-current", async (ctx) => {
  const username = ctx.from?.username;
  const message = ctx.update.callback_query.message;
  if (!message) return;
  if (!username) {
    editMessage(
      message,
      "Something went wrong, contact @woojiahao to receive support"
    );
    return;
  }
  const game = await getUserCurrentGame(username);

  if (!game) {
    editMessage(
      message,
      "You do not have any ongoing games, use /start to create one"
    );
    return;
  }

  ctx.deleteMessage(message.message_id);
  await sendPhotoWithCaption(
    ctx,
    getBoardImage(game.fen, game.userSide),
    `You are ${game.userSide}. Make moves using messages like \`e2e4\`. Receive more help using /help`
  );
});

bot.command("me", async (ctx) => {
  const username = ctx.from.username;
  const [message, editMessage] = await sendEditableMessage(
    ctx,
    `Retrieving your current game...`
  );
  if (!username) {
    editMessage("Something went wrong, contact @woojiahao to receive support");
    return;
  }
  const game = await getUserCurrentGame(username);

  if (!game) {
    editMessage("You do not have any ongoing games, use /start to create one");
    return;
  }

  ctx.deleteMessage(message.message_id);
  await sendPhotoWithCaption(
    ctx,
    getBoardImage(game.fen, game.userSide),
    `You are ${game.userSide}. Make moves using messages like \`e2e4\`. Receive more help using /help`
  );
});

bot.hears(/^[^/]([\w\d ]+)$/, async (ctx) => {
  const chess = new Chess();
  try {
    const move = ctx.message.text;
    chess.move(move);

    const username = ctx.from.username;
    const [message, editMessage] = await sendEditableMessage(
      ctx,
      `Retrieving your current game...`
    );
    if (!username) {
      editMessage(
        "Something went wrong, contact @woojiahao to receive support"
      );
      return;
    }
    const game = await makeMove(username, move);

    if (game instanceof Error) {
      if (game === missingGameError) {
        editMessage(
          "You do not have any ongoing games, use /start to create one"
        );
      } else if (game === chessEngineError) {
        editMessage("Something wrong happened internally");
      } else if (game === invalidMoveError) {
        // TODO: Potentially can ignore invalid moves instead
        editMessage("Invalid move");
      }
    } else {
      ctx.deleteMessage(message.message_id);
      sendPhotoWithCaption(
        ctx,
        getBoardImage(game.game.fen, game.game.userSide),
        `
      *Your move!*

      Minerva Chess played *${game.engineMove}*

      Make moves using messages like \`e2e4\`. Receive more help using /help
      `.replace(/  +/g, "")
      );
    }
  } catch (e) {
    return;
  }
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
