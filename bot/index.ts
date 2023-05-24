import * as dotenv from "dotenv";
dotenv.config();

import bot from "./services/telegram/telegram";

async function main() {
  await bot.launch();

  // Enable graceful stop
  process.once("SIGINT", () => bot.stop("SIGINT"));
  process.once("SIGTERM", () => bot.stop("SIGTERM"));
}

(async () => {
  await main();
})();
