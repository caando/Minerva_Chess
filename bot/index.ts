import * as dotenv from "dotenv";
import { addCommands, connect } from "./services/telegram/telegram";
dotenv.config();

async function main() {
  const bot = connect();
  addCommands(bot);
  await bot.launch();

  // Enable graceful stop
  process.once("SIGINT", () => bot.stop("SIGINT"));
  process.once("SIGTERM", () => bot.stop("SIGTERM"));
}

(async () => {
  await main();
})();
