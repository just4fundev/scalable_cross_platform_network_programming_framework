ProjectStructure()

add_library(Connection STATIC ${all_files})
target_include_directories(Connection PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if (WIN32)
	target_compile_definitions(Connection PUBLIC -D_WIN32_WINNT=0x0A00)
	target_compile_definitions(Connection PUBLIC -DWIN32_LEAN_AND_MEAN)
endif()

target_compile_definitions(Connection PUBLIC -DNOMINMAX)

set(BOOST_REQUIRED_VERSION 1.82)
find_package(Boost ${BOOST_REQUIRED_VERSION} REQUIRED system)
target_include_directories(Connection PUBLIC ${Boost_INCLUDE_DIR})

PublishLibrary(${CMAKE_CURRENT_SOURCE_DIR} Connection)