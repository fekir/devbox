cmake_minimum_required(VERSION 3.3)

project(${APP_NAME}_utils CXX)

if( CMAKE_SYSTEM_NAME STREQUAL "Windows" )
	set(PLATFORM_SPECIFIC_FILES
		windows/process_windows.hpp
	)
else() # may need further checking
	include_directories(posix)
	set(PLATFORM_SPECIFIC_FILES
		posix/glib_iterator.hpp
		posix/glib_memory.hpp
		posix/gtk_memory.hpp
		posix/posix_memory.hpp
		openssl_memory.hpp # openssl
		posix/utils.hpp
		posix/utils.cpp
		posix/process_posix.hpp
		posix/filesystem_utils_caja.hpp
		posix/filesystem_utils_posix.hpp
	)
endif()


set(SOURCE_FILES
	string_utils.hpp
	hexdump.cpp
	hexdump.hpp
	process.hpp
	process_base.hpp
	filesystem_utils.hpp
	filesystem_utils_base.hpp
	${PLATFORM_SPECIFIC_FILES}
)

add_library(${PROJECT_NAME} ${SOURCE_FILES})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)

if( CMAKE_SYSTEM_NAME STREQUAL "Windows" )
	set(PLATFORM_SPECIFIC_TEST_FILES
		test/test_process_windows.cpp
	)
else() # may need further checking

	set_target_properties(${PROJECT_NAME}
		PROPERTIES
		LINK_FLAGS "${CAJA_LDFLAGS} ${CAJA_CFLAGS_OTHER}"
	)

endif()


# Test Project
if( CMAKE_SYSTEM_NAME STREQUAL "Windows" )
	set(PLATFORM_SPECIFIC_TEST_FILES
		test/test_process_windows.cpp
	)
else() # may need further checking
	set(PLATFORM_SPECIFIC_TEST_FILES
		test/test_utils.cpp
		test/test_iterator.cpp
		test/test_process.cpp
		test/test_hash.cpp # openssl
	)

	set(PLATFORM_SPECIFIC_TEST_LIBRARIES
		${OPENSSL_LIBRARIES}
)

	set(PLATFORM_SPECIFIC_LINK_FLAGS
		"${CAJA_LDFLAGS} ${CAJA_CFLAGS_OTHER}"
	)
endif()

set(TEST_FILES
	${PLATFORM_SPECIFIC_TEST_FILES}
	test/test_hexdump.cpp
	test/test_string_utils.cpp
	test/test_filesystem_base.cpp
)
source_group("Test Files" FILES ${TEST_FILES})

add_definitions( -DTEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/test/data/" )
set(PROJECT_NAME_TEST "${PROJECT_NAME}Test")
add_executable(${PROJECT_NAME_TEST} test/main.cpp ${SOURCE_FILES} ${TEST_FILES})

target_link_libraries(${PROJECT_NAME_TEST} ${PLATFORM_SPECIFIC_TEST_LIBRARIES})

set_property(TARGET ${PROJECT_NAME_TEST} PROPERTY CXX_STANDARD 11)

set_target_properties(${PROJECT_NAME_TEST}
	PROPERTIES
	LINK_FLAGS "${PLATFORM_SPECIFIC_LINK_FLAGS}"
)

