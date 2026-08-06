#include <a_samp>
#include <discord-connector>

main()
{
    print("Discord Connector Extended - Feature Test Gamemode");
}

public OnGameModeInit()
{
    SetGameModeText("DCC Test GM");

    new cmd_name[33] = "test";
    new cmd_desc[33] = "Test all DCC extended features";
    DCC_CreateCommand(cmd_name, cmd_desc, "OnTestCommand");
    DCC_AddCommandOption("ping", "Send a ping embed", DCC_OPTION_BOOLEAN, false);
    
    printf("[DCC-Test] Gamemode started! Make sure to set DCC_BOT_TOKEN environment variable.");
    return 1;
}

forward OnTestCommand(DCC_Interaction:interaction, DCC_User:user);
public OnTestCommand(DCC_Interaction:interaction, DCC_User:user)
{
    new bool:ping_opt = false;
    new opt_count;
    DCC_GetInteractionOptionCount(interaction, opt_count);
    for (new i = 0; i < opt_count; ++i)
    {
        new path[32], value[32], DCC_CommandOptionType:type;
        DCC_GetInteractionOption(interaction, i, path, value, type);
        if (!strcmp(path, "ping"))
        {
            ping_opt = (!strcmp(value, "true", true) || !strcmp(value, "1", true));
        }
    }

    if (ping_opt)
    {
        new DCC_Embed:embed = DCC_CreateEmbed("Pong!", "This is a test embed from slash command.", "https://example.com", "", 0x57F287);
        DCC_AddEmbedField(embed, "Status", "Working", true);
        DCC_SetEmbedFooter(embed, "DCC Extended Test");
        
        DCC_SendInteractionEmbed(interaction, embed);
        return 1;
    }

    new DCC_Component:btn_primary = DCC_CreateButton("Primary Button", DCC_STYLE_PRIMARY, "btn_1");
    new DCC_Component:btn_danger = DCC_CreateButton("Danger Button", DCC_STYLE_DANGER, "btn_2");
    new DCC_Component:btn_link = DCC_CreateButton("Link Button", DCC_STYLE_LINK, "", "https://github.com/NathanKanaeru/samp-discord-connector-extended");

    new DCC_ActionRow:row1 = DCC_CreateActionRow();
    DCC_AddComponentToActionRow(row1, btn_primary);
    DCC_AddComponentToActionRow(row1, btn_danger);
    DCC_AddComponentToActionRow(row1, btn_link);

    new DCC_Component:select_menu = DCC_CreateSelectMenu(DCC_COMPONENT_STRING_SELECT, "sel_1", "Choose an option...", 1, 1);
    DCC_AddSelectMenuOption(select_menu, "Option A", "opt_a", "Description for A");
    DCC_AddSelectMenuOption(select_menu, "Option B", "opt_b", "Description for B");

    new DCC_ActionRow:row2 = DCC_CreateActionRow();
    DCC_AddComponentToActionRow(row2, select_menu);

    new DCC_ActionRow:rows[2];
    rows[0] = row1;
    rows[1] = row2;
    
    new DCC_Embed:embed = DCC_CreateEmbed("Interactive Menu", "Click a button or select an option to test Modals.", "", "", 0x5865F2);
    
    new DCC_Channel:ch;
    DCC_GetInteractionChannel(interaction, ch);
    DCC_SendChannelMessageEx(ch, "", embed, rows, 2);
    
    DCC_SendInteractionMessage(interaction, "I've sent an interactive menu to this channel!");
    return 1;
}

public DCC_OnInteraction(DCC_Interaction:interaction, DCC_User:user, DCC_InteractionType:type, const custom_id[])
{
    switch (type)
    {
        case DCC_INTERACTION_COMPONENT:
        {
            if (!strcmp(custom_id, "btn_1") || !strcmp(custom_id, "sel_1"))
            {
                new DCC_Modal:modal = DCC_CreateModal("Feature Test Modal", "test_modal");
                DCC_AddModalInput(modal, "text_val", DCC_TEXT_STYLE_SHORT, "Enter some text:", 1, 50, true, "", "Type here...");
                
                new DCC_Component:rank = DCC_CreateSelectMenu(DCC_COMPONENT_STRING_SELECT, "rank_pref", "Pick a rank", 1, 1);
                DCC_AddSelectMenuOption(rank, "Option 1", "1");
                DCC_AddSelectMenuOption(rank, "Option 2", "2");
                DCC_AddModalComponent(modal, rank, "Select something");

                new DCC_Component:file = DCC_CreateFileUpload("file_upload", 1, 1, false);
                DCC_AddModalComponent(modal, file, "Upload a file (Optional)");

                DCC_SendInteractionModal(interaction, modal);
            }
            else if (!strcmp(custom_id, "btn_2"))
            {
                DCC_SendInteractionMessage(interaction, "Danger button clicked!");
            }
        }

        case DCC_INTERACTION_MODAL_SUBMIT:
        {
            if (!strcmp(custom_id, "test_modal"))
            {
                new text_val[51];
                DCC_GetModalInputValue(interaction, "text_val", text_val);

                new rank_count, rank_val[16];
                DCC_GetModalSelectCount(interaction, "rank_pref", rank_count);
                if (rank_count > 0)
                    DCC_GetModalSelectValue(interaction, "rank_pref", 0, rank_val);

                new file_count, file_url[256];
                DCC_GetModalAttachmentCount(interaction, "file_upload", file_count);
                if (file_count > 0)
                    DCC_GetModalAttachmentUrl(interaction, "file_upload", 0, file_url);

                new response[512];
                format(response, sizeof(response), "Modal received!\nText: %s\nSelect: %s\nFile URL: %s", text_val, rank_val, (file_count > 0) ? file_url : "None");
                
                DCC_SendInteractionMessage(interaction, response);
            }
        }
    }
    return 1;
}

forward DCC_OnChannelMessage(DCC_Message:message);
public DCC_OnChannelMessage(DCC_Message:message)
{
    new DCC_User:author;
    DCC_GetMessageAuthor(message, author);

    new bool:is_bot;
    DCC_IsUserBot(author, is_bot);
    if (is_bot)
        return 1;

    new content[256];
    DCC_GetMessageContent(message, content);

    if (!strcmp(content, "!test-ping"))
    {
        new DCC_Channel:ch;
        DCC_GetMessageChannel(message, ch);
        DCC_SendChannelMessage(ch, "Pong! The connector is listening to standard chat messages as well.");
    }
    return 1;
}
