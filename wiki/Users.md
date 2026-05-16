# Users

`DCC_User:` is the handle to a Discord user. The plugin caches every user it sees on guilds the bot is part of, plus DMs, plus interaction targets. You will receive `DCC_User:` handles via callbacks and several natives.

## Lookup

```pawn
new DCC_User:by_id   = DCC_FindUserById  ("123456789012345678");
new DCC_User:by_name = DCC_FindUserByName("Nathan", "0001");   // legacy discriminator
```

* `DCC_FindUserById` is the canonical way — IDs are stable.
* `DCC_FindUserByName` matches `username#discriminator`. Discord migrated most accounts to the new "unique username" system, so the discriminator is often `"0"` for them. Pass an empty string `""` to ignore the discriminator and match the first user with that username.

Both natives return `DCC_INVALID_USER` (`DCC_User:0`) if no match.

## Reading user data

```pawn
new id[DCC_ID_SIZE], name[DCC_USERNAME_SIZE], discriminator[8];
new bool:is_bot, bool:is_verified;

DCC_GetUserId         (user, id);
DCC_GetUserName       (user, name);
DCC_GetUserDiscriminator(user, discriminator);
DCC_IsUserBot         (user, is_bot);
DCC_IsUserVerified    (user, is_verified);
```

Always check `is_bot` first when reacting to messages — otherwise your bot will reply to itself, and to other bots, in an infinite loop.

## Per-guild user data

Information that depends on the guild (nickname, roles, voice channel, status) is exposed via the [[Guilds]] API:

* `DCC_GetGuildMemberNickname(guild, user, dest)`
* `DCC_GetGuildMemberRole / RoleCount / HasGuildMemberRole`
* `DCC_GetGuildMemberVoiceChannel`
* `DCC_GetGuildMemberStatus` (requires the **Presence Intent**)

## User events

```pawn
forward DCC_OnUserUpdate(DCC_User:user);
```

Fires whenever a cached user's username, discriminator, or avatar changes.

## Reference

```pawn
DCC_User:DCC_FindUserByName(const user_name[], const user_discriminator[]);
DCC_User:DCC_FindUserById  (const user_id[]);

DCC_GetUserName        (DCC_User:user, dest[DCC_USERNAME_SIZE], max_size = sizeof dest);
DCC_GetUserId          (DCC_User:user, dest[DCC_ID_SIZE],       max_size = DCC_ID_SIZE);
DCC_GetUserDiscriminator(DCC_User:user, dest[],                  max_size = sizeof dest);
DCC_IsUserBot          (DCC_User:user, &bool:is_bot);
DCC_IsUserVerified     (DCC_User:user, &bool:is_verified);
```
