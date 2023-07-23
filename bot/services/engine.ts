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
    let prevMessage = "";
    let lastMessage = "";
    engine.stdout.on("data", (data: Buffer) => {
      const lines = data.toString().split(/\r?\n/)
      lastMessage = lastMessage.concat(lines[0]);
      if (lines.length > 1) {
        prevMessage = lastMessage;
        lastMessage = lines[lines.length-1];
      }
    });
    engine.stdin.write(`ucinewgame\n`);
    engine.stdin.write(`position fen ${FEN}\n`);
    engine.stdin.write(`go movetime ${COMPUTING_TIME_PER_MOVE}\n`);
    await this.sleep(COMPUTING_TIME_PER_MOVE * 2);
    const match = prevMessage.match(/bestmove ([+a-zA-Z0-9]+)/);
    engine.stdin.write(`quit\n`);
    engine.kill("SIGKILL");
    return match ? match[1] : null;
  }
}

const engine = new EngineService("./dependencies/minerva_chess");

export { EngineService, engine };
