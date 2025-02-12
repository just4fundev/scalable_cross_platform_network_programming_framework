ProjectStructure()

add_executable(GameServer ${all_files})
target_include_directories(GameServer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(GameServer Connection BaseServer Database GameServerPublic)