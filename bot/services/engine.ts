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

    let lastMessage: Buffer = Buffer.alloc(0);
    engine.stdout.on("data", (data: Buffer) => {
      lastMessage = data;
    });
    engine.stdin.write(`ucinewgame\n`);
    engine.stdin.write(`position fen ${FEN}\n`);
    engine.stdin.write(`go movetime ${COMPUTING_TIME_PER_MOVE}\n`);
    await this.sleep(COMPUTING_TIME_PER_MOVE * 2);
    const match = lastMessage.toString().match(/bestmove ([+a-zA-Z0-9]+)/);
    return match ? match[1] : null;
  }
}

const engine = new EngineService("./dependencies/Stockfish/src/stockfish");

export { EngineService, engine };
