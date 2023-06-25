import { Sequelize } from "sequelize-typescript";
import { Op } from "sequelize";
import { Game, GameStatus } from "./models/game";
import { User } from "./models/user";
import { History, HistoryPlayer } from "./models/history";

const models = [Game, User, History];

const db = new Sequelize({
  database: "database_development",
  dialect: "sqlite",
  username: "root",
  password: "",
  storage: "./minerva_chess.db",
  models: models
});

export const whiteStartPos =
  "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

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
    },
    include: History
  });
  return games.map((game) => game.toJSON());
}

export async function addGame(userId: number): Promise<Game> {
  const playerIsWhite: boolean = Math.random() >= 0.5;
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
      status: "STARTED"
    }
  });
}

export async function endGame(game: Game) {
  game.set("status", "ENDED");
  await game.save();
}

export async function updateGameFen(game: Game, fen: string) {
  game.set("fen", fen);
  await game.save();
}

export async function addGameHistory(
  game: Game,
  move: string,
  fen: string,
  player: HistoryPlayer
) {
  return await History.create(
    {
      gameId: game.id,
      fen: fen,
      player: player,
      move: move
    },
    {
      fields: ["gameId", "fen", "player", "move"]
    }
  );
}

export async function getGameHistory(game: Game) {
  const records = [
    { fen: whiteStartPos, player: HistoryPlayer.USER, move: "startpos" }
  ];
  const history = await History.findAll({
    where: {
      gameId: game.id
    },
    order: [["id", "ASC"]]
  });
  return records.concat(
    history
      .map((h) => h.toJSON())
      .map((h) => ({ fen: h.fen, player: h.player, move: h.move }))
  );
}

export async function getGame(gameId: number) {
  return await Game.findOne({
    where: {
      id: gameId
    }
  });
}

export async function forfeitGame(game: Game) {
  game.set("status", "FORFEITED");
  await game.save();
}

export async function getUserGames(username: string) {
  const games = await Game.findAll({
    where: {
      "$player.username$": username
    },
    include: [User],
    order: [["createdAt", "DESC"]]
  });
  return games.map((game) => game.toJSON());
}
