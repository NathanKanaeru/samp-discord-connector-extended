# Slash Commands

Slash Commands are the modern way users interact with bots. They provide a native UI for commands with parameter validation and help text.

## Registering Commands
You can register commands globally or per-guild. 

* **Global Commands**: Available in all guilds. May take up to 1 hour to propagate.
* **Guild Commands**: Available only in a specific guild. Update instantly.

### Example: Registering a Guild Command
```pawn
new DCC_Guild:myGuild = DCC_FindGuildById("123456789...");

// DCC_CreateCommand(name[], description[], guild = DCC_INVALID_GUILD);
DCC_CreateCommand("kick", "Kick a player from the game", myGuild);

// DCC_AddCommandOption(option_name[], description[], type, required = false);
DCC_AddCommandOption("playerid", "The ID of the player to kick", DCC_COMMAND_OPTION_INTEGER, true);
DCC_AddCommandOption("reason", "The reason for the kick", DCC_COMMAND_OPTION_STRING, false);
```

## Handling Commands
Commands trigger the `DCC_OnInteraction` callback with `type == DCC_INTERACTION_COMMAND`.

```pawn
public DCC_OnInteraction(DCC_Interaction:interaction, DCC_User:user, DCC_InteractionType:type, const custom_id[])
{
    if (type == DCC_INTERACTION_COMMAND && !strcmp(custom_id, "kick"))
    {
        new targetid, reason[128];
        
        // Get integer option
        DCC_GetInteractionOption(interaction, "playerid", targetid);
        
        // Get string option (returns 0 if not provided)
        if (!DCC_GetInteractionOption(interaction, "reason", reason))
        {
            format(reason, sizeof(reason), "No reason specified");
        }
        
        // Logic to kick player in SA:MP
        Kick(targetid);
        
        new response[200];
        format(response, sizeof(response), "Player %d has been kicked for: %s", targetid, reason);
        DCC_SendInteractionMessage(interaction, response);
    }
    return 1;
}
```

## Supported Option Types
| Constant | Description |
|---|---|
| `DCC_COMMAND_OPTION_SUB_COMMAND` | Creates a sub-command. |
| `DCC_COMMAND_OPTION_STRING` | Text input. |
| `DCC_COMMAND_OPTION_INTEGER` | Number input (integer). |
| `DCC_COMMAND_OPTION_BOOLEAN` | True/False toggle. |
| `DCC_COMMAND_OPTION_USER` | User picker. |
| `DCC_COMMAND_OPTION_CHANNEL` | Channel picker. |
| `DCC_COMMAND_OPTION_ROLE` | Role picker. |
| `DCC_COMMAND_OPTION_MENTIONABLE` | User or Role picker. |
| `DCC_COMMAND_OPTION_NUMBER` | Number input (float). |
