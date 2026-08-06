<h1 align="center">SA-MP Discord Connector — Extended</h1>

<p align="center">
  <em>A modern bridge between your San Andreas Multiplayer server and Discord, with full support for buttons, select menus, modals, file uploads, and slash commands.</em>
</p>

<p align="center">
  <a href="https://github.com/NathanKanaeru/samp-discord-connector-extended/actions"><img src="https://github.com/NathanKanaeru/samp-discord-connector-extended/workflows/Build/badge.svg" alt="Build status"></a>
  <a href="https://github.com/NathanKanaeru/samp-discord-connector-extended/releases"><img src="https://img.shields.io/github/release/NathanKanaeru/samp-discord-connector-extended.svg" alt="Latest release"></a>
  <a href="LICENSE"><img src="https://img.shields.io/github/license/NathanKanaeru/samp-discord-connector-extended.svg" alt="License: zlib"></a>
  <img src="https://img.shields.io/badge/Discord%20API-v10-5865F2.svg" alt="Discord API v10">
  <img src="https://img.shields.io/badge/SA--MP-0.3.7%20%7C%20open.mp-yellow.svg" alt="SA-MP / open.mp">
</p>

---

## Table of Contents

- [Overview](#overview)
- [What's Different in the Extended Fork](#whats-different-in-the-extended-fork)
- [Feature Matrix](#feature-matrix)
- [Installation](#installation)
- [Configuration](#configuration)
- [Quick Start](#quick-start)
- [Component Cookbook](#component-cookbook)
  - [Buttons](#buttons)
  - [Select Menus](#select-menus)
  - [Modals — Text Inputs](#modals--text-inputs)
  - [Modals — Selects & File Upload](#modals--selects--file-upload)
  - [Slash Commands](#slash-commands)
  - [Embeds](#embeds)
- [Interaction Handling](#interaction-handling)
- [Native Reference (cheat sheet)](#native-reference-cheat-sheet)
- [Building From Source](#building-from-source)
- [Troubleshooting](#troubleshooting)
- [Compatibility](#compatibility)
- [Project Structure](#project-structure)
- [Contributing](#contributing)
- [Credits](#credits)
- [License](#license)

---

## Overview

**SA-MP Discord Connector — Extended** is a SA-MP / open.mp plugin that exposes the modern Discord API to Pawn. It maintains a persistent gateway connection so your gamemode can react to chat events, and a REST client so it can post messages, build interactive UIs, and manage guild data.

Where the original `samp-discord-connector` ends at sending plain messages and embeds, this fork brings the full Discord v10 interaction model to Pawn: buttons, select menus, modals, modal-side file uploads, slash commands with options, and rich embeds — all without leaving the Pawn script.

```pawn
public OnGameModeInit()
{
    new DCC_Component:btn = DCC_CreateButton("Verify Me", DCC_STYLE_SUCCESS, "verify_btn");
    new DCC_ActionRow:row = DCC_CreateActionRow();
    DCC_AddComponentToActionRow(row, btn);

    new DCC_ActionRow:rows[1] = { row };
    new DCC_Channel:ch = DCC_FindChannelByName("verification");
    DCC_SendChannelMessageEx(ch, "Click the button to start verification.",
                             DCC_INVALID_EMBED, rows, 1);
    return 1;
}
```

---

## What's Different in the Extended Fork

| Capability                     | Original | Extended (this fork)                |
|--------------------------------|----------|-------------------------------------|
| Channel / Message / Reactions  | ✅       | ✅                                  |
| Embeds                         | ✅       | ✅                                  |
| Slash Commands                 | ❌       | ✅ Full options support             |
| Buttons (link + action)        | ❌       | ✅ All five styles                  |
| Select Menus (string)          | ❌       | ✅                                  |
| Select Menus (user/role/mentionable/channel) | ❌ | ✅                          |
| Modals with text inputs        | ❌       | ✅ Up to 5 inputs                   |
| Modals with select menus       | ❌       | ✅ via Components V2 Label wrapper  |
| Modals with file upload        | ❌       | ✅ Receives Discord attachment IDs  |
| Validation of malformed payloads | ❌     | ✅ Logs precise reason before POST  |
| Visible HTTP error logging     | ❌       | ✅ Non-2xx responses always logged  |

---

## Feature Matrix

- **Discord API v10** — slash commands, application interactions, components.
- **Persistent WebSocket** — gateway connection with intents, automatic reconnect.
- **Interactive Components** — buttons (5 styles), 5 types of select menus.
- **Modals** — pop-up forms holding text inputs, select menus, and file uploads (up to 5 components total).
- **Action Rows** — multi-row layouts following Discord's component limits.
- **Slash Commands** — global or per-guild, with sub-commands and typed options (string, integer, user, channel, role, …).
- **Rich Embeds** — title, description, fields, footer, thumbnail, image, color, timestamp.
- **Reactions** — add, remove, listen for unicode and custom emojis.
- **Guild Management** — channels, roles, members, nicknames, voice channels, bans.
- **Bot Presence** — status (online / idle / DND / invisible), activity text, typing indicator.
- **Validation & Logging** — every component is validated before POST; HTTP failures are logged at WARNING level, with body dumps so Discord's rejection reason is visible.

---

## Installation

1. Grab the latest binary for your OS from the [Releases page](https://github.com/NathanKanaeru/samp-discord-connector-extended/releases).
2. Drop the plugin into your server's `plugins/` directory:
   - Linux → `discord-connector.so`
   - Windows → `discord-connector.dll`
3. Copy the matching `discord-connector.inc` to `pawno/include/`.
4. Register the plugin in `server.cfg`:
   ```cfg
   plugins discord-connector
   ```
5. Configure your bot — see the next section.

> **Note**
> Both `.so` and `.dll` are 32-bit (i386 / x86) because SA-MP plugins must match the server's architecture.

---

## Configuration

You can configure the bot via `server.cfg` **or** via environment variables. Environment variables take precedence — useful for CI / containerised setups.

### `server.cfg`
```cfg
discord_bot_token   YOUR_BOT_TOKEN
discord_bot_intents 131071
```

### Environment variables
| Variable           | Purpose                                              |
|--------------------|------------------------------------------------------|
| `DCC_BOT_TOKEN`    | Discord bot token                                    |
| `DCC_BOT_INTENTS`  | Gateway intents bitmask (defaults to all = 131071)   |

### Required permissions
Invite your bot with at least the following scopes / permissions:

- `bot`, `applications.commands`
- View Channels · Send Messages · Read Message History · Add Reactions
- Use Application Commands · Create Public/Private Threads (optional)
- Whatever else your gamemode actually needs (kick, ban, manage roles, …)

---

## Quick Start

```pawn
#include <a_samp>
#include <discord-connector>

public OnGameModeInit()
{
    SetGameModeText("Discord-connected RP");
    return 1;
}

public DCC_OnChannelMessage(DCC_Message:message)
{
    new DCC_User:author;
    DCC_GetMessageAuthor(message, author);

    new bool:is_bot;
    DCC_IsUserBot(author, is_bot);
    if (is_bot)
        return 1; // ignore other bots / our own posts

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

---

## Component Cookbook

### Buttons

```pawn
new DCC_Component:join = DCC_CreateButton(
    "Join Faction",        // label
    DCC_STYLE_SUCCESS,     // style: PRIMARY / SECONDARY / SUCCESS / DANGER / LINK
    "fac_join"             // custom_id (matched in DCC_OnInteraction)
);

new DCC_Component:wiki = DCC_CreateButton(
    "Read the Wiki",
    DCC_STYLE_LINK,
    "",                    // custom_id is unused for LINK buttons
    "https://example.com/wiki"
);

new DCC_ActionRow:row = DCC_CreateActionRow();
DCC_AddComponentToActionRow(row, join);
DCC_AddComponentToActionRow(row, wiki);
```

**Layout limits** (enforced by the connector — payloads that violate them are rejected with a clear log line):
- Max 5 Action Rows per message.
- Max 5 buttons per Action Row.
- Buttons and select menus cannot share an Action Row.

### Select Menus

```pawn
new DCC_Component:job = DCC_CreateSelectMenu(
    DCC_COMPONENT_STRING_SELECT,
    "job_pick",           // custom_id
    "Choose a job…",      // placeholder
    1, 1                  // min/max selectable
);
DCC_AddSelectMenuOption(job, "Police Officer", "pd", "Join LSPD");
DCC_AddSelectMenuOption(job, "Medic",          "md", "Join Hospital");
DCC_AddSelectMenuOption(job, "Mechanic",       "mc", "Service vehicles");
```

Discord-populated lists (no `AddSelectMenuOption` needed):

| Component                       | What it lists       |
|---------------------------------|---------------------|
| `DCC_COMPONENT_USER_SELECT`     | Guild members       |
| `DCC_COMPONENT_ROLE_SELECT`     | Guild roles         |
| `DCC_COMPONENT_MENTION_SELECT`  | Users + roles       |
| `DCC_COMPONENT_CHANNEL_SELECT`  | Guild channels      |

### Modals — Text Inputs

```pawn
new DCC_Modal:m = DCC_CreateModal("Account Registration", "reg_modal");

DCC_AddModalInput(m, "username", DCC_TEXT_STYLE_SHORT,
                  "In-Game Name", 3, 20, true);
DCC_AddModalInput(m, "biography", DCC_TEXT_STYLE_PARAGRAPH,
                  "Player Bio", 10, 500, false);

// Modals are sent as a response to an interaction:
DCC_SendInteractionModal(interaction, m);
```

### Modals — Selects & File Upload

The connector lets you put *any* input-style component into a modal. `DCC_AddModalComponent` wraps each one in the correct Discord container automatically (Action Row for text inputs, Label for selects / file uploads).

```pawn
new DCC_Modal:m = DCC_CreateModal("Faction Application", "fac_app");

// 1) plain text input
DCC_AddModalInput(m, "reason", DCC_TEXT_STYLE_PARAGRAPH,
                  "Why do you want to join?", 30, 500, true);

// 2) select menu inside the modal
new DCC_Component:rank = DCC_CreateSelectMenu(
    DCC_COMPONENT_STRING_SELECT, "rank_pref", "Pick a preferred rank", 1, 1);
DCC_AddSelectMenuOption(rank, "Cadet",     "1");
DCC_AddSelectMenuOption(rank, "Officer",   "2");
DCC_AddSelectMenuOption(rank, "Detective", "3");

DCC_AddModalComponent(m, rank, "Preferred Rank", "Subject to change after approval");

// 3) file upload
new DCC_Component:proof = DCC_CreateFileUpload("evidence", 1, 3, true);
DCC_AddModalComponent(m, proof, "Proof of Identity", "PNG / PDF, up to 3 files");
```

A modal can hold **up to 5 components** in total.

### Slash Commands

```pawn
public OnGameModeInit()
{
    new DCC_Command:cmd = DCC_CreateCommand("teleport",
        "Teleport a player to coordinates", "OnTeleportCommand");

    new DCC_Option:p = DCC_AddCommandOption(
        "player", "Target player", DCC_OPTION_USER, true);
    DCC_AddCommandOption("x", "X coord", DCC_OPTION_INTEGER, true);
    DCC_AddCommandOption("y", "Y coord", DCC_OPTION_INTEGER, true);
    DCC_AddCommandOption("z", "Z coord", DCC_OPTION_INTEGER, true);
    return 1;
}

forward OnTeleportCommand(DCC_Interaction:interaction, DCC_User:user);
public OnTeleportCommand(DCC_Interaction:interaction, DCC_User:user)
{
    DCC_SendInteractionMessage(interaction, "Teleporting…");
}
```

### Embeds

```pawn
new DCC_Embed:e = DCC_CreateEmbed(
    "Server status",                // title
    "All systems nominal",          // description
    "https://example.com",
    "",                             // ISO 8601 timestamp
    0x57F287                        // color
);
DCC_AddEmbedField(e, "Players online", "42",  true);
DCC_AddEmbedField(e, "Uptime",         "3d",  true);
DCC_SetEmbedFooter(e, "Updated automatically");

new DCC_Channel:ch = DCC_FindChannelByName("status");
DCC_SendChannelEmbedMessage(ch, e);
```

---

## Interaction Handling

Every interactive event lands in a single forward:

```pawn
forward DCC_OnInteraction(DCC_Interaction:interaction, DCC_User:user,
                          DCC_InteractionType:type, const custom_id[]);
```

`type` distinguishes the source:

| Constant                          | Triggered by                        |
|-----------------------------------|-------------------------------------|
| `DCC_INTERACTION_COMMAND`         | A slash command (also routed to its dedicated callback) |
| `DCC_INTERACTION_COMPONENT`       | Button click / select-menu pick     |
| `DCC_INTERACTION_MODAL_SUBMIT`    | User submitted a modal              |

A complete handler:

```pawn
public DCC_OnInteraction(DCC_Interaction:interaction, DCC_User:user,
                         DCC_InteractionType:type, const custom_id[])
{
    switch (type)
    {
        case DCC_INTERACTION_COMPONENT:
        {
            if (!strcmp(custom_id, "verify_btn"))
            {
                DCC_SendInteractionModal(interaction, GetVerifyModal());
            }
            else if (!strcmp(custom_id, "job_pick"))
            {
                new count, value[32];
                DCC_GetInteractionMenuCount(interaction, count);
                if (count > 0)
                    DCC_GetInteractionMenuValue(interaction, 0, value);

                new msg[64];
                format(msg, sizeof msg, "You picked: %s", value);
                DCC_SendInteractionMessage(interaction, msg);
            }
        }

        case DCC_INTERACTION_MODAL_SUBMIT:
        {
            if (!strcmp(custom_id, "fac_app"))
            {
                new reason[501];
                DCC_GetModalInputValue(interaction, "reason", reason);

                new rank_count, rank[8];
                DCC_GetModalSelectCount(interaction, "rank_pref", rank_count);
                if (rank_count > 0)
                    DCC_GetModalSelectValue(interaction, "rank_pref", 0, rank);

                new file_count;
                DCC_GetModalAttachmentCount(interaction, "evidence", file_count);
                for (new i = 0; i < file_count; i++)
                {
                    new url[256];
                    DCC_GetModalAttachmentUrl(interaction, "evidence", i, url);
                    printf("attachment %d: %s", i, url);
                }

                DCC_SendInteractionMessage(interaction,
                    "Thanks! Your application has been received.");
            }
        }
    }
    return 1;
}
```

---

## Native Reference (cheat sheet)

Full signatures live in [`discord-connector.inc.in`](discord-connector.inc.in). Highlights:

### Components
```pawn
DCC_Component:DCC_CreateButton(text[], DCC_ButtonStyle:style, custom_id[],
                               url[]="", bool:disabled=false, DCC_Emoji:emoji=DCC_Emoji:0);
DCC_Component:DCC_CreateSelectMenu(DCC_ComponentType:type, custom_id[],
                                   placeholder[]="", min_values=1, max_values=1,
                                   bool:disabled=false);
bool:DCC_AddSelectMenuOption(DCC_Component:menu, label[], value[],
                             description[]="", DCC_Emoji:emoji=DCC_Emoji:0,
                             bool:is_default=false);
DCC_Component:DCC_CreateTextInput(custom_id[], DCC_TextInputStyle:style, label[],
                                  min_length=-1, max_length=-1, bool:required=true,
                                  value[]="", placeholder[]="");
DCC_Component:DCC_CreateFileUpload(custom_id[], min_values=1, max_values=1,
                                   bool:required=true);
DCC_ActionRow:DCC_CreateActionRow();
bool:DCC_AddComponentToActionRow(DCC_ActionRow:row, DCC_Component:component);
```

### Modals
```pawn
DCC_Modal:DCC_CreateModal(title[], custom_id[]);
bool:DCC_AddModalInput(DCC_Modal:m, custom_id[], DCC_TextInputStyle:style, label[],
                       min_length=-1, max_length=-1, bool:required=true,
                       value[]="", placeholder[]="");
bool:DCC_AddModalComponent(DCC_Modal:m, DCC_Component:component,
                           label[]="", description[]="");
DCC_SendInteractionModal(DCC_Interaction:i, DCC_Modal:m);
```

### Reading interaction submissions
```pawn
DCC_GetInteractionMenuCount(DCC_Interaction:i, &count);
DCC_GetInteractionMenuValue(DCC_Interaction:i, offset, dest[], max_size=sizeof dest);

DCC_GetModalInputValue (DCC_Interaction:i, custom_id[], dest[], max_size=sizeof dest);
DCC_GetModalSelectCount(DCC_Interaction:i, custom_id[], &count);
DCC_GetModalSelectValue(DCC_Interaction:i, custom_id[], offset, dest[], max_size=sizeof dest);

DCC_GetModalAttachmentCount(DCC_Interaction:i, custom_id[], &count);
DCC_GetModalAttachmentId   (DCC_Interaction:i, custom_id[], offset, dest[DCC_ID_SIZE], max_size=DCC_ID_SIZE);
DCC_GetModalAttachmentUrl  (DCC_Interaction:i, custom_id[], offset, dest[], max_size=sizeof dest);
```

### Sending messages with components
```pawn
DCC_SendChannelMessage(DCC_Channel:ch, message[],
                       callback[]="", format[]="", {Float,_}:...);
DCC_SendChannelMessageEx(DCC_Channel:ch, message[], DCC_Embed:embed,
                         DCC_ActionRow:rows[], rows_size,
                         callback[]="", format[]="", {Float,_}:...);
DCC_SendInteractionMessage(DCC_Interaction:i, message[]="", bool:ephemeral=false);
DCC_SendInteractionEmbed  (DCC_Interaction:i, DCC_Embed:e, message[]="", bool:ephemeral=false);
```

For the full list — channels, users, roles, guilds, embeds, emojis, reactions, slash-command builders — open `pawno/include/discord-connector.inc` after installation, or look at the [API Reference page](wiki/API-Reference.md) on the wiki.

---

## Building From Source

The plugin is a 32-bit shared library. The official build path uses CMake + Conan 1.x for dependency resolution (Boost 1.79 header-only, OpenSSL 1.1.1, fmt 7.1.3).

### Linux

```bash
# 1) Toolchain — multilib is required to produce 32-bit binaries.
sudo apt install build-essential gcc-multilib g++-multilib \
                 ninja-build python3-pip git
python3 -m pip install --user "conan<2" "cmake>=3.19,<4"

# 2) Clone with submodules.
git clone --recursive https://github.com/NathanKanaeru/samp-discord-connector-extended
cd samp-discord-connector-extended

# 3) Configure & build.
export CMAKE_POLICY_VERSION_MINIMUM=3.5
cmake -S . -B build -G Ninja \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_C_FLAGS=-m32 \
      -DCMAKE_CXX_FLAGS=-m32 \
      -DCMAKE_EXE_LINKER_FLAGS=-m32 \
      -DCMAKE_SHARED_LINKER_FLAGS=-m32 \
      -DGH_ACBUILD=TRUE
cmake --build build --config Release

# Output:
#   build/artifact/plugins/discord-connector.so
#   build/artifact/pawno/include/discord-connector.inc
```

> If your distro ships gcc 15+, install `gcc-14` / `g++-14` (Conan 1.x doesn't yet recognise gcc 15) and add `-DCMAKE_C_COMPILER=gcc-14 -DCMAKE_CXX_COMPILER=g++-14` to the configure command.

A reproducible `Earthfile` is provided for CI builds:
```bash
earthly +build --BUILD_TYPE=Release
```

### Windows

```powershell
git clone --recursive https://github.com/NathanKanaeru/samp-discord-connector-extended
cd samp-discord-connector-extended

# Visual Studio 2022, Win32 platform, MSVC v143
mkdir build; cd build
cmake .. -A Win32 -T v143,host=x86 -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

Output ends up in `build\artifact\plugins\discord-connector.dll`.

---

## Troubleshooting

> **My buttons / select menus don't show up in Discord.**
> Open `dc-connector.log` (created in the SA-MP working directory) and look for `WARNING` or `ERROR` lines. The connector logs the precise reason every time a payload is rejected — most commonly:
> - `button N: must have either a label or an emoji`
> - `button N: non-link buttons require a non-empty custom_id`
> - `action row N: buttons and select menus cannot coexist`
> - `<HTTP method> /channels/.../messages --> 400 Bad Request: { … Discord error … }`

> **`could not prepare callback` when calling `DCC_SendChannelMessageEx`.**
> Your callback name and format string don't match the variadic arguments. Either pass an empty string for the callback (no callback) or make sure `format` has one specifier per argument (`d`, `s`, `i`, `f`, `b`).

> **The plugin loads but never connects to Discord.**
> Make sure `DCC_BOT_TOKEN` (or `discord_bot_token` in `server.cfg`) is set, the token is for an *application bot* (not a user token), and your firewall lets the SA-MP server reach `discord.com:443` and `gateway.discord.gg:443`.

> **Slash commands don't appear in the guild.**
> Global commands take up to an hour to propagate. For development, register guild-scoped commands by passing a non-`DCC_INVALID_GUILD` argument to `DCC_CreateCommand` — those appear instantly.

> **`Conan: Invalid setting '15' is not a valid 'settings.compiler.version' value`** when building.
> Conan 1.x doesn't yet recognise gcc 15. Install gcc-14 and pass `-DCMAKE_CXX_COMPILER=g++-14 -DCMAKE_C_COMPILER=gcc-14` to CMake, or upgrade to Conan 2 with the appropriate cmake-conan integration.

---

## Compatibility

- **Server**: SA-MP 0.3.7 R2/R3, open.mp.
- **Architecture**: 32-bit only (matches the SA-MP server runtime).
- **OS**: Linux x86 (i386), Windows x86.
- **Discord API**: v10 (modern interactions, Components V2 modal layout).

---

## Project Structure

```
samp-discord-connector-extended/
├── src/                   # plugin sources (Pawn natives, Discord client, components, modals)
├── libs/                  # vendored dependencies (samp-sdk, samp-log-core, fmt, json, date)
├── wiki/                  # markdown docs published to the GitHub Wiki
├── discord-connector.inc.in   # template for the Pawn include header
├── CMakeLists.txt         # top-level build script (Conan + multi-platform)
├── Earthfile              # reproducible Linux build pipeline
└── .github/workflows/     # GitHub Actions CI (Linux + Windows, Debug + Release)
```

---

## Contributing

Pull requests are welcome. A few guidelines:

- Match the existing code style (`tab` indentation, `m_` member prefix, `snake_case` for free functions, `PascalCase` for classes).
- New natives must come with a Pawn declaration in `discord-connector.inc.in` and a wiki page describing them.
- If you change the wire protocol or component layout, please add a Validate() rule that fails fast with a clear log line — this is the single best feature against silent Discord 400 errors.
- Build the plugin and load it in a test gamemode before opening the PR.

For larger changes, please open an issue first to discuss the approach.

---

## Credits

- **maddinat0r** — author of the original [`samp-discord-connector`](https://github.com/maddinat0r/samp-discord-connector) plugin this project is forked from.
- **NathanKanaeru** — lead developer of the *Extended* fork: slash commands, components, modals, file uploads, validation, and tooling.
- **SA-MP team** — for San Andreas Multiplayer.
- **open.mp team** — for keeping the platform alive.
- Every contributor who has filed a bug or sent a patch — thank you.

---

## License

Released under the [zlib license](LICENSE). See `LICENSE` for full text.
