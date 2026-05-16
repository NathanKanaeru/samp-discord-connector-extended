# Roles

`DCC_Role:` represents a guild role. Roles are cached per-guild, and updates from the gateway keep them fresh. Use roles to gate features in your gamemode, decorate names, or grant permissions to staff.

## Lookup

```pawn
new DCC_Role:moderator = DCC_FindRoleByName(guild, "Moderator");
new DCC_Role:by_id     = DCC_FindRoleById  ("123456789012345678");
```

`DCC_FindRoleByName` requires a `DCC_Guild:` because role names are not unique
across servers. Both natives return `DCC_INVALID_ROLE` if no match.

## Reading role data

```pawn
new id[DCC_ID_SIZE], name[64];
new color, position, perm_high, perm_low;
new bool:hoist, bool:mentionable;

DCC_GetRoleId          (role, id);
DCC_GetRoleName        (role, name);
DCC_GetRoleColor       (role, color);            // alias: DCC_GetRoleColour
DCC_GetRolePosition    (role, position);
DCC_GetRolePermissions (role, perm_high, perm_low); // 64-bit permissions split into two cells
DCC_IsRoleHoist        (role, hoist);            // displayed separately in the member list
DCC_IsRoleMentionable  (role, mentionable);
```

The 64-bit Discord permissions bitmask is returned as two 32-bit halves. To check a single flag:

```pawn
const PERM_KICK_MEMBERS = 0x00000002;
DCC_GetRolePermissions(role, perm_high, perm_low);
if (perm_low & PERM_KICK_MEMBERS) { /* role can kick */ }
```

## Modifying a role

These all live under [[Guilds]] because they need a guild handle. Quick reference:

```pawn
DCC_SetGuildRoleName       (guild, role, "VIP");
DCC_SetGuildRoleColor      (guild, role, 0xFF5733);
DCC_SetGuildRolePosition   (guild, role, 4);
DCC_SetGuildRolePermissions(guild, role, perm_high, perm_low);
DCC_SetGuildRoleHoist      (guild, role, true);
DCC_SetGuildRoleMentionable(guild, role, false);
```

## Creating and deleting

```pawn
DCC_CreateGuildRole(guild, "Beta Tester", "OnRoleCreated");
forward OnRoleCreated();
public  OnRoleCreated()
{
    new DCC_Role:r = DCC_GetCreatedGuildRole();
    DCC_SetGuildRoleColor(g_PrimaryGuild, r, 0x00FF00);
}

DCC_DeleteGuildRole(guild, role);
```

## Granting / revoking on a member

```pawn
DCC_AddGuildMemberRole   (guild, user, role);
DCC_RemoveGuildMemberRole(guild, user, role);
```

## Role events

```pawn
forward DCC_OnGuildRoleCreate(DCC_Guild:guild, DCC_Role:role);
forward DCC_OnGuildRoleUpdate(DCC_Guild:guild, DCC_Role:role);
forward DCC_OnGuildRoleDelete(DCC_Guild:guild, DCC_Role:role);
```

## Reference

```pawn
DCC_Role:DCC_FindRoleByName(DCC_Guild:guild, const role_name[]);
DCC_Role:DCC_FindRoleById  (const role_id[]);

DCC_GetRoleId         (DCC_Role:role, dest[DCC_ID_SIZE], max_size = sizeof dest);
DCC_GetRoleName       (DCC_Role:role, dest[],            max_size = sizeof dest);
DCC_GetRoleColor      (DCC_Role:role, &color);
DCC_GetRoleColour     (DCC_Role:role, &colour) = DCC_GetRoleColor; // alias
DCC_GetRolePermissions(DCC_Role:role, &perm_high, &perm_low);
DCC_IsRoleHoist       (DCC_Role:role, &bool:is_hoist);
DCC_GetRolePosition   (DCC_Role:role, &position);
DCC_IsRoleMentionable (DCC_Role:role, &bool:is_mentionable);
```
