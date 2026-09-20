cmake_minimum_required(VERSION 3.20)

project(Clipo
    VERSION 1.0.0
    DESCRIPTION "Fast and lightweight clipboard manager"
    LANGUAGES CXX
)


# =========================================================
# C++ configuration
# =========================================================

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


# =========================================================
# Qt automatic code generation
# =========================================================

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)


# =========================================================
# Dependencies
# =========================================================

find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
)

find_package(SQLite3 REQUIRED)

find_package(X11 REQUIRED)


# =========================================================
# Clipo executable
# =========================================================

add_executable(clipo

    # -----------------------------------------------------
    # Main application
    # -----------------------------------------------------

    src/main.cpp


    # -----------------------------------------------------
    # Clipboard module
    # -----------------------------------------------------

    src/clipboard/ClipboardManager.h
    src/clipboard/ClipboardManager.cpp


    # -----------------------------------------------------
    # Database module
    # -----------------------------------------------------

    src/database/Database.h
    src/database/Database.cpp


    # -----------------------------------------------------
    # UI module
    # -----------------------------------------------------

    src/ui/PopupWindow.h
    src/ui/PopupWindow.cpp

    src/ui/ClipboardHistoryView.h
    src/ui/ClipboardHistoryView.cpp


    # -----------------------------------------------------
    # Platform module
    # -----------------------------------------------------

    src/platform/PasteController.h
    src/platform/PasteController.cpp

    src/platform/linux/X11PasteController.cpp

    src/platform/GlobalShortcut.h
    src/platform/GlobalShortcut.cpp

    src/platform/linux/X11GlobalShortcut.cpp


    # -----------------------------------------------------
    # Settings module
    # -----------------------------------------------------

    src/settings/SettingsManager.h
    src/settings/SettingsManager.cpp

    src/settings/SettingsDialog.h
    src/settings/SettingsDialog.cpp


    # -----------------------------------------------------
    # Qt resources
    #
    # The QSS files are included through resources.qrc.
    # -----------------------------------------------------

    resources/resources.qrc
)


# =========================================================
# Include directories
# =========================================================

target_include_directories(clipo PRIVATE

    ${X11_INCLUDE_DIR}
)


# =========================================================
# Libraries
# =========================================================

target_link_libraries(clipo PRIVATE

    Qt6::Core
    Qt6::Gui
    Qt6::Widgets

    SQLite::SQLite3

    ${X11_LIBRARIES}
    Xtst
)


# =========================================================
# C++ standard
# =========================================================

target_compile_features(
    clipo PRIVATE
    cxx_std_20
)


# =========================================================
# Compiler warnings
# =========================================================

if(MSVC)

    target_compile_options(
        clipo PRIVATE
        /W4
    )

else()

    target_compile_options(
        clipo PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )

endif()


# =========================================================
# Installation
#
# These rules define where Clipo is installed on Linux.
#
# Executable:
#     /usr/bin/clipo
#
# Desktop launcher:
#     /usr/share/applications/clipo.desktop
#
# Application icon:
#     /usr/share/icons/hicolor/512x512/apps/clipo.png
#
# =========================================================

include(GNUInstallDirs)


# ---------------------------------------------------------
# Install executable
# ---------------------------------------------------------

install(
    TARGETS clipo
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)


# ---------------------------------------------------------
# Install desktop launcher
# ---------------------------------------------------------

install(
    FILES
        ${CMAKE_SOURCE_DIR}/packaging/clipo.desktop
    DESTINATION
        ${CMAKE_INSTALL_DATADIR}/applications
)


# ---------------------------------------------------------
# Install application icon
# ---------------------------------------------------------

install(
    FILES
        ${CMAKE_SOURCE_DIR}/resources/icons/clipo.png
    DESTINATION
        ${CMAKE_INSTALL_DATADIR}/icons/hicolor/512x512/apps
)


# =========================================================
# CPack configuration
#
# CPack converts the CMake installation tree into a
# distributable package.
#
# For Ubuntu/Debian we use the DEB generator.
#
# =========================================================

set(CPACK_PACKAGE_NAME "clipo")

set(CPACK_PACKAGE_VENDOR
    "Clipo"
)

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY
    "Fast and lightweight clipboard manager"
)

set(CPACK_PACKAGE_DESCRIPTION
    "Clipo is a fast, lightweight and local-first clipboard manager."
)

set(CPACK_PACKAGE_VERSION
    ${PROJECT_VERSION}
)

set(CPACK_PACKAGE_CONTACT
    "Clipo Project"
)

set(CPACK_RESOURCE_FILE_LICENSE
    "${CMAKE_SOURCE_DIR}/LICENSE"
)


# =========================================================
# Debian package settings
# =========================================================

set(CPACK_DEBIAN_PACKAGE_NAME
    "clipo"
)

set(CPACK_DEBIAN_PACKAGE_VERSION
    ${PROJECT_VERSION}
)

set(CPACK_DEBIAN_PACKAGE_SECTION
    "utils"
)

set(CPACK_DEBIAN_PACKAGE_PRIORITY
    "optional"
)

set(CPACK_DEBIAN_FILE_NAME
    "clipo-${PROJECT_VERSION}-Linux-amd64.deb"
)


# ---------------------------------------------------------
# Runtime dependencies
#
# These are runtime libraries, not development packages.
# ---------------------------------------------------------

set(CPACK_DEBIAN_PACKAGE_DEPENDS
    "libqt6core6, libqt6gui6, libqt6widgets6, libsqlite3-0, libx11-6, libxtst6"
)


# =========================================================
# Package generator
# =========================================================

set(CPACK_GENERATOR
    "DEB"
)

include(CPack)