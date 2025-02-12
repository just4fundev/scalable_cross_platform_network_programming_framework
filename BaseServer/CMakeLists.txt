cmake_minimum_required (VERSION 3.8)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

project("BaseServer")

include("../CMake/ProjectStructure.cmake")
include("../CMake/PublishLibrary.cmake")

add_subdirectory("BaseServer")
set_target_properties(BaseServer PROPERTIES FOLDER "Project")