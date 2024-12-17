
set(CMAKE_CXX_STANDARD 14)

add_subdirectory(GameSave)
add_subdirectory(LevelLoad)
add_subdirectory(NpcTxt)
add_subdirectory(38aWarpEffects)
add_subdirectory(ReadWrite)

add_library(catch2 STATIC "common/catch_amalgamated.cpp")
target_include_directories(catch2 PRIVATE "common")
set_target_properties(catch2 PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib"
)
