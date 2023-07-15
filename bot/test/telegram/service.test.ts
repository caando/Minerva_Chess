import { exec } from "child_process";
import { addUser, getGames, getUser } from "../../services/database";
import { startGame } from "../../services/telegram/service";

describe("service layer", () => {
  // Before every test, create the database from scratch to ensure data purity
  beforeEach(async () => {
    await new Promise((resolve, reject) => {
      exec("sequelize db:migrate", { env: process.env }, (err) =>
        err ? reject(err) : resolve(null)
      );
    });
  });
  // After every test, tear down the entire database by deleting the file
  afterEach(async () => {
    await new Promise((resolve, reject) => {
      exec("rm ./minerva_chess_test.db", { env: process.env }, (err) =>
        err ? reject(err) : resolve(null)
      );
    });
  });
  test("create game", async () => {
    await addUser("johndoe");
    console.log(await getUser("johndoe"));
  });
});
