import { Context, NarrowedContext, Telegraf } from "telegraf";
import { Message, Update } from "telegraf/typings/core/types/typegram";
import { Game, GameStatus, UserSide } from "../models/game";

export const menuText = `
    👑 *Minerva Chess* 👑

    Hello! I am @MinervaChessBot, an interactive UI for the Minerva Chess engine.

    Minerva Chess is a chess engine built by Jikun and Jia Hao for their Orbital 2023 Apollo project.

    The chess engine is built with C++ and this Telegram bot is built using Typescript.

    To get started, I have prepared a few actions you can take! As a part of our move to improve the user experience with this Telegram bot, we have decided to move away from the use of commands and focus on user interaction through buttons per message.

    If you want more help on using this bot, click on the _"More help"_ button below!

    *Command glossary*
    1. /menu to get this prompt again
    2. /start to start a game
    3. /board to view your currently active game
    4. /help to view the help document

    Enjoy!
  `.replace(/  +/g, "");

export const tutorText = `
    *Usage Guide*

    Minerva Chess is a chess engine built by Jikun and Jia Hao for their Orbital 2023 Apollo project.

    The chess engine is built with C++ and this Telegram bot is built using Typescript.

    Motivated to make the user experience as streamlined as possible, we have opted for a more touch heavy flow. You will be able to navigate and interact the bot with minimal typing.

    *Creating games*
    You can create a new game from /menu or use /start to start a game! You will be *randomly* assigned a side.

    To forfeit the current game, you can select "Forfeit game" button.

    *Moving your pieces*
    To make moves, use the Algebraic Notation of Long Algebraic Notation to indicate the moves.

    For instance, if you wish to move the pawn from \`e2\` to \`e4\`, you can simply send \`e2e4\` in the chat.

    _Note:_ Algebraic Notation is *case-sensitive*. Pieces are denoted by uppercase letters while board files are denoted by lowercase letters. It is intentionally designed so.

    Here are some of the common moves used in chess:
    1. Move piece (piece, from, to): e2e4
    2. Kingside castling: O-O (not zero)
    3. Queenside castling: O-O-O
    4. Capture (x): exd4
    5. Check (+): Qe3+
    6. Checkmate (++ or #): Qg2#
    7. En passant: e.p.

    The following are the notations for all board pieces:
    1. King: K
    2. Queen: Q
    3. Rook: R
    4. Bishop: B
    5. Knight: N
    6. Pawn: No notation

    Prepend the piece notation before a move to move that piece.

    Playing against Minerva Chess in a group chat? Do not worry, only your moves will be registered!

    *Reviewing your plays*
    If you want to look back at your past games, you can access them using /menu and selecting the "Past games" button.

    We have built a simple yet powerful move replay into the chessboard so you are able to navigate between the board states to review your plays.

    You can also view your play statistics using /menu and selecting the "Play statistics" button.

    *Returning to a game*
    If you are returning to a game and lost the previous game state, you can find your ongoing game from /menu through the "View current game" button or use /board
  `.replace(/  +/g, "");

export function getBoardImage(fen: string, side: UserSide) {
  const r = Math.floor(Math.random() * 101);
  return `https://fen2image.chessvision.ai/${fen}?pov=${side.toLowerCase()}&random=${r}`;
}

export async function sendEditableMessage(
  bot: Telegraf<Context<Update>>,
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

export async function editMessage(
  bot: Telegraf<Context<Update>>,
  message: Message,
  text: string
) {
  try {
    await bot.telegram.editMessageText(
      message.chat.id,
      message.message_id,
      undefined,
      text
    );
  } catch (e) {
    console.log(e);
  }
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

export function gameStatusToText(status: GameStatus) {
  switch (status) {
    case GameStatus.BOT_WIN:
      return "Bot has won";
    case GameStatus.USER_WIN:
      return "You won";
    case GameStatus.DRAW:
      return "The game is a draw";
    case GameStatus.STALEMATE:
      return "The game is a stalemate";
    case GameStatus.FORFEITED:
      return "You forfeited";
    case GameStatus.ENDED:
      return "The game was ended";
    case GameStatus.STARTED:
      return "Game is ongoing";
  }
}

export async function deleteButtons(context: Context<Update>) {
  try {
    await context.editMessageReplyMarkup(undefined);
  } catch (e) {
    console.log(e);
  }
}
