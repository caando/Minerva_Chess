import { DataType, Column, Model, Table, HasMany } from "sequelize-typescript"
import { Game } from './game';

@Table({ tableName: "users", paranoid: true })
export class User extends Model {
  @Column({
    autoIncrement: true,
    primaryKey: true,
    allowNull: false,
    type: DataType.BIGINT,
  })
  id!: number

  @Column({
    allowNull: false,
    unique: true,
    type: DataType.TEXT,
  })
  username!: string

  @HasMany(() => Game)
  games?: Game[]

}
