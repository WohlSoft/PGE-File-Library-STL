
add_executable(38AWarpEffect 38a_warp_effect.cpp)

if(PGEFL_QT_SUPPORT)
    target_include_directories(38AWarpEffect PUBLIC ${Qt5Core_INCLUDE_DIRS})
    target_compile_definitions(38AWarpEffect PUBLIC ${Qt5Core_DEFINITIONS})
    target_link_libraries(38AWarpEffect PRIVATE pgefl_qt ${Qt5Core_LIBRARIES} pgefl_test_common catch2)
else()
    target_link_libraries(38AWarpEffect PRIVATE pgefl pgefl_test_common catch2)
endif()

target_compile_definitions(38AWarpEffect PRIVATE
    -DTEST_WORKDIR="${CMAKE_CURRENT_SOURCE_DIR}"
)

add_test(NAME 38AWarpEffect COMMAND 38AWarpEffect WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
