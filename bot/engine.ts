import { spawn } from "node:child_process";

export default class EngineService {
  private executable: string;
  constructor(executable: string) {
    this.executable = executable;
  }

  async getMove(FEN: string, computingTime: number): Promise<string> {
    const engine = await spawn(this.executable); // does not have a defined type yet

    let lastMessage: Buffer = Buffer.alloc(0);
    engine.stdout.on("data", (data: Buffer) => {
      lastMessage = data;
    });
    engine.stdin.write(`ucinewgame\n`);
    engine.stdin.write(`position ${FEN}\n`);
    engine.stdin.write(`go\n`);
    const sleep = (ms: number) => new Promise((r) => setTimeout(r, ms));
    await sleep(computingTime);
    engine.stdin.write(`stop\n`);
    await sleep(10);
    return lastMessage.toString();
  }
}
