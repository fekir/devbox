cmake_minimum_required(VERSION 3.2)

project(caja-devbox CXX)

set(SOURCE_FILES
	implementation.cpp
	menu.hpp
	menu.cpp
	propertypage.hpp
	propertypage.cpp
)

#set(CMAKE_CXX_VISIBILITY_PRESET hidden)
#set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)
add_library(${PROJECT_NAME} SHARED
	${SOURCE_FILES}
)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)

set_target_properties(${PROJECT_NAME}
	PROPERTIES
	LINK_FLAGS "${CAJA_LDFLAGS} ${CAJA_CFLAGS_OTHER}"
)
target_link_libraries(${PROJECT_NAME} ${OPENSSL_LIBRARIES})

install(
	TARGETS ${PROJECT_NAME}
	LIBRARY DESTINATION "/usr/lib/x86_64-linux-gnu/caja/extensions-2.0/"
)

set(TEST_FILES
	test/test_interface.cpp
)

source_group("Test Files" FILES ${TEST_FILES})

set(PROJECT_NAME_TEST "${PROJECT_NAME}Test")
add_executable(${PROJECT_NAME_TEST} test/main.cpp ${SOURCE_FILES} ${TEST_FILES})
target_link_libraries(${PROJECT_NAME_TEST} ${OPENSSL_LIBRARIES})

set_property(TARGET ${PROJECT_NAME_TEST} PROPERTY CXX_STANDARD 11)

set_target_properties(${PROJECT_NAME_TEST}
		PROPERTIES
		LINK_FLAGS "${CAJA_LDFLAGS} ${CAJA_CFLAGS_OTHER}"
)
