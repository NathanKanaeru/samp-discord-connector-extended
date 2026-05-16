# Message Components

Message Components are the interactive elements that sit at the bottom of Discord messages: buttons and select menus. They live inside *Action Rows*, and a message can carry up to 5 Action Rows.

* [Buttons](#buttons) — five styles, link or action.
* [Select menus](#select-menus) — string options + 4 dynamic types (user, role, mentionable, channel).
* [Action Rows](#action-rows) — layout container.
* [Sending components](#sending-components) — channels and interaction replies.
* [Validation and limits](#validation-and-limits)
* [Disabling components](#disabling-components)
* [Reference](#reference)

## Buttons

```pawn
// Action button — fires DCC_OnInteraction(custom_id="apply_btn")
new DCC_Component:apply = DCC_CreateButton(
    "Apply Now",            // label (max 80)
    DCC_STYLE_SUCCESS,      // PRIMARY / SECONDARY / SUCCESS / DANGER / LINK
    "apply_btn");           // custom_id (max 100, required for non-LINK)

// Link button — opens a URL in the browser, no callback
new DCC_Component:wiki = DCC_CreateButton(
    "Read the Wiki",
    DCC_STYLE_LINK,
    "",                     // custom_id is unused for LINK
    "https://example.com/wiki");

// Disabled button with an emoji
new DCC_Emoji:lock = DCC_CreateEmoji("🔒");
new DCC_Component:locked = DCC_CreateButton(
    "Locked",
    DCC_STYLE_SECONDARY,
    "locked_btn",
    "",
    true,                   // disabled
    lock);                  // emoji decoration
```

Style reference:

| Constant              | Color          | Typical use                       |
|-----------------------|----------------|-----------------------------------|
| `DCC_STYLE_PRIMARY`   | Blue (blurple) | Default action ("Submit").        |
| `DCC_STYLE_SECONDARY` | Grey           | Cancel / secondary action.        |
| `DCC_STYLE_SUCCESS`   | Green          | Confirm / accept.                 |
| `DCC_STYLE_DANGER`    | Red            | Destructive ("Delete").           |
| `DCC_STYLE_LINK`      | Grey + arrow   | External URL — no callback fired. |

## Select menus

```pawn
new DCC_Component:job = DCC_CreateSelectMenu(
    DCC_COMPONENT_STRING_SELECT,
    "job_pick",            // custom_id
    "Choose a job…",       // placeholder
    1, 1,                  // min / max selectable
    false);                // disabled

DCC_AddSelectMenuOption(job, "Police Officer", "pd", "Join LSPD");
DCC_AddSelectMenuOption(job, "Medic",          "md", "Join Hospital");
DCC_AddSelectMenuOption(job, "Mechanic",       "mc", "Service vehicles");
```

`min_values` / `max_values` set how many entries the user must / can pick. Set both to 1 for a single-pick dropdown, or `min=0, max=N` for an optional multi-select.

Discord-populated lists (do **not** call `AddSelectMenuOption`):

| Component constant              | What it lists  |
|---------------------------------|----------------|
| `DCC_COMPONENT_USER_SELECT`     | Guild members  |
| `DCC_COMPONENT_ROLE_SELECT`     | Guild roles    |
| `DCC_COMPONENT_MENTION_SELECT`  | Users + roles  |
| `DCC_COMPONENT_CHANNEL_SELECT`  | Guild channels |

```pawn
new DCC_Component:user_pick = DCC_CreateSelectMenu(
    DCC_COMPONENT_USER_SELECT, "report_user", "Pick a user to report", 1, 1);
```

## Action Rows

```pawn
new DCC_ActionRow:row1 = DCC_CreateActionRow();
DCC_AddComponentToActionRow(row1, btn1);
DCC_AddComponentToActionRow(row1, btn2);

new DCC_ActionRow:row2 = DCC_CreateActionRow();
DCC_AddComponentToActionRow(row2, job);
```

## Sending components

To a channel:

```pawn
new DCC_ActionRow:rows[2] = { row1, row2 };
DCC_SendChannelMessageEx(channel, "Pick an action:",
    DCC_INVALID_EMBED, rows, 2);
```

As a slash-command reply, the same rows array can be passed to interaction sending — but for now, the cleanest path is:

* Reply with text (`DCC_SendInteractionMessage`) and then post the components as a follow-up channel message, **or**
* Build the components in advance and use `DCC_SendChannelMessageEx` from inside the slash command callback.

## Validation and limits

The connector validates components locally before sending. Violations are logged at `ERROR` level and the message is **not** posted. Common rules:

* Max 5 Action Rows per message.
* Max 5 buttons per Action Row.
* Max 1 select menu per Action Row, alone.
* Buttons and select menus cannot share a row.
* Non-LINK buttons require a non-empty `custom_id` (≤100 chars).
* LINK buttons require a non-empty `url`.
* Every button needs a label OR an emoji.
* String selects need at least one option.

Look at `dc-connector.log` for the exact message — see [[Troubleshooting]].

## Disabling components

```pawn
// Buttons created as disabled
DCC_CreateButton("Soon™", DCC_STYLE_PRIMARY, "soon_btn", "", true);

// Select menu created as disabled
DCC_CreateSelectMenu(DCC_COMPONENT_STRING_SELECT, "ro", "Read-only",
    1, 1, true);
```

To toggle later, build a new component with the new `disabled` flag and edit the message.

## Reference

```pawn
DCC_Component:DCC_CreateButton(const text[], DCC_ButtonStyle:style,
                               const custom_id[],
                               const url[]="",
                               bool:disabled = false,
                               DCC_Emoji:emoji = DCC_Emoji:0);

DCC_Component:DCC_CreateSelectMenu(DCC_ComponentType:type, const custom_id[],
                                   const placeholder[]="",
                                   min_values = 1, max_values = 1,
                                   bool:disabled = false);
bool:DCC_AddSelectMenuOption(DCC_Component:menu,
                             const label[], const value[],
                             const description[]="",
                             DCC_Emoji:emoji = DCC_Emoji:0,
                             bool:is_default = false);

DCC_ActionRow:DCC_CreateActionRow();
bool:DCC_AddComponentToActionRow(DCC_ActionRow:row, DCC_Component:component);

// sending
DCC_SendChannelMessageEx(DCC_Channel:ch, const message[],
                         DCC_Embed:embed,
                         DCC_ActionRow:rows[], rows_size,
                         const callback[]="", const format[]="", {Float,_}:...);
```

For reading what the user clicked / picked, see [[Interactions]].
