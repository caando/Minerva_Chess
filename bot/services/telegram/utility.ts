import { Context, NarrowedContext } from "telegraf";
import {
  InlineKeyboardButton,
  Message,
  Update
} from "telegraf/typings/core/types/typegram";
import { Game, UserSide } from "../models/game";
import bot from "./telegram";
import { getGameHistory } from "../database";

export const helpText = `
    👑 *Minerva Chess* 👑

    Hello! I am @MinervaChessBot, an interactive UI for the Minerva Chess engine.

    Minerva Chess is a chess engine built by Jikun and Jia Hao for their Orbital 2023 Apollo project.

    The chess engine is built with C++ and this Telegram bot is built using Typescript.

    To get started, I have prepared a few actions you can take!

    If you want more help on using this bot, click on the _"More help"_ button below!

    *Command glossary*

    1. /help to get this prompt again
    2. /start to start a game
    3. /me to view your currently active game
    4. /forfeit to forfeit your currently active game
    5. /games to view all your past games
    6. /history <game> to retrieve the game history of a game played

    Enjoy!
  `.replace(/  +/g, "");

export const tutorText = `
    *Usage Guide*

    Minerva Chess is a chess engine built by Jikun and Jia Hao for their Orbital 2023 Apollo project.

    The chess engine is built with C++ and this Telegram bot is built using Typescript.

    *Creating games*
    To start a game, use the /start command. Your side will be randomly chosen and you will be playing
    against the Minerva Chess engine.

    You are only able to play one active game at a time. If you wish to reset the game, please use
    /forfeit to forfeit the current game before using /start again.

    *Moving your pieces*
    To make moves, use the Algebraic Notation of Long Algebraic Notation to indicate the moves.
    For instance, if you wish to move the pawn from \`e2\` to \`e4\`, you can simply send \`e2e4\`
    in the chat.

    Playing against Minerva Chess in a group chat? Do not worry, only your moves will be registered!

    *Reviewing your plays*
    If you wish to pick up a game from where you last played, you can use /me to view the currently
    active game. Otherwise, you might find the /games and /history commands handy when searching for
    your previously played games.
  `.replace(/  +/g, "");

export function getBoardImage(fen: string, side: UserSide) {
  return `https://fen2image.chessvision.ai/${fen}?pov=${side.toLowerCase()}`;
}

export async function sendEditableMessage(
  context: NarrowedContext<
    Context<Update>,
    {
      message: Update.New & Update.NonChannel & Message.TextMessage;
      update_id: number;
    }
  >,
  message: string
): Promise<[Message.TextMessage, (text: string) => void]> {
  const msg = await context.reply(message);
  const editMessage = function (text: string) {
    bot.telegram.editMessageText(msg.chat.id, msg.message_id, undefined, text);
  };
  return [msg, editMessage];
}

export async function editMessage(message: Message, text: string) {
  bot.telegram.editMessageText(
    message.chat.id,
    message.message_id,
    undefined,
    text
  );
}

// Helper function to send image with markdown caption
export async function sendPhotoWithCaption<T extends Update>(
  context: NarrowedContext<Context<Update>, T>,
  photoUrl: string,
  caption: string
) {
  context.sendPhoto(photoUrl, {
    caption: caption,
    parse_mode: "Markdown"
  });
}

export async function sendChessboard<T extends Update>(
  context: NarrowedContext<Context<Update>, T>,
  game: Game,
  title: string,
  body: string
) {
  context.sendPhoto(getBoardImage(game.fen, game.userSide), {
    caption: `
        *${title}*

        ${body}

        Make moves using messages like \`e2e4\`. Receive more help using /help
      `.replace(/  +/g, ""),
    parse_mode: "Markdown"
  });
}
