# Interactions

An *interaction* is a single user action that originates inside Discord and lands in your gamemode: a slash command run, a button click, a select-menu pick, or a modal submission. Every interaction comes with a `DCC_Interaction:` handle that lets you read the data the user sent and respond exactly once.

This page is the cookbook for everything you can do with that handle.

* [Interaction types](#interaction-types)
* [The single OnInteraction forward](#the-single-oninteraction-forward)
* [Reading the context](#reading-the-context) — channel, guild, content.
* [Reading slash command options](#reading-slash-command-options)
* [Reading select menu picks (in messages)](#reading-select-menu-picks-in-messages)
* [Reading modal submissions](#reading-modal-submissions)
* [Responding](#responding) — text, embed, modal, components.
* [Reference](#reference)

## Interaction types

```pawn
enum DCC_InteractionType
{
    DCC_INTERACTION_COMMAND      = 2,   // a slash command
    DCC_INTERACTION_COMPONENT    = 3,   // a button click or a select menu pick
    DCC_INTERACTION_MODAL_SUBMIT = 5    // a modal was submitted
};
```

Slash commands also dispatch to the dedicated callback you specified in `DCC_CreateCommand`, so you can keep the per-command logic separate.

## The single OnInteraction forward

```pawn
forward DCC_OnInteraction(DCC_Interaction:interaction, DCC_User:user,
                          DCC_InteractionType:type, const custom_id[]);
```

* `interaction` — handle for reading data and responding.
* `user` — the Discord user who triggered the interaction.
* `type` — see the enum above.
* `custom_id` — the id you assigned when creating the button / select menu / modal. For slash commands it is the command name.

Switch on `type` first, then on `custom_id`:

```pawn
public DCC_OnInteraction(DCC_Interaction:interaction, DCC_User:user,
                         DCC_InteractionType:type, const custom_id[])
{
    switch (type)
    {
        case DCC_INTERACTION_COMPONENT:    HandleComponent(interaction, user, custom_id);
        case DCC_INTERACTION_MODAL_SUBMIT: HandleModalSubmit(interaction, user, custom_id);
        case DCC_INTERACTION_COMMAND:      /* usually goes via the command callback */;
    }
    return 1;
}
```

## Reading the context

Every interaction exposes its origin:

```pawn
new DCC_Channel:ch;
new DCC_Guild  :guild;
new content[256];

DCC_GetInteractionChannel(interaction, ch);
DCC_GetInteractionGuild  (interaction, guild);
DCC_GetInteractionContent(interaction, content);  // first option value (commands)
```

`DCC_GetInteractionGuild` returns `DCC_INVALID_GUILD` for DM interactions.

## Reading slash command options

```pawn
new opt_count;
DCC_GetInteractionOptionCount(interaction, opt_count);

new path[64], value[256];
new DCC_CommandOptionType:type;

for (new i = 0; i < opt_count; i++)
{
    DCC_GetInteractionOption(interaction, i, path, value, type);
    printf("[option %d] %s = %s (type %d)", i, path, value, _:type);
}
```

`path` is the dotted path through any sub-command groups (e.g. `player.kick.reason`). For `OPTION_USER` / `OPTION_CHANNEL` / `OPTION_ROLE`, `value` is the snowflake of the picked entity — feed it to `DCC_FindUserById` / `DCC_FindChannelById` / `DCC_FindRoleById`.

Mentions inside string options:

```pawn
new mention_count;
DCC_GetInteractionMentionCount(interaction, mention_count);
for (new i = 0; i < mention_count; i++)
{
    new DCC_User:u;
    DCC_GetInteractionMention(interaction, i, u);
}
```

See [[Slash Commands]] for the full registration / option-tree story.

## Reading select menu picks (in messages)

When a user picks one or more entries from a message-bound select menu, `DCC_OnInteraction` fires with `type == DCC_INTERACTION_COMPONENT` and `custom_id` set to the menu id.

```pawn
new count;
DCC_GetInteractionMenuCount(interaction, count);

new value[64];
for (new i = 0; i < count; i++)
{
    DCC_GetInteractionMenuValue(interaction, i, value);
    printf("picked: %s", value);
}
```

For a `STRING_SELECT`, `value` is the per-option `value` you passed to `DCC_AddSelectMenuOption`.
For `USER_SELECT` / `ROLE_SELECT` / `MENTION_SELECT` / `CHANNEL_SELECT`, `value` is the snowflake of the picked entity.

## Reading modal submissions

Modal submissions arrive as `type == DCC_INTERACTION_MODAL_SUBMIT`. Use the field-specific natives keyed by the `custom_id` you set when building the modal:

```pawn
// Text input
new bio[2001];
DCC_GetModalInputValue(interaction, "biography", bio);

// Select menu inside the modal (1 or more values)
new pick_count, pick[64];
DCC_GetModalSelectCount(interaction, "rank_pref", pick_count);
if (pick_count > 0)
    DCC_GetModalSelectValue(interaction, "rank_pref", 0, pick);

// File upload (1 or more attachments)
new attach_count;
DCC_GetModalAttachmentCount(interaction, "proof", attach_count);
for (new i = 0; i < attach_count; i++)
{
    new id[DCC_ID_SIZE], url[256];
    DCC_GetModalAttachmentId (interaction, "proof", i, id);
    DCC_GetModalAttachmentUrl(interaction, "proof", i, url);
}
```

The connector walks both V1 (Action Row > Text Input) and V2 (Label > component) payload shapes, so the same call works regardless of how the modal was built.

## Responding

You **must** respond within 3 seconds. There are four mutually exclusive ways to respond:

```pawn
// 1. Plain text
DCC_SendInteractionMessage(interaction, "Done.");

// 2. Embed
DCC_SendInteractionEmbed(interaction, my_embed);
DCC_SendInteractionEmbed(interaction, my_embed, "Optional text on top.");

// 3. Modal — only allowed for COMMAND or COMPONENT types
DCC_SendInteractionModal(interaction, my_modal);

// 4. Defer (handled internally)
//    The connector always sends DEFERRED_CHANNEL_MESSAGE_WITH_SOURCE for
//    slash commands, then your call to one of the responders above edits
//    the deferred message. This means you can take longer than 3 seconds
//    inside the command callback.
```

After the first response, subsequent calls **edit** the original message:

```pawn
DCC_SendInteractionMessage(interaction, "Working on it…");
// ... a few hundred ms later
DCC_SendInteractionMessage(interaction, "All done!");
```

## Reference

```pawn
// context
DCC_GetInteractionChannel(DCC_Interaction:i, &DCC_Channel:channel);
DCC_GetInteractionGuild  (DCC_Interaction:i, &DCC_Guild:guild);
DCC_GetInteractionContent(DCC_Interaction:i, dest[], max_size = sizeof dest);

// command options
DCC_GetInteractionOptionCount(DCC_Interaction:i, &count);
DCC_GetInteractionOption     (DCC_Interaction:i, offset,
                              path[], value[], &DCC_CommandOptionType:type,
                              path_size = sizeof path,
                              value_size = sizeof value);
DCC_GetInteractionMentionCount(DCC_Interaction:i, &count);
DCC_GetInteractionMention     (DCC_Interaction:i, offset, &DCC_User:user);

// component select menus (in messages)
DCC_GetInteractionMenuCount(DCC_Interaction:i, &count);
DCC_GetInteractionMenuValue(DCC_Interaction:i, offset, dest[], max_size = sizeof dest);

// modal submissions
DCC_GetModalInputValue     (DCC_Interaction:i, const custom_id[],
                            dest[], max_size = sizeof dest);
DCC_GetModalSelectCount    (DCC_Interaction:i, const custom_id[], &count);
DCC_GetModalSelectValue    (DCC_Interaction:i, const custom_id[], offset,
                            dest[], max_size = sizeof dest);
DCC_GetModalAttachmentCount(DCC_Interaction:i, const custom_id[], &count);
DCC_GetModalAttachmentId   (DCC_Interaction:i, const custom_id[], offset,
                            dest[DCC_ID_SIZE], max_size = DCC_ID_SIZE);
DCC_GetModalAttachmentUrl  (DCC_Interaction:i, const custom_id[], offset,
                            dest[], max_size = sizeof dest);

// respond
DCC_SendInteractionMessage(DCC_Interaction:i, const message[] = "");
DCC_SendInteractionEmbed  (DCC_Interaction:i, DCC_Embed:embed, const message[] = "");
DCC_SendInteractionModal  (DCC_Interaction:i, DCC_Modal:modal);

forward DCC_OnInteraction(DCC_Interaction:interaction, DCC_User:user,
                          DCC_InteractionType:type, const custom_id[]);
```
