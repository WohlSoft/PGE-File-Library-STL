
add_executable(LevelLoadTest level_load.cpp)

target_compile_definitions(LevelLoadTest PRIVATE
    -DTEST_WORKDIR="${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(LevelLoadTest PRIVATE pgefl pgefl_test_common catch2)
add_test(NAME LevelLoadTest COMMAND LevelLoadTest WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
