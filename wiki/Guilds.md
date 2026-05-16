# Guilds

A *guild* is what end-users call a "server". Every cached guild gives you access to its channels, roles, members, and settings. Most of the heavy bot administration lives here: setting nicknames, managing roles, kicking, banning, moving voice users, creating channels.

* [Lookup](#lookup)
* [Metadata](#metadata)
* [Iterating channels and roles](#iterating-channels-and-roles)
* [Members](#members) — nicknames, roles, voice, status.
* [Channels](#creating-channels) — create / delete inside a guild.
* [Roles](#roles) — create / edit / delete.
* [Moderation](#moderation) — kick, ban, unban.
* [Callbacks](#callbacks)
* [Reference](#reference)

## Lookup

```pawn
new DCC_Guild:by_name = DCC_FindGuildByName("MyServer");
new DCC_Guild:by_id   = DCC_FindGuildById  ("987654321098765432");

new DCC_Guild:all[16];
new total = DCC_GetAllGuilds(all);
```

`DCC_GetAllGuilds` fills your array with every guild the bot is in and returns the count actually written. Pass a buffer that is large enough for your bot's expected reach.

## Metadata

```pawn
new id[DCC_ID_SIZE], name[64], owner[DCC_ID_SIZE];
DCC_GetGuildId     (guild, id);
DCC_GetGuildName   (guild, name);
DCC_GetGuildOwnerId(guild, owner);
```

Rename the guild (requires *Manage Server*):

```pawn
DCC_SetGuildName(guild, "New Server Name");
```

## Iterating channels and roles

```pawn
new channel_count;
DCC_GetGuildChannelCount(guild, channel_count);
for (new i = 0; i < channel_count; i++)
{
    new DCC_Channel:ch;
    DCC_GetGuildChannel(guild, i, ch);
    new name[64];
    DCC_GetChannelName(ch, name);
    printf("channel #%d: %s", i, name);
}

new role_count;
DCC_GetGuildRoleCount(guild, role_count);
for (new i = 0; i < role_count; i++)
{
    new DCC_Role:r;
    DCC_GetGuildRole(guild, i, r);
}
```

## Members

### Iterating members

```pawn
new member_count;
DCC_GetGuildMemberCount(guild, member_count);
for (new i = 0; i < member_count; i++)
{
    new DCC_User:u;
    DCC_GetGuildMember(guild, i, u);

    new uname[DCC_USERNAME_SIZE];
    DCC_GetUserName(u, uname);
    printf("member #%d: %s", i, uname);
}
```

> `DCC_GetGuildMember*` only sees members that the bot has cached. Most bots see the full list only when the **Server Members Intent** is enabled.

### Nickname

```pawn
new nick[DCC_NICKNAME_SIZE];
DCC_GetGuildMemberNickname(guild, user, nick);

DCC_SetGuildMemberNickname(guild, user, "BobTheBuilder");
DCC_SetGuildMemberNickname(guild, user, "");           // reset
```

### Voice channel

```pawn
new DCC_Channel:vc;
DCC_GetGuildMemberVoiceChannel(guild, user, vc);
if (vc != DCC_INVALID_CHANNEL)
    DCC_SetGuildMemberVoiceChannel(guild, user, target_voice_channel);
```

`DCC_INVALID_CHANNEL` means the user is not currently in a voice channel.

### Roles on a member

```pawn
new count;
DCC_GetGuildMemberRoleCount(guild, user, count);
for (new i = 0; i < count; i++)
{
    new DCC_Role:r;
    DCC_GetGuildMemberRole(guild, user, i, r);
}

new bool:has_admin;
DCC_HasGuildMemberRole(guild, user, admin_role, has_admin);

DCC_AddGuildMemberRole   (guild, user, vip_role);
DCC_RemoveGuildMemberRole(guild, user, vip_role);
```

### Presence

```pawn
new DCC_UserPresenceStatus:status;
DCC_GetGuildMemberStatus(guild, user, status);
// INVALID, ONLINE, IDLE, DO_NOT_DISTURB, OFFLINE
```

Requires the **Presence Intent**.

## Creating channels

```pawn
DCC_CreateGuildChannel(guild, "events", GUILD_TEXT, "OnEventChannelCreated");

forward OnEventChannelCreated();
public  OnEventChannelCreated()
{
    new DCC_Channel:ch = DCC_GetCreatedGuildChannel();
    DCC_SendChannelMessage(ch, "Channel ready!");
}
```

Available channel types: `GUILD_TEXT`, `GUILD_VOICE`, `GUILD_CATEGORY`. See
[[Channels]] for the rest of the channel API.

## Roles

```pawn
DCC_CreateGuildRole(guild, "Beta Tester", "OnBetaRoleCreated");

forward OnBetaRoleCreated();
public  OnBetaRoleCreated()
{
    new DCC_Role:r = DCC_GetCreatedGuildRole();
    DCC_SetGuildRoleColor      (g_Guild, r, 0x00FF00);
    DCC_SetGuildRoleHoist      (g_Guild, r, true);
    DCC_SetGuildRoleMentionable(g_Guild, r, false);
}

// later...
DCC_DeleteGuildRole(guild, role);
```

Modifying an existing role:

```pawn
DCC_SetGuildRoleName       (guild, role, "VIP");
DCC_SetGuildRoleColor      (guild, role, 0xFF5733);
DCC_SetGuildRolePosition   (guild, role, 5);
DCC_SetGuildRolePermissions(guild, role, perm_high, perm_low);
DCC_SetGuildRoleHoist      (guild, role, true);
DCC_SetGuildRoleMentionable(guild, role, true);
```

## Moderation

```pawn
DCC_RemoveGuildMember     (guild, user);                 // kicks the user
DCC_CreateGuildMemberBan  (guild, user, "spamming");     // ban with reason
DCC_RemoveGuildMemberBan  (guild, user);                 // unban
```

The bot needs *Kick Members* / *Ban Members* on the guild.

## Callbacks

```pawn
forward DCC_OnGuildCreate(DCC_Guild:guild);
forward DCC_OnGuildUpdate(DCC_Guild:guild);
forward DCC_OnGuildDelete(DCC_Guild:guild);

forward DCC_OnGuildMemberAdd        (DCC_Guild:guild, DCC_User:user);
forward DCC_OnGuildMemberUpdate     (DCC_Guild:guild, DCC_User:user);
forward DCC_OnGuildMemberRemove     (DCC_Guild:guild, DCC_User:user);
forward DCC_OnGuildMemberVoiceUpdate(DCC_Guild:guild, DCC_User:user, DCC_Channel:channel);

forward DCC_OnGuildRoleCreate(DCC_Guild:guild, DCC_Role:role);
forward DCC_OnGuildRoleUpdate(DCC_Guild:guild, DCC_Role:role);
forward DCC_OnGuildRoleDelete(DCC_Guild:guild, DCC_Role:role);
```

`DCC_OnGuildMemberVoiceUpdate` reports `channel = DCC_INVALID_CHANNEL` when the user disconnects from voice entirely.

## Reference

```pawn
// lookup
DCC_Guild:DCC_FindGuildByName(const guild_name[]);
DCC_Guild:DCC_FindGuildById  (const guild_id[]);
DCC_GetAllGuilds(DCC_Guild:dest[], max_size = sizeof dest);

// metadata
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
