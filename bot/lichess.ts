import axios from "axios";
import ndjson from "ndjson";
import axiosRetry from "axios-retry";
import * as dotenv from "dotenv";
import { Game } from "./models";
dotenv.config();

const request = axios.create({
  baseURL: "https://lichess.org/api/",
  headers: {
    Authorization: `Bearer ${process.env.LICHESS_TOKEN}`
  }
});

axiosRetry(request, {
  retries: 5,
  retryDelay: (retryCount) => {
    console.log(`Retrying (${retryCount}/5)...`);
    return retryCount * 30000;
  }
});

const startPosFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";

export async function healthCheck(): Promise<boolean> {
  try {
    const user = await request.get("/account");
    console.log(user);
    return true;
  } catch (error) {
    console.log(error);
    return false;
  }
}

// TODO: Let users customize the level of the game
export async function challenge(): Promise<Error | Game> {
  try {
    const game = await request.post("/challenge/ai", {
      level: 2,
      clock: {
        limit: 10800,
        increment: 60
      },
      color: "random",
      variant: "standard"
    });

    return {
      id: game.data.id,
      userSide: game.data.player
    };
  } catch (error) {
    return error as Error;
  }
}

export async function getGame(
  gameId: string,
  onLoad: (fen: string) => void,
  onClose: () => void = () => {
    return;
  }
) {
  try {
    const { data } = await request.get(`/bot/game/stream/${gameId}`, {
      headers: {
        Accept: "application/x-ndjson"
      },
      responseType: "stream"
    });

    let hasLoaded = false;

    // Lichess return ndJSON for the board state so read that and parse the first which is the initial board state
    data
      .pipe(ndjson.parse())
      .on("data", (obj: any) => {
        if (hasLoaded) throw new Error("finished");
        const { fen } = obj;
        onLoad(getFEN(fen));
        hasLoaded = true;
      })
      .on("end", () => {
        onClose();
      })
      .on("error", () => {
        console.log("error detected");
      });
  } catch (error) {
    console.log(error);
  }
}

function getFEN(fen: string) {
  return !fen ? startPosFEN : fen;
}
