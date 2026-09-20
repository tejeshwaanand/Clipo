# Installing Clipo

Clipo is a fast, lightweight and local-first clipboard manager for Linux.

Clipo stores clipboard history locally using SQLite and does not require a
cloud account or remote backend.

## Requirements

Clipo v1.0.0 currently targets:

- Ubuntu/Debian-based Linux
- 64-bit `amd64` systems
- X11 desktop sessions

Clipo requires the following runtime libraries:

- Qt 6 Core
- Qt 6 GUI
- Qt 6 Widgets
- SQLite 3
- X11
- XTest

## Install from `.deb`

The recommended way to install Clipo on Ubuntu/Debian is using the
provided Debian package.

Download the latest `.deb` package from the GitHub Releases page.

For Clipo v1.0.0:

```bash
sudo apt install ./clipo-1.0.0-Linux-amd64.deb
```

Using `apt` allows Ubuntu/Debian to resolve the package's runtime
dependencies.

## Start Clipo

After installation, start Clipo with:

```bash
clipo
```

You can also start Clipo from the desktop application's menu.

## Verify Installation

Check whether the Clipo executable is installed:

```bash
which clipo
```

Expected output will normally be similar to:

```text
/usr/bin/clipo
```

You can also check the installed package:

```bash
dpkg -s clipo
```

## Desktop Application

The Debian package installs the desktop launcher at:

```text
/usr/share/applications/clipo.desktop
```

The application icon is installed at:

```text
/usr/share/icons/hicolor/512x512/apps/clipo.png
```

After installation, Clipo should appear in the desktop application menu.

## Global Shortcut

The default global shortcut is:

```text
Ctrl + Shift + V
```

The shortcut can be changed from Clipo Settings.

## X11 Requirement

Clipo v1.0.0 uses X11-specific functionality for:

- Global keyboard shortcuts
- Activating the previously focused application
- Automated paste operations

Check your current session with:

```bash
echo $XDG_SESSION_TYPE
```

The current Linux implementation expects:

```text
x11
```

Native Wayland support is planned for a future release.

## Clipboard Database

Clipo stores clipboard history locally using SQLite.

The database is created in the user's local application data directory.

No remote database or cloud service is required.

Clipboard history therefore remains on the local machine during normal
operation.

## Uninstall

To remove Clipo:

```bash
sudo apt remove clipo
```

To remove Clipo and its package configuration:

```bash
sudo apt purge clipo
```

## Troubleshooting

### Clipo does not start

Start Clipo from a terminal:

```bash
clipo
```

Check the terminal output for errors.

### Global shortcut does not work

Open Clipo Settings and verify the configured shortcut.

The default shortcut is:

```text
Ctrl + Shift + V
```

Make sure another application is not already using the same shortcut.

### Paste does not work

Clipo v1.0.0 uses X11 automation to send `Ctrl+V` to the previously
active application.

Make sure you are running an X11 session:

```bash
echo $XDG_SESSION_TYPE
```

### Package dependencies are missing

Update the package index:

```bash
sudo apt update
```

Then repair dependencies if required:

```bash
sudo apt --fix-broken install
```

You can then reinstall the package:

```bash
sudo apt install ./clipo-1.0.0-Linux-amd64.deb
```

## Current Platform Limitations

Clipo v1.0.0 currently supports the Linux/X11 implementation.

The following are not part of v1.0.0:

- Native Wayland global shortcuts
- Native Wayland paste integration
- Windows support
- macOS support

Additional platform support is planned for future releases.
