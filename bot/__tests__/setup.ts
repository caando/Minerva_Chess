import { exec } from "child_process";

// Before every test, create the database from scratch to ensure data purity
beforeEach(() => {
  return new Promise((resolve, reject) => {
    exec("sequelize db:migrate", { env: process.env }, (err) =>
      err ? reject(err) : resolve(null)
    );
  });
});

// After every test, tear down the entire database by undoing all migrations
afterEach(() => {
  return new Promise((resolve, reject) => {
    exec("sequelize db:migrate:undo:all", { env: process.env }, (err) =>
      err ? reject(err) : resolve(null)
    );
  });
});

jest.mock("../../services/engine");
