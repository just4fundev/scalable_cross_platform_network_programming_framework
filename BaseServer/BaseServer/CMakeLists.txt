ProjectStructure()

add_library(BaseServer STATIC ${all_files})
target_include_directories(BaseServer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(BaseServer Connection)

PublishLibrary(${CMAKE_CURRENT_SOURCE_DIR} BaseServer)