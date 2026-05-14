# API Reference

This page lists the primary native functions provided by the plugin. For a complete list, refer to `discord-connector.inc`.

## Channel Management
* `DCC_FindChannelById(channel_id[])`: Returns `DCC_Channel`.
* `DCC_GetChannelName(DCC_Channel:channel, dest[], size)`
* `DCC_SendChannelMessage(DCC_Channel:channel, message[])`: Simple message send.
* `DCC_SendChannelMessageEx(DCC_Channel:channel, message[], DCC_Embed:embed, DCC_ActionRow:rows[], count)`: Advanced send with UI components.

## User & Guild Management
* `DCC_FindUserById(user_id[])`: Returns `DCC_User`.
* `DCC_GetUserName(DCC_User:user, dest[], size)`
* `DCC_FindGuildById(guild_id[])`: Returns `DCC_Guild`.
* `DCC_GetGuildMemberNickname(DCC_Guild:guild, DCC_User:user, dest[], size)`
* `DCC_HasGuildMemberRole(DCC_Guild:guild, DCC_User:user, DCC_Role:role)`: Returns boolean.

## Components (Buttons & Menus)
* `DCC_CreateButton(label[], style, custom_id[])`: Returns `DCC_Component`.
* `DCC_CreateSelectMenu(type, custom_id[], placeholder[])`: Returns `DCC_Component`.
* `DCC_AddSelectMenuOption(DCC_Component:menu, label[], value[], description[])`
* `DCC_CreateActionRow()`: Returns `DCC_ActionRow`.
* `DCC_AddComponentToActionRow(DCC_ActionRow:row, DCC_Component:comp)`

## Modals
* `DCC_CreateModal(title[], custom_id[])`: Returns `DCC_Modal`.
* `DCC_AddModalInput(DCC_Modal:modal, custom_id[], style, label[], min, max, required)`
* `DCC_SendInteractionModal(DCC_Interaction:interaction, DCC_Modal:modal)`
* `DCC_GetModalInputValue(DCC_Interaction:interaction, custom_id[], dest[])`

## Interaction Data Retrieval
* `DCC_SendInteractionMessage(DCC_Interaction:interaction, message[])`: Responds to an interaction.
* `DCC_GetInteractionOption(DCC_Interaction:interaction, name[], dest[])`: Gets Slash Command parameters.
* `DCC_GetInteractionMenuValue(DCC_Interaction:interaction, offset, dest[])`: Gets selected value from a menu.
* `DCC_GetInteractionMenuCount(DCC_Interaction:interaction, &count)`: Gets number of selected items.

## Embeds
* `DCC_CreateEmbed(title[], description[], color)`
* `DCC_AddEmbedField(DCC_Embed:embed, name[], value[], inline)`
* `DCC_SetEmbedFooter(DCC_Embed:embed, text[], icon_url[])`
* `DCC_SetEmbedImage(DCC_Embed:embed, url[])`
