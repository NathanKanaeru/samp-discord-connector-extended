# API Reference

Every native exported by `discord-connector.inc`, grouped by category and accompanied by its full Pawn signature. For prose explanations and end-to-end examples, follow the cross-references to the topic pages.

* [Channels](#channels)
* [Messages](#messages)
* [Users](#users)
* [Roles](#roles)
* [Guilds](#guilds)
* [Bot](#bot)
* [Misc](#misc)
* [Embeds](#embeds)
* [Emojis](#emojis)
* [Reactions](#reactions)
* [Slash Commands](#slash-commands)
* [Components](#components)
* [Modals](#modals)
* [Interactions](#interactions)

---

## Channels

See [[Channels]].

```pawn
DCC_Channel:DCC_FindChannelByName(const channel_name[]);
DCC_Channel:DCC_FindChannelById  (const channel_id[]);

DCC_GetChannelId            (DCC_Channel:channel, dest[DCC_ID_SIZE], max_size = sizeof dest);
DCC_GetChannelType          (DCC_Channel:channel, &DCC_ChannelType:type);
DCC_GetChannelGuild         (DCC_Channel:channel, &DCC_Guild:guild);
DCC_GetChannelName          (DCC_Channel:channel, dest[], max_size = sizeof dest);
DCC_GetChannelTopic         (DCC_Channel:channel, dest[], max_size = sizeof dest);
DCC_GetChannelPosition      (DCC_Channel:channel, &position);
DCC_IsChannelNsfw           (DCC_Channel:channel, &bool:is_nsfw);
DCC_GetChannelParentCategory(DCC_Channel:channel, &DCC_Channel:category);

DCC_SendChannelMessage     (DCC_Channel:channel, const message[],
                            const callback[]="", const format[]="", {Float,_}:...);
DCC_SendChannelMessageEx   (DCC_Channel:channel, const message[],
                            DCC_Embed:embed, DCC_ActionRow:rows[], rows_size,
                            const callback[]="", const format[]="", {Float,_}:...);

DCC_SetChannelName          (DCC_Channel:channel, const name[]);
DCC_SetChannelTopic         (DCC_Channel:channel, const topic[]);
DCC_SetChannelPosition      (DCC_Channel:channel, position);
DCC_SetChannelNsfw          (DCC_Channel:channel, bool:is_nsfw);
DCC_SetChannelParentCategory(DCC_Channel:channel, DCC_Channel:parent_category);
DCC_DeleteChannel           (DCC_Channel:channel);
```

---

## Messages

See [[Messages]].

```pawn
DCC_GetMessageId             (DCC_Message:message, dest[DCC_ID_SIZE], max_size = DCC_ID_SIZE);
DCC_GetMessageChannel        (DCC_Message:message, &DCC_Channel:channel);
DCC_GetMessageAuthor         (DCC_Message:message, &DCC_User:author);
DCC_GetMessageContent        (DCC_Message:message, dest[], max_size = sizeof dest);
DCC_IsMessageTts             (DCC_Message:message, &bool:is_tts);
DCC_IsMessageMentioningEveryone(DCC_Message:message, &bool:mentions_everyone);
DCC_GetMessageUserMentionCount (DCC_Message:message, &count);
DCC_GetMessageUserMention      (DCC_Message:message, offset, &DCC_User:user);
DCC_GetMessageRoleMentionCount (DCC_Message:message, &count);
DCC_GetMessageRoleMention      (DCC_Message:message, offset, &DCC_Role:role);

DCC_DeleteMessage            (DCC_Message:message);
DCC_Message:DCC_GetCreatedMessage();
DCC_DeleteInternalMessage    (DCC_Message:message);
DCC_EditMessage              (DCC_Message:message, const content[],
                              DCC_Embed:embed = DCC_Embed:0);
DCC_SetMessagePersistent     (DCC_Message:message, bool:persistent);
DCC_CacheChannelMessage      (const channel_id[DCC_ID_SIZE],
                              const message_id[DCC_ID_SIZE],
                              const callback[] = "",
                              const format[]   = "", {Float,_}:...);
```

---

## Users

See [[Users]].

```pawn
DCC_User:DCC_FindUserByName(const user_name[], const user_discriminator[]);
DCC_User:DCC_FindUserById  (const user_id[]);

DCC_GetUserName        (DCC_User:user, dest[DCC_USERNAME_SIZE], max_size = sizeof dest);
DCC_GetUserId          (DCC_User:user, dest[DCC_ID_SIZE],       max_size = DCC_ID_SIZE);
DCC_GetUserDiscriminator(DCC_User:user, dest[],                  max_size = sizeof dest);
DCC_IsUserBot          (DCC_User:user, &bool:is_bot);
DCC_IsUserVerified     (DCC_User:user, &bool:is_verified);
```

---

## Roles

See [[Roles]].

```pawn
DCC_Role:DCC_FindRoleByName(DCC_Guild:guild, const role_name[]);
DCC_Role:DCC_FindRoleById  (const role_id[]);

DCC_GetRoleId         (DCC_Role:role, dest[DCC_ID_SIZE], max_size = sizeof dest);
DCC_GetRoleName       (DCC_Role:role, dest[],            max_size = sizeof dest);
DCC_GetRoleColor      (DCC_Role:role, &color);
DCC_GetRoleColour     (DCC_Role:role, &colour) = DCC_GetRoleColor;       // alias
DCC_GetRolePermissions(DCC_Role:role, &perm_high, &perm_low);
DCC_IsRoleHoist       (DCC_Role:role, &bool:is_hoist);
DCC_GetRolePosition   (DCC_Role:role, &position);
DCC_IsRoleMentionable (DCC_Role:role, &bool:is_mentionable);
```

---

## Guilds

See [[Guilds]].

```pawn
DCC_Guild:DCC_FindGuildByName(const guild_name[]);
DCC_Guild:DCC_FindGuildById  (const guild_id[]);
DCC_GetAllGuilds(DCC_Guild:dest[], max_size = sizeof dest);

DCC_GetGuildId     (DCC_Guild:guild, dest[DCC_ID_SIZE], max_size = sizeof dest);
DCC_GetGuildName   (DCC_Guild:guild, dest[],            max_size = sizeof dest);
DCC_GetGuildOwnerId(DCC_Guild:guild, dest[DCC_ID_SIZE], max_size = sizeof dest);
DCC_SetGuildName   (DCC_Guild:guild, const name[]);

// channels
DCC_GetGuildChannel       (DCC_Guild:guild, offset, &DCC_Channel:channel);
DCC_GetGuildChannelCount  (DCC_Guild:guild, &count);
DCC_CreateGuildChannel    (DCC_Guild:guild, const name[], DCC_ChannelType:type,
                           const callback[]="", const format[]="", {Float,_}:...);
DCC_Channel:DCC_GetCreatedGuildChannel();

// roles
DCC_GetGuildRole         (DCC_Guild:guild, offset, &DCC_Role:role);
DCC_GetGuildRoleCount    (DCC_Guild:guild, &count);
DCC_CreateGuildRole      (DCC_Guild:guild, const name[],
                          const callback[]="", const format[]="", {Float,_}:...);
DCC_Role:DCC_GetCreatedGuildRole();
DCC_DeleteGuildRole      (DCC_Guild:guild, DCC_Role:role);
DCC_SetGuildRoleName     (DCC_Guild:guild, DCC_Role:role, const name[]);
DCC_SetGuildRolePosition (DCC_Guild:guild, DCC_Role:role, position);
DCC_SetGuildRolePermissions(DCC_Guild:guild, DCC_Role:role, perm_high, perm_low);
DCC_SetGuildRoleColor    (DCC_Guild:guild, DCC_Role:role, color);
DCC_SetGuildRoleHoist    (DCC_Guild:guild, DCC_Role:role, bool:hoist);
DCC_SetGuildRoleMentionable(DCC_Guild:guild, DCC_Role:role, bool:mentionable);

// members
DCC_GetGuildMember           (DCC_Guild:guild, offset, &DCC_User:user);
DCC_GetGuildMemberCount      (DCC_Guild:guild, &count);
DCC_GetGuildMemberNickname   (DCC_Guild:guild, DCC_User:user, dest[DCC_NICKNAME_SIZE],
                              max_size = sizeof dest);
DCC_GetGuildMemberVoiceChannel(DCC_Guild:guild, DCC_User:user, &DCC_Channel:channel);
DCC_GetGuildMemberRole       (DCC_Guild:guild, DCC_User:user, offset, &DCC_Role:role);
DCC_GetGuildMemberRoleCount  (DCC_Guild:guild, DCC_User:user, &count);
DCC_HasGuildMemberRole       (DCC_Guild:guild, DCC_User:user, DCC_Role:role, &bool:has_role);
DCC_GetGuildMemberStatus     (DCC_Guild:guild, DCC_User:user, &DCC_UserPresenceStatus:status);
DCC_SetGuildMemberNickname   (DCC_Guild:guild, DCC_User:user, const nickname[]);
DCC_SetGuildMemberVoiceChannel(DCC_Guild:guild, DCC_User:user, DCC_Channel:channel);
DCC_AddGuildMemberRole       (DCC_Guild:guild, DCC_User:user, DCC_Role:role);
DCC_RemoveGuildMemberRole    (DCC_Guild:guild, DCC_User:user, DCC_Role:role);
DCC_RemoveGuildMember        (DCC_Guild:guild, DCC_User:user);
DCC_CreateGuildMemberBan     (DCC_Guild:guild, DCC_User:user, const reason[]="");
DCC_RemoveGuildMemberBan     (DCC_Guild:guild, DCC_User:user);
```

---

## Bot

See [[Bot]].

```pawn
DCC_BotPresenceStatus:DCC_GetBotPresenceStatus();
DCC_SetBotPresenceStatus(DCC_BotPresenceStatus:status);
DCC_SetBotActivity(const name[]);
DCC_ClearBotActivity();                                        // stock
DCC_TriggerBotTypingIndicator(DCC_Channel:channel);
DCC_SetBotNickname(DCC_Guild:guild, const nickname[]);
DCC_ClearBotNickname(DCC_Guild:guild);                         // stock
DCC_CreatePrivateChannel(DCC_User:user, const callback[],
                         const format[] = "", {Float,_}:...);
DCC_Channel:DCC_GetCreatedPrivateChannel();
```

---

## Misc

```pawn
DCC_EscapeMarkdown(const src[], dest[], max_size = sizeof dest);
```

Escapes Discord markdown so user-provided text echoed back is rendered literally.

---

## Embeds

See [[Embeds]].

```pawn
DCC_Embed:DCC_CreateEmbed(const title[]=""        , const description[]=""    ,
                          const url[]=""          , const timestamp[]=""      ,
                          color = 0,
                          const footer_text[]=""  , const footer_icon_url[]="",
                          const thumbnail_url[]="", const image_url[]="");
DCC_DeleteEmbed(DCC_Embed:embed);

DCC_SendChannelEmbedMessage(DCC_Channel:ch, DCC_Embed:embed, const message[]="",
                            const callback[]="", const format[]="", {Float,_}:...);

DCC_AddEmbedField     (DCC_Embed:embed, const name[], const value[], bool:inline = false);
DCC_SetEmbedTitle     (DCC_Embed:embed, const title[]);
DCC_SetEmbedDescription(DCC_Embed:embed, const description[]);
DCC_SetEmbedUrl       (DCC_Embed:embed, const url[]);
DCC_SetEmbedTimestamp (DCC_Embed:embed, const timestamp[]);
DCC_SetEmbedColor     (DCC_Embed:embed, color);
DCC_SetEmbedColour    (DCC_Embed:embed, colour) = DCC_SetEmbedColor;     // alias
DCC_SetEmbedFooter    (DCC_Embed:embed, const footer_text[], const footer_icon_url[]="");
DCC_SetEmbedThumbnail (DCC_Embed:embed, const thumbnail_url[]);
DCC_SetEmbedImage     (DCC_Embed:embed, const image_url[]);
```

---

## Emojis

See [[Emojis and Reactions]].

```pawn
DCC_Emoji:DCC_CreateEmoji(const name[DCC_EMOJI_NAME_SIZE],
                          const snowflake[DCC_ID_SIZE] = "");
DCC_DeleteEmoji(DCC_Emoji:emoji);
DCC_GetEmojiName(DCC_Emoji:emoji, dest[DCC_EMOJI_NAME_SIZE],
                 maxlen = DCC_EMOJI_NAME_SIZE);
```

---

## Reactions

```pawn
DCC_CreateReaction       (DCC_Message:message, DCC_Emoji:reaction_emoji);
DCC_DeleteMessageReaction(DCC_Message:message, DCC_Emoji:reaction_emoji = DCC_Emoji:0);
```

---

## Slash Commands

See [[Slash Commands]].

```pawn
DCC_Command:DCC_CreateCommand(const command_name[DCC_COMMAND_SIZE],
                              const description[DCC_COMMAND_DESCRIPTION_SIZE],
                              const callback[],
                              bool:allow_everyone = true,
                              DCC_Guild:guild = DCC_INVALID_GUILD);
DCC_DeleteCommand(DCC_Command:command);
DCC_Option:DCC_AddCommandOption(const name[], const description[],
                                DCC_CommandOptionType:type,
                                bool:required = true,
                                DCC_Option:parent_option = DCC_Option:0);
```

---

## Components

See [[Message Components]].

```pawn
DCC_Component:DCC_CreateButton(const text[], DCC_ButtonStyle:style,
                               const custom_id[], const url[]="",
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
DCC_Component:DCC_CreateTextInput(const custom_id[], DCC_TextInputStyle:style,
                                  const label[],
                                  min_length = -1, max_length = -1,
                                  bool:required = true,
                                  const value[] = "", const placeholder[] = "");
DCC_Component:DCC_CreateFileUpload(const custom_id[],
                                   min_values = 1, max_values = 1,
                                   bool:required = true);
DCC_ActionRow:DCC_CreateActionRow();
bool:DCC_AddComponentToActionRow(DCC_ActionRow:row, DCC_Component:component);
```

---

## Modals

See [[Modals]].

```pawn
DCC_Modal:DCC_CreateModal(const title[], const custom_id[]);
bool:DCC_AddModalInput(DCC_Modal:modal, const custom_id[], DCC_TextInputStyle:style,
                       const label[],
                       min_length = -1, max_length = -1, bool:required = true,
                       const value[] = "", const placeholder[] = "");
bool:DCC_AddModalComponent(DCC_Modal:modal, DCC_Component:component,
                           const label[] = "", const description[] = "");
DCC_SendInteractionModal(DCC_Interaction:interaction, DCC_Modal:modal);
```

---

## Interactions

See [[Interactions]].

```pawn
DCC_GetInteractionOptionCount(DCC_Interaction:i, &option_count);
DCC_GetInteractionOption     (DCC_Interaction:i, offset,
                              path[], value[], &DCC_CommandOptionType:type,
                              path_size = sizeof path,
                              value_size = sizeof value);
DCC_GetInteractionMentionCount(DCC_Interaction:i, &count);
DCC_GetInteractionMention     (DCC_Interaction:i, offset, &DCC_User:user);
DCC_GetInteractionContent     (DCC_Interaction:i, dest[], max_size = sizeof dest);
DCC_GetInteractionChannel     (DCC_Interaction:i, &DCC_Channel:channel);
DCC_GetInteractionGuild       (DCC_Interaction:i, &DCC_Guild:guild);

DCC_GetInteractionMenuCount(DCC_Interaction:i, &count);
DCC_GetInteractionMenuValue(DCC_Interaction:i, offset, dest[],
                            max_size = sizeof dest);

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

DCC_SendInteractionMessage(DCC_Interaction:i, const message[] = "");
DCC_SendInteractionEmbed  (DCC_Interaction:i, DCC_Embed:embed,
                           const message[] = "");
DCC_SendInteractionModal  (DCC_Interaction:i, DCC_Modal:modal);
```
