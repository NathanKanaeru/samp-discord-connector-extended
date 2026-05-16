# Bot

Natives that act on the bot user itself: presence, activity, typing indicator, per-guild nickname, and opening direct-message channels.

* [Presence and activity](#presence-and-activity)
* [Typing indicator](#typing-indicator)
* [Bot nickname per guild](#bot-nickname-per-guild)
* [Opening a private DM channel](#opening-a-private-dm-channel)
* [Misc — Markdown escaping](#misc--markdown-escaping)
* [Reference](#reference)

## Presence and activity

```pawn
enum DCC_BotPresenceStatus
{
    INVALID,
    ONLINE,
    IDLE,
    DO_NOT_DISTURB,
    INVISIBLE,
    OFFLINE
};
```

```pawn
DCC_SetBotPresenceStatus(ONLINE);
DCC_SetBotPresenceStatus(DO_NOT_DISTURB);
DCC_SetBotPresenceStatus(INVISIBLE);

DCC_SetBotActivity("with players on Los Santos");  // shows as "Playing ..."
DCC_ClearBotActivity();                            // stock helper
```

Read the current presence:

```pawn
new DCC_BotPresenceStatus:status = DCC_GetBotPresenceStatus();
```

> The activity prefix in the Discord client is fixed at "Playing". Custom prefixes (Streaming, Watching, Listening, Competing) require the new presence API and are not yet exposed by this connector.

## Typing indicator

Show the "*Bot is typing…*" indicator for ~10 seconds in a channel:

```pawn
DCC_TriggerBotTypingIndicator(ch);
```

Useful when the bot is about to post a long answer that takes more than a few hundred milliseconds to compute.

## Bot nickname per guild

```pawn
DCC_SetBotNickname(guild, "GameMaster");
DCC_ClearBotNickname(guild);   // stock — equivalent to passing ""
```

Requires *Change Nickname* on the guild.

## Opening a private DM channel

```pawn
new DCC_Channel:g_DM;

forward OnDmChannelReady();
public  OnDmChannelReady()
{
    g_DM = DCC_GetCreatedPrivateChannel();
    DCC_SendChannelMessage(g_DM, "Hi, this is your verification code: 4815");
}

DCC_CreatePrivateChannel(target_user, "OnDmChannelReady");
```

Inside the callback you must call `DCC_GetCreatedPrivateChannel` to grab the new channel handle. After that, treat it like any other channel — send messages, embeds, even buttons in a DM.

## Misc — Markdown escaping

When the bot echoes user-typed content back, you usually want to neutralise Discord markdown so `_underscores_` or `**bold**` do not get rendered:

```pawn
new safe[256];
DCC_EscapeMarkdown(user_text, safe);
```

## Reference

```pawn
// presence and activity
DCC_BotPresenceStatus:DCC_GetBotPresenceStatus();
DCC_SetBotPresenceStatus(DCC_BotPresenceStatus:status);
DCC_SetBotActivity(const name[]);
DCC_ClearBotActivity();          // stock

// typing indicator
DCC_TriggerBotTypingIndicator(DCC_Channel:channel);

// nickname per-guild
DCC_SetBotNickname(DCC_Guild:guild, const nickname[]);
DCC_ClearBotNickname(DCC_Guild:guild);  // stock

// DM channel
DCC_CreatePrivateChannel(DCC_User:user, const callback[],
                         const format[]="", {Float,_}:...);
DCC_Channel:DCC_GetCreatedPrivateChannel();

// misc
DCC_EscapeMarkdown(const src[], dest[], max_size = sizeof dest);
```
