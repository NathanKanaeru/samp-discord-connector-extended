# Modals

Modals are pop-up forms that appear in front of the Discord client. They can hold up to **5 components** in total: text inputs, select menus, file uploads, or any combination.

Modals can only be sent in response to an interaction (button click, select menu pick, or slash command).

* [Component types allowed in a modal](#component-types-allowed-in-a-modal)
* [Building a modal — text inputs](#building-a-modal--text-inputs)
* [Adding selects and file uploads](#adding-selects-and-file-uploads)
* [Sending the modal](#sending-the-modal)
* [Reading the submission](#reading-the-submission)
* [Limits](#limits)
* [Reference](#reference)

## Component types allowed in a modal

| Component         | Constructor                                | Notes                            |
|-------------------|--------------------------------------------|----------------------------------|
| Text input        | `DCC_CreateTextInput` / `DCC_AddModalInput` | Single-line or paragraph.        |
| String select     | `DCC_CreateSelectMenu(STRING_SELECT, ...)`  | Custom dropdown.                 |
| User select       | `DCC_CreateSelectMenu(USER_SELECT, ...)`    | Lists guild members.             |
| Role select       | `DCC_CreateSelectMenu(ROLE_SELECT, ...)`    | Lists guild roles.               |
| Mentionable select| `DCC_CreateSelectMenu(MENTION_SELECT, ...)` | Users + roles.                   |
| Channel select    | `DCC_CreateSelectMenu(CHANNEL_SELECT, ...)` | Lists guild channels.            |
| File upload       | `DCC_CreateFileUpload`                     | 1+ attachments, returns CDN URL. |

## Building a modal — text inputs

Use `DCC_AddModalInput` for plain text rows — that is the historical layout that every Discord client supports.

```pawn
new DCC_Modal:m = DCC_CreateModal("Account Registration", "reg_modal");

DCC_AddModalInput(m, "username", DCC_TEXT_STYLE_SHORT,
                  "In-Game Name", 3, 20, true);
DCC_AddModalInput(m, "biography", DCC_TEXT_STYLE_PARAGRAPH,
                  "Player Bio", 10, 500, false);
```

Text input styles:

```pawn
enum DCC_TextInputStyle
{
    DCC_TEXT_STYLE_SHORT     = 1,    // single line
    DCC_TEXT_STYLE_PARAGRAPH = 2     // multi-line
};
```

## Adding selects and file uploads

Build the component first, then attach it to the modal with `DCC_AddModalComponent`. The `label` (visible above the field) and optional `description` (small grey text below it) are surfaced to the user.

```pawn
new DCC_Modal:m = DCC_CreateModal("Faction Application", "fac_app");

// 1) Plain text input via the legacy path
DCC_AddModalInput(m, "reason", DCC_TEXT_STYLE_PARAGRAPH,
                  "Why do you want to join?", 30, 500, true);

// 2) String select inside the modal
new DCC_Component:rank = DCC_CreateSelectMenu(
    DCC_COMPONENT_STRING_SELECT, "rank_pref",
    "Pick a preferred rank", 1, 1);
DCC_AddSelectMenuOption(rank, "Cadet",     "1");
DCC_AddSelectMenuOption(rank, "Officer",   "2");
DCC_AddSelectMenuOption(rank, "Detective", "3");
DCC_AddModalComponent(m, rank,
    "Preferred Rank",
    "Subject to change after approval");

// 3) File upload
new DCC_Component:proof = DCC_CreateFileUpload("evidence", 1, 3, true);
DCC_AddModalComponent(m, proof,
    "Proof of Identity",
    "PNG / PDF, up to 3 files");
```

You can mix legacy text inputs and `AddModalComponent` rows in the same modal — the connector picks the correct Discord wrapper (Action Row vs Label) automatically.

## Sending the modal

```pawn
public DCC_OnInteraction(DCC_Interaction:interaction, DCC_User:user,
                         DCC_InteractionType:type, const custom_id[])
{
    if (type == DCC_INTERACTION_COMPONENT && !strcmp(custom_id, "open_form"))
    {
        DCC_SendInteractionModal(interaction, m);
    }
    return 1;
}
```

If the modal is malformed, the connector logs the precise reason at `ERROR` level (`modal N: ...`) and refuses to send instead of letting Discord reject the request silently.

## Reading the submission

When the user submits the modal, `DCC_OnInteraction` fires again with `type == DCC_INTERACTION_MODAL_SUBMIT`. The `custom_id` matches the modal's id.

```pawn
public DCC_OnInteraction(DCC_Interaction:interaction, DCC_User:user,
                         DCC_InteractionType:type, const custom_id[])
{
    if (type != DCC_INTERACTION_MODAL_SUBMIT) return 1;
    if (strcmp(custom_id, "fac_app"))         return 1;

    // 1) Text inputs
    new reason[501];
    DCC_GetModalInputValue(interaction, "reason", reason);

    // 2) Select menu (handles 1+ values)
    new rank_count, rank[8];
    DCC_GetModalSelectCount(interaction, "rank_pref", rank_count);
    if (rank_count > 0)
        DCC_GetModalSelectValue(interaction, "rank_pref", 0, rank);

    // 3) File uploads
    new file_count;
    DCC_GetModalAttachmentCount(interaction, "evidence", file_count);
    for (new i = 0; i < file_count; i++)
    {
        new attach_id[DCC_ID_SIZE], url[256];
        DCC_GetModalAttachmentId (interaction, "evidence", i, attach_id);
        DCC_GetModalAttachmentUrl(interaction, "evidence", i, url);
        printf("attachment[%d] id=%s url=%s", i, attach_id, url);
    }

    DCC_SendInteractionMessage(interaction, "Thanks! Your application has been received.");
    return 1;
}
```

Notes:

* Files are stored on Discord's CDN — you do not need to host them. The URL stays valid for as long as Discord keeps the message; for permanent storage download the file inside your gamemode (e.g. via an external script).
* `DCC_GetModalSelectCount` / `Value` work for both V1 (text-only) and V2 (selects/files) modal submissions because the connector walks both layouts.

## Limits

* **5 components** total per modal.
* Modal title — up to **45** characters.
* Custom IDs — up to **100** characters.
* Text input value — up to **4000** characters.
* User must submit within ~15 minutes or the interaction expires.

## Reference

```pawn
// build
DCC_Modal:DCC_CreateModal(const title[], const custom_id[]);
bool:DCC_AddModalInput(DCC_Modal:m, const custom_id[], DCC_TextInputStyle:style,
                       const label[],
                       min_length = -1, max_length = -1, bool:required = true,
                       const value[] = "", const placeholder[] = "");
bool:DCC_AddModalComponent(DCC_Modal:m, DCC_Component:component,
                           const label[] = "", const description[] = "");

DCC_Component:DCC_CreateTextInput(const custom_id[], DCC_TextInputStyle:style,
                                  const label[],
                                  min_length = -1, max_length = -1,
                                  bool:required = true,
                                  const value[] = "", const placeholder[] = "");
DCC_Component:DCC_CreateFileUpload(const custom_id[],
                                   min_values = 1, max_values = 1,
                                   bool:required = true);

// send
DCC_SendInteractionModal(DCC_Interaction:i, DCC_Modal:m);

// retrieve
DCC_GetModalInputValue       (DCC_Interaction:i, const custom_id[],
                              dest[], max_size = sizeof dest);
DCC_GetModalSelectCount      (DCC_Interaction:i, const custom_id[], &count);
DCC_GetModalSelectValue      (DCC_Interaction:i, const custom_id[], offset,
                              dest[], max_size = sizeof dest);
DCC_GetModalAttachmentCount  (DCC_Interaction:i, const custom_id[], &count);
DCC_GetModalAttachmentId     (DCC_Interaction:i, const custom_id[], offset,
                              dest[DCC_ID_SIZE], max_size = DCC_ID_SIZE);
DCC_GetModalAttachmentUrl    (DCC_Interaction:i, const custom_id[], offset,
                              dest[], max_size = sizeof dest);
```
