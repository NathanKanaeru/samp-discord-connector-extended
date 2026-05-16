# Troubleshooting

When something does not behave as expected, the connector exposes its internal state via two log destinations:

1. **`dc-connector.log`** — written in the SA-MP working directory by the plugin.
2. **`server_log.txt`** — captures everything `Logger::LogToAll` writes, plus the rest of the SA-MP server logging.

`WARNING` and `ERROR` lines are always written, even at the default log level. Bring up the log first when diagnosing — most issues spell themselves out there.

* [The plugin loads but never connects](#the-plugin-loads-but-never-connects)
* [Buttons or select menus don't show up](#buttons-or-select-menus-dont-show-up)
* [Slash commands don't appear](#slash-commands-dont-appear)
* [Modal submission values are empty](#modal-submission-values-are-empty)
* ["could not prepare callback"](#could-not-prepare-callback)
* [Member events never fire](#member-events-never-fire)
* [Build issues](#build-issues)

---

## The plugin loads but never connects

* Make sure `DCC_BOT_TOKEN` (env var) **or** `discord_bot_token` (in `server.cfg`) is set, and that the value is for an *application bot*, not a user token.
* Verify the bot has not been disabled in the Developer Portal.
* Allow outbound HTTPS to:
  * `discord.com:443` (REST API)
  * `gateway.discord.gg:443` (gateway WebSocket)
* If the host runs behind a proxy or restrictive firewall, whitelist those.

The startup log line you want to see is:

```
discord-connector: <version> successfully loaded.
```

If you only see `timeout while initializing data.`, the WebSocket handshake failed — check your network egress and the bot token.

---

## Buttons or select menus don't show up

After the modal/component refactor, every component is validated locally before the message is posted. Check the log for lines such as:

```
button 3: must have either a label or an emoji
button 7: non-link buttons require a non-empty custom_id
button 7: link buttons require a non-empty URL
button 7: custom_id is longer than the 100 char limit
action row 2: buttons and select menus cannot coexist
action row 2: at most 5 buttons per row, got 7
select menu 4: string select must have at least one option
modal 1: must contain at least one component
```

If the local validation passes but Discord still rejects the payload, the `WARNING` line shows the HTTP method, path, and Discord's body verbatim:

```
POST /channels/<id>/messages --> 400 Bad Request: {"code":50035,"errors":{...}}
```

Decode the `errors` object — Discord points exactly at the offending field.

The single biggest cause in older builds was that 4xx responses were silently dropped. They are now logged at `WARNING` level by default — keep `samplog` log level at `WARNING` or higher.

---

## Slash commands don't appear

* **Global commands take up to one hour to propagate the first time.** During development, register guild-scoped commands by passing a real `DCC_Guild:` to `DCC_CreateCommand` — they appear instantly.
* The bot must have the `applications.commands` scope when invited. If you only granted `bot`, kick the bot and re-invite with both scopes.
* Watch for `WARNING ... received a command interaction for command X but no command exists` — it means Discord delivered an interaction for a command that the connector has not registered locally. This usually means a command was deleted on Discord's side without `DCC_DeleteCommand` being called.

---

## Modal submission values are empty

* The `custom_id` you pass to `DCC_GetModalInputValue` / `DCC_GetModalSelectValue` / `DCC_GetModalAttachmentUrl` must match exactly what you used when **building** the modal. Case-sensitive.
* For modals that mix text inputs and select / file uploads (V2 layout) the connector now walks both `data.components[].components[]` and `data.components[].component`. If a value still comes back empty, check that the user actually filled the field — `min_values` of 0 makes a select / file-upload optional.

---

## "could not prepare callback"

```
DCC_SendChannelMessage: could not prepare callback
```

The `callback` and `format` arguments do not match the variadic args you passed. Either:

* Pass `""` for `callback` (and `""` for `format`) when you do not want a callback, or
* Make sure `format` has one specifier per variadic argument — `d`/`i` for integers, `s` for strings, `f` for floats, `b` for bools, `r` for references, `a` for arrays followed by a `d` for the size. See [[Callbacks#format-string-spec]] for the full list.

---

## Member events never fire

`DCC_OnGuildMemberAdd` / `Update` / `Remove` and `DCC_GetGuildMemberStatus` rely on **privileged intents** that you must explicitly enable in the Discord Developer Portal:

* **Server Members Intent** — for `Add` / `Update` / `Remove`.
* **Presence Intent** — for `DCC_GetGuildMemberStatus` and the status fields of `Update`.
* **Message Content Intent** — for `DCC_GetMessageContent` to return real text on messages that don't mention the bot.

Bots that exceed a certain server count (≥ 100 guilds, in Discord's words) need verification before the privileged intents become available.

---

## Build issues

### `Conan: Invalid setting '15' is not a valid 'settings.compiler.version' value`

Conan 1.x does not yet recognise gcc 15. Two paths:

* Install `gcc-14` and `g++-14` and pass them explicitly:
  ```bash
  cmake -S . -B build \
        -DCMAKE_C_COMPILER=gcc-14 \
        -DCMAKE_CXX_COMPILER=g++-14 \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_C_FLAGS=-m32 \
        -DCMAKE_CXX_FLAGS=-m32
  ```
* Or upgrade to Conan 2 with the appropriate cmake-conan integration.

### `cannot find Scrt1.o` / `cannot find -lgcc` when adding `-m32`

Multilib libraries are missing. On Debian / Ubuntu:

```bash
sudo apt install gcc-multilib g++-multilib libc6-dev-i386
```

### `error: 'uint16_t' was not declared`

A vendored library (typically `yaml-cpp` 0.7.0 from `log-core`) misses `<cstdint>` includes when built with gcc 15+. Build with gcc-14 (see the first item above) or patch the vendored source to add the missing include.

---

## Still stuck?

1. Increase verbosity: set the `samplog` level to `DEBUG` to see every HTTP request and gateway frame the plugin processes.
2. Reproduce with a minimal gamemode — strip everything except the failing flow.
3. Open an issue on the [GitHub repository](https://github.com/NathanKanaeru/samp-discord-connector-extended/issues) with the `WARNING` / `ERROR` lines from `dc-connector.log`. The exact Discord rejection body is the most useful piece of information you can attach.
