# Emojis and Reactions

Discord has two flavours of emoji:

* **Unicode emoji** — `🙂`, `🚀`, …  Created from the Unicode glyph string.
* **Custom guild emoji** — created on a guild, identified by a snowflake.

The connector represents both with `DCC_Emoji:` handles.

* [Creating emoji handles](#creating-emoji-handles)
* [Reading the name back](#reading-the-name-back)
* [Adding and removing reactions](#adding-and-removing-reactions)
* [Listening to reactions](#listening-to-reactions)
* [Reference](#reference)

## Creating emoji handles

```pawn
// Unicode emoji - leave snowflake empty
new DCC_Emoji:thumbs = DCC_CreateEmoji("👍");

// Custom guild emoji - pass both name and snowflake
new DCC_Emoji:logo = DCC_CreateEmoji("server_logo", "987654321098765432");
```

The handle is owned by your script. Free it when you no longer need it:

```pawn
DCC_DeleteEmoji(thumbs);
```

> All natives that take a `DCC_Emoji:` (button decorations, select menu options, reactions) **do not** consume the handle, so reuse the same handle across calls if you want.

## Reading the name back

```pawn
new name[DCC_EMOJI_NAME_SIZE];
DCC_GetEmojiName(thumbs, name);
```

For unicode emojis this returns the glyph itself; for custom emojis it returns
the configured name (e.g. `"server_logo"`).

## Adding and removing reactions

You need a valid (persistent or freshly-created) `DCC_Message:` handle. See [[Messages#persistence-and-caching]].

```pawn
// React to a message
new DCC_Emoji:rocket = DCC_CreateEmoji("🚀");
DCC_CreateReaction(message, rocket);

// Remove only the bot's own rocket reaction
DCC_DeleteMessageReaction(message, rocket);

// Remove ALL reactions on the message (requires Manage Messages)
DCC_DeleteMessageReaction(message);   // emoji defaults to 0
```

## Listening to reactions

```pawn
forward DCC_OnMessageReaction(DCC_Message:message, DCC_User:reaction_user,
                              DCC_Emoji:emoji, DCC_MessageReactionType:reaction_type);
```

`reaction_type` describes what happened:

```pawn
enum DCC_MessageReactionType
{
    REACTION_ADD = 0,         // user added a reaction
    REACTION_REMOVE,          // user removed their reaction
    REACTION_REMOVE_ALL,      // someone cleared all reactions on the message
    REACTION_REMOVE_EMOJI     // bot removed every instance of one emoji
};
```

For `REACTION_REMOVE_ALL`, both `reaction_user` and `emoji` are `INVALID`. For `REACTION_REMOVE_EMOJI`, only `reaction_user` is invalid.

```pawn
public DCC_OnMessageReaction(DCC_Message:message, DCC_User:reaction_user,
                             DCC_Emoji:emoji, DCC_MessageReactionType:type)
{
    if (type != REACTION_ADD)
        return 1;

    new ename[DCC_EMOJI_NAME_SIZE];
    DCC_GetEmojiName(emoji, ename);

    if (!strcmp(ename, "👍"))
    {
        new DCC_User:author;  // ignore self-reacts
        DCC_GetMessageAuthor(message, author);
        if (author != reaction_user)
        {
            // award some reputation here
        }
    }
    return 1;
}
```

> The emoji handle delivered to `DCC_OnMessageReaction` is **owned by the connector** and is freed after the callback returns. If you need it longer, capture the name/snowflake into your own buffers.

## Reference

```pawn
DCC_Emoji:DCC_CreateEmoji(const name[DCC_EMOJI_NAME_SIZE],
                          const snowflake[DCC_ID_SIZE] = "");
DCC_DeleteEmoji          (DCC_Emoji:emoji);
DCC_GetEmojiName         (DCC_Emoji:emoji, dest[DCC_EMOJI_NAME_SIZE],
                          maxlen = DCC_EMOJI_NAME_SIZE);

DCC_CreateReaction       (DCC_Message:message, DCC_Emoji:reaction_emoji);
DCC_DeleteMessageReaction(DCC_Message:message, DCC_Emoji:reaction_emoji = DCC_Emoji:0);

forward DCC_OnMessageReaction(DCC_Message:message, DCC_User:reaction_user,
                              DCC_Emoji:emoji, DCC_MessageReactionType:reaction_type);
```
