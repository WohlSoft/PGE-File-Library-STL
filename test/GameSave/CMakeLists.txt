
set(CMAKE_CXX_STANDARD 11)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/../common
  ${CMAKE_SOURCE_DIR})

add_executable(SaveUserDataTest save_userdata.cpp $<TARGET_OBJECTS:Catch-objects>)
target_link_libraries(SaveUserDataTest PRIVATE pgefl)
add_test(NAME SaveUserDataTest COMMAND SaveUserDataTest)

