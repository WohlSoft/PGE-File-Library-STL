
set(CMAKE_CXX_STANDARD 14)

add_library(pgefl_test_common INTERFACE)
target_include_directories(pgefl_test_common INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/common
    ${PGEFileLibrary_SOURCE_DIR}
)

if(PGEFL_ENABLE_RWOPS)
    find_package(SDL2 REQUIRED)

    if(MINGW AND NOT SDL2_LIBRARIES)
        set(SDL2_LIBRARIES mingw32 SDL2main SDL2)
    endif()

    target_compile_definitions(pgefl_test_common INTERFACE -DPGEFL_ENABLE_RWOPS)
    target_link_libraries(pgefl_test_common INTERFACE ${SDL2_LIBRARIES})
endif()

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
