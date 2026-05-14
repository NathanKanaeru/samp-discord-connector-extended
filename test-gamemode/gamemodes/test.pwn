#include <a_samp>
#include <discord-connector>

// GANTI DENGAN ID CHANNEL ANDA
#define TEST_CHANNEL_ID "123456789012345678"

new DCC_Component:g_TestButton;
new DCC_Component:g_TestMenu;
new DCC_Modal:g_TestModal;

main()
{
    print("SA:MP Discord Connector Extended Test Gamemode");
}

public OnGameModeInit()
{
    print("Gamemode Init: Kirim '!test' di Discord untuk mencoba fitur baru.");
    
    // 1. Persiapkan Modal
    g_TestModal = DCC_CreateModal("Input Data", "modal_test");
    DCC_AddModalInput(g_TestModal, "input_nama", DCC_TEXT_STYLE_SHORT, "Nama Anda", 3, 20, true, "", "Masukkan nama...");
    DCC_AddModalInput(g_TestModal, "input_bio", DCC_TEXT_STYLE_PARAGRAPH, "Biografi", 10, 100, false, "", "Ceritakan tentang diri Anda...");
    
    // 2. Persiapkan Button
    g_TestButton = DCC_CreateButton("Klik Saya!", DCC_STYLE_SUCCESS, "btn_test");
    
    // 3. Persiapkan Select Menu
    g_TestMenu = DCC_CreateSelectMenu(DCC_COMPONENT_STRING_SELECT, "menu_test", "Pilih salah satu...");
    DCC_AddSelectMenuOption(g_TestMenu, "Opsi Pertama", "opt1", "Ini adalah opsi 1");
    DCC_AddSelectMenuOption(g_TestMenu, "Opsi Kedua", "opt2", "Ini adalah opsi 2");
    
    return 1;
}

public DCC_OnMessageCreate(DCC_Message:message)
{
    new content[128];
    DCC_GetMessageContent(message, content);
    
    if (!strcmp(content, "!test"))
    {
        new DCC_Channel:channel;
        DCC_GetMessageChannel(message, channel);
        
        new channel_id[DCC_ID_SIZE];
        DCC_GetChannelId(channel, channel_id);
        
        if (!strcmp(channel_id, TEST_CHANNEL_ID))
        {
            // Put components into Action Rows
            new DCC_ActionRow:row1 = DCC_CreateActionRow();
            DCC_AddComponentToActionRow(row1, g_TestButton);
            
            new DCC_ActionRow:row2 = DCC_CreateActionRow();
            DCC_AddComponentToActionRow(row2, g_TestMenu);
            
            new DCC_ActionRow:rows[2];
            rows[0] = row1;
            rows[1] = row2;
            
            // Send Message with all components
            DCC_SendChannelMessageEx(channel, "Halo! Ini adalah pesan test untuk fitur **Buttons** dan **Select Menus**.", DCC_INVALID_EMBED, rows, 2);
            print("Test command received: Components sent to Discord!");
        }
    }
    return 1;
}

public DCC_OnInteraction(DCC_Interaction:interaction, DCC_User:user, DCC_InteractionType:type, const custom_id[])
{
    new username[DCC_USERNAME_SIZE];
    DCC_GetUserName(user, username);
    printf("Interaction Event: User=%s, Type=%d, ID=%s", username, _:type, custom_id);
    
    if (type == DCC_INTERACTION_COMPONENT)
    {
        if (!strcmp(custom_id, "btn_test"))
        {
            DCC_SendInteractionMessage(interaction, "Terima kasih telah mengklik tombol! Sekarang silakan isi modal berikut.");
            DCC_SendInteractionModal(interaction, g_TestModal);
        }
        else if (!strcmp(custom_id, "menu_test"))
        {
            new count;
            DCC_GetInteractionMenuCount(interaction, count);
            
            new value[128];
            DCC_GetInteractionMenuValue(interaction, 0, value);
            
            new msg[200];
            format(msg, sizeof(msg), "Anda memilih **%s** dari menu (Total: %d opsi dipilih).", value, count);
            DCC_SendInteractionMessage(interaction, msg);
        }
    }
    else if (type == DCC_INTERACTION_MODAL_SUBMIT)
    {
        if (!strcmp(custom_id, "modal_test"))
        {
            new name[21], bio[101];
            DCC_GetModalInputValue(interaction, "input_nama", name);
            DCC_GetModalInputValue(interaction, "input_bio", bio);
            
            new msg[256];
            format(msg, sizeof(msg), "Data diterima!\nNama: %s\nBio: %s", name, bio);
            DCC_SendInteractionMessage(interaction, msg);
        }
    }
    return 1;
}
