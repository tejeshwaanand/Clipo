# Clipo Architecture

Clipo is designed as a modular native desktop application.

The application is written in C++20 and uses Qt 6 for the desktop
application framework and user interface.

SQLite is used for persistent clipboard history.

The architecture separates clipboard handling, persistence, UI,
settings and platform-specific functionality so that platform-specific
implementations can be replaced or extended independently.

## High-Level Architecture

```text
                    ┌─────────────────────┐
                    │        User         │
                    └──────────┬──────────┘
                               │
                               │ Global Shortcut
                               ▼
                    ┌─────────────────────┐
                    │   GlobalShortcut    │
                    └──────────┬──────────┘
                               │
                               ▼
                    ┌─────────────────────┐
                    │    PopupWindow      │
                    │                     │
                    │ Search / History    │
                    │ Settings / Controls │
                    └──────────┬──────────┘
                               │
                ┌──────────────┼──────────────┐
                │              │              │
                ▼              ▼              ▼
        ┌─────────────┐ ┌─────────────┐ ┌─────────────┐
        │ Clipboard   │ │  Database   │ │  Settings   │
        │ Manager     │ │   SQLite    │ │   Manager    │
        └──────┬──────┘ └─────────────┘ └─────────────┘
               │
               ▼
        System Clipboard
```

## Project Structure

The main source tree is:

```text
Clipo/
├── src/
│   ├── main.cpp
│   │
│   ├── clipboard/
│   │   ├── ClipboardManager.h
│   │   └── ClipboardManager.cpp
│   │
│   ├── database/
│   │   ├── Database.h
│   │   └── Database.cpp
│   │
│   ├── platform/
│   │   ├── PasteController.h
│   │   ├── PasteController.cpp
│   │   ├── GlobalShortcut.h
│   │   ├── GlobalShortcut.cpp
│   │   └── linux/
│   │       ├── X11PasteController.cpp
│   │       └── X11GlobalShortcut.cpp
│   │
│   ├── settings/
│   │   ├── SettingsManager.h
│   │   ├── SettingsManager.cpp
│   │   ├── SettingsDialog.h
│   │   └── SettingsDialog.cpp
│   │
│   └── ui/
│       ├── PopupWindow.h
│       ├── PopupWindow.cpp
│       ├── ClipboardHistoryView.h
│       └── ClipboardHistoryView.cpp
│
├── resources/
│   ├── resources.qrc
│   ├── icons/
│   │   └── clipo.png
│   └── styles/
│       ├── dark.qss
│       └── light.qss
│
├── packaging/
│   └── clipo.desktop
│
├── docs/
├── tests/
├── cmake/
├── CMakeLists.txt
└── LICENSE
```

## Main Application

File:

```text
src/main.cpp
```

The main application initializes and connects the major Clipo modules.

The main components are:

- Qt application
- Database
- Settings manager
- Clipboard manager
- Popup window
- Global shortcut

Signals and slots are used to connect the components.

## Clipboard Manager

Files:

```text
src/clipboard/ClipboardManager.h
src/clipboard/ClipboardManager.cpp
```

`ClipboardManager` monitors the system clipboard using Qt's clipboard
change notification.

The application uses:

```text
QClipboard::dataChanged
```

instead of continuously polling the clipboard.

When new text is detected, `ClipboardManager` emits:

```text
clipboardChanged
```

The application then:

1. Stores the clipboard content in SQLite.
2. Notifies the visible history view so the UI can refresh.

This provides live clipboard updates without repeatedly checking the
clipboard on a timer.

## Database

Files:

```text
src/database/Database.h
src/database/Database.cpp
```

Clipo uses SQLite for persistent clipboard history.

The current schema is:

```sql
CREATE TABLE IF NOT EXISTS clipboard_history (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    content TEXT NOT NULL UNIQUE,
    timestamp INTEGER NOT NULL
);
```

The database layer provides operations for:

- Database initialization
- Adding clipboard entries
- Reading clipboard history
- Deleting individual entries
- Clearing clipboard history

### Duplicate Entries

Clipboard content is unique.

When the same content is copied again, SQLite updates its timestamp
instead of creating another duplicate row.

The current insertion logic uses an SQLite conflict handler:

```sql
ON CONFLICT(content)
DO UPDATE SET timestamp = excluded.timestamp;
```

## User Interface

Files:

```text
src/ui/
```

The main popup is implemented by:

```text
PopupWindow
```

The popup provides:

- Clipboard history
- Search
- Clear history
- Keyboard navigation
- Settings access
- Popup positioning
- Theme handling
- Transparency handling

