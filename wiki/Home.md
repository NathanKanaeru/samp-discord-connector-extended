# Welcome to SA:MP Discord Connector Extended

**SA:MP Discord Connector Extended** is a high-performance plugin that bridges your SA:MP (San Andreas Multiplayer) server with Discord. It leverages the latest Discord API features, providing a rich, interactive experience with Buttons, Select Menus, Modals, and Slash Commands.

## Table of Contents
* [Getting Started](Home#getting-started)
    * [Requirements](Home#requirements)
    * [Installation](Home#installation)
    * [Configuration](Home#configuration)
* [Interactive Features](Home#interactive-features)
    * [[Message Components]](Message-Components) (Buttons & Menus)
    * [[Modals]](Modals) (Pop-up Forms)
    * [[Slash Commands]](Slash-Commands)
* [Reference](Home#reference)
    * [[Full API Reference]](API-Reference)
    * [[Callbacks Guide]](Callbacks)

---

## Getting Started

### Requirements
* **SA:MP Server**: Version 0.3.7 or Open.MP.
* **Discord Bot**: Created via the [Discord Developer Portal](https://discord.com/developers/applications).
* **Gateway Intents**: The following **Privileged Intents** must be enabled in the Developer Portal:
    * `Presence Intent`
    * `Server Members Intent`
    * `Message Content Intent` (Required for reading commands and text)

### Installation
1. Download the latest release (`.dll` for Windows, `.so` for Linux).
2. Place the plugin file in your server's `plugins/` directory.
3. Add `discord-connector` to the `plugins` line in your `server.cfg`.
4. Copy `discord-connector.inc` to your `pawno/include/` directory.

### Configuration
Configure your bot by adding these lines to your `server.cfg`:
```cfg
discord_bot_token YOUR_SECRET_TOKEN
discord_bot_intents 131071 # Enables all intents for full functionality
```

## Interactive Features
This plugin allows your PAWN scripts to control Discord entities as if they were native game elements:
* **Buttons & Menus**: Create dynamic UIs directly in Discord messages.
* **Modals**: Gather structured data from users via pop-up forms.
* **Slash Commands**: Register and handle modern `/` commands with auto-completion support.
* **Embeds**: Send beautiful, formatted rich-text messages.
