cmake_minimum_required(VERSION 3.2)

if( CMAKE_SYSTEM_NAME STREQUAL "Windows" )
else()
	add_subdirectory(caja_plugin)
endif()
