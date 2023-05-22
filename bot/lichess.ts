import axios from "axios";
import axiosRetry from "axios-retry";
import * as dotenv from "dotenv";
dotenv.config();

const request = axios.create({
  baseURL: "https://lichess.org/api/",
  headers: {
    Authorization: `Bearer ${process.env.LICHESS_TOKEN}`
  }
});

axiosRetry(request, {
  retries: 5,
  retryDelay: (retryCount) => retryCount * 2000
});

export type ApiGame = {
  id: string;
  side: "white" | "black";
};

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
export async function challenge(): Promise<Error | ApiGame> {
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
      side: game.data.player
    };
  } catch (error) {
    return error as Error;
  }
}
