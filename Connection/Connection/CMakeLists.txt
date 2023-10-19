# Copyright Cristian Pagán Díaz. All Rights Reserved.

ProjectStructure()

add_library(Connection STATIC ${all_files})
target_include_directories(Connection PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if (WIN32)
	add_definitions(-D_WIN32_WINNT=0x0A00)
endif()

set(BOOST_REQUIRED_VERSION 1.73)
find_package(Boost ${BOOST_REQUIRED_VERSION} REQUIRED system)
target_include_directories(Connection PUBLIC ${Boost_INCLUDE_DIR})

PublishLibrary(${CMAKE_CURRENT_SOURCE_DIR} Connection)