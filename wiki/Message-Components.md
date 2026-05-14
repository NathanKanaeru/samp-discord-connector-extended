# Message Components

Message Components are interactive elements that appear at the bottom of Discord messages. They allow users to interact with your bot through simple clicks or selections.

## Buttons
Buttons are clickable elements. There are two types: **Action Buttons** (trigger a callback) and **Link Buttons** (open a URL).

### Button Styles
| Style | PAWN Constant | Description |
|---|---|---|
| Blue | `DCC_STYLE_PRIMARY` | Standard action button. |
| Grey | `DCC_STYLE_SECONDARY` | Subtle action button. |
| Green | `DCC_STYLE_SUCCESS` | Confirm/Success action. |
| Red | `DCC_STYLE_DANGER` | Delete/Danger action. |
| Link | `DCC_STYLE_LINK` | URL button (No callback triggered). |

### Example: Creating an Action Button
```pawn
// DCC_CreateButton(label[], DCC_ButtonStyle:style, custom_id[]);
new DCC_Component:btn = DCC_CreateButton("Apply Now", DCC_STYLE_SUCCESS, "apply_btn");
```

## Select Menus
Select Menus provide a dropdown list for users to choose one or multiple items.

### Menu Types
* `DCC_COMPONENT_STRING_SELECT`: List of custom text options.
* `DCC_COMPONENT_USER_SELECT`: List of server members.
* `DCC_COMPONENT_ROLE_SELECT`: List of server roles.
* `DCC_COMPONENT_MENTION_SELECT`: Mixed list of users and roles.
* `DCC_COMPONENT_CHANNEL_SELECT`: List of server channels.

### Example: String Select Menu
```pawn
new DCC_Component:menu = DCC_CreateSelectMenu(DCC_COMPONENT_STRING_SELECT, "job_selection", "Select a job...");
DCC_AddSelectMenuOption(menu, "Police Officer", "job_pd", "Join the LSPD");
DCC_AddSelectMenuOption(menu, "Medic", "job_md", "Join the Hospital");
```

## Action Rows & Layout Limits
Components must be placed inside an **Action Row** to be displayed.

### Core Rules & Limits:
* **Max 5 Action Rows** per message.
* **Max 5 Buttons** per Action Row.
* **Max 1 Select Menu** per Action Row.
* **Buttons and Select Menus cannot coexist in the same Action Row.**

### Example: Layouting
```pawn
new DCC_ActionRow:row1 = DCC_CreateActionRow();
DCC_AddComponentToActionRow(row1, btn1);
DCC_AddComponentToActionRow(row1, btn2);

new DCC_ActionRow:row2 = DCC_CreateActionRow();
DCC_AddComponentToActionRow(row2, menu1);

new DCC_ActionRow:rows[2];
rows[0] = row1;
rows[1] = row2;

DCC_SendChannelMessageEx(channel, "Choose your action:", DCC_INVALID_EMBED, rows, 2);
```

## Handling Interactions
When a component is clicked/selected, `DCC_OnInteraction` is called with `type == DCC_INTERACTION_COMPONENT`.

```pawn
public DCC_OnInteraction(DCC_Interaction:interaction, DCC_User:user, DCC_InteractionType:type, const custom_id[])
{
    if (type == DCC_INTERACTION_COMPONENT)
    {
        if (!strcmp(custom_id, "apply_btn"))
        {
            DCC_SendInteractionMessage(interaction, "You clicked the apply button!");
        }
    }
    return 1;
}
```
