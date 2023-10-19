# Copyright Cristian Pagán Díaz. All Rights Reserved.

ProjectStructure()

add_executable(GameServer ${all_files})
target_include_directories(GameServer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(GameServer BaseServer)

if (WIN32)
	add_definitions(-D_WIN32_WINNT=0x0A00)
endif()