# Copyright Cristian Pagán Díaz. All Rights Reserved.

cmake_minimum_required (VERSION 3.8)

project("GameServer")

include("../CMake/ProjectStructure.cmake")

add_subdirectory("../BaseServer" "BaseServer")
set_target_properties(BaseServer PROPERTIES FOLDER "Dependencies")

add_subdirectory("GameServer")
set_target_properties(GameServer PROPERTIES FOLDER "Project")