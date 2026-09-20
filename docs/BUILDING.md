# Building Clipo

This document describes how to build, test and package Clipo from source.

## Requirements

Clipo is built using:

- C++20
- Qt 6
- CMake
- Ninja
- SQLite 3
- X11
- XTest
- Git

## Install Build Dependencies

On Ubuntu/Debian:

```bash
sudo apt update

sudo apt install -y \
    git \
    build-essential \
    cmake \
    ninja-build \
    pkg-config \
    qt6-base-dev \
    qt6-tools-dev \
    qt6-tools-dev-tools \
    libsqlite3-dev \
    libx11-dev \
    libxtst-dev
```

## Clone the Repository

Using SSH:

```bash
git clone git@github.com:tejeshwaanand/Clipo.git
cd Clipo
```

Or using HTTPS:

```bash
git clone https://github.com/tejeshwaanand/Clipo.git
cd Clipo
```

## Configure the Build

Clipo uses CMake with Ninja.

Create the build directory and configure the project:

```bash
cmake -S . -B build -G Ninja
```

## Build

Build Clipo using:

```bash
cmake --build build
```

The executable will be generated at:

```text
build/clipo
```

## Run from the Build Directory

Run the locally built application:

```bash
./build/clipo
```

Running directly from the build directory is useful when developing
or testing changes before installing Clipo.

## Clean Build

To remove the existing build:

```bash
rm -rf build
```

Then configure and build again:

```bash
cmake -S . -B build -G Ninja
cmake --build build
```

## Local Installation Test

You can test the installation tree without modifying the system.

First remove any previous test installation:

```bash
rm -rf install-test
```

Then install into a local directory:

```bash
cmake --install build \
    --prefix "$PWD/install-test"
```

The resulting structure should look similar to:

```text
install-test/
├── bin/
│   └── clipo
└── share/
    ├── applications/
    │   └── clipo.desktop
    └── icons/
        └── hicolor/
            └── 512x512/
                └── apps/
                    └── clipo.png
```

## System Installation

To install Clipo system-wide:

```bash
sudo cmake --install build
```

The executable is installed to:

```text
/usr/bin/clipo
```

The desktop launcher is installed to:

```text
/usr/share/applications/clipo.desktop
```

The application icon is installed to:

```text
/usr/share/icons/hicolor/512x512/apps/clipo.png
```

## Create a Debian Package

Clipo uses CPack to generate Debian packages.

After building the application:

```bash
cpack --config build/CPackConfig.cmake
```

The generated package for version 1.0.0 is:

```text
clipo-1.0.0-Linux-amd64.deb
```

## Inspect the Debian Package

List the files contained in the package:

```bash
dpkg-deb -c clipo-1.0.0-Linux-amd64.deb
```

Inspect package metadata:

```bash
dpkg-deb -I clipo-1.0.0-Linux-amd64.deb
```

The package contains:

```text
/usr/bin/clipo
/usr/share/applications/clipo.desktop
/usr/share/icons/hicolor/512x512/apps/clipo.png
```

## Test the Debian Package

Install the generated package:

```bash
sudo apt install ./clipo-1.0.0-Linux-amd64.deb
```

Start Clipo:

```bash
clipo
```

Test the following functionality after installation:

- Application launch
- Global shortcut
- Clipboard monitoring
- Clipboard history
- Search
- Keyboard navigation
- Paste
- Clear history
- Settings
- Theme selection
- Transparency
- Popup positioning
- Desktop application entry

## Development Workflow

A typical development cycle is:

```bash
git pull

cmake -S . -B build -G Ninja

cmake --build build

./build/clipo
```

Before committing changes:

```bash
git status
```

Review modified files:

```bash
git diff
```

## Compiler Warnings

Clipo enables additional compiler warnings.

For GCC/Clang:

```text
-Wall
-Wextra
-Wpedantic
```

For MSVC:

```text
/W4
```

## Project Version

The project version is defined in:

```text
CMakeLists.txt
```

Current version:

```text
1.0.0
```

When preparing a new release, update the project version before
creating the release package.

## Release Package

Release packages should not be committed to the source repository.

The generated `.deb` file should be uploaded as an asset of the
corresponding GitHub Release.

For example:

```text
v1.0.0
└── clipo-1.0.0-Linux-amd64.deb
```

## Build Artifacts

The following directories/files are generated during development and
should not normally be committed to Git:

```text
build/
install-test/
_CPack_Packages/
*.deb
```

These paths are ignored by the project's `.gitignore`.
