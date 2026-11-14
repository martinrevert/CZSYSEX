cmake_minimum_required(VERSION 3.20)
project(CZ3000SysEx LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    add_compile_options(/utf-8)
endif()

add_compile_definitions(NOMINMAX)

# Include directories
include_directories(
    src
    external/imgui
    external/imgui/backends
)

# ImGui Sources
set(IMGUI_SOURCES
    external/imgui/imgui.cpp
    external/imgui/imgui_draw.cpp
    external/imgui/imgui_tables.cpp
    external/imgui/imgui_widgets.cpp
    external/imgui/backends/imgui_impl_win32.cpp
    external/imgui/backends/imgui_impl_dx11.cpp
)

# CZ Application Sources
set(APP_SOURCES
    src/main.cpp
    src/core/cz_types.h
    src/core/cz_patch.h
    src/core/cz_math.h
    src/core/cz_math.cpp
    src/core/cz_patch.cpp
    src/core/cz_sysex.h
    src/core/cz_sysex.cpp
    src/core/cz_bank.h
    src/core/cz_bank.cpp
    src/core/cz_i18n.h
    src/core/cz_i18n.cpp
    src/midi/cz_midi.h
    src/midi/cz_midi.cpp
    src/ui/cz_lcd.h
    src/ui/cz_lcd.cpp
    src/ui/cz_tutorial.h
    src/ui/cz_tutorial.cpp
    src/ui/cz_manual.h
    src/ui/cz_manual.cpp
    src/ui/cz_envelope_graph.h
    src/ui/cz_envelope_graph.cpp
    src/ui/cz_librarian.h
    src/ui/cz_librarian.cpp
    src/ui/cz_panel.h
    src/ui/cz_panel.cpp
)

# Main Executable (Win32 GUI application)
add_executable(CZ3000SysEx WIN32 ${APP_SOURCES} ${IMGUI_SOURCES})

target_link_libraries(CZ3000SysEx PRIVATE
    d3d11
    dxgi
    d3dcompiler
    winmm
)

# Core Tests Executable
add_executable(test_sysex
    tests/test_sysex.cpp
    src/core/cz_math.cpp
    src/core/cz_patch.cpp
    src/core/cz_sysex.cpp
    src/core/cz_bank.cpp
)
