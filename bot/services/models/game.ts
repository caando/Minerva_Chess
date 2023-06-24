import {
  BelongsTo,
  Column,
  DataType,
  ForeignKey,
  HasOne,
  Model,
  Table
} from "sequelize-typescript";

import { User } from "./user";
import { History } from "./history";

export enum GameStatus {
  STARTED = "STARTED",
  ENDED = "ENDED",
  FORFEITED = "FORFEITED",
  USER_WIN = "USER_WIN",
  BOT_WIN = "BOT_WIN"
}

export enum UserSide {
  WHITE = "WHITE",
  BLACK = "BLACK"
}

@Table({ tableName: "games" })
export class Game extends Model {
  @Column({
    autoIncrement: true,
    primaryKey: true,
    allowNull: false,
    type: DataType.BIGINT
  })
  id!: number;

  @Column({
    allowNull: false,
    unique: false,
    type: DataType.TEXT
  })
  fen!: string;

  @Column({
    allowNull: false,
    unique: false,
    type: DataType.ENUM(...Object.values(UserSide))
  })
  userSide!: UserSide;

  @Column({
    allowNull: false,
    type: DataType.ENUM(...Object.values(GameStatus)),
    defaultValue: GameStatus.STARTED
  })
  status!: GameStatus;

  @ForeignKey(() => User)
  userId!: number;

  @BelongsTo(() => User)
  player!: User;

  @HasOne(() => History, "gameId")
  history!: History;
}
