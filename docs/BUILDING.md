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

## Build Environment and Compatibility

Clipo can be built from source on systems that provide the required
development dependencies.

When building from source, Clipo is linked against the Qt and other
system libraries available on the build machine. Therefore, the resulting
binary is intended for systems with compatible library versions.

For the official Linux release package, Clipo is built in an Ubuntu 22.04
environment. This provides a consistent baseline for the distributed
Debian package and ensures compatibility with Ubuntu 22.04 and compatible
newer systems.

You do not need to use the official release build environment when
developing or building Clipo for your own system.

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

The generated package will be:

```text
clipo-<version>-Linux-amd64.deb
```

For example, for version 1.0.0:

```text
clipo-1.0.0-Linux-amd64.deb
```

The generated package uses the libraries available in the build
environment.

For development or local testing, you can build the package directly
on your own system.

Official Clipo release packages are built using Ubuntu 22.04 as the
release build environment to provide a consistent compatibility baseline.

## Reproducible Linux Release Build

Official Linux release packages are built using Ubuntu 22.04.

The purpose of using a fixed Ubuntu version for release builds is to
provide a consistent baseline for the compiled binary and its system
library dependencies.

A release build can be reproduced using an Ubuntu 22.04 environment,
such as a Docker container.

Example:

```bash
docker run --rm -it \
    -v "$PWD:/workspace/Clipo" \
    -w /workspace/Clipo \
    ubuntu:22.04 \
    bash
```

Inside the Ubuntu 22.04 environment, install the build dependencies
listed above. The Qt/OpenGL development packages below may also be
required by Qt6Gui:

```bash
apt update

apt install -y \
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
    libxtst-dev \
    libgl1-mesa-dev \
    libglu1-mesa-dev \
    libegl1-mesa-dev \
    libopengl-dev
```

Then build Clipo:

```bash
cmake -S . -B build-ubuntu22 -G Ninja
cmake --build build-ubuntu22
```

Create the Debian package:

```bash
cpack --config build-ubuntu22/CPackConfig.cmake
```

This produces the Debian package using the Ubuntu 22.04 build
environment.

The Docker environment is not required for normal Clipo development
or local source builds.

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

A locally generated Debian package can be tested before release.

Install the generated package:

```bash
sudo apt install ./clipo-<version>-Linux-amd64.deb
```

For version 1.0.0:

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

Official Linux release packages are built using the documented
Ubuntu 22.04 release build environment.

The generated `.deb` file should be uploaded as an asset of the
corresponding GitHub Release.

For example:

```text
v1.0.0
└── clipo-1.0.0-Linux-amd64.deb
```

The official release package is intended to provide a consistent
Linux compatibility baseline. Developers building Clipo from source
may build directly on their own supported development environment.

## Build Artifacts

The following directories/files are generated during development and
should not normally be committed to Git:

```text
build/
build-*/
install-test/
_CPack_Packages/
*.deb
```

These paths are ignored by the project's `.gitignore`.
