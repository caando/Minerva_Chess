import {
  addGame,
  addGameHistory,
  addUser,
  endGame,
  forfeitGame,
  getGame,
  getGameHistory,
  getGames,
  getUser,
  getUserGames,
  getUserOngoingGame,
  setGameStatus,
  updateGameFen,
  userHasExistingGame,
  whiteStartPos
} from "../services/database";
import { GameStatus } from "../services/models/game";
import { HistoryPlayer } from "../services/models/history";

describe("getGames", () => {
  beforeEach(() => {
    return addUser("johndoe");
  });

  test("no games returns empty array", async () => {
    const games = await getGames();
    expect(games).toHaveLength(0);
  });

  test("some games return all unended games", async () => {
    const endedGame = await addGame(1);
    endedGame.set("status", "ENDED");
    await endedGame.save();
    await addGame(2);
    const games = await getGames();
    expect(games).toHaveLength(1);
  });
});

describe("addGame", () => {
  beforeEach(() => {
    return addUser("johndoe");
  });

  test("random of less than 0.5 is BLACK", async () => {
    jest.spyOn(global.Math, "random").mockReturnValue(0.4);
    await addGame(1);
    const game = await getGame(1);
    expect(game).not.toBeNull();
    if (!game) return;
    expect(game.status).toBe("STARTED");
    expect(game.userSide).toBe("BLACK");
  });

  test("random of greater than 0.5 is WHITE", async () => {
    jest.spyOn(global.Math, "random").mockReturnValue(0.6);
    await addGame(1);
    const game = await getGame(1);
    expect(game).not.toBeNull();
    if (!game) return;
    expect(game.status).toBe("STARTED");
    expect(game.userSide).toBe("WHITE");
  });
});

describe("getUser", () => {
  test("create user if not exists", async () => {
    const user = await getUser("johndoe");
    expect(user.dataValues.username).toBe("johndoe");
  });

  test("return existing user if previously created", async () => {
    await addUser("johndoe");
    const user = await getUser("johndoe");
    expect(user.dataValues.username).toBe("johndoe");
  });
});

describe("existing game", () => {
  beforeEach(() => {
    return addUser("johndoe");
  });

  test("retrieves nothing if ongoing game is not present", async () => {
    const johndoe = await getUser("johndoe");
    const ongoingGame = await getUserOngoingGame(johndoe);
    const hasOngoingGame = await userHasExistingGame(johndoe);
    expect(ongoingGame).toBeNull();
    expect(hasOngoingGame).not.toBeTruthy();
  });

  test("retrieves only ongoing game once created", async () => {
    const endedGame = await addGame(1);
    endedGame.set("status", "ENDED");
    await addGame(1);
    const johndoe = await getUser("johndoe");
    const ongoingGame = await getUserOngoingGame(johndoe);
    const hasOngoingGame = await userHasExistingGame(johndoe);
    expect(ongoingGame?.dataValues.status).toBe("STARTED");
    expect(hasOngoingGame).toBeTruthy();
  });
});

describe("game status changes", () => {
  beforeEach(async () => {
    await addUser("johndoe");
    await addGame(1);
  });

  test("forfeitGame changes game status to be FORFEITED", async () => {
    const game = await getGame(1);
    if (!game) return;
    await forfeitGame(game);
    const updatedGame = await getGame(1);
    expect(updatedGame?.dataValues.status).toBe("FORFEITED");
  });

  test("endGame changes game status to be ENDED", async () => {
    const game = await getGame(1);
    if (!game) return;
    await endGame(game);
    const updatedGame = await getGame(1);
    expect(updatedGame?.dataValues.status).toBe("ENDED");
  });

  test("setGameStatus changes game status to be value specified", async () => {
    const game = await getGame(1);
    if (!game) return;
    await setGameStatus(game, GameStatus.STALEMATE);
    const updatedGame = await getGame(1);
    expect(updatedGame?.dataValues.status).toBe(GameStatus.STALEMATE);
  });
});

describe("update game fen", () => {
  test("sets fen field in database", async () => {
    await addUser("johndoe");
    const game = await addGame(1);
    await updateGameFen(game, "mock fen");
    const updatedGame = await getGame(1);
    expect(updatedGame?.dataValues.fen).toBe("mock fen");
  });
});

// Note that these test cases are independent of any engine moves (those are
// tested separately under the service layer)
describe("game history", () => {
  beforeEach(() => {
    return addUser("johndoe");
  });

  test("new game has one history", async () => {
    const game = await addGame(1);
    if (!game) return;
    const history = await getGameHistory(game);
    expect(history).toHaveLength(1);
    expect(history[0].fen).toBe(whiteStartPos);
  });

  test("game with N moves has N + 1 history", async () => {
    const game = await addGame(1);
    if (!game) return;
    const expected = [
      { fen: whiteStartPos, move: "startpos", player: HistoryPlayer.USER },
      { fen: "mock fen 1", move: "e2e4", player: HistoryPlayer.BOT },
      { fen: "mock fen 2", move: "Kd2e5", player: HistoryPlayer.USER },
      { fen: "mock fen 3", move: "Bc6c8", player: HistoryPlayer.BOT }
    ];
    await addGameHistory(game, "e2e4", "mock fen 1", HistoryPlayer.BOT);
    await addGameHistory(game, "Kd2e5", "mock fen 2", HistoryPlayer.USER);
    await addGameHistory(game, "Bc6c8", "mock fen 3", HistoryPlayer.BOT);
    const history = await getGameHistory(game);
    expect(history).toHaveLength(4);
    for (let i = 0; i < 4; i++) {
      expect(history[i].fen).toBe(expected[i].fen);
      expect(history[i].move).toBe(expected[i].move);
      expect(history[i].player).toBe(expected[i].player);
    }
  });
});

describe("get user games", () => {
  test("retrieves all relevant user games only", async () => {
    await addUser("johndoe");
    await addUser("maryanne");
    await addUser("peterpan");

    await addGame(1);
    await addGame(1);
    await addGame(1);
    await addGame(2);
    await addGame(2);
    await addGame(3);

    const johnGames = await getUserGames("johndoe");
    expect(johnGames).toHaveLength(3);
    for (let i = 0; i < 3; i++)
      expect(johnGames[i].player.username).toBe("johndoe");
    const maryGames = await getUserGames("maryanne");
    expect(maryGames).toHaveLength(2);
    for (let i = 0; i < 2; i++)
      expect(maryGames[i].player.username).toBe("maryanne");
    const peterGames = await getUserGames("peterpan");
    expect(peterGames).toHaveLength(1);
    for (let i = 0; i < 1; i++)
      expect(peterGames[i].player.username).toBe("peterpan");
  });
});
