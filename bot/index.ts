import * as dotenv from "dotenv";
import { Context, NarrowedContext, Telegraf } from "telegraf";
import { Update } from "typegram";
import {
  addGame,
  getGames,
  getUserOngoingGame,
  testConnection,
  userHasExistingGame
} from "./database";
import { challenge, getGame, healthCheck } from "./lichess";
import { Message } from "telegraf/typings/core/types/typegram";
dotenv.config();

// TODO: Setup logger
const bot: Telegraf<Context<Update>> = new Telegraf(
  process.env.TELEGRAM_BOT_TOKEN as string
);

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
  /games to list the games currently being played
  /board <game id> to view the board state of a previously started match
  /me to view the board state of your current game
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

  if (await userHasExistingGame(username)) {
    editCreateMessage(
      "You already have an existing game, finish that game first. Use /me to view the board state of that game"
    );
    return;
  }

  const game = await challenge();
  if (game instanceof Error) {
    editCreateMessage(
      "Failed to create a new challenge, contact @woojiahao to receive support"
    );
    return;
  }

  await addGame(game.id, username);
  editCreateMessage(`Game started, you are ${game.userSide}`);
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

  const game = await getUserOngoingGame(username);
  if (!game) {
    editMessage("You do not have any ongoing games, use /start to create one");
    return;
  }

  let hasLoaded = false;
  await getGame(game.id, async (fen) => {
    if (!hasLoaded)
      await ctx.sendPhoto(`https://fen2image.chessvision.ai/${fen}`);
    hasLoaded = true;
  });
});

async function main() {
  bot.launch();
}

(async () => {
  await main();
})();
