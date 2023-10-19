# Copyright Cristian Pagán Díaz. All Rights Reserved.

ProjectStructure()

add_library(Connector SHARED ${all_files})
target_include_directories(Connector PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Connector Connection)

if (WIN32)
	add_definitions(-D_WIN32_WINNT=0x0A00)
endif()