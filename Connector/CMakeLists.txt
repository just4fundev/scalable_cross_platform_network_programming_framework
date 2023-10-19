# Copyright Cristian Pagán Díaz. All Rights Reserved.

cmake_minimum_required (VERSION 3.8)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

project("Connector")

include("../CMake/ProjectStructure.cmake")

add_subdirectory("../Connection" "Connection")
set_target_properties(Connection PROPERTIES FOLDER "Dependencies")

add_subdirectory("Connector")
set_target_properties(Connector PROPERTIES FOLDER "Project")