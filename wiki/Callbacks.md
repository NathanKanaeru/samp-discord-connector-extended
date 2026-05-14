# Callbacks Reference

The plugin triggers these callbacks in your PAWN script. Ensure they are defined as `public` to be detected by the plugin.

## Messaging & Channels

### DCC_OnMessageCreate
Triggered when a new message is sent in a visible channel.
```pawn
public DCC_OnMessageCreate(DCC_Message:message)
```

### DCC_OnMessageDelete
Triggered when a message is deleted.
```pawn
public DCC_OnMessageDelete(DCC_Message:message)
```

---

## Modern Interactions

### DCC_OnInteraction
The primary callback for Buttons, Select Menus, Modals, and Slash Commands.
```pawn
public DCC_OnInteraction(DCC_Interaction:interaction, DCC_User:user, DCC_InteractionType:type, const custom_id[])
```
* `interaction`: Handle to the interaction object.
* `user`: The Discord user who triggered it.
* `type`: The type of interaction (Command, Component, or Modal).
* `custom_id`: The unique ID assigned when creating the element.

---

## Guild & Members

### DCC_OnGuildMemberAdd
Triggered when a user joins a guild.
```pawn
public DCC_OnGuildMemberAdd(DCC_Guild:guild, DCC_User:user)
```

### DCC_OnGuildMemberRemove
Triggered when a user leaves (or is kicked/banned from) a guild.
```pawn
public DCC_OnGuildMemberRemove(DCC_Guild:guild, DCC_User:user)
```

### DCC_OnGuildMemberUpdate
Triggered when a member's nickname, roles, or status changes.
```pawn
public DCC_OnGuildMemberUpdate(DCC_Guild:guild, DCC_User:user)
```

---

## Infrastructure

### DCC_OnBotCapabilityError
Triggered if the bot lacks permissions or encounters a gateway error.
```pawn
public DCC_OnBotCapabilityError(DCC_BotCapability:capability, DCC_BotCapabilityError:error)
```
