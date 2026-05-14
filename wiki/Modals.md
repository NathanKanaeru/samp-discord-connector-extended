# Modals

Modals are pop-up forms that allow you to collect structured text input from users. They are ideal for registration forms, reports, or feedback systems.

## Creating a Modal
A modal consists of a title and one or more text inputs.

### Text Input Styles
* `DCC_TEXT_STYLE_SHORT`: Single-line input.
* `DCC_TEXT_STYLE_PARAGRAPH`: Multi-line text area.

### Example: User Registration Form
```pawn
// DCC_CreateModal(title[], custom_id[]);
new DCC_Modal:regModal = DCC_CreateModal("Account Registration", "reg_modal");

// DCC_AddModalInput(DCC_Modal:modal, custom_id[], style, label[], min, max, required);
DCC_AddModalInput(regModal, "username", DCC_TEXT_STYLE_SHORT, "In-Game Name", 3, 20, true);
DCC_AddModalInput(regModal, "biography", DCC_TEXT_STYLE_PARAGRAPH, "Player Bio", 10, 500, false);
```

## Triggering a Modal
Modals can **only** be sent as a response to an interaction (Button click, Select Menu selection, or Slash Command). You cannot send a modal to a channel directly.

```pawn
public DCC_OnInteraction(DCC_Interaction:interaction, DCC_User:user, DCC_InteractionType:type, const custom_id[])
{
    if (type == DCC_INTERACTION_COMPONENT && !strcmp(custom_id, "start_reg_btn"))
    {
        // Respond with the modal
        DCC_SendInteractionModal(interaction, regModal);
    }
    return 1;
}
```

## Retrieving Submitted Data
When the user clicks "Submit", the `DCC_OnInteraction` callback is triggered with `type == DCC_INTERACTION_MODAL_SUBMIT`.

```pawn
public DCC_OnInteraction(DCC_Interaction:interaction, DCC_User:user, DCC_InteractionType:type, const custom_id[])
{
    if (type == DCC_INTERACTION_MODAL_SUBMIT && !strcmp(custom_id, "reg_modal"))
    {
        new name[21], bio[501];
        
        // DCC_GetModalInputValue(interaction, input_custom_id[], dest[]);
        DCC_GetModalInputValue(interaction, "username", name);
        DCC_GetModalInputValue(interaction, "biography", bio);
        
        printf("New registration: Name=%s", name);
        
        // You must respond to the interaction
        DCC_SendInteractionMessage(interaction, "Thank you! Your registration has been received.");
    }
    return 1;
}
```

## Critical Notes
* Modals have a timeout. If the user takes too long to fill it out, the interaction might expire.
* You can have up to **5 text inputs** in a single modal.
