ProjectStructure()

add_library(GameServerPublic STATIC ${all_files})
target_include_directories(GameServerPublic PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(GameServerPublic Connection)

PublishLibrary(${CMAKE_CURRENT_SOURCE_DIR} GameServerPublic)