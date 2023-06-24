import { Chess } from "chess.js";
import { Context, Telegraf } from "telegraf";
import { message } from "telegraf/filters";
import { Update } from "typegram";
import { getUser, getUserOngoingGame } from "../database";
import { engine } from "../engine";
import { chessEngineError, createGameError } from "./errors";
import IgnoreOldMiddleWare from "./middlewares/ignoreOld";
import { getHelpTextConfiguration, startGame } from "./service";
import {
  editMessage,
  getBoardImage,
  helpText,
  sendEditableMessage,
  tutorText
} from "./utility";

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

// bot.action("view-game", (ctx) => {
// })

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

bot.action("start-game", async (ctx) => {
  const message = ctx.update.callback_query.message;
  if (!message) return;
  bot.telegram.editMessageText(
    message.chat.id,
    message.message_id,
    undefined,
    "We will start a new game!"
  );
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
    await ctx.sendPhoto(getBoardImage(game.fen, game.userSide), {
      caption: `Your challenge against the engine has started. You are ${game.userSide}. Make moves using messages like \`e2e4\`. Receive more help using /help`,
      parse_mode: "Markdown"
    });
  }
});

bot.command("me", async (ctx) => {
  const username = ctx.from.username;
  const [_, editMessage] = await sendEditableMessage(
    ctx,
    `Retrieving your current game...`
  );
  if (!username) {
    editMessage("Something went wrong, contact @woojiahao to receive support");
    return;
  }

  const user = await getUser(username);
  const game = await getUserOngoingGame(user);

  if (!game) {
    editMessage("You do not have any ongoing games, use /start to create one");
    return;
  }
  if (game.userSide === "BLACK") {
    await ctx.sendPhoto(
      `https://fen2image.chessvision.ai/${game.fen}?pov=black`
    );
  } else {
    await ctx.sendPhoto(`https://fen2image.chessvision.ai/${game.fen}`);
  }
});

bot.command("move", async (ctx) => {
  const username = ctx.from.username;
  const [_, editMessage] = await sendEditableMessage(
    ctx,
    `Retrieving your current game...`
  );
  if (!username) {
    editMessage("Something went wrong, contact @woojiahao to receive support");
    return;
  }

  const user = await getUser(username);
  const game = await getUserOngoingGame(user);

  if (!game) {
    editMessage("You do not have any ongoing games, use /start to create one");
    return;
  }

  const move = ctx.update.message.text.slice(6);
  const chess = new Chess(game.fen);
  try {
    chess.move(move);
    const engineMove: string | null = await engine.getMove(chess.fen());
    if (!engineMove) {
      editMessage("Something wrong happened internally");
      return;
    }
    chess.move(engineMove);
    game.fen = chess.fen();
    game.save();
    if (game.userSide === "BLACK") {
      await ctx.sendPhoto(
        `https://fen2image.chessvision.ai/${game.fen}?pov=black`
      );
    } else {
      await ctx.sendPhoto(`https://fen2image.chessvision.ai/${game.fen}`);
    }
  } catch (e) {
    console.log(e);
    editMessage("Invalid move!");
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
