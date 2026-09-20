# Clipo

Clipo is a fast, lightweight, local-first clipboard manager for Linux and Windows.

It keeps clipboard history locally and provides a keyboard-first popup for quickly searching and pasting previous text.

## Features

- Fast native desktop application
- Local-only clipboard history
- Text clipboard support in v1.0
- Event-driven clipboard monitoring
- Search clipboard history
- Clear clipboard history
- Configurable global shortcut
- Light, dark, and system theme support
- Adjustable popup transparency
- Smart popup positioning
- Keyboard-first navigation
- SQLite-based local storage
- Linux/X11 paste support
- Debian package support
- Open source under the MIT License

## Current Release

**Version:** 1.0.0

The 1.0 release focuses on reliable text clipboard management.

File and image clipboard support is planned for v2.0.

## Screenshots

Screenshots will be added here as the UI is finalized.

## Installation

### Ubuntu / Debian

Download the latest `.deb` package from the GitHub Releases page.

Install it with:

```bash
sudo apt install ./clipo-1.0.0-Linux-amd64.deb
```

After installation, launch Clipo from the application menu or run:

```bash
clipo
```

### Building from Source

See [BUILDING.md](docs/BUILDING.md).

## Quick Start

1. Start Clipo.
2. Copy text normally.
3. Open Clipo using the configured global shortcut.
4. Search or navigate through clipboard history.
5. Select an entry to paste it into the previous application.
6. Use **Clear** when you want to remove clipboard history.

The default global shortcut is:

```text
Ctrl + Shift + V
```

The shortcut can be changed from Settings.

## Keyboard Controls

| Key | Action |
|---|---|
| Global shortcut | Show/hide Clipo |
| Up / Down | Navigate history |
| Enter | Paste selected item |
| Escape | Hide Clipo |
| Ctrl + F | Focus search |

## Configuration

Clipo provides settings for:

- Theme: System / Dark / Light
- Popup transparency
- Global shortcut
- Popup position

Settings are stored locally using Qt's settings system.

Clipboard history is stored locally in SQLite.

## Privacy

Clipo is designed as a local-first application.

Clipboard data is stored locally on the user's machine. Clipo does not require a backend server or cloud account for clipboard history.

Users should still treat clipboard contents as sensitive data because copied passwords, tokens, personal information, and other confidential text may be stored in clipboard history.

## Architecture

The main components are:

```text
Clipboard
    |
    v
ClipboardManager
    |
    v
Database (SQLite)
    |
    v
ClipboardHistoryView
    |
    v
PopupWindow
    |
    v
PasteController
```

Platform-specific functionality is separated from the main application logic.

See [ARCHITECTURE.md](docs/ARCHITECTURE.md) for details.

## Project Structure

```text
Clipo/
├── CMakeLists.txt
├── LICENSE
├── README.md
├── packaging/
│   └── clipo.desktop
├── resources/
│   ├── icons/
│   ├── styles/
│   └── resources.qrc
├── src/
│   ├── clipboard/
│   ├── database/
│   ├── platform/
│   ├── settings/
│   └── ui/
├── docs/
└── tests/
```

## Building

Requirements and build instructions are documented in:

[docs/BUILDING.md](docs/BUILDING.md)

Basic build:

```bash
cmake -S . -B build -G Ninja
cmake --build build
```

Run:

```bash
./build/clipo
```

## Packaging

Clipo can be packaged as a Debian package:

```bash
cpack --config build/CPackConfig.cmake
```

This produces:

```text
clipo-1.0.0-Linux-amd64.deb
```

The generated package should be treated as a release artifact and uploaded to GitHub Releases rather than committed to the source repository.

## Documentation

- [Installation](docs/INSTALLATION.md)
- [Building from Source](docs/BUILDING.md)
- [Usage](docs/USAGE.md)
- [Architecture](docs/ARCHITECTURE.md)
- [Roadmap](docs/ROADMAP.md)
- [Contributing](CONTRIBUTING.md)

## Roadmap

### v1.x

- Improve Linux desktop integration
- Improve keyboard navigation
- Improve UI polish
- Improve testing and reliability
- Improve Windows support

### v2.0

- Image clipboard support
- File clipboard support
- Rich clipboard previews
- Additional platform-specific integrations

See [ROADMAP.md](docs/ROADMAP.md) for the detailed roadmap.

## Contributing

Contributions are welcome.

Before contributing:

1. Read [CONTRIBUTING.md](CONTRIBUTING.md).
2. Build the project locally.
3. Keep changes focused.
4. Add or update documentation when behavior changes.
5. Test the affected functionality.

## License

Clipo is released under the MIT License.

See [LICENSE](LICENSE) for the complete license text.

## Author

**Tejeshwa Anand**

Clipo is developed as an open-source project focused on providing a fast, lightweight, and local-first clipboard experience.
