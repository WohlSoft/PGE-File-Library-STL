
add_executable(ReadWriteTest read_write_test.cpp)

if(PGEFL_QT_SUPPORT)
    target_include_directories(ReadWriteTest PUBLIC ${Qt5Core_INCLUDE_DIRS})
    target_compile_definitions(ReadWriteTest PUBLIC ${Qt5Core_DEFINITIONS})
    target_link_libraries(ReadWriteTest PRIVATE pgefl_qt ${Qt5Core_LIBRARIES} pgefl_test_common catch2)
else()
    target_link_libraries(ReadWriteTest PRIVATE pgefl pgefl_test_common catch2)
endif()

target_compile_definitions(ReadWriteTest PRIVATE
    -DTEST_WORKDIR="${CMAKE_CURRENT_SOURCE_DIR}"
    -DTEST_WRITEDIR="${CMAKE_CURRENT_BINARY_DIR}/write-tests"
)

make_directory("${CMAKE_CURRENT_BINARY_DIR}/write-tests")

add_test(NAME ReadWriteTest COMMAND ReadWriteTest WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
