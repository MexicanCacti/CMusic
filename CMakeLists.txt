cmake_minimum_required(VERSION 3.29)
project(CMusic LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)

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

find_package(Qt6
        REQUIRED COMPONENTS Core Widgets Multimedia
)
qt_standard_project_setup()

file(GLOB_RECURSE SOURCES
        ./src/*.cpp
        ./src/*.hpp
        ./src/*.h
        ./src/*.c
        ./src/*/*.cpp
        ./src/*/*.hpp
        ./src/*/*.c
        ./src/*/*.h
)

qt_add_executable(CMusic
        ${SOURCES}
)

# Links libraries to executable
target_link_libraries( CMusic PRIVATE
        Qt6::Core
        Qt6::Widgets
        Qt6::Multimedia
        sqlite3
        OpenAL
)

include(GNUInstallDirs)

install(TARGETS CMusic
        BUNDLE DESTINATION .
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
        TARGET CMusic
        OUTPUT_SCRIPT deploy_script
        NO_UNSUPPORTED_PLATFORM_ERROR
)

install(SCRIPT ${deploy_script})
