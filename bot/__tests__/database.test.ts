import { exec } from "child_process";
import { addGame, addUser, getGame } from "../services/database";

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
});
