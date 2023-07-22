import {
  addGame,
  addUser,
  getGame,
  getGames,
  getUser,
  getUserOngoingGame,
  userHasExistingGame
} from "../services/database";

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
