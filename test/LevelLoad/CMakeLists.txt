
set(CMAKE_CXX_STANDARD 14)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../common
    ${CMAKE_SOURCE_DIR}
)

add_executable(LevelLoadTest level_load.cpp)

target_compile_definitions(LevelLoadTest PRIVATE
    -DTEST_WORKDIR="${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(LevelLoadTest PRIVATE pgefl catch2)
add_test(NAME LevelLoadTest COMMAND LevelLoadTest WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
