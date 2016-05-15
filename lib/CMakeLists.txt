cmake_minimum_required(VERSION 3.3)

project(${APP_NAME}_utils CXX)


set(SOURCE_FILES
	utils.hpp
	glib_iterator.hpp
	glib_memory.hpp
	gtk_memory.hpp
	posix_memory.hpp
	openssl_memory.hpp
	utils.cpp
	hexdump.cpp
	hexdump.hpp
)

add_library(${PROJECT_NAME} ${SOURCE_FILES})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)

set_target_properties(${PROJECT_NAME}
        PROPERTIES
        LINK_FLAGS "${CAJA_LDFLAGS} ${CAJA_CFLAGS_OTHER}"
)


set(TEST_FILES
	test/test_utils.cpp
	test/test_iterator.cpp
	test/test_hash.cpp
	test/test_hexdump.cpp
)

source_group("Test Files" FILES ${TEST_FILES})

add_definitions( -DTEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/test/data/" )
set(PROJECT_NAME_TEST "${PROJECT_NAME}Test")
add_executable(${PROJECT_NAME_TEST} test/main.cpp ${SOURCE_FILES} ${TEST_FILES})
target_link_libraries(${PROJECT_NAME_TEST} ${OPENSSL_LIBRARIES})

set_property(TARGET ${PROJECT_NAME_TEST} PROPERTY CXX_STANDARD 11)

set_target_properties(${PROJECT_NAME_TEST}
        PROPERTIES
        LINK_FLAGS "${CAJA_LDFLAGS} ${CAJA_CFLAGS_OTHER}"
)

#target_link_libraries(${PROJECT_NAME_TEST} ${WIN_LIBRARIES_TO_LINK})
#target_compile_definitions(${PROJECT_NAME_TEST} PUBLIC "DONOTSAFEREGKEY") # unit test should never change (at least permanently) state of system
#target_include_directories(${PROJECT_NAME_TEST} PUBLIC ${PROJECT_SOURCE_DIR})
