# Getting Started

This page walks through wiring the connector into a brand-new gamemode end to end: install, configure, send a message, react to a message, register a slash command, and respond to a button click. Once you have these patterns, the rest of the wiki is a reference.

## 1. Make sure the plugin is loaded

In your server log you should see, immediately after start-up:

```
discord-connector: <version> successfully loaded.
```

If you see `bot token not specified in environment variable or server config.` instead, go back to [[Home#configuring-the-bot]] and double-check the token / env var.

## 2. Skeleton gamemode

```pawn
#include <a_samp>
#include <discord-connector>

public OnGameModeInit()
{
    SetGameModeText("Discord-connected RP");
    return 1;
}
```

That is enough to load the plugin. Everything that follows is reactive: callbacks fire when something happens on Discord, and natives let you talk back.

## 3. Reading a message

```pawn
public DCC_OnChannelMessage(DCC_Message:message)
{
    new DCC_User:author;
    DCC_GetMessageAuthor(message, author);

    new bool:is_bot;
    DCC_IsUserBot(author, is_bot);
    if (is_bot)
        return 1;   // ignore other bots and our own posts

    new content[256];
    DCC_GetMessageContent(message, content);

    if (!strcmp(content, "!players"))
    {
        new DCC_Channel:ch;
        DCC_GetMessageChannel(message, ch);

        new reply[64];
        format(reply, sizeof reply, "Players online: %d / %d",
            GetPlayerPoolSize() + 1, GetMaxPlayers());
        DCC_SendChannelMessage(ch, reply);
    }
    return 1;
}
```

> **Tip** — to read user-typed message content you must enable the **Message Content Intent** in the Discord Developer Portal. Without it, `DCC_GetMessageContent` returns an empty string for messages from non-mentioned channels.

## 4. Posting an announcement on round start

```pawn
public OnGameModeInit()
{
    new DCC_Channel:ch = DCC_FindChannelByName("announcements");
    if (ch != DCC_INVALID_CHANNEL)
    {
        new DCC_Embed:e = DCC_CreateEmbed("New round started!",
            "Hop on now and grab a faction slot before they fill up.",
            "", "", 0x57F287);
        DCC_AddEmbedField(e, "Server",  "play.example.com",  true);
        DCC_AddEmbedField(e, "Players", "0 / 200",           true);
        DCC_SendChannelEmbedMessage(ch, e);
    }
    return 1;
}
```

`DCC_CreateEmbed` returns an `DCC_Embed:` handle that you reuse for `DCC_SetEmbed*` and `DCC_AddEmbedField`. The embed is consumed by the `Send` call — you do not need to delete it manually.

## 5. Registering a slash command

```pawn
public OnGameModeInit()
{
    new DCC_Guild:guild = DCC_FindGuildById("123456789012345678");

    DCC_CreateCommand(
        "kick",                     // name
        "Kick a player from the game", // description
        "OnKickCommand",            // Pawn callback
        true,                       // allow everyone
        guild);                     // omit / DCC_INVALID_GUILD = global

    DCC_AddCommandOption("playerid", "Target player ID",
                         DCC_OPTION_INTEGER, true);
    DCC_AddCommandOption("reason",   "Reason for the kick",
                         DCC_OPTION_STRING, false);
    return 1;
}

forward OnKickCommand(DCC_Interaction:interaction, DCC_User:user);
public OnKickCommand(DCC_Interaction:interaction, DCC_User:user)
{
    // Read the typed options
    new option_count;
    DCC_GetInteractionOptionCount(interaction, option_count);

    new playerid = INVALID_PLAYER_ID;
    new reason[128] = "No reason given";
    new path[32], value[128];
    new DCC_CommandOptionType:type;

    for (new i = 0; i < option_count; i++)
    {
        DCC_GetInteractionOption(interaction, i, path, value, type);
        if (!strcmp(path, "playerid"))
            playerid = strval(value);
        else if (!strcmp(path, "reason"))
            strcat(reason, value, sizeof reason);
    }

    if (playerid == INVALID_PLAYER_ID || !IsPlayerConnected(playerid))
    {
        DCC_SendInteractionMessage(interaction, "Player is not online.");
        return;
    }

    Kick(playerid);

    new feedback[200];
    format(feedback, sizeof feedback, "Kicked player %d. Reason: %s",
        playerid, reason);
    DCC_SendInteractionMessage(interaction, feedback);
}
```

Guild-scoped commands appear instantly. Global commands (omit the `guild` argument) take up to one hour to propagate the first time.

## 6. Reacting to a button

A button lives in an Action Row, and an Action Row goes into a message:

```pawn
public OnGameModeInit()
{
    new DCC_Channel:ch = DCC_FindChannelByName("verification");

    new DCC_Component:btn = DCC_CreateButton("Verify Me",
        DCC_STYLE_SUCCESS, "verify_btn");

    new DCC_ActionRow:row = DCC_CreateActionRow();
    DCC_AddComponentToActionRow(row, btn);

    new DCC_ActionRow:rows[1] = { row };
    DCC_SendChannelMessageEx(ch, "Click below to start verification.",
        DCC_INVALID_EMBED, rows, 1);
    return 1;
}

public DCC_OnInteraction(DCC_Interaction:interaction, DCC_User:user,
                         DCC_InteractionType:type, const custom_id[])
{
    if (type == DCC_INTERACTION_COMPONENT && !strcmp(custom_id, "verify_btn"))
    {
        DCC_SendInteractionMessage(interaction,
            "Check your DMs for the verification code.");
    }
    return 1;
}
```

If the button never shows up:

* Check `dc-connector.log` and the SA-MP log for `WARNING` lines starting with
  `button N:`. The connector validates the payload before posting and explains
  what is wrong.
* See [[Troubleshooting]] for the full diagnosis flow.

## 7. Asking the user to fill a form (modal)

```pawn
new DCC_Modal:g_RegistrationModal;

public OnGameModeInit()
{
    g_RegistrationModal = DCC_CreateModal("Account Registration", "reg_modal");

    DCC_AddModalInput(g_RegistrationModal, "username",
        DCC_TEXT_STYLE_SHORT, "In-Game Name", 3, 20, true);
    DCC_AddModalInput(g_RegistrationModal, "email",
        DCC_TEXT_STYLE_SHORT, "Email", 5, 100, true);
    return 1;
}

public DCC_OnInteraction(DCC_Interaction:interaction, DCC_User:user,
                         DCC_InteractionType:type, const custom_id[])
{
    if (type == DCC_INTERACTION_COMPONENT && !strcmp(custom_id, "verify_btn"))
    {
        DCC_SendInteractionModal(interaction, g_RegistrationModal);
    }
    else if (type == DCC_INTERACTION_MODAL_SUBMIT && !strcmp(custom_id, "reg_modal"))
    {
        new name[21], email[101];
        DCC_GetModalInputValue(interaction, "username", name);
        DCC_GetModalInputValue(interaction, "email",    email);

        new feedback[200];
        format(feedback, sizeof feedback,
            "Welcome %s. Confirmation sent to %s.", name, email);
        DCC_SendInteractionMessage(interaction, feedback);
    }
    return 1;
}
```

For modals with select menus and file uploads, see [[Modals]].

## What's next

* [[Channels]] / [[Messages]] / [[Embeds]] — basic content APIs.
* [[Slash Commands]] — sub-commands, option types, mention parsing.
* [[Message Components]] / [[Modals]] — interactive UIs.
* [[Interactions]] — every way to read submission data.
* [[Callbacks]] — every event you can listen to.
* [[Troubleshooting]] — when something does not behave as expected.
