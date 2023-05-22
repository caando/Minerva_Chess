import { timeStamp } from "console";
import { DataTypes, Op, Sequelize } from "sequelize";

export const db = new Sequelize({
  dialect: "sqlite",
  storage: "./minerva_chess.db"
});

export const Game = db.define(
  "games",
  {
    id: {
      type: DataTypes.INTEGER,
      primaryKey: true,
      autoIncrement: true
    },
    gameId: {
      type: DataTypes.TEXT,
      allowNull: false
    },
    status: {
      type: DataTypes.TEXT,
      allowNull: false,
      defaultValue: "STARTED"
    },
    username: {
      type: DataTypes.TEXT,
      allowNull: false
    }
  },
  {
    timestamps: false
  }
);

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

export async function addGame(gameId: string, username: string) {
  await Game.create(
    {
      gameId: gameId,
      status: "STARTED",
      username: username
    },
    {
      fields: ["gameId", "status", "username"]
    }
  );
}

export async function userHasExistingGame(username: string) {
  const game = await Game.findOne({
    where: {
      username: username,
      [Op.not]: [{ status: "ENDED" }]
    }
  });
  return game !== null;
}
