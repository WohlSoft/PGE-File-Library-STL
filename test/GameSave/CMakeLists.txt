
add_executable(SaveUserDataTest save_userdata.cpp)
target_link_libraries(SaveUserDataTest PRIVATE pgefl pgefl_test_common catch2)

target_compile_definitions(SaveUserDataTest PRIVATE
    -DTEST_WRITEDIR="${CMAKE_CURRENT_BINARY_DIR}/write-tests"
)

make_directory("${CMAKE_CURRENT_BINARY_DIR}/write-tests")

add_test(NAME SaveUserDataTest COMMAND SaveUserDataTest)
