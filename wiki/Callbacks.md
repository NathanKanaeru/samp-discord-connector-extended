# Callbacks

Every event the connector forwards to your gamemode is listed here. Add the corresponding `public` function to start receiving it. The plugin uses `amx_FindPublic` so the callback is recognised automatically as long as the signature matches.

* [Format-string spec](#format-string-spec) — for variadic callback arguments.
* [Channel events](#channel-events)
* [Message events](#message-events)
* [Reaction events](#reaction-events)
* [User events](#user-events)
* [Guild events](#guild-events)
* [Member events](#member-events)
* [Role events](#role-events)
* [Interaction events](#interaction-events)

---

## Format-string spec

Several natives accept a Pawn callback **plus** a format string and trailing variadic arguments (`DCC_SendChannelMessage`, `DCC_SendChannelMessageEx`, `DCC_SendChannelEmbedMessage`, `DCC_CacheChannelMessage`, `DCC_CreateGuildChannel`, `DCC_CreateGuildRole`, `DCC_CreatePrivateChannel`).

The format string has one specifier per variadic argument:

| Specifier | Meaning  | Pawn type        |
|-----------|----------|------------------|
| `d` / `i` | integer  | `cell` / enum    |
| `f`       | float    | `Float:`         |
| `b`       | boolean  | `bool:`          |
| `s`       | string   | string literal / `name[]` |
| `a`       | array    | the **next** argument must be `d` (size) |
| `r`       | reference| `&dest`          |

Example:

```pawn
forward OnPosted(channel, count, Float:duration);
public  OnPosted(channel, count, Float:duration) { /* ... */ }

DCC_SendChannelMessage(ch, "Done!", "OnPosted",
    "ddf", _:ch, 4, 1.25);
```

Pass an empty string as the callback name to skip the callback entirely.

---

## Channel events

```pawn
forward DCC_OnChannelCreate(DCC_Channel:channel);
forward DCC_OnChannelUpdate(DCC_Channel:channel);
forward DCC_OnChannelDelete(DCC_Channel:channel);
```

| Callback                | Fires when                                                |
|-------------------------|-----------------------------------------------------------|
| `DCC_OnChannelCreate`   | A new channel becomes visible to the bot.                 |
| `DCC_OnChannelUpdate`   | A channel's name, topic, NSFW flag, position or parent changes. |
| `DCC_OnChannelDelete`   | A channel is deleted (or the bot loses access).           |

After `DCC_OnChannelDelete`, the `DCC_Channel:` handle is invalidated.

---

## Message events

```pawn
forward DCC_OnMessageCreate(DCC_Message:message);
forward DCC_OnMessageDelete(DCC_Message:message);
```

`DCC_OnMessageCreate` covers every message the bot can see — guild messages, DMs, even messages from other bots. Filter early:

```pawn
public DCC_OnMessageCreate(DCC_Message:message)
{
    new DCC_User:author;
    DCC_GetMessageAuthor(message, author);

    new bool:is_bot;
    DCC_IsUserBot(author, is_bot);
    if (is_bot) return 1;

    /* ... */
    return 1;
}
```

The `DCC_Message:` handle is freed automatically after the callback returns unless you call `DCC_SetMessagePersistent(message, true)` — see [[Messages#persistence-and-caching]].

---

## Reaction events

```pawn
forward DCC_OnMessageReaction(DCC_Message:message,
                              DCC_User:reaction_user,
                              DCC_Emoji:emoji,
                              DCC_MessageReactionType:reaction_type);
```

`reaction_type`:

| Constant                | Fires when                                                       |
|-------------------------|-----------------------------------------------------------------|
| `REACTION_ADD`          | A user reacted.                                                 |
| `REACTION_REMOVE`       | A user un-reacted.                                              |
| `REACTION_REMOVE_ALL`   | Someone wiped all reactions on a message.                       |
| `REACTION_REMOVE_EMOJI` | A bot removed every instance of one specific emoji on a message.|

For `REMOVE_ALL`, `reaction_user` and `emoji` are invalid. For `REMOVE_EMOJI`, only `reaction_user` is invalid. The emoji handle is freed after the callback returns — copy out what you need.

---

## User events

```pawn
forward DCC_OnUserUpdate(DCC_User:user);
```

Fires whenever a cached user's username, discriminator or avatar changes.

---

## Guild events

```pawn
forward DCC_OnGuildCreate(DCC_Guild:guild);
forward DCC_OnGuildUpdate(DCC_Guild:guild);
forward DCC_OnGuildDelete(DCC_Guild:guild);
```

`DCC_OnGuildCreate` fires:

* once for each guild during startup, after the bot syncs it; and
* whenever the bot is invited to a new guild.

`DCC_OnGuildUpdate` fires for renames and other guild-level changes. `DCC_OnGuildDelete` fires when the bot is removed or the guild is deleted.

---

## Member events

```pawn
forward DCC_OnGuildMemberAdd        (DCC_Guild:guild, DCC_User:user);
forward DCC_OnGuildMemberUpdate     (DCC_Guild:guild, DCC_User:user);
forward DCC_OnGuildMemberRemove     (DCC_Guild:guild, DCC_User:user);
forward DCC_OnGuildMemberVoiceUpdate(DCC_Guild:guild, DCC_User:user, DCC_Channel:channel);
```

| Callback                          | Fires when                                                |
|-----------------------------------|-----------------------------------------------------------|
| `DCC_OnGuildMemberAdd`            | A user joins the guild.                                   |
| `DCC_OnGuildMemberUpdate`         | A member's nickname, role list, or status changes.        |
| `DCC_OnGuildMemberRemove`         | A user leaves, is kicked, or is banned.                   |
| `DCC_OnGuildMemberVoiceUpdate`    | A user joins / leaves / moves between voice channels.     |

`DCC_OnGuildMemberAdd` and `DCC_OnGuildMemberUpdate` require the **Server Members Intent** to be enabled in the Discord Developer Portal. `DCC_OnGuildMemberUpdate` also relies on the **Presence Intent** for the status fields.

`DCC_OnGuildMemberVoiceUpdate` reports `channel = DCC_INVALID_CHANNEL` when the user disconnects entirely.

---

## Role events

```pawn
forward DCC_OnGuildRoleCreate(DCC_Guild:guild, DCC_Role:role);
forward DCC_OnGuildRoleUpdate(DCC_Guild:guild, DCC_Role:role);
forward DCC_OnGuildRoleDelete(DCC_Guild:guild, DCC_Role:role);
```

Mirror events for role lifecycle. After `DCC_OnGuildRoleDelete` the handle is invalidated.

---

## Interaction events

```pawn
forward DCC_OnInteraction(DCC_Interaction:interaction, DCC_User:user,
                          DCC_InteractionType:type, const custom_id[]);
```

Fires once per slash command, button click, select-menu pick, or modal submission. The interaction handle is valid until the callback returns; use one of the responder natives (see [[Interactions]]) before then.

For slash commands the dedicated callback you specified in `DCC_CreateCommand` is also invoked, alongside the generic `DCC_OnInteraction`. Use whichever fits your code structure better.