The clipboard history presentation is implemented separately by:

```text
ClipboardHistoryView
```

This keeps the history list separate from the main popup window.

## Settings

Files:

```text
src/settings/
```

`SettingsManager` stores user preferences using Qt's:

```text
QSettings
```

Current settings include:

- Theme
- Transparency
- Global shortcut
- Popup position

The settings UI is implemented by:

```text
SettingsDialog
```

## Platform Layer

Files:

```text
src/platform/
```

Platform-specific functionality is separated from the main application
through abstraction classes.

Current abstractions include:

```text
PasteController
GlobalShortcut
```

The current Linux implementation uses X11.

### X11 Paste

File:

```text
src/platform/linux/X11PasteController.cpp
```

The X11 paste implementation:

1. Remembers the previously active window.
2. Activates the previous window when required.
3. Sends `Ctrl+V`.
4. Clears the remembered window.

This allows a selected clipboard entry to be pasted into the application
that was active before Clipo was opened.

### X11 Global Shortcut

File:

```text
src/platform/linux/X11GlobalShortcut.cpp
```

The X11 global shortcut implementation registers and monitors the
configured global keyboard shortcut.

The default shortcut is:

```text
Ctrl + Shift + V
```

The shortcut can be changed from Settings.

## Signal Flow

A clipboard update follows this general flow:

```text
System Clipboard
       │
       ▼
QClipboard::dataChanged
       │
       ▼
ClipboardManager
       │
       ▼
clipboardChanged()
       │
       ├──────────────► Database::addEntry()
       │
       └──────────────► PopupWindow::refreshHistory()
```

This allows an already-open popup to update immediately when new
clipboard content is copied.

## Paste Flow

```text
User selects item
       │
       ▼
Enter key
       │
       ▼
PopupWindow
       │
       ▼
PasteController
       │
       ▼
X11PasteController
       │
       ▼
Previously active application
       │
       ▼
Ctrl + V
```

## Search Flow

```text
User types search text
       │
       ▼
Search Box
       │
       ▼
ClipboardHistoryView
       │
       ▼
Stored clipboard entries
       │
       ▼
Matching history items
```

## Settings Flow

```text
User opens Settings
       │
       ▼
SettingsDialog
       │
       ▼
SettingsManager
       │
       ▼
QSettings
       │
       ▼
Local user configuration
```

## Resource System

Clipo uses Qt resources through:

```text
resources/resources.qrc
```

The resource system contains application assets such as:

```text
resources/
├── icons/
│   └── clipo.png
└── styles/
    ├── dark.qss
    └── light.qss
```

The QSS files provide the application's light and dark visual styles.

## Packaging

The desktop launcher is located at:

```text
packaging/clipo.desktop
```

CMake installs:

```text
/usr/bin/clipo
/usr/share/applications/clipo.desktop
/usr/share/icons/hicolor/512x512/apps/clipo.png
```

CPack is used to generate the Debian package.

## Data Flow

When text is copied:

```text
Copied Text
    │
    ▼
ClipboardManager
    │
    ▼
Database
    │
    ▼
SQLite
```

When the popup is opened:

```text
Global Shortcut
    │
    ▼
PopupWindow
    │
    ▼
Database
    │
    ▼
ClipboardHistoryView
```

When an item is pasted:

```text
Selected Clipboard Item
    │
    ▼
PasteController
    │
    ▼
X11PasteController
    │
    ▼
Previously Active Window
```

## Event-Driven Design

Clipo uses an event-driven architecture.

Clipboard changes are received through Qt signals instead of a periodic
polling loop.

This avoids unnecessary background work while the clipboard remains
unchanged.

The design also makes the clipboard monitoring component easier to
extend for additional clipboard formats in future versions.

## Privacy and Security

Clipo is local-first.

Clipboard history is stored locally using SQLite.

The application does not require a remote backend for normal clipboard
operation.

Because clipboard contents can contain sensitive information, the local
Clipo database should be treated as sensitive user data.

## Version 1.0 Scope

The current architecture is focused on text clipboard management.

Implemented:

- Text clipboard history
- SQLite persistence
- Search
- Clear history
- Global shortcut
- Keyboard navigation
- X11 paste
- Light/dark/system theme
- Transparency
- Popup positioning
- Settings
- Debian packaging

Planned for future releases:

- Image clipboard support
- File clipboard support
- Image thumbnails
- File previews
- Rich clipboard formats
- Additional platform backends
- Native Wayland support
- Windows support
