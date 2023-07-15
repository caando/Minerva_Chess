import { Chess } from "chess.js";
import { exec } from "child_process";
import {
  addGame,
  addUser,
  getGame,
  getUser,
  whiteStartPos
} from "../../services/database";
import { engine } from "../../services/engine";
import { ongoingGameError } from "../../services/telegram/errors";
import { startGame } from "../../services/telegram/service";
import { Game } from "../../services/models/game";

jest.mock("../../services/engine");

// Before every test, create the database from scratch to ensure data purity
beforeEach(() => {
  return new Promise((resolve, reject) => {
    exec("sequelize db:migrate", { env: process.env }, (err) =>
      err ? reject(err) : resolve(null)
    );
  });
});

// After every test, tear down the entire database by undoing all migrations
afterEach(() => {
  return new Promise((resolve, reject) => {
    exec("sequelize db:migrate:undo:all", { env: process.env }, (err) =>
      err ? reject(err) : resolve(null)
    );
  });
});

describe("start game", () => {
  beforeEach(() => {
    return addUser("johndoe");
  });

  test("returns ongoing game error if already exists", async () => {
    await addGame(1);
    expect(await startGame("johndoe")).toBe(ongoingGameError);
  });

  test("creates game with player as WHITE", async () => {
    const johndoe = await getUser("johndoe");
    jest.spyOn(global.Math, "random").mockReturnValue(0.7);
    const game = await addGame(johndoe.id);
    expect(game.dataValues.fen).toBe(whiteStartPos);
    expect(game.dataValues.userId).toBe(johndoe.id);
    expect(game.dataValues.userSide).toBe("WHITE");
  });

  test("creates game with player as BLACK and makes engine move first", async () => {
    // Need to comment because TS complains that there isn't a method
    // Jest adds this method to let us mock the engine calls without using the actual engine
    // eslint-disable-next-line @typescript-eslint/ban-ts-comment
    // @ts-ignore
    engine.getMove.mockImplementation(async () => "e2e4");
    // Ensure BLACK
    jest.spyOn(global.Math, "random").mockReturnValue(0.2);
    const game = await startGame("johndoe");
    const chess = new Chess(whiteStartPos);
    chess.move("e2e4");
    expect(game).toBeDefined();
    expect(game).toBeInstanceOf(Game);
    if (game instanceof Error) return;
    expect(game.dataValues.fen).toBe(chess.fen());
  });
});
