import { spawn } from "node:child_process";

const COMPUTING_TIME_PER_MOVE = 500; // milliseconds

class EngineService {
  private executable: string;
  constructor(executable: string) {
    this.executable = executable;
  }

  private sleep(ms: number) {
    return new Promise((r) => setTimeout(r, ms));
  }

  async getMove(FEN: string): Promise<string | null> {
    const engine = await spawn(this.executable); // does not have a defined type yet
    const messages: string[] = [];
    engine.stdout.on("data", (data: Buffer) => {
      messages.push(data.toString());
    });
    engine.stdin.write(`ucinewgame\n`);
    engine.stdin.write(`position fen ${FEN}\n`);
    engine.stdin.write(`go movetime ${COMPUTING_TIME_PER_MOVE}\n`);
    await this.sleep(COMPUTING_TIME_PER_MOVE * 2);
    engine.stdin.write(`quit\n`);
    const allData = messages
      .join("")
      .split(/\r?\n/)
      .map((line) => line.trim())
      .filter((line) => line !== "");
    let bestMove = null;
    for (const line of allData) {
      const match = line.match(/bestmove ([+a-zA-Z0-9]+)/);
      if (match) bestMove = match[1];
    }
    engine.kill("SIGKILL");
    return bestMove;
  }
}

const engine = new EngineService("./dependencies/minerva_chess");

export { EngineService, engine };
