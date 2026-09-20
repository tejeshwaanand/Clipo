# Using Clipo

Clipo is designed to provide quick access to clipboard history without
requiring a cloud service or user account.

Clipo runs locally and stores clipboard history using SQLite.

## Starting Clipo

Launch Clipo from the application menu or terminal:

```bash
clipo
```

Once running, Clipo remains available in the background so that it can
monitor clipboard changes and respond to the global shortcut.

## Opening Clipo

The default global shortcut is:

```text
Ctrl + Shift + V
```

Press the shortcut to open the Clipo popup.

The global shortcut can be changed from Settings.

## Clipboard History

When text is copied, Clipo automatically detects the clipboard change
and stores the text in its local SQLite database.

Clipboard monitoring is event-driven using the Qt clipboard change
notification mechanism.

Clipo does not continuously poll the clipboard.

## Selecting an Item

Use the mouse to select an item from clipboard history.

The selected item can be expanded to view more of its content.

## Searching Clipboard History

Use the search box at the top of the popup.

Start typing to filter the clipboard history.

Search is performed against stored text entries.

To remove the current search filter, clear the search field.

## Keyboard Navigation

Clipo supports keyboard-based navigation.

| Key | Action |
|-----|--------|
| `Ctrl + Shift + V` | Open Clipo |
| `↑` | Select previous clipboard item |
| `↓` | Select next clipboard item |
| `Enter` | Paste selected item |
| `Esc` | Close Clipo |

The global shortcut can be customized from Settings.

## Pasting an Item

Select a clipboard item and press:

```text
Enter
```

Clipo restores the selected clipboard content and requests a paste
operation.

On the current Linux/X11 implementation, Clipo sends:

```text
Ctrl + V
```

to the previously active application.

The application that was active before opening Clipo therefore receives
the pasted clipboard content.

## Clearing Clipboard History

Use the:

```text
Clear
```

button beside the search field.

Clipo asks for confirmation before deleting the clipboard history.

After confirmation, the stored clipboard entries are removed from the
local database.

## Settings

Open the Settings panel to configure Clipo.

Current settings include:

- Theme
- Transparency
- Global shortcut
- Popup position

### Theme

Available options:

- System
- Dark
- Light

When `System` is selected, Clipo follows the configured application
theme where supported by the current implementation.

### Transparency

The popup opacity can be adjusted from Settings.

The default opacity is:

```text
88%
```

The supported range is:

```text
75% - 100%
```

### Global Shortcut

The keyboard shortcut used to open Clipo can be customized.

The default shortcut is:

```text
Ctrl + Shift + V
```

### Popup Position

Clipo supports configurable popup positioning.

The default position is:

```text
Near Cursor
```

The popup can also be moved manually by dragging its header.

## Closing the Popup

Press:

```text
Esc
```

to close the Clipo popup.

Closing the popup hides the interface while Clipo continues running in
the background.

This allows clipboard monitoring and the global shortcut to remain
available.

## Exiting Clipo Completely

Closing the popup does not terminate the Clipo process.

To terminate Clipo completely, stop the running process using your
desktop environment or terminal.

For example:

```bash
pkill clipo
```

Use this command only when you want to completely stop Clipo.

## Privacy

Clipo is local-first.

Clipboard history is stored locally using SQLite.

Clipo does not require:

- An online account
- A cloud service
- A remote clipboard server
- Network access for normal clipboard operation

Clipboard contents remain on the local machine during normal operation.

Because clipboard contents may contain sensitive information, users
should treat the local Clipo database as sensitive user data.

## Current Limitations

Clipo v1.0.0 focuses on text clipboard content.

The following are not part of v1.0.0:

- Image clipboard history
- Image thumbnails
- Image previews
- File clipboard history
- File previews
- Multiple-file clipboard entries
- Rich HTML clipboard management
- Native Wayland global shortcut support
- Native Wayland paste integration
- Windows support

These capabilities are planned for future releases.

## Recommended Workflow

A typical Clipo workflow is:

```text
Copy text
   ↓
Clipo automatically stores it
   ↓
Press Ctrl + Shift + V
   ↓
Search or navigate history
   ↓
Select an item
   ↓
Press Enter
   ↓
Text is pasted into the previous application
```
