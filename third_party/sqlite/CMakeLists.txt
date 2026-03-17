#[[
    add_libary: Create CMake target representing a lib
    sqlite3: target name, used for linking
    STATIC: compile into a .a or .lib archive & linked into executable @ buiild time
    sqlite3.c: source file to compile to produce lib
]]
add_library(sqlite3 STATIC sqlite3.c)

#[[
    target_include_directories: set include search paths
    sqlite3: target receiving the config
    PUBLIC: Lib can include any directories from this dir, anything that links
        against this library inherits include path automatically
    ${CMAKE_CURRENT_SOURCE_DIR}: Expands to directory containing this CMakeLists.txt
]]
target_include_directories(sqlite3 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Now any code linking against sqlite3 can just do: #include "sqlite3.h"