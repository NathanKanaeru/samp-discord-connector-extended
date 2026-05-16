# Channels

Channels are the buckets that hold messages, voice connections, and DMs. Every channel that the bot can see is cached when the bot connects, and updates from the gateway keep the cache fresh. Most of your code will start by resolving a `DCC_Channel:` handle, then call methods on it.

* [Channel handles](#channel-handles) — `DCC_FindChannelByName`, `DCC_FindChannelById`.
* [Channel metadata](#channel-metadata) — id, type, guild, name, topic, position, NSFW, parent.
* [Sending messages](#sending-messages) — plain, embed, with components, callback flow.
* [Editing channels](#editing-channels) — name, topic, position, NSFW, parent category.
* [Creating and deleting channels](#creating-and-deleting-channels).
* [Channel callbacks](#channel-callbacks).
* [Reference](#reference).

## Channel types

```pawn
enum DCC_ChannelType
{
    GUILD_TEXT     = 0,
    DM             = 1,
    GUILD_VOICE    = 2,
    GROUP_DM       = 3,
    GUILD_CATEGORY = 4
};
```

## Channel handles

```pawn
new DCC_Channel:ch = DCC_FindChannelByName("general");
new DCC_Channel:bn = DCC_FindChannelById  ("123456789012345678");
```

Both natives return `DCC_INVALID_CHANNEL` when nothing matches. Always check the
return value before using the handle.

`DCC_FindChannelByName` matches the **first** channel with that name, across all
guilds the bot is in. If you have multiple guilds with channels of the same
name, prefer `DCC_FindChannelById` or `DCC_GetGuildChannel` (see [[Guilds]]).

## Channel metadata

```pawn
new id_buf[DCC_ID_SIZE], name[64], topic[1024];
new DCC_ChannelType:type;
new DCC_Guild:guild;
new pos;
new bool:nsfw;
new DCC_Channel:parent;

DCC_GetChannelId            (ch, id_buf);
DCC_GetChannelType          (ch, type);
DCC_GetChannelGuild         (ch, guild);
DCC_GetChannelName          (ch, name);
DCC_GetChannelTopic         (ch, topic);
DCC_GetChannelPosition      (ch, pos);
DCC_IsChannelNsfw           (ch, nsfw);
DCC_GetChannelParentCategory(ch, parent);
```

`DCC_GetChannelGuild` returns `DCC_INVALID_GUILD` for DMs and group DMs.
`DCC_GetChannelParentCategory` returns `DCC_INVALID_CHANNEL` when the channel is
not nested inside a category.

## Sending messages

### Plain text

```pawn
DCC_SendChannelMessage(ch, "Hello, world!");
```

### With a callback

If you want to operate on the resulting message (for example to react to it
later, or store its id for an edit), pass a callback:

```pawn
forward OnAnnouncementSent(channel, custom_data);
public  OnAnnouncementSent(channel, custom_data)
{
    new DCC_Message:msg = DCC_GetCreatedMessage();
    new id[DCC_ID_SIZE];
    DCC_GetMessageId(msg, id);
    printf("posted message %s in channel %d (custom=%d)", id, channel, custom_data);
}

DCC_SendChannelMessage(ch, "Hello!", "OnAnnouncementSent", "dd", _:ch, 42);
```

The format string `"dd"` describes the variadic arguments — see [[Callbacks#format-specifiers]] for the full list.

### With an embed

```pawn
new DCC_Embed:e = DCC_CreateEmbed("Round started", "Join now!", "", "", 0x57F287);
DCC_SendChannelEmbedMessage(ch, e);
```

### With buttons or select menus

`DCC_SendChannelMessageEx` is the rich variant — it accepts an embed handle and
an array of Action Rows.

```pawn
new DCC_ActionRow:rows[2] = { row1, row2 };
DCC_SendChannelMessageEx(ch, "Choose your action:",
    DCC_INVALID_EMBED, rows, 2);
```

Pass `DCC_INVALID_EMBED` when you do not want an embed. The plugin validates each row before sending and refuses to post invalid payloads — see [[Message Components]] for the layout rules.

## Editing channels

```pawn
DCC_SetChannelName          (ch, "new-name");           // 2..100 chars
DCC_SetChannelTopic         (ch, "Welcome to the server!");
DCC_SetChannelPosition      (ch, 5);
DCC_SetChannelNsfw          (ch, true);
DCC_SetChannelParentCategory(ch, category_channel);
```

Setting a topic only works on `GUILD_TEXT` channels. Setting a parent category
only works when `category_channel` is of type `GUILD_CATEGORY`.

## Creating and deleting channels

Creating a channel is a guild operation, so it lives under [[Guilds]]:

```pawn
DCC_CreateGuildChannel(guild, "events", GUILD_TEXT, "OnEventChannelCreated");
```

Inside the callback, retrieve the new handle with `DCC_GetCreatedGuildChannel`.

```pawn
forward OnEventChannelCreated();
public  OnEventChannelCreated()
{
    new DCC_Channel:ch = DCC_GetCreatedGuildChannel();
    DCC_SendChannelMessage(ch, "This channel was just created!");
}
```

Deleting:

```pawn
DCC_DeleteChannel(ch);
```

The bot needs *Manage Channels* permission in the guild for both create and delete.

## Channel callbacks

```pawn
forward DCC_OnChannelCreate(DCC_Channel:channel);
forward DCC_OnChannelUpdate(DCC_Channel:channel);
forward DCC_OnChannelDelete(DCC_Channel:channel);
```

Update fires whenever name, topic, position, NSFW flag or parent change. See
[[Callbacks]] for the full callback list.

## Reference

```pawn
// lookup
DCC_Channel:DCC_FindChannelByName(const channel_name[]);
DCC_Channel:DCC_FindChannelById  (const channel_id[]);

// metadata
DCC_GetChannelId            (DCC_Channel:channel, dest[DCC_ID_SIZE], max_size = sizeof dest);
DCC_GetChannelType          (DCC_Channel:channel, &DCC_ChannelType:type);
DCC_GetChannelGuild         (DCC_Channel:channel, &DCC_Guild:guild);
DCC_GetChannelName          (DCC_Channel:channel, dest[], max_size = sizeof dest);
DCC_GetChannelTopic         (DCC_Channel:channel, dest[], max_size = sizeof dest);
DCC_GetChannelPosition      (DCC_Channel:channel, &position);
DCC_IsChannelNsfw           (DCC_Channel:channel, &bool:is_nsfw);
DCC_GetChannelParentCategory(DCC_Channel:channel, &DCC_Channel:category);

// messages
DCC_SendChannelMessage     (DCC_Channel:ch, const message[],
                            const callback[]="", const format[]="", {Float,_}:...);
DCC_SendChannelMessageEx   (DCC_Channel:ch, const message[], DCC_Embed:embed,
                            DCC_ActionRow:rows[], rows_size,
                            const callback[]="", const format[]="", {Float,_}:...);
DCC_SendChannelEmbedMessage(DCC_Channel:ch, DCC_Embed:embed, const message[]="",
                            const callback[]="", const format[]="", {Float,_}:...);

// edit
DCC_SetChannelName          (DCC_Channel:ch, const name[]);
DCC_SetChannelTopic         (DCC_Channel:ch, const topic[]);
DCC_SetChannelPosition      (DCC_Channel:ch, position);
DCC_SetChannelNsfw          (DCC_Channel:ch, bool:is_nsfw);
DCC_SetChannelParentCategory(DCC_Channel:ch, DCC_Channel:parent_category);
DCC_DeleteChannel           (DCC_Channel:ch);
```
