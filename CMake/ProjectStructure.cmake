# Copyright Cristian Pagán Díaz. All Rights Reserved.

macro(CollectFiles dir)
	file(GLOB category_files "${dir}/*.cpp" "${dir}/*.h" "${dir}/*.inl" "${dir}/*.hpp" "${dir}/*.hint")
endmacro()

macro (CollectSubdirectories dir relative_dir)
	file(GLOB subdirectories RELATIVE "${dir}/${relative_dir}" "${dir}/${relative_dir}/*")
	foreach(subdirectory ${subdirectories})
		if(IS_DIRECTORY "${dir}/${relative_dir}/${subdirectory}")
			if ("${relative_dir}" STREQUAL "")
				set(category "${subdirectory}")
			else()
				set(category "${relative_dir}/${subdirectory}")
			endif()
			
			list(APPEND categories ${category})
			CollectSubdirectories(${dir} "${category}")
		endif()
	endforeach()
endmacro()

macro(CollectProjectStructure dir base_group)
	foreach(category ${categories})
		CollectFiles("${dir}/${category}")
		source_group("${base_group}/${category}" FILES ${category_files})
		list(APPEND all_files ${category_files})
	endforeach()
endmacro()

macro(ProjectStructure)
	CollectSubdirectories(${CMAKE_CURRENT_SOURCE_DIR} "")
	CollectProjectStructure(${CMAKE_CURRENT_SOURCE_DIR} "Code")

	CollectFiles(${CMAKE_CURRENT_SOURCE_DIR})
	list(APPEND all_files ${category_files})
	source_group("Code" FILES ${category_files})
endmacro()