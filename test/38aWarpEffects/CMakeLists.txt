
set(CMAKE_CXX_STANDARD 11)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../common
    ${CMAKE_SOURCE_DIR}
)

add_executable(38AWarpEffect 38a_warp_effect.cpp $<TARGET_OBJECTS:Catch-objects>)

if(PGEFL_QT_SUPPORT)
    target_include_directories(38AWarpEffect PUBLIC ${Qt5Core_INCLUDE_DIRS})
    target_compile_definitions(38AWarpEffect PUBLIC ${Qt5Core_DEFINITIONS})
    target_link_libraries(38AWarpEffect PRIVATE pgefl_qt ${Qt5Core_LIBRARIES})
else()
    target_link_libraries(38AWarpEffect PRIVATE pgefl)
endif()
add_test(NAME 38AWarpEffect COMMAND 38AWarpEffect WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
