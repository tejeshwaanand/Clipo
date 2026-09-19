cmake_minimum_required(VERSION 3.20)

project(Clipo
    VERSION 0.1.0
    DESCRIPTION "Fast and lightweight clipboard manager"
    LANGUAGES CXX
)

# ------------------------------------------------------------
# C++ configuration
# ------------------------------------------------------------

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ------------------------------------------------------------
# Qt configuration
# ------------------------------------------------------------

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
)
find_package(SQLite3 REQUIRED)

# ------------------------------------------------------------
# Clipo executable
# ------------------------------------------------------------

add_executable(clipo
    src/main.cpp

    # Clipboard module
    src/clipboard/ClipboardManager.h
    src/clipboard/ClipboardManager.cpp

    # Database module
    src/database/Database.h
    src/database/Database.cpp

    # UI module
    src/ui/PopupWindow.h
    src/ui/PopupWindow.cpp
)

# ------------------------------------------------------------
# Qt libraries
# ------------------------------------------------------------

target_link_libraries(clipo PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    SQLite::SQLite3
)

# ------------------------------------------------------------
# Compiler configuration
# ------------------------------------------------------------

target_compile_features(clipo PRIVATE cxx_std_20)

if(MSVC)

    # Higher warning level for Microsoft Visual C++.
    target_compile_options(clipo PRIVATE /W4)

else()

    # Enable useful warnings on GCC and Clang.
    target_compile_options(clipo PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )

endif()