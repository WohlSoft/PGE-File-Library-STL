
set(CMAKE_CXX_STANDARD 11)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/../common
  ${CMAKE_SOURCE_DIR})

add_executable(LevelLoadTest level_load.cpp $<TARGET_OBJECTS:Catch-objects>)
target_link_libraries(LevelLoadTest PRIVATE pgefl)
add_test(NAME LevelLoadTest COMMAND LevelLoadTest WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
