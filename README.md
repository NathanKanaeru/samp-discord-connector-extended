# SA:MP Discord Connector Extended

[![Build](https://github.com/NathanKanaeru/samp-discord-connector-extended/workflows/Build/badge.svg)](https://github.com/NathanKanaeru/samp-discord-connector-extended/actions)
[![Latest Release](https://img.shields.io/github/release/NathanKanaeru/samp-discord-connector-extended.svg)](https://github.com/NathanKanaeru/samp-discord-connector-extended/releases)
[![License](https://img.shields.io/github/license/NathanKanaeru/samp-discord-connector-extended.svg)](LICENSE)

**SA:MP Discord Connector Extended** is an enhanced version of the original Discord connector for San Andreas Multiplayer. This version provides full support for modern Discord UI features, allowing you to create a truly interactive bridge between your game server and Discord community.

## 🚀 Key Features
*   **Full Interaction Support**: Native support for **Buttons** and **Select Menus**.
*   **Pop-up Forms**: Implementation of **Modals** for structured user input.
*   **Modern API**: Fully compatible with Discord API v10 and Slash Commands.
*   **Rich Layouts**: Multi-row support using **Action Rows**.
*   **Comprehensive Coverage**: Includes all original features (Channels, Messages, Users, Guilds, Roles, Embeds, Emojis).

## 🛠️ Installation
1.  Download the latest release for your OS from the [Releases](https://github.com/NathanKanaeru/samp-discord-connector-extended/releases) page.
2.  Extract the plugin (`.dll` or `.so`) to your server's `plugins/` folder.
3.  Add `discord-connector` to your `server.cfg`.
4.  Copy `discord-connector.inc` to your `pawno/include/` directory.
5.  Configure your bot token in `server.cfg`:
    ```cfg
    discord_bot_token YOUR_SECRET_TOKEN
    discord_bot_intents 131071
    ```

## 📖 Documentation
For detailed guides on how to use the new interactive components, please visit our **[GitHub Wiki](https://github.com/NathanKanaeru/samp-discord-connector-extended/wiki)**.

### Quick Example (Button)
```pawn
public OnGameModeInit() {
    new DCC_Component:btn = DCC_CreateButton("Verify Me", DCC_STYLE_SUCCESS, "verify_btn");
    new DCC_ActionRow:row = DCC_CreateActionRow();
    DCC_AddComponentToActionRow(row, btn);
    // ... send message with row
}
```

## 🏗️ Build Instructions
The plugin must be compiled as a **32-bit** library.
1.  Install a C++ compiler (MSVC for Windows, GCC for Linux).
2.  Install [CMake](http://www.cmake.org/) and [Conan](https://conan.io) (v1.x).
3.  Clone recursively: `git clone --recursive https://github.com/NathanKanaeru/samp-discord-connector-extended.git`
4.  Build using CMake:
    ```bash
    mkdir build && cd build
    cmake .. -A Win32 # On Windows
    cmake --build . --config Release
    ```

## 📜 Credits & Contributors
*   **maddinat0r**: Original author of the `samp-discord-connector` plugin.
*   **NathanKanaeru**: Lead developer and contributor for the **Extended** version (Interactive Components & Modals support).
*   **SA:MP Team**: For the original San Andreas Multiplayer platform.
*   **All Contributors**: Everyone who has submitted bug reports and patches.

---
Licensed under the [Zlib License](LICENSE).
