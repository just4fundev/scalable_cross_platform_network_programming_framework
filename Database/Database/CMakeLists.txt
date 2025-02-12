ProjectStructure()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../Dependencies/CMake")

add_library(Database STATIC ${all_files})
target_include_directories(Database PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if (WIN32)
	target_compile_definitions(Database PUBLIC -D_WIN32_WINNT=0x0A00)
	target_compile_definitions(Database PUBLIC -DWIN32_LEAN_AND_MEAN)
endif()

target_compile_definitions(Database PUBLIC -DNOMINMAX)

set(BOOST_REQUIRED_VERSION 1.73)
find_package(Boost ${BOOST_REQUIRED_VERSION} REQUIRED system)
target_include_directories(Database PUBLIC ${Boost_INCLUDE_DIR})

find_package(OpenSSL 1.1 REQUIRED COMPONENTS Crypto SSL)
target_link_libraries(Database OpenSSL::Crypto OpenSSL::SSL)

target_compile_features(Database PUBLIC cxx_std_20)

PublishLibrary(${CMAKE_CURRENT_SOURCE_DIR} Database)