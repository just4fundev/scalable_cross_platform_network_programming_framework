# Copyright Cristian Pagán Díaz. All Rights Reserved.

macro(PublishLibrary dir lib)
	foreach(category ${categories})
		if("${dir}/${category}" MATCHES "Include/Public$")
			target_include_directories(${lib} PUBLIC "${dir}/${category}")
		endif()
	endforeach()
endmacro()