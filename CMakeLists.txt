cmake_minimum_required(VERSION 3.2)

macro(my_check_modules)
	pkg_check_modules(${ARGV})

	# For some reason, pkg_check_modules defines _CFLAGS with semicolon separators,
	# but the COMPILE_FLAGS property takes space-separated values.  So we correct.
	# (It seems common for projects to set the specialized INCLUDE_DIRECTORIES and
	# such rather than COMPILE_FLAGS, but we have no idea what _CFLAGS might
	# contain, so we need to use it.  It won't all be includes and defines.
	# For example, -pthread is a common CFLAG from pkg-config.)
	string(REPLACE ";" " " MY_STRIPPED "${${ARGV0}_CFLAGS}")
	set(${ARGV0}_CFLAGS "${MY_STRIPPED}")

	# Same with _LDFLAGS
	string(REPLACE ";" " " MY_STRIPPED "${${ARGV0}_LDFLAGS}")
	set(${ARGV0}_LDFLAGS "${MY_STRIPPED}")
endmacro()


set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/)

if(MSVC)
	# flags for msvc compiler
	include(MSVC_FLAGS)
else()
	# flags for gcc (mingw)/clang compiler
	include(GCC_FLAGS)
endif()

include(cppcheck)

# FIXME: should add  ${CATCH_INCLUDE_DIRS} only for test targets
find_package(catch REQUIRED)
include_directories(SYSTEM ${CATCH_INCLUDE_DIRS})

if( CMAKE_SYSTEM_NAME STREQUAL "Windows" )
else() # FIXME: assuming Linux and Caja, but should check if using caja/nemo/....
	find_package(GTK2 2.6 REQUIRED gtk)
	include_directories(SYSTEM ${GTK2_INCLUDE_DIRS})

	find_package(PkgConfig REQUIRED)

	pkg_check_modules(GLIB REQUIRED glib-2.0)
	include_directories(SYSTEM ${GLIB_INCLUDE_DIRS})

	my_check_modules(CAJA REQUIRED libcaja-extension)
	include_directories(SYSTEM ${CAJA_INCLUDE_DIRS})

	#find_package(CryptoPP REQUIRED)
	find_package(OpenSSL REQUIRED)
	include_directories(SYSTEM ${OPENSSL_INCLUDE_DIR})
	add_definitions(-DGTK_DISABLE_DEPRECATED)
endif()

set(APP_NAME "DevBox")

include_directories(lib)
analyze_directory_with_cppcheck(${APP_NAME} ${CMAKE_CURRENT_SOURCE_DIR} "")


add_subdirectory(lib)
add_subdirectory(src)
