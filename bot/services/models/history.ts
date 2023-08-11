import {
  BelongsTo,
  Column,
  DataType,
  ForeignKey,
  Model,
  Table
} from "sequelize-typescript";

import { Game } from "./game";

export enum HistoryPlayer {
  USER = "USER",
  BOT = "BOT"
}

@Table({ tableName: "histories" })
export class History extends Model {
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
    type: DataType.TEXT
  })
  move!: string;

  @Column({
    allowNull: false,
    unique: false,
    type: DataType.ENUM(...Object.values(HistoryPlayer))
  })
  player!: HistoryPlayer;

  @ForeignKey(() => Game)
  gameId!: number;

  @BelongsTo(() => Game)
  game!: Game;
}
