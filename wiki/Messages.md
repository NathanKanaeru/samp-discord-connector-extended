# Messages

Discord messages flow through the connector both ways: incoming messages (gateway events) become `DCC_Message:` handles delivered to your callbacks, and outgoing messages are produced by the channel and interaction APIs. This page covers everything you can do with a message handle on the Pawn side.

* [Message lifecycle](#message-lifecycle) — how Pawn handles are created and freed.
* [Reading a message](#reading-a-message) — author, content, mentions, channel, ID.
* [Editing](#editing).
* [Deleting](#deleting).
* [Persistence and caching](#persistence-and-caching) — keep messages alive across callbacks.
* [Reactions](#reactions) — see [[Emojis and Reactions]].
* [Reference](#reference).

## Message lifecycle

When the gateway delivers a `MESSAGE_CREATE`, the connector:

1. Builds a `DCC_Message:` handle and fires `DCC_OnMessageCreate`.
2. After your handler returns, the message is **freed automatically** unless you mark it persistent with `DCC_SetMessagePersistent(message, true)` or you stash its `DCC_Message:` value via `DCC_GetCreatedMessage` after a `DCC_SendChannelMessage*` call.

This means: if you want to edit or react to a message later, set it persistent. Otherwise the handle becomes invalid as soon as `DCC_OnMessageCreate` returns.

## Reading a message

```pawn
public DCC_OnMessageCreate(DCC_Message:message)
{
    // Author
    new DCC_User:author;
    DCC_GetMessageAuthor(message, author);

    // Content
    new content[2001];
    DCC_GetMessageContent(message, content);

    // Channel and id
    new DCC_Channel:channel;
    DCC_GetMessageChannel(message, channel);

    new id_buf[DCC_ID_SIZE];
    DCC_GetMessageId(message, id_buf);

    // Flags
    new bool:tts, bool:everyone;
    DCC_IsMessageTts              (message, tts);
    DCC_IsMessageMentioningEveryone(message, everyone);

    // Mentioned users
    new umc;
    DCC_GetMessageUserMentionCount(message, umc);
    for (new i = 0; i < umc; i++)
    {
        new DCC_User:u;
        DCC_GetMessageUserMention(message, i, u);
        new uname[DCC_USERNAME_SIZE];
        DCC_GetUserName(u, uname);
        printf("mentioned user: %s", uname);
    }

    // Mentioned roles
    new rmc;
    DCC_GetMessageRoleMentionCount(message, rmc);
    for (new i = 0; i < rmc; i++)
    {
        new DCC_Role:r;
        DCC_GetMessageRoleMention(message, i, r);
    }
    return 1;
}
```

> Reading the actual user-typed text requires the **Message Content Intent** to be enabled in the Discord Developer Portal. Without it, `DCC_GetMessageContent` returns an empty string for messages that do not mention the bot.

## Editing

```pawn
DCC_EditMessage(message, "edited content");

// or, with a different embed:
new DCC_Embed:e = DCC_CreateEmbed("New title", "Updated body");
DCC_EditMessage(message, "see embed", e);
```

The embed handle is consumed and freed by the call. The message handle must
still be valid (persistent or freshly created — see below).

## Deleting

```pawn
DCC_DeleteMessage(message);
```

Removes the message on Discord. The Pawn handle is freed automatically afterwards.

If you only want to drop the **local cache entry** without touching Discord, use
`DCC_DeleteInternalMessage`:

```pawn
DCC_DeleteInternalMessage(message);
```

That is useful for persistent messages you no longer want to keep tracking.

## Persistence and caching

By default, message handles are short-lived. There are two ways to make a message handle stick around so you can edit or react to it later.

### Persistent messages

```pawn
public DCC_OnMessageCreate(DCC_Message:message)
{
    new content[64];
    DCC_GetMessageContent(message, content);
    if (!strcmp(content, "!status"))
    {
        // Mark this incoming message as persistent so we can edit it later.
        DCC_SetMessagePersistent(message, true);
        new DCC_Message:cached = message;

        // Later... edit it from somewhere else
        DCC_EditMessage(cached, "Status: OK");
    }
    return 1;
}
```

### Capturing a message after sending

`DCC_SendChannelMessage*` is fire-and-forget by default. To capture the resulting `DCC_Message:` handle, pass a callback. Inside that callback, `DCC_GetCreatedMessage()` returns the freshly-posted message:

```pawn
forward OnRulesPosted();
public  OnRulesPosted()
{
    new DCC_Message:msg = DCC_GetCreatedMessage();
    DCC_SetMessagePersistent(msg, true);  // keep it forever
}

DCC_SendChannelMessage(rules_channel, "Server rules:\n1. ...", "OnRulesPosted");
```

### Loading an existing message into the cache

If you know the snowflake of a message that already exists on Discord (for example, a sticky info post stored in your config), you can pull it into the cache and get a handle:

```pawn
forward OnRulesLoaded();
public  OnRulesLoaded()
{
    new DCC_Message:msg = DCC_GetCreatedMessage();
    DCC_SetMessagePersistent(msg, true);
    g_RulesMessage = msg;
}

DCC_CacheChannelMessage("rules_channel_id", "rules_message_id", "OnRulesLoaded");
```

## Reactions

See [[Emojis and Reactions]] for `DCC_CreateReaction`, `DCC_DeleteMessageReaction`, and the `DCC_OnMessageReaction` callback.

## Reference

```pawn
// metadata
DCC_GetMessageId             (DCC_Message:message, dest[DCC_ID_SIZE], max_size = DCC_ID_SIZE);
DCC_GetMessageChannel        (DCC_Message:message, &DCC_Channel:channel);
DCC_GetMessageAuthor         (DCC_Message:message, &DCC_User:author);
DCC_GetMessageContent        (DCC_Message:message, dest[], max_size = sizeof dest);
DCC_IsMessageTts             (DCC_Message:message, &bool:is_tts);
DCC_IsMessageMentioningEveryone(DCC_Message:message, &bool:mentions_everyone);
DCC_GetMessageUserMentionCount (DCC_Message:message, &count);
DCC_GetMessageUserMention      (DCC_Message:message, offset, &DCC_User:user);
DCC_GetMessageRoleMentionCount (DCC_Message:message, &count);
DCC_GetMessageRoleMention      (DCC_Message:message, offset, &DCC_Role:role);

// modify
DCC_EditMessage              (DCC_Message:message, const content[], DCC_Embed:embed = DCC_Embed:0);
DCC_DeleteMessage            (DCC_Message:message);          // delete on Discord
DCC_DeleteInternalMessage    (DCC_Message:message);          // free local handle only
DCC_SetMessagePersistent     (DCC_Message:message, bool:persistent);

// retrieval
DCC_Message:DCC_GetCreatedMessage();                          // for SendChannelMessage callbacks
DCC_CacheChannelMessage      (const channel_id[DCC_ID_SIZE],
                              const message_id[DCC_ID_SIZE],
                              const callback[] = "",
                              const format[]   = "", {Float,_}:...);
```
