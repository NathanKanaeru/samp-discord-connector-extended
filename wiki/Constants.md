# Constants

Every enum and `#define` exported by `discord-connector.inc`. Use this page as the single source of truth when picking a constant.

* [Tag types and `INVALID` sentinels](#tag-types-and-invalid-sentinels)
* [Buffer sizes](#buffer-sizes)
* [Channel types](#channel-types) — `DCC_ChannelType`
* [Presence](#presence) — `DCC_UserPresenceStatus`, `DCC_BotPresenceStatus`
* [Reactions](#reactions) — `DCC_MessageReactionType`
* [Components](#components) — `DCC_ComponentType`, `DCC_ButtonStyle`, `DCC_TextInputStyle`
* [Interactions](#interactions) — `DCC_InteractionType`
* [Slash command options](#slash-command-options) — `DCC_CommandOptionType`

---

## Tag types and `INVALID` sentinels

The connector represents Discord objects with strongly-tagged Pawn handles. Each one has an `INVALID` sentinel:

| Tag                | Sentinel                | Notes                                  |
|--------------------|-------------------------|----------------------------------------|
| `DCC_Channel:`     | `DCC_INVALID_CHANNEL`   | Returned by `DCC_FindChannel*` on miss.|
| `DCC_User:`        | `DCC_INVALID_USER`      |                                        |
| `DCC_Role:`        | `DCC_INVALID_ROLE`      |                                        |
| `DCC_Guild:`       | `DCC_INVALID_GUILD`     |                                        |
| `DCC_Component:`   | `DCC_INVALID_COMPONENT` | Buttons / select menus / inputs.       |
| `DCC_ActionRow:`   | `DCC_INVALID_ACTION_ROW`| Layout container.                      |
| `DCC_Modal:`       | `DCC_INVALID_MODAL`     |                                        |
| `DCC_Embed:`       | `DCC_INVALID_EMBED`     |                                        |
| `DCC_Emoji:`       | `DCC_Emoji:0`           | No named macro - compare to `0` or `DCC_Emoji:0`. |
| `DCC_Message:`     | `DCC_Message:0`         | No named macro.                        |
| `DCC_Interaction:` | —                       | Only delivered by callbacks.           |
| `DCC_Command:`     | —                       | Returned only from `DCC_CreateCommand`.|
| `DCC_Option:`      | `DCC_Option:0`          | No named macro.                        |

Always check `handle != DCC_INVALID_*` before using lookup results.

---

## Buffer sizes

```pawn
#define DCC_ID_SIZE                  (20 + 1)   // discord snowflake (20 chars) + null
#define DCC_USERNAME_SIZE            (32 + 1)
#define DCC_NICKNAME_SIZE            (32 + 1)
#define DCC_EMOJI_NAME_SIZE          (32 + 1)
#define DCC_COMMAND_SIZE             (32 + 1)
#define DCC_COMMAND_DESCRIPTION_SIZE (32 + 1)
```

Use them when declaring destination buffers:

```pawn
new id  [DCC_ID_SIZE];
new name[DCC_USERNAME_SIZE];
DCC_GetUserId  (user, id);
DCC_GetUserName(user, name);
```

> The `DCC_COMMAND_DESCRIPTION_SIZE` constant uses 32 chars only because the Pawn signature has to declare a fixed length; Discord itself accepts up to 100 chars in the command description. Truncate longer text in your script before passing it.

---

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

Used by `DCC_GetChannelType` and `DCC_CreateGuildChannel`.

---

## Presence

```pawn
enum DCC_UserPresenceStatus
{
    INVALID = 0,
    ONLINE,
    IDLE,
    DO_NOT_DISTURB,
    OFFLINE
};
```

Returned by `DCC_GetGuildMemberStatus`. Requires the **Presence Intent**.

```pawn
enum DCC_BotPresenceStatus
{
    INVALID = 0,
    ONLINE,
    IDLE,
    DO_NOT_DISTURB,
    INVISIBLE,
    OFFLINE
};
```

Used by `DCC_GetBotPresenceStatus` and `DCC_SetBotPresenceStatus`. Note that the bot variant has an extra `INVISIBLE` state that does not exist on the user side.

---

## Reactions

```pawn
enum DCC_MessageReactionType
{
    REACTION_ADD = 0,
    REACTION_REMOVE,
    REACTION_REMOVE_ALL,
    REACTION_REMOVE_EMOJI
};
```

Delivered as the fourth argument of `DCC_OnMessageReaction`. See [[Emojis and Reactions]].

---

## Components

```pawn
enum DCC_ComponentType
{
    DCC_COMPONENT_ACTION_ROW     = 1,
    DCC_COMPONENT_BUTTON         = 2,
    DCC_COMPONENT_STRING_SELECT  = 3,
    DCC_COMPONENT_TEXT_INPUT     = 4,
    DCC_COMPONENT_USER_SELECT    = 5,
    DCC_COMPONENT_ROLE_SELECT    = 6,
    DCC_COMPONENT_MENTION_SELECT = 7,
    DCC_COMPONENT_CHANNEL_SELECT = 8,
    DCC_COMPONENT_LABEL          = 18,    // V2 wrapper for selects/files in modals
    DCC_COMPONENT_FILE_UPLOAD    = 19     // modal-only file upload component
};
```

Pass to `DCC_CreateSelectMenu` to choose the menu flavour. The `LABEL` and `FILE_UPLOAD` types are managed internally by `DCC_AddModalComponent` / `DCC_CreateFileUpload` — you usually do not refer to them by hand.

```pawn
enum DCC_ButtonStyle
{
    DCC_STYLE_PRIMARY   = 1,    // blurple
    DCC_STYLE_SECONDARY = 2,    // grey
    DCC_STYLE_SUCCESS   = 3,    // green
    DCC_STYLE_DANGER    = 4,    // red
    DCC_STYLE_LINK      = 5     // grey, opens a URL, no callback
};
```

```pawn
enum DCC_TextInputStyle
{
    DCC_TEXT_STYLE_SHORT     = 1,    // single line
    DCC_TEXT_STYLE_PARAGRAPH = 2     // multi-line text area
};
```

---

## Interactions

```pawn
enum DCC_InteractionType
{
    DCC_INTERACTION_COMMAND      = 2,
    DCC_INTERACTION_COMPONENT    = 3,
    DCC_INTERACTION_MODAL_SUBMIT = 5
};
```

Third argument of `DCC_OnInteraction`. See [[Interactions]].

---

## Slash command options

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

Used by `DCC_AddCommandOption` and the `&type` out-parameter of `DCC_GetInteractionOption`. See [[Slash Commands]].
