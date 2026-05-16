# Embeds

Embeds are the rich, structured message blocks Discord renders with a coloured stripe, title, body, fields, footer, and inline images. They are the right tool for status posts, scoreboards, server announcements, and slash-command responses.

* [Lifecycle](#lifecycle)
* [Creating an embed in one call](#creating-an-embed-in-one-call)
* [Building piece by piece](#building-piece-by-piece)
* [Sending an embed](#sending-an-embed)
* [Limits](#limits)
* [Reference](#reference)

## Lifecycle

`DCC_CreateEmbed` returns an `DCC_Embed:` handle. The handle is **owned by your script** until it is consumed by a `Send` call (`DCC_SendChannelEmbedMessage`, `DCC_SendChannelMessageEx`, `DCC_SendInteractionEmbed`, or `DCC_EditMessage`). Once consumed it is freed automatically — you do not call `DCC_DeleteEmbed` after sending.

If you build an embed but decide not to send it, free it manually:

```pawn
DCC_DeleteEmbed(e);
```

## Creating an embed in one call

```pawn
new DCC_Embed:e = DCC_CreateEmbed(
    "Server status",                   // title
    "All systems nominal.",            // description
    "https://example.com/status",      // url (clickable title)
    "",                                // ISO 8601 timestamp, "" for none
    0x57F287,                          // accent color (RGB)
    "Updated automatically",           // footer text
    "",                                // footer icon url
    "https://example.com/thumb.png",   // small thumbnail (top-right)
    "https://example.com/banner.png"); // large image (bottom)
```

All arguments after `description` are optional and default to empty / 0.

## Building piece by piece

You can also start empty and configure with setters and field calls:

```pawn
new DCC_Embed:e = DCC_CreateEmbed();

DCC_SetEmbedTitle      (e, "Welcome");
DCC_SetEmbedDescription(e, "Read the rules and check your role assignment.");
DCC_SetEmbedColor      (e, 0xFEE75C);     // also: DCC_SetEmbedColour
DCC_SetEmbedTimestamp  (e, "2026-05-16T20:00:00Z");
DCC_SetEmbedUrl        (e, "https://example.com");
DCC_SetEmbedFooter     (e, "Last updated by Bot", "https://example.com/icon.png");
DCC_SetEmbedThumbnail  (e, "https://example.com/thumb.png");
DCC_SetEmbedImage      (e, "https://example.com/banner.png");

// Add fields - up to 25.
DCC_AddEmbedField(e, "Players online", "42", true);   // inline = side-by-side
DCC_AddEmbedField(e, "Slots free",     "158", true);
DCC_AddEmbedField(e, "Notes",          "RP weekend", false);
```

## Sending an embed

```pawn
// Plain embed in a channel
DCC_SendChannelEmbedMessage(channel, e);

// Embed with extra text in a channel
DCC_SendChannelEmbedMessage(channel, e, "@here new round!");

// Embed AND interactive components in a channel
new DCC_ActionRow:rows[1] = { row };
DCC_SendChannelMessageEx(channel, "Pick an option:", e, rows, 1);

// Embed as a slash-command reply
DCC_SendInteractionEmbed(interaction, e);

// Edit a message to attach a new embed
DCC_EditMessage(persistent_message, "see embed", e);
```

## Limits

Discord enforces these and will return `400 Bad Request` if you exceed them:

* Title — 256 chars.
* Description — 4096 chars.
* Up to 25 fields.
* Field name — 256 chars.
* Field value — 1024 chars.
* Footer text — 2048 chars.
* Author name — 256 chars.
* Total length across all text — 6000 chars.

The connector does not currently validate embed lengths locally, so over-long embeds will surface as a `WARNING` log line containing Discord's rejection message.

## Reference

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
DCC_SetEmbedColour    (DCC_Embed:embed, colour) = DCC_SetEmbedColor; // alias
DCC_SetEmbedFooter    (DCC_Embed:embed, const footer_text[], const footer_icon_url[]="");
DCC_SetEmbedThumbnail (DCC_Embed:embed, const thumbnail_url[]);
DCC_SetEmbedImage     (DCC_Embed:embed, const image_url[]);
```
