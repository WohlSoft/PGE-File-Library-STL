
set(CMAKE_CXX_STANDARD 14)

add_subdirectory(GameSave)
add_subdirectory(LevelLoad)
add_subdirectory(NpcTxt)
add_subdirectory(38aWarpEffects)
add_subdirectory(ReadWrite)

add_library(Catch-objects OBJECT "common/catch_amalgamated.cpp")
target_include_directories(Catch-objects PRIVATE "common")
