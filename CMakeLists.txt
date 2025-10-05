cmake_minimum_required(VERSION 3.20)
project(coin3d CXX)

set(CMAKE_C_STANDARD 11)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_COMPILER "emcc")

# Force Emscripten toolchain when building
if (EMSCRIPTEN)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")   #builds .html + .js + .wasm
endif()

add_executable(coin3d
    src/main.cpp
    src/imgui/imgui.cpp
    src/imgui/imgui_demo.cpp
    src/imgui/imgui_draw.cpp
    src/imgui/imgui_tables.cpp
    src/imgui/imgui_widgets.cpp
)

set(SOURCES
    src/main.cpp
    src/imgui/imgui.cpp
    src/imgui/imgui_demo.cpp
    src/imgui/imgui_draw.cpp
    src/imgui/imgui_tables.cpp
    src/imgui/imgui_widgets.cpp
)

if (EMSCRIPTEN)
    # Common Emscripten settings for WebGL + filesystem
    set_target_properties(coin3d PROPERTIES
        LINK_FLAGS "-s USE_WEBGL2=1 -s FULL_ES3=1 -s WASM=1 -s ASYNCIFY --shell-file ${CMAKE_SOURCE_DIR}/template.html"
    )

    add_custom_target(run
        COMMAND ${CMAKE_COMMAND} --build . --target coin3d
        COMMAND ${CMAKE_COMMAND} -E echo "Starting server at http://localhost:8080/"
        COMMAND python3 -m http.server 8080 --directory ${CMAKE_BINARY_DIR}/
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        USES_TERMINAL
    )
endif()

# Disable stupid warning
target_compile_options(coin3d PRIVATE -Wno-c99-designator)
