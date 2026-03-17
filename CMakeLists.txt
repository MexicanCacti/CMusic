cmake_minimum_required(VERSION 3.29)
project(CMusic LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)

# Raylib
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOK "" FORCE)

FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG 5.5
)
FetchContent_MakeAvailable(raylib)

# OpenAL
set(ALSOFT_EXAMPLES OFF CACHE BOOL "" FORCE)
set(ALSOFT_UTILS OFF CACHE BOOL "" FORCE)
set(ALSOFT_NO_CONFIG_UTIL ON CACHE BOOL "" FORCE)

FetchContent_Declare(
        openal_soft
        GIT_REPOSITORY https://github.com/kcat/openal-soft.git
        GIT_TAG 1.25.1
)
FetchContent_MakeAvailable(openal_soft)

# Enter directories listed & execute CMakeLists.txt located there
add_subdirectory(
        third_party/sqlite
)

file(GLOB_RECURSE SOURCES
        ./src/*.cpp
        ./src/*.hpp
        ./src/*.h
        ./src/*.c
)

add_executable(CMusic
        ${SOURCES}
)

# Links libraries to executable
target_link_libraries( CMusic PRIVATE
        raylib
        sqlite3
        OpenAL
)