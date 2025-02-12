cmake_minimum_required (VERSION 3.8)

project("GameServer")

include("../CMake/ProjectStructure.cmake")

add_subdirectory("../Connection" "Connection")
set_target_properties(Connection PROPERTIES FOLDER "Dependencies")

add_subdirectory("../BaseServer" "BaseServer")
set_target_properties(BaseServer PROPERTIES FOLDER "Dependencies")

add_subdirectory("../Database" "Database")
set_target_properties(Database PROPERTIES FOLDER "Dependencies")

add_subdirectory("../GameServerPublic" "GameServerPublic")
set_target_properties(GameServerPublic PROPERTIES FOLDER "Dependencies")

add_subdirectory("GameServer")
set_target_properties(GameServer PROPERTIES FOLDER "Project")