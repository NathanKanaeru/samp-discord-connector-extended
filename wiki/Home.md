# Welcome to SA-MP Discord Connector — Extended

A SA-MP / open.mp plugin that bridges your game server with the Discord v10 API. It maintains a persistent gateway connection so your gamemode can react to chat events, and a REST client so it can send messages, build interactive UIs, manage members, and run slash commands.

## Index

* [[Getting Started]] — install, configure, write your first handler.
* **Discord objects**
  * [[Channels]] — find / create / edit / delete text and voice channels.
  * [[Messages]] — post, edit, cache, and listen to messages.
  * [[Users]] — read user identity and metadata.
  * [[Roles]] — inspect and update roles.
  * [[Guilds]] — manage members, roles, channels, bans, and voice.
  * [[Bot]] — set the bot's presence, activity, nickname, typing indicator, and open DMs.
  * [[Emojis and Reactions]] — add or remove reactions, listen for reaction events.
  * [[Embeds]] — build rich embeds with fields, footer, thumbnail, image.
* **Interactivity**
  * [[Slash Commands]] — register `/commands` with typed options and sub-commands.
  * [[Message Components]] — buttons and 5 types of select menus.
  * [[Modals]] — pop-up forms with text inputs, select menus, and file uploads.
  * [[Interactions]] — read and respond to every kind of interaction event.
* **Reference**
  * [[Callbacks]] — every `forward` the plugin can fire.
  * [[Constants]] — enums and `#define`s exported by the include file.
  * [[API Reference]] — every native, grouped by topic, with signatures.
  * [[Troubleshooting]] — diagnose silent failures and Discord 4xx errors.

## What this plugin gives you

* Persistent gateway WebSocket with automatic reconnect and intent control.
* All modern Discord v10 surface area exposed to Pawn:
  * Channels, messages, embeds, reactions, users, roles, guild membership, voice channels, bans.
  * Slash commands with global or per-guild scope, sub-commands, and typed options.
  * Buttons (5 styles), select menus (5 types), modals with text inputs / selects / file uploads.
  * Bot presence, activity, typing indicator, private DMs.
* Eager validation of payloads — every component is checked locally first, and Discord's `400 Bad Request` body is logged at `WARNING` level so silent rejections become visible.

## Requirements

* **Server** — SA-MP 0.3.7 R2/R3 or open.mp.
* **Architecture** — 32-bit (i386 / x86) plugin that matches the SA-MP server runtime.
* **Discord bot** — created at the [Discord Developer Portal](https://discord.com/developers/applications). Enable any privileged intents your gamemode actually needs:
  * **Server Members Intent** — needed for `DCC_OnGuildMemberAdd / Update / Remove`.
  * **Message Content Intent** — needed to read user-typed message text.
  * **Presence Intent** — needed for `DCC_GetGuildMemberStatus`.

## Installing the plugin

1. Download the latest release for your OS from the [releases page](https://github.com/NathanKanaeru/samp-discord-connector-extended/releases).
2. Drop the binary into your server's `plugins/` directory:
   * Linux → `discord-connector.so`
   * Windows → `discord-connector.dll`
3. Copy `discord-connector.inc` into your `pawno/include/` folder.
4. Add `discord-connector` to the `plugins` line of `server.cfg`:
   ```cfg
   plugins discord-connector
   ```
5. Configure the bot token (next section), restart the server, watch the log for
   `discord-connector: <version> successfully loaded.`

## Configuring the bot

You can configure the bot from `server.cfg` **or** through environment variables. Environment variables take precedence — that is useful for CI / Docker / systemd setups where you do not want to commit secrets.

### `server.cfg`
```cfg
discord_bot_token   YOUR_BOT_TOKEN
discord_bot_intents 131071    ; bitmask of gateway intents — 131071 = all
```

### Environment variables
| Variable          | Purpose                                                            |
|-------------------|--------------------------------------------------------------------|
| `DCC_BOT_TOKEN`   | Discord bot token (overrides `discord_bot_token`).                 |
| `DCC_BOT_INTENTS` | Gateway intents bitmask (overrides `discord_bot_intents`, default 131071). |

### Required scopes / permissions
When you generate the bot invite URL, include at minimum:

* Scopes — `bot`, `applications.commands`.
* Permissions — *View Channels*, *Send Messages*, *Read Message History*,
  *Add Reactions*, *Use Application Commands*.
* Add anything else your gamemode actually does (kick, ban, manage roles,
  manage channels, …).

## A 30-second sanity check

```pawn
#include <a_samp>
#include <discord-connector>

public OnGameModeInit()
{
    SetGameModeText("Discord-connected RP");
    new DCC_Channel:ch = DCC_FindChannelByName("general");
    if (ch != DCC_INVALID_CHANNEL)
        DCC_SendChannelMessage(ch, "Server is online.");
    return 1;
}

public DCC_OnChannelMessage(DCC_Message:message)
{
    new content[256];
    DCC_GetMessageContent(message, content);

    if (!strcmp(content, "!ping"))
    {
        new DCC_Channel:ch;
        DCC_GetMessageChannel(message, ch);
        DCC_SendChannelMessage(ch, "pong!");
    }
    return 1;
}
```

If `Server is online.` appears in your `#general` channel and `!ping` answers
with `pong!`, the plugin is wired up correctly. From there, jump to
[[Getting Started]] for a complete walkthrough, or pick the topic you need from
the index above.
