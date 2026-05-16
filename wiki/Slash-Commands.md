# Slash Commands

Slash commands are how modern Discord clients invoke bots. They appear in the auto-complete menu, validate parameters before delivery, and route to a single Pawn callback per command.

* [Lifecycle and scope](#lifecycle-and-scope)
* [Registering a command](#registering-a-command)
* [Adding options](#adding-options)
* [Sub-commands and groups](#sub-commands-and-groups)
* [Receiving the interaction](#receiving-the-interaction)
* [Reading typed options](#reading-typed-options)
* [Mentions inside string options](#mentions-inside-string-options)
* [Replying to the command](#replying-to-the-command)
* [Deleting a command](#deleting-a-command)
* [Reference](#reference)

## Lifecycle and scope

A slash command can be **global** or **guild-scoped**:

* **Guild-scoped** — appears only in the specified guild, updates instantly. Best for development and per-server features.
* **Global** — appears in every guild the bot is in, but Discord may take up to one hour to propagate the first time.

Pass `DCC_INVALID_GUILD` (or omit the argument) for global, or a real `DCC_Guild:` for guild-scoped.

## Registering a command

```pawn
public OnGameModeInit()
{
    new DCC_Guild:guild = DCC_FindGuildById("123456789012345678");

    DCC_CreateCommand(
        "kick",                            // name (max 32 chars, lowercase, [a-z0-9_-])
        "Kick a player from the server",   // description (max 100 chars)
        "OnSlashKick",                     // Pawn callback (also receives DCC_OnInteraction)
        true,                              // allow_everyone — false = hide from non-admins
        guild);                            // omit / DCC_INVALID_GUILD = global

    return 1;
}
```

The fourth argument (`allow_everyone`) is only meaningful for guild-scoped commands. Setting it to `false` requires the user to have administrator permission to see the command.

## Adding options

```pawn
DCC_AddCommandOption("playerid",  "Target player ID", DCC_OPTION_INTEGER, true);
DCC_AddCommandOption("reason",    "Reason",           DCC_OPTION_STRING,  false);
```

Options must be added **immediately** after `DCC_CreateCommand` and before any other `DCC_CreateCommand`. They attach to the most recently created command.

### Option types

```pawn
enum DCC_CommandOptionType
{
    DCC_OPTION_SUB_COMMAND       = 1,
    DCC_OPTION_SUB_COMMAND_GROUP = 2,
    DCC_OPTION_STRING            = 3,
    DCC_OPTION_INTEGER           = 4,
    DCC_OPTION_BOOLEAN           = 5,
    DCC_OPTION_USER              = 6,
    DCC_OPTION_CHANNEL           = 7,
    DCC_OPTION_ROLE              = 8
};
```

For `OPTION_USER`, `OPTION_CHANNEL`, `OPTION_ROLE`: Discord renders a picker UI and delivers the selected snowflake as the option value.

## Sub-commands and groups

Discord groups commands into a 2-level tree at most:

```
/admin player kick playerid:42 reason:Spamming
^^^^^ ^^^^^^^ ^^^^ ^^^^^^^^^^^^^^^^^^^^^^^^^^^^
 root   group  sub  options
```

Build it like this:

```pawn
new DCC_Command:cmd = DCC_CreateCommand("admin",
    "Admin commands", "OnAdminCommand", true, guild);

// Group: admin player ...
new DCC_Option:player_group = DCC_AddCommandOption(
    "player", "Player management",
    DCC_OPTION_SUB_COMMAND_GROUP, false);

// Sub-command: admin player kick ...
new DCC_Option:kick_sub = DCC_AddCommandOption(
    "kick", "Kick a player",
    DCC_OPTION_SUB_COMMAND, false, player_group);

// Options of the kick sub-command
DCC_AddCommandOption("playerid", "Target ID", DCC_OPTION_INTEGER, true,  kick_sub);
DCC_AddCommandOption("reason",   "Reason",    DCC_OPTION_STRING,  false, kick_sub);
```

Pass the parent `DCC_Option:` as the fifth argument when nesting.

## Receiving the interaction

Each command routes to **two** places when invoked:

1. The dedicated callback you specified in `DCC_CreateCommand` (e.g. `OnSlashKick`).
2. `DCC_OnInteraction` with `type == DCC_INTERACTION_COMMAND`.

You will usually use the dedicated callback because it carries the typed parameters in a clean way:

```pawn
forward OnSlashKick(DCC_Interaction:interaction, DCC_User:user);
public  OnSlashKick(DCC_Interaction:interaction, DCC_User:user)
{
    // user is the Discord user who ran the command.
    DCC_SendInteractionMessage(interaction, "Kicking…");
}
```

## Reading typed options

`DCC_GetInteractionOption` walks the option tree as a flat list, exposing the *full path* (e.g. `player.kick.reason`) and the value as a string:

```pawn
public OnAdminCommand(DCC_Interaction:interaction, DCC_User:user)
{
    new option_count;
    DCC_GetInteractionOptionCount(interaction, option_count);

    new path[64], value[256];
    new DCC_CommandOptionType:type;
    new playerid = -1, reason[128];

    for (new i = 0; i < option_count; i++)
    {
        DCC_GetInteractionOption(interaction, i, path, value, type);

        if (!strcmp(path, "player.kick.playerid"))
            playerid = strval(value);
        else if (!strcmp(path, "player.kick.reason"))
            strcat(reason, value, sizeof reason);
    }
    /* ... */
}
```

For a `DCC_OPTION_USER` option, `value` is the user's snowflake (you can pass it to `DCC_FindUserById`). Same for `DCC_OPTION_CHANNEL` and `DCC_OPTION_ROLE`.

## Mentions inside string options

When a user passes a `<@!user_id>` mention inside a `DCC_OPTION_STRING`, the connector parses it and exposes the mentioned users via:

```pawn
new mention_count;
DCC_GetInteractionMentionCount(interaction, mention_count);

for (new i = 0; i < mention_count; i++)
{
    new DCC_User:u;
    DCC_GetInteractionMention(interaction, i, u);
}
```

You can also retrieve the channel and guild context of the interaction:

```pawn
new DCC_Channel:ch;
new DCC_Guild  :guild;
DCC_GetInteractionChannel(interaction, ch);
DCC_GetInteractionGuild  (interaction, guild);

new content[256];
DCC_GetInteractionContent(interaction, content);   // first option value, fallback to name
```

## Replying to the command

```pawn
// Plain text
DCC_SendInteractionMessage(interaction, "Kicked.");

// Embed
new DCC_Embed:e = DCC_CreateEmbed("Kick result", "Player removed", "", "", 0xED4245);
DCC_SendInteractionEmbed(interaction, e);

// Pop-up modal (cannot be combined with the responses above)
DCC_SendInteractionModal(interaction, my_modal);
```

You **must** respond to every command interaction within 3 seconds. If you need longer, send an immediate placeholder message and edit it later via the message returned in `DCC_GetCreatedMessage()`.

## Deleting a command

```pawn
DCC_DeleteCommand(cmd);   // removes from Discord and frees the handle
```

## Reference

```pawn
DCC_Command:DCC_CreateCommand(const command_name[DCC_COMMAND_SIZE],
                              const description[DCC_COMMAND_DESCRIPTION_SIZE],
                              const callback[],
                              bool:allow_everyone = true,
                              DCC_Guild:guild = DCC_INVALID_GUILD);
DCC_DeleteCommand(DCC_Command:command);
DCC_Option:DCC_AddCommandOption(const name[], const description[],
                                DCC_CommandOptionType:type,
                                bool:required = true,
                                DCC_Option:parent_option = DCC_Option:0);

// receive
DCC_GetInteractionOptionCount (DCC_Interaction:i, &option_count);
DCC_GetInteractionOption      (DCC_Interaction:i, offset,
                               path[], value[],
                               &DCC_CommandOptionType:type,
                               path_size = sizeof path,
                               value_size = sizeof value);
DCC_GetInteractionMentionCount(DCC_Interaction:i, &mention_count);
DCC_GetInteractionMention     (DCC_Interaction:i, offset, &DCC_User:user);
DCC_GetInteractionContent     (DCC_Interaction:i, dest[], max_size = sizeof dest);
DCC_GetInteractionChannel     (DCC_Interaction:i, &DCC_Channel:channel);
DCC_GetInteractionGuild       (DCC_Interaction:i, &DCC_Guild:guild);

// respond
DCC_SendInteractionMessage(DCC_Interaction:i, const message[] = "");
DCC_SendInteractionEmbed  (DCC_Interaction:i, DCC_Embed:embed, const message[] = "");
DCC_SendInteractionModal  (DCC_Interaction:i, DCC_Modal:modal);
```
