cmake_minimum_required(VERSION 3.20)

project(Clipo
    VERSION 0.1.0
    DESCRIPTION "Fast and lightweight clipboard manager"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
)

add_executable(clipo
    src/main.cpp
)

target_link_libraries(clipo PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

target_compile_features(clipo PRIVATE cxx_std_20)

if(MSVC)
    target_compile_options(clipo PRIVATE /W4)
else()
    target_compile_options(clipo PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )
endif()