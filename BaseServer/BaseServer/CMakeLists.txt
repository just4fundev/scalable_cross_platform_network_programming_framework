# Copyright Cristian Pagán Díaz. All Rights Reserved.

ProjectStructure()

add_library(BaseServer STATIC ${all_files})
target_include_directories(BaseServer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(BaseServer Connection)

PublishLibrary(${CMAKE_CURRENT_SOURCE_DIR} BaseServer)

if (WIN32)
	add_definitions(-D_WIN32_WINNT=0x0A00)
endif()