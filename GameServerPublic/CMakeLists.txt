cmake_minimum_required (VERSION 3.8)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

project("GameServerPublic")

include("../CMake/ProjectStructure.cmake")
include("../CMake/PublishLibrary.cmake")

add_subdirectory("GameServerPublic")
set_target_properties(GameServerPublic PROPERTIES FOLDER "Project")