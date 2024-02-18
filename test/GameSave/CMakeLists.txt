
set(CMAKE_CXX_STANDARD 14)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../common
    ${CMAKE_SOURCE_DIR}
)

add_executable(SaveUserDataTest save_userdata.cpp)
target_link_libraries(SaveUserDataTest PRIVATE pgefl catch2)

target_compile_definitions(SaveUserDataTest PRIVATE
    -DTEST_WRITEDIR="${CMAKE_CURRENT_BINARY_DIR}/write-tests"
)

make_directory("${CMAKE_CURRENT_BINARY_DIR}/write-tests")

add_test(NAME SaveUserDataTest COMMAND SaveUserDataTest)
