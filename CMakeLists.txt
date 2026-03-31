cmake_minimum_required(VERSION 3.29)
project(CMusic LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)

set(RESOURCE_DIR "${CMAKE_SOURCE_DIR}/src/resources")

# Enter directories listed & execute CMakeLists.txt located there
add_subdirectory(
        third_party/sqlite
)

find_package(Qt6
        REQUIRED COMPONENTS Core Widgets Multimedia OpenGLWidgets
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

file(GLOB_RECURSE RESOURCE_FILES "${RESOURCE_DIR}/*.*")

qt_add_executable(CMusic
        ${SOURCES}
        third_party/kissfft/kiss_fft.c
        third_party/kissfft/kiss_fftr.c
)

qt_add_resources(CMusic "resources"
        PREFIX "/resources"
        BASE "${RESOURCE_DIR}"
        FILES ${RESOURCE_FILES}
)

target_include_directories(CMusic PRIVATE
        third_party/kissfft
)

# Links libraries to executable
target_link_libraries( CMusic PRIVATE
        Qt6::Core
        Qt6::Widgets
        Qt6::Multimedia
        Qt6::OpenGLWidgets
        sqlite3
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
