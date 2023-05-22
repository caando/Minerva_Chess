import * as dotenv from "dotenv";
import bot from './services/telegram'

dotenv.config();

async function main() {
  bot.launch();
}

(async () => {
  await main();
})();
