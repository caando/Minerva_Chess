import { Sequelize } from "sequelize-typescript";
import { Op } from "sequelize";
import { Game } from "./models/game";
import { User } from "./models/user";

const models = [Game, User];

const db = new Sequelize({
  database: "database_development",
  dialect: "sqlite",
  username: "root",
  password: "",
  storage: "./minerva_chess.db",
  models: models
});

const whiteStartPos =
  "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
const blackStartPos =
  "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b KQkq - 0 1";

export async function testConnection() {
  try {
    await db.authenticate();
    console.log("Database authenticated successfully");
  } catch (error) {
    console.error("Unable to connect to database:", error);
  }
}

export async function getGames() {
  const games = await Game.findAll({
    where: {
      [Op.not]: [
        {
          status: "ENDED"
        }
      ]
    }
  });
  return games.map((game) => game.toJSON());
}

export async function addGame(userId: number): Promise<Game> {
  // const playerIsWhite: boolean = Math.random() >= 0.5;
  const playerIsWhite = false;
  return await Game.create(
    {
      status: "STARTED",
      playerIsWhite: playerIsWhite,
      userId: userId,
      userSide: playerIsWhite ? "WHITE" : "BLACK",
      fen: whiteStartPos
    },
    {
      fields: ["status", "playerIsWhite", "userId", "userSide", "fen"]
    }
  );
}

export async function addUser(username: string): Promise<User> {
  return User.create({ username: username });
}

export async function getUser(username: string): Promise<User> {
  const user = await User.findOne({
    where: {
      username: username
    }
  });
  return user || addUser(username);
}

export async function userHasExistingGame(user: User) {
  const game = await getUserOngoingGame(user);
  return !!game;
}

export async function getUserOngoingGame(user: User): Promise<Game | null> {
  return Game.findOne({
    where: {
      userId: user.id,
      [Op.not]: [{ status: "ENDED" }]
    }
  });
}
