import * as dotenv from "dotenv";
import { Context, Telegraf } from "telegraf";
import { Update } from "typegram";
dotenv.config();

const bot: Telegraf<Context<Update>> = new Telegraf(
  process.env.TELEGRAM_BOT_TOKEN as string
);

bot.start((ctx) => {
  ctx.reply("Hello " + ctx.from.first_name + "!");
});

bot.launch();
