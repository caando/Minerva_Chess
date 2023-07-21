import { Chess } from "chess.js";
import { exec } from "child_process";
import {
  addGame,
  addUser,
  getGame,
  getGameHistory,
  getUser,
  getUserGames,
  whiteStartPos
} from "../../services/database";
import { engine } from "../../services/engine";
import { Game, GameStatus } from "../../services/models/game";
import {
  invalidMoveError,
  missingGameError,
  ongoingGameError
} from "../../services/telegram/errors";
import {
  chunkSelectGamesAction,
  makeMove,
  startGame
} from "../../services/telegram/service";

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
    const gameHistory = await getGameHistory(game);
    // Game history should include the black move too
    expect(gameHistory).toHaveLength(1);
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
    const gameHistory = await getGameHistory(game);
    // Game history should include the black move too
    expect(gameHistory).toHaveLength(2);
  });
});

describe("makeMove", () => {
  beforeEach(() => {
    // For all test cases, start the game as WHITE for player
    jest.spyOn(global.Math, "random").mockReturnValue(0.7);
    return addUser("johndoe");
  });

  test("missing game returns error", async () => {
    const result = await makeMove("johndoe", "e2e4");
    expect(result).toBe(missingGameError);
  });

  test("attempt invalid move returns error", async () => {
    await addGame(1);
    // Make invalid move
    const result = await makeMove("johndoe", "e2e5");
    expect(result).toBe(invalidMoveError);
  });

  test("regular move saves game history and adds engine move", async () => {
    // eslint-disable-next-line @typescript-eslint/ban-ts-comment
    // @ts-ignore
    engine.getMove.mockImplementation(async () => "d7d5");

    const game = await addGame(1);
    const result = await makeMove("johndoe", "e2e4");
    expect(result).not.toBe(invalidMoveError);
    if (result instanceof Error) return;
    expect(result.status).toBe(GameStatus.STARTED);
    expect(result.engineMove).toBe("d7d5");
    expect(result.game.fen).toBe(
      "rnbqkbnr/ppp1pppp/8/3p4/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 2"
    );
    const gameHistory = await getGameHistory(game);
    expect(gameHistory).toHaveLength(3);
  });

  test("user move causing checkmate updates game status and returns user win state", async () => {
    const game = await addGame(1);
    game.set("fen", "k7/8/2B5/1Q6/8/8/8/7K w - - 0 1");
    await game.save();
    const result = await makeMove("johndoe", "Qb5b7");
    expect(result).not.toBe(invalidMoveError);
    if (result instanceof Error) return;
    expect(result.status).toBe(GameStatus.USER_WIN);
    expect(result.engineMove).toBeNull();
    expect(result.game.fen).toBe("k7/1Q6/2B5/8/8/8/8/7K b - - 1 1");
  });

  test("user move causing stalemate updates game status and returns stalemate state", async () => {
    const game = await addGame(1);
    game.set("fen", "7k/8/8/6Q1/8/8/8/K7 w - - 0 1");
    await game.save();
    const result = await makeMove("johndoe", "Qg5g6");
    expect(result).not.toBe(invalidMoveError);
    if (result instanceof Error) return;
    expect(result.status).toBe(GameStatus.STALEMATE);
    expect(result.engineMove).toBeNull();
    expect(result.game.fen).toBe("7k/8/6Q1/8/8/8/8/K7 b - - 1 1");
  });

  test("user move causing draw updates game status and returns draw state", async () => {
    const game = await addGame(1);
    game.set("fen", "8/5K2/8/7B/3k4/8/8/8 w - - 0 1");
    await game.save();
    const result = await makeMove("johndoe", "Bh5g4");
    expect(result).not.toBe(invalidMoveError);
    if (result instanceof Error) return;
    expect(result.status).toBe(GameStatus.DRAW);
    expect(result.engineMove).toBeNull();
    expect(result.game.fen).toBe("8/5K2/8/8/3k2B1/8/8/8 b - - 1 1");
  });

  test("bot move causing checkmate updates game status and returns bot win state", async () => {
    const game = await addGame(1);
    game.set("fen", "2n5/3q4/2b4P/2K5/8/8/n7/1r5k w - - 0 1");
    await game.save();
    // eslint-disable-next-line @typescript-eslint/ban-ts-comment
    // @ts-ignore
    engine.getMove.mockImplementation(async () => "Rb1c1");

    const result = await makeMove("johndoe", "h6h7");
    expect(result).not.toBe(invalidMoveError);
    if (result instanceof Error) return;
    expect(result.status).toBe(GameStatus.BOT_WIN);
    expect(result.engineMove).toBe("Rb1c1");
    expect(result.game.fen).toBe("2n5/3q3P/2b5/2K5/8/8/n7/2r4k w - - 1 2");
  });

  test("bot move causing stalemate updates game status and returns stalemate status", async () => {
    const game = await addGame(1);
    game.set("fen", "8/7K/8/6q1/8/8/8/k7 w - - 0 1");
    await game.save();
    // eslint-disable-next-line @typescript-eslint/ban-ts-comment
    // @ts-ignore
    engine.getMove.mockImplementation(async () => "Qg5g6");
    const result = await makeMove("johndoe", "Kh7h8");
    expect(result).not.toBe(invalidMoveError);
    if (result instanceof Error) return;
    expect(result.status).toBe(GameStatus.STALEMATE);
    expect(result.engineMove).toBe("Qg5g6");
    expect(result.game.fen).toBe("7K/8/6q1/8/8/8/8/k7 w - - 2 2");
  });

  test("bot move causing draw updates game status and returns draw status", async () => {
    const game = await addGame(1);
    game.set("fen", "8/1k3b2/3R4/8/8/8/8/K7 w - - 0 1");
    await game.save();
    // eslint-disable-next-line @typescript-eslint/ban-ts-comment
    // @ts-ignore
    engine.getMove.mockImplementation(async () => "Kb7c6");
    const result = await makeMove("johndoe", "Rd6c6");
    expect(result).not.toBe(invalidMoveError);
    if (result instanceof Error) return;
    expect(result.status).toBe(GameStatus.DRAW);
    expect(result.engineMove).toBe("Kb7c6");
    expect(result.game.fen).toBe("8/5b2/2k5/8/8/8/8/K7 w - - 0 2");
  });
});

