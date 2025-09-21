cmake_minimum_required(VERSION 3.20)
project(loadout)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add executable with source files from src/
add_executable(loadout
    src/main.cpp
    src/ProcessWatcher.cpp
    src/Logger.h
    src/ProcessWatcher.h
        src/Events.h
)

# Set output directory to bin subdirectory
set_target_properties(loadout PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Optional: Print where the executable will be built
message(STATUS "Executable will be built at: ${CMAKE_BINARY_DIR}/bin/loadout.exe")