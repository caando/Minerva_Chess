import { Context, NarrowedContext, Telegraf } from "telegraf";
import { Update } from "typegram";
import { Message } from "telegraf/typings/core/types/typegram";
import { engine } from "../engine";
import { Chess } from "chess.js";
import { UserSide } from "../models/game";
import {
  getUser,
  addGame,
  getGames,
  getUserOngoingGame,
  testConnection,
  userHasExistingGame
} from "../database";
import IgnoreOldMiddleWare from "./middlewares/ignoreOld";

// TODO: Setup logger
const bot: Telegraf<Context<Update>> = new Telegraf(
  process.env.TELEGRAM_BOT_TOKEN as string
);

bot.use(IgnoreOldMiddleWare);

async function sendEditableMessage(
  context: NarrowedContext<
    Context<Update>,
    {
      message: Update.New & Update.NonChannel & Message.TextMessage;
      update_id: number;
    }
  >,
  message: string
): Promise<[Message.TextMessage, (text: string) => void]> {
  const msg = await context.reply(message);
  const editMessage = function (text: string) {
    bot.telegram.editMessageText(msg.chat.id, msg.message_id, undefined, text);
  };
  return [msg, editMessage];
}

bot.command("ping", (ctx) => {
  ctx.reply(`Hello ${ctx.from.first_name}!`);
});

bot.help((ctx) => {
  const helpMsg = `
  /ping to test the bot online status
  /start to start a new game with the chess engine
  /move <move> play the move in game (in chess notation)
  /me to view the board state of your current game
  /teabag teabag
  `.replace(/  +/g, "");
  ctx.reply(helpMsg);
});

bot.command("start", async (ctx) => {
  const username = ctx.from.username;
  const [_, editCreateMessage] = await sendEditableMessage(
    ctx,
    "Processing your challenge..."
  );
  if (!username) {
    editCreateMessage(
      "Something went wrong, contact @woojiahao to receive support"
    );
    return;
  }

  const user = await getUser(username);

  let prevGame = await getUserOngoingGame(user);
  if (prevGame !== null) {
    prevGame.set("status", "ENDED");
    prevGame.save();
  }

  const game = await addGame(user.id)

  if (game instanceof Error) {
    editCreateMessage(
      "Failed to create a new challenge, contact @woojiahao to receive support"
    );
    return;
  }

  editCreateMessage(`Game started, you are ${game.userSide}`);

  if (game.userSide === UserSide.BLACK) {
    const chess = new Chess(game.fen);
    const move: string | null = await engine.getMove(chess.fen());
    if (!move) {
      editCreateMessage("Something went wrong internally");
      return;
    }
    chess.move(move);
    await game.set("fen", chess.fen())
    game.save();
  }

  if (game.userSide === "BLACK") {
    await ctx.sendPhoto(`https://fen2image.chessvision.ai/${game.fen}?pov=black`);
  } else {
    await ctx.sendPhoto(`https://fen2image.chessvision.ai/${game.fen}`);
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
    await ctx.sendPhoto(`https://fen2image.chessvision.ai/${game.fen}?pov=black`);
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
      await ctx.sendPhoto(`https://fen2image.chessvision.ai/${game.fen}?pov=black`);
    } else {
      await ctx.sendPhoto(`https://fen2image.chessvision.ai/${game.fen}`);
    }
  } catch (e) {
    console.log(e)
    editMessage("Invalid move!");
    return;
  }
});

bot.command('teabag', async (ctx) => {
  await ctx.sendAnimation(`https://tenor.com/view/halo-teabag-gif-12948320`);
})

export default bot;