describe("chunkSelectGamesAction", () => {
  // Handle cases of 0 game, 1 game, 9 games, 51 games

  beforeEach(() => {
    return addUser("johndoe");
  });

  test("chunk no games returns no actions", async () => {
    const chunks = await chunkSelectGamesAction("johndoe", 8, 50);
    expect(chunks).toHaveLength(7);
    for (const chunk of chunks) {
      expect(chunk).toHaveLength(0);
    }
  });

  test("chunk one games returns arrays with very first action set", async () => {
    await addGame(1);
    const chunks = await chunkSelectGamesAction("johndoe", 8, 50);
    expect(chunks).toHaveLength(7);
    expect(chunks[0]).toHaveLength(1);
    for (let i = 1; i < 7; i++) {
      expect(chunks[i]).toHaveLength(0);
    }
  });

  test("chunk nine games returns arrays overflowed actions", async () => {
    for (let i = 0; i < 9; i++) await addGame(1);
    const chunks = await chunkSelectGamesAction("johndoe", 8, 50);
    expect(chunks).toHaveLength(7);
    expect(chunks[0]).toHaveLength(8);
    expect(chunks[1]).toHaveLength(1);
    for (let i = 2; i < 7; i++) {
      expect(chunks[i]).toHaveLength(0);
    }
  });

  test("chunk 51 games returns most recent 50", async () => {
    // Ensure WHITE
    jest.spyOn(global.Math, "random").mockReturnValue(0.7);
    for (let i = 0; i < 51; i++) await addGame(1);
    const chunks = await chunkSelectGamesAction("johndoe", 8, 50);
    const games = await getUserGames("johndoe");
    expect(games).toHaveLength(51);
    expect(chunks).toHaveLength(7);
    expect(chunks[6]).toHaveLength(2);
    for (let i = 0; i < 6; i++) {
      expect(chunks[i]).toHaveLength(8);
    }
    expect(chunks[0][0]).toMatchObject({
      text: "1",
      callback_data: `render-board:${games[0].id}:-1`
    });
    expect(chunks[6][1]).toMatchObject({
      text: "50",
      callback_data: `render-board:${games[49].id}:-1`
    });
  });
});
