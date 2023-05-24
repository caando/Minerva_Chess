import { Context } from "telegraf";

const IgnoreOldMiddleWare = (ctx: Context, next: () => Promise<void>) => {
  const now = new Date().getTime();
  if (
    ctx.updateType === "message" &&
    (ctx.message?.date || -1) + 30 < now / 1000
  )
    return;
  next();
};

export default IgnoreOldMiddleWare;
