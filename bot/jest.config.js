/** @type {import('ts-jest').JestConfigWithTsJest} */
module.exports = {
  preset: 'ts-jest',
  testEnvironment: 'node',
  testPathIgnorePatterns: [
    "dist/",
    "<rootDir>/__tests__/setup.ts"
  ],
  modulePathIgnorePatterns: [
    "services/telegram/telegram.ts"
  ],
  setupFilesAfterEnv: ["<rootDir>/__tests__/setup.ts"]
};