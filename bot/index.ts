import * as dotenv from "dotenv";
import { Context, Telegraf } from "telegraf";
import { Update } from "typegram";
import {
  addGame,
  getGames,
  testConnection,
  userHasExistingGame
} from "./database";
import { challenge, healthCheck } from "./lichess";
import { Message } from "telegraf/typings/core/types/typegram";
dotenv.config();

// TODO: Setup logger
const bot: Telegraf<Context<Update>> = new Telegraf(
  process.env.TELEGRAM_BOT_TOKEN as string
);

function editMessage(message: Message, text: string) {
  bot.telegram.editMessageText(
    message.chat.id,
    message.message_id,
    undefined,
    text
  );
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
  const statusMsg = await ctx.reply("Processing your challenge...");
  if (!username) {
    editMessage(
      statusMsg,
      "Something happened, contact @woojiahao to receive support"
    );
    return;
  }

  if (await userHasExistingGame(username)) {
    editMessage(
      statusMsg,
      "You already have an existing game, finish that game first. Use /me to view the board state of that game"
    );
    return;
  }

  const game = await challenge();
  if (game instanceof Error) {
    editMessage(
      statusMsg,
      "Failed to create a new challenge, contact @woojiahao to receive support"
    );
    return;
  }

  await addGame(game.id, username);
  editMessage(statusMsg, `Game started, you are ${game.side}`);
});

async function main() {
  bot.launch();
}

(async () => {
  await main();
})();
