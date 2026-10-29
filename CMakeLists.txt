cmake_minimum_required(VERSION 3.20)
project(game C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# --- raylib -----------------------------------------------------------------
# Fetched and built from source, so the same version is used on desktop and web.
# Under emcmake, raylib picks PLATFORM=Web on its own.
include(FetchContent)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG        6.2
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(raylib)

# --- game -------------------------------------------------------------------
add_executable(game src/main.c)
target_link_libraries(game PRIVATE raylib)

if(MSVC)
    target_compile_options(game PRIVATE /W4)
else()
    target_compile_options(game PRIVATE -Wall -Wextra)
endif()

set(ASSETS_DIR "${CMAKE_SOURCE_DIR}/assets")

if(EMSCRIPTEN)
    # Emit index.html/.js/.wasm/.data so the build dir can be served as-is
    set_target_properties(game PROPERTIES OUTPUT_NAME "index" SUFFIX ".html")
    target_link_options(game PRIVATE
        -sUSE_GLFW=3
        -sALLOW_MEMORY_GROWTH=1
        --shell-file "${CMAKE_SOURCE_DIR}/web/shell.html"
        # Packs assets/ into index.data, mounted at /assets in the virtual FS
        --preload-file "${ASSETS_DIR}@assets"
    )
    # Relink when the shell or any asset changes
    file(GLOB_RECURSE ASSET_FILES CONFIGURE_DEPENDS "${ASSETS_DIR}/*")
    set_property(TARGET game APPEND PROPERTY LINK_DEPENDS
        "${CMAKE_SOURCE_DIR}/web/shell.html" ${ASSET_FILES})
else()
    # Desktop: copy assets next to the executable (main.c chdirs there on startup)
    add_custom_command(TARGET game POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${ASSETS_DIR}" "$<TARGET_FILE_DIR:game>/assets"
    )
    if(APPLE)
        target_link_libraries(game PRIVATE
            "-framework IOKit" "-framework Cocoa" "-framework OpenGL" "-framework QuartzCore")
    endif()
endif()
