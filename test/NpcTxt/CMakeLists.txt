
set(CMAKE_CXX_STANDARD 11)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/../common
  ${CMAKE_SOURCE_DIR})

add_executable(NpcTxtTest npctxt.cpp $<TARGET_OBJECTS:Catch-objects>)
if(PGEFL_QT_SUPPORT)
    target_include_directories(NpcTxtTest PUBLIC ${Qt5Core_INCLUDE_DIRS})
    target_compile_definitions(NpcTxtTest PUBLIC ${Qt5Core_DEFINITIONS})
    target_link_libraries(NpcTxtTest PRIVATE pgefl_qt ${Qt5Core_LIBRARIES})
else()
    target_link_libraries(NpcTxtTest PRIVATE pgefl)
endif()
add_test(NAME NpcTxtTest COMMAND NpcTxtTest WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
