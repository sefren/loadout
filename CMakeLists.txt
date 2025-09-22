cmake_minimum_required(VERSION 3.20)
project(loadout)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Collect source files
set(SOURCES
        src/main.cpp
        src/ProcessWatcher.cpp
        src/Actions/Command.h
        src/Actions/ProcessCommandHandler.h
)

# Create the executable
add_executable(loadout ${SOURCES} ${HEADERS})

# Add include directories
target_include_directories(loadout PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/src/Actions
)

# Output directory
set_target_properties(loadout PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Copy compile_commands.json to project root
add_custom_command(TARGET loadout POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_BINARY_DIR}/compile_commands.json
        ${CMAKE_SOURCE_DIR}/compile_commands.json
        COMMENT "Copying compile_commands.json to project root for clangd"
        VERBATIM
)

if(EXISTS "${CMAKE_BINARY_DIR}/compile_commands.json")
    configure_file(
            ${CMAKE_BINARY_DIR}/compile_commands.json
            ${CMAKE_SOURCE_DIR}/compile_commands.json
            COPYONLY
    )
endif()

# Debug info
message(STATUS "Executable will be built at: ${CMAKE_BINARY_DIR}/bin/loadout.exe")